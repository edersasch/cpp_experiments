#ifndef TESTS_FS_HISTORY_QT_FS_HISTORY_TEST
#define TESTS_FS_HISTORY_QT_FS_HISTORY_TEST

#include "fs_history_qt/fs_history.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <QTemporaryDir>
#include <QTemporaryFile>

#include <memory>
#include <vector>

class FS_History_Test_Slots : public QObject
{
    Q_OBJECT

public:
    FS_History_Test_Slots() = default;
    virtual ~FS_History_Test_Slots() = default;

    MOCK_METHOD(void, elements_changed, (const QStringList&));
};

class File_History_Test : public QObject, public ::testing::Test
{
    Q_OBJECT

public:
    File_History_Test();
    virtual ~File_History_Test() = default;

protected:
    static constexpr int history_size { 5 };
    std::vector<std::unique_ptr<QTemporaryFile>> fill(std::size_t amount);
    FsHistory mFileHistory;
    ::testing::StrictMock<FS_History_Test_Slots> mHistorySlots;
};

class Dir_History_Test : public QObject, public ::testing::Test
{
    Q_OBJECT

public:
    Dir_History_Test();
    virtual ~Dir_History_Test() = default;

protected:
    static constexpr int history_size { 5 };
    QTemporaryDir mFallback;
    FsHistory mDirHistory;
    ::testing::StrictMock<FS_History_Test_Slots> mHistorySlots;
};

#endif // TESTS_FS_HISTORY_QT_FS_HISTORY_TEST
