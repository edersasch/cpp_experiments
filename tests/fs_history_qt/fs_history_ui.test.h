#ifndef TESTS_FS_HISTORY_QT_FS_HISTORY_UI_TEST
#define TESTS_FS_HISTORY_QT_FS_HISTORY_UI_TEST

#include "fs_history_qt/fs_history_ui.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

class FS_History_UI_Test_Slots
        : public QObject
{
    Q_OBJECT

public:
    FS_History_UI_Test_Slots() = default;
    virtual ~FS_History_UI_Test_Slots() = default;

    MOCK_METHOD(void, current_element_changed, (const QString&));
};

class File_History_UI_Test
        : public QObject
        , public ::testing::Test
{
    Q_OBJECT

public:
    File_History_UI_Test();
    virtual ~File_History_UI_Test() = default;

protected:
    const int history_size {5};
    FS_History_UI file_history_ui;
    ::testing::StrictMock<FS_History_UI_Test_Slots> history_ui_slots;
};

#endif // TESTS_FS_HISTORY_QT_FS_HISTORY_UI_TEST
