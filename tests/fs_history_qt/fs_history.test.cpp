#include "fs_history.test.h"

#include "qt_gtest_helper/qt_gtest_helper.h"

File_History_Test::File_History_Test()
    : file_history(history_size)
{
    connect(&file_history, &FS_History::elements_changed, &history_slots, &FS_History_Test_Slots::elements_changed);
    EXPECT_EQ(FS_History::Operation_Mode::OP_FILE, file_history.operation_mode());
    EXPECT_CALL(history_slots, elements_changed(QStringList({})));
    processEvents();
}

std::vector<std::unique_ptr<QTemporaryFile>> File_History_Test::fill(std::size_t amount)
{
    std::vector<std::unique_ptr<QTemporaryFile>> ret(amount);
    QStringList fn;
    for (auto& qtf : ret) {
        qtf = std::make_unique<QTemporaryFile>();
        qtf->open();
        fn.prepend(qtf->fileName());
        if (fn.size() > history_size) {
            fn.removeLast();
        }
        processEvents();
        EXPECT_CALL(history_slots, elements_changed(fn));
        file_history.set_current_element(qtf->fileName());
        processEvents();
    }
    return ret;
}

Dir_History_Test::Dir_History_Test()
    : dir_history(fallback.path(), history_size)
{
    EXPECT_EQ(FS_History::Operation_Mode::OP_DIR, dir_history.operation_mode());
    connect(&dir_history, &FS_History::elements_changed, &history_slots, &FS_History_Test_Slots::elements_changed);
}

TEST(move_to_front, string_vector)
{
    std::vector<std::string> test_vector {"2", "3", "4", "1", "5"};
    std::vector<std::string> reference_vector {"1", "2", "3", "4", "5"};
    move_to_front(test_vector, 3);
    EXPECT_EQ(test_vector, reference_vector);
}

TEST(recent_files, verbose_ctor)
{
    static constexpr int history_size = 5;
    QTemporaryFile f2;
    QTemporaryFile f4;
    f2.open();
    f4.open();
    FS_History file_history(FS_History::Operation_Mode::OP_FILE, "n/a", history_size, {"1", f2.fileName(), "3", f4.fileName(), "5"});
    processEvents();
    EXPECT_EQ(QStringList({f2.fileName(), f4.fileName()}), file_history.get_elements());
}

TEST(recent_files, list_ctor)
{
    QTemporaryFile f1;
    QTemporaryDir d3;
    QTemporaryFile f9;
    f1.open();
    f9.open();
    FS_History file_history({f1.fileName(), "2", d3.path(), "4", "5", "6", "7", "8", f9.fileName()});
    processEvents();
    EXPECT_EQ(QStringList({f1.fileName(), f9.fileName()}), file_history.get_elements());
}

TEST(recent_files, history_size_ctor)
{
    QTemporaryFile f1;
    QTemporaryFile f2;
    QTemporaryFile f3;
    f1.open();
    f2.open();
    f3.open();
    FS_History file_history(2, {f1.fileName(), f1.fileName(), f2.fileName(), "4", f3.fileName(), "6", "7", "8", "9"});
    processEvents();
    EXPECT_EQ(QStringList({f1.fileName(), f2.fileName()}), file_history.get_elements());
}

TEST(recent_files, fallback_dir_ctor)
{
    static constexpr int history_size = 5;
    QTemporaryDir d3;
    QTemporaryFile f4;
    QTemporaryDir d6;
    f4.open();
    FS_History dir_history("n/a", history_size, {"1", "2", d3.path(), f4.fileName(), "5", d6.path(), d6.path(), d3.path(), "9"});
    processEvents();
    EXPECT_EQ(QStringList({d3.path(), d6.path()}), dir_history.get_elements());
}

TEST_F(File_History_Test, deny_set_current_element)
{
    file_history.set_current_element("1");
    processEvents();
}

TEST_F(File_History_Test, set_current_element)
{
    QTemporaryFile f1;
    f1.open();
    EXPECT_CALL(history_slots, elements_changed(QStringList({f1.fileName()})));
    file_history.set_current_element(f1.fileName());
    processEvents();
}

TEST_F(File_History_Test, truncate_elements)
{
    static constexpr int num_entries = 10;
    fill(num_entries);
}

TEST_F(File_History_Test, set_existing_element)
{
    auto qtfs = fill(2);
    auto names = file_history.get_elements();
    move_to_front(names, 1);
    EXPECT_CALL(history_slots, elements_changed(QStringList(names)));
    file_history.set_current_element(names.first());
    processEvents();
}

TEST_F(Dir_History_Test, set_fallback)
{
    EXPECT_CALL(history_slots, elements_changed(QStringList({fallback.path()})));
    processEvents();
}

TEST_F(Dir_History_Test, detect_dir_removal)
{
    EXPECT_CALL(history_slots, elements_changed(QStringList({fallback.path()})));
    processEvents();
    QTemporaryDir d1;
    EXPECT_CALL(history_slots, elements_changed(QStringList({d1.path(), fallback.path()})));
    dir_history.set_current_element(d1.path());
    processEvents();
    EXPECT_CALL(history_slots, elements_changed(QStringList({fallback.path()})));
    d1.remove();
    processEvents();
}
