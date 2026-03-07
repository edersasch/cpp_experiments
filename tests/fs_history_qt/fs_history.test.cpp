#include "fs_history.test.h"

#include "qt_gtest_helper/qt_gtest_helper.h"

File_History_Test::File_History_Test()
: mFileHistory(history_size)
{
    connect(&mFileHistory, &FsHistory::elements_changed, &mHistorySlots, &FS_History_Test_Slots::elements_changed);
    EXPECT_EQ(FsHistory::Operation_Mode::OP_FILE, mFileHistory.operation_mode());
    EXPECT_CALL(mHistorySlots, elements_changed(QStringList({})));
    processEvents();
}

std::vector<std::unique_ptr<QTemporaryFile>> File_History_Test::fill(std::size_t amount)
{
    std::vector<std::unique_ptr<QTemporaryFile>> ret(amount);
    QStringList fileNames;
    for (auto& qtf : ret) {
        qtf = std::make_unique<QTemporaryFile>();
        qtf->open();
        fileNames.prepend(qtf->fileName());
        if (fileNames.size() > history_size) {
            fileNames.removeLast();
        }
        processEvents();
        EXPECT_CALL(mHistorySlots, elements_changed(fileNames));
        mFileHistory.set_current_element(qtf->fileName());
        processEvents();
    }
    return ret;
}

Dir_History_Test::Dir_History_Test()
: mDirHistory(mFallback.path(), history_size)
{
    EXPECT_EQ(FsHistory::Operation_Mode::OP_DIR, mDirHistory.operation_mode());
    connect(&mDirHistory, &FsHistory::elements_changed, &mHistorySlots, &FS_History_Test_Slots::elements_changed);
}

TEST(move_to_front, string_vector)
{
    std::vector<std::string> test_vector { "2", "3", "4", "1", "5" };
    const std::vector<std::string> reference_vector { "1", "2", "3", "4", "5" };
    move_to_front(test_vector, 3);
    EXPECT_EQ(test_vector, reference_vector);
}

TEST(recent_files, verbose_ctor)
{
    static constexpr int history_size = 5;
    QTemporaryFile file2;
    QTemporaryFile file4;
    file2.open();
    file4.open();
    const FsHistory file_history(FsHistory::Operation_Mode::OP_FILE, "n/a", history_size,
        { "1", file2.fileName(), "3", file4.fileName(), "5" });
    processEvents();
    EXPECT_EQ(QStringList({ file2.fileName(), file4.fileName() }), file_history.get_elements());
}

TEST(recent_files, list_ctor)
{
    QTemporaryFile file1;
    const QTemporaryDir dir3;
    QTemporaryFile file9;
    file1.open();
    file9.open();
    const FsHistory file_history({ file1.fileName(), "2", dir3.path(), "4", "5", "6", "7", "8", file9.fileName() });
    processEvents();
    EXPECT_EQ(QStringList({ file1.fileName(), file9.fileName() }), file_history.get_elements());
}

TEST(recent_files, history_size_ctor)
{
    QTemporaryFile file1;
    QTemporaryFile file2;
    QTemporaryFile file3;
    file1.open();
    file2.open();
    file3.open();
    const FsHistory file_history(
        2, { file1.fileName(), file1.fileName(), file2.fileName(), "4", file3.fileName(), "6", "7", "8", "9" });
    processEvents();
    EXPECT_EQ(QStringList({ file1.fileName(), file2.fileName() }), file_history.get_elements());
}

TEST(recent_files, fallback_dir_ctor)
{
    static constexpr int history_size = 5;
    const QTemporaryDir dir3;
    QTemporaryFile file4;
    const QTemporaryDir dir6;
    file4.open();
    const FsHistory dir_history("n/a", history_size,
        { "1", "2", dir3.path(), file4.fileName(), "5", dir6.path(), dir6.path(), dir3.path(), "9" });
    processEvents();
    EXPECT_EQ(QStringList({ dir3.path(), dir6.path() }), dir_history.get_elements());
}

TEST_F(File_History_Test, deny_set_current_element)
{
    mFileHistory.set_current_element("1");
    processEvents();
}

TEST_F(File_History_Test, set_current_element)
{
    QTemporaryFile file1;
    file1.open();
    EXPECT_CALL(mHistorySlots, elements_changed(QStringList({ file1.fileName() })));
    mFileHistory.set_current_element(file1.fileName());
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
    auto names = mFileHistory.get_elements();
    move_to_front(names, 1);
    EXPECT_CALL(mHistorySlots, elements_changed(QStringList(names)));
    mFileHistory.set_current_element(names.first());
    processEvents();
}

TEST_F(Dir_History_Test, set_fallback)
{
    EXPECT_CALL(mHistorySlots, elements_changed(QStringList({ mFallback.path() })));
    processEvents();
}

TEST_F(Dir_History_Test, detect_dir_removal)
{
    EXPECT_CALL(mHistorySlots, elements_changed(QStringList({ mFallback.path() })));
    processEvents();
    QTemporaryDir dir1;
    EXPECT_CALL(mHistorySlots, elements_changed(QStringList({ dir1.path(), mFallback.path() })));
    mDirHistory.set_current_element(dir1.path());
    processEvents();
    EXPECT_CALL(mHistorySlots, elements_changed(QStringList({ mFallback.path() })));
    dir1.remove();
    processEvents();
}
