#include "fs_history_ui.test.h"
#include "qt_utilities/combobox_label_elide_left_proxy_style.h"

#include "qt_gtest_helper/qt_gtest_helper.h"

#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QMenu>
#include <QComboBox>
#include <QToolButton>
#include <QWidget>
#include <QVBoxLayout>

File_History_UI_Test::File_History_UI_Test()
    : file_history_ui(history_size)
{
    connect(&file_history_ui, &FS_History_UI::current_element_changed, &history_ui_slots, &FS_History_UI_Test_Slots::current_element_changed);
    EXPECT_CALL(history_ui_slots, current_element_changed(QString("")));
    processEvents();
}

TEST(recent_files_ui, verbose_ctor)
{
    static constexpr int history_size = 5;
    QTemporaryFile f2;
    QTemporaryFile f4;
    f2.open();
    f4.open();
    FS_History_UI file_history_ui(FS_History::Operation_Mode::OP_FILE, "n/a", history_size, {"1", f2.fileName(), "3", f4.fileName(), "5"});
    processEvents();
    EXPECT_EQ(QStringList({f2.fileName(), f4.fileName()}), file_history_ui.get_elements());
}

TEST(recent_files_ui, list_ctor)
{
    QTemporaryFile f1;
    QTemporaryDir d3;
    QTemporaryFile f9;
    f1.open();
    f9.open();
    FS_History_UI file_history_ui({f1.fileName(), "2", d3.path(), "4", "5", "6", "7", "8", f9.fileName()});
    processEvents();
    EXPECT_EQ(QStringList({f1.fileName(), f9.fileName()}), file_history_ui.get_elements());
}

TEST(recent_files_ui, history_size_ctor)
{
    QTemporaryFile f1;
    QTemporaryFile f2;
    QTemporaryFile f3;
    f1.open();
    f2.open();
    f3.open();
    FS_History_UI file_history_ui(2, {f1.fileName(), f1.fileName(), f2.fileName(), "4", f3.fileName(), "6", "7", "8", "9"});
    processEvents();
    EXPECT_EQ(QStringList({f1.fileName(), f2.fileName()}), file_history_ui.get_elements());
}

TEST(recent_files_ui, fallback_dir_ctor)
{
    static constexpr int history_size = 5;
    QTemporaryDir d3;
    QTemporaryFile f4;
    QTemporaryDir d6;
    f4.open();
    FS_History_UI dir_history_ui("n/a", history_size, {"1", "2", d3.path(), f4.fileName(), "5", d6.path(), d6.path(), d3.path(), "9"});
    processEvents();
    EXPECT_EQ(QStringList({d3.path(), d6.path()}), dir_history_ui.get_elements());
}

TEST_F(File_History_UI_Test, add_two_elements)
{
    QWidget w;
    QVBoxLayout l;
    auto menu = file_history_ui.menu();
    auto cb = file_history_ui.combobox();
    auto bb = file_history_ui.browse_button();
    cb->setStyle(new Combobox_Label_Elide_Left_Proxy_Style);
    bb->setStyle(new Combobox_Label_Elide_Left_Proxy_Style); // must not hurt
    l.addWidget(menu);
    l.addWidget(cb);
    l.addWidget(bb);
    w.setLayout(&l);
    w.show();
    file_history_ui.set_current_element("1");
    processEvents(); // nothing, invalid element
    QTemporaryFile f1;
    f1.open();
    EXPECT_CALL(history_ui_slots, current_element_changed(f1.fileName()));
    file_history_ui.set_current_element(f1.fileName());
    processEvents();
    QTemporaryFile f2;
    f2.open();
    EXPECT_CALL(history_ui_slots, current_element_changed(f2.fileName()));
    file_history_ui.set_current_element(f2.fileName());
    processEvents();
}
