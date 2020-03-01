#ifndef TESTS_FS_FILTER_QT_FS_FILTER_TEST
#define TESTS_FS_FILTER_QT_FS_FILTER_TEST

#include "fs_filter_qt/fs_filter.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <QTemporaryFile>
#include <QTemporaryDir>

#include <vector>
#include <memory>

class FS_Filter_Test_Slots
        : public QObject
{
    Q_OBJECT

public:
    FS_Filter_Test_Slots() = default;
    virtual ~FS_Filter_Test_Slots() = default;

    MOCK_METHOD(void, current_element_changed, (const QString&));
};

class FS_Filter_Test
        : public QObject
        , public ::testing::Test
{
    Q_OBJECT

public:
    FS_Filter_Test();
    virtual ~FS_Filter_Test() = default;

protected:
    void mkfile(const QString& path);
    void mkpath(const QString& path);
    void compare_selection(const QSet<QString>& expected) const;

    QTemporaryDir root_dir {};
    FS_Filter fsf;
    QTreeView& fsf_view;
    FS_Sort_Filter_Proxy_Model* fsf_proxy;
    QFileSystemModel* fsf_model;
    QDir hierarchy {root_dir.path()};
    QSet<QString> fsset;
    ::testing::StrictMock<FS_Filter_Test_Slots> fsf_slots;
};

#endif // TESTS_FS_FILTER_QT_FS_FILTER_TEST
