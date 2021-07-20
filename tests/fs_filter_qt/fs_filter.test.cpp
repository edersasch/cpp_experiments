#include "fs_filter.test.h"

#include "qt_gtest_helper/qt_gtest_helper.h"

FS_Filter_Test::FS_Filter_Test()
    : fsf(root_dir.path())
    , fsf_view(fsf.get_view())
{
    fsf.set_auto_expand(true);
    fsf.set_focus();
    fsf_view.setSelectionMode(QAbstractItemView::ExtendedSelection);
    fsf_model = qobject_cast<QFileSystemModel*>(fsf_view.model());
    EXPECT_TRUE(fsf.get_search_text().isEmpty());
}

void FS_Filter_Test::mkfile(const QString& path)
{
    QFile f(hierarchy.absolutePath() + "/" + path);
    f.open(QIODevice::WriteOnly);
    fsset.insert(f.fileName());
}

void FS_Filter_Test::mkpath(const QString& path)
{
    QDir dir(hierarchy.absolutePath() + "/" + path);
    hierarchy.mkpath(path);
    while (dir.path().size() > hierarchy.path().size()) {
        fsset.insert(dir.path());
        dir.cdUp();
    }
}

void FS_Filter_Test::compare_selection(const QSet<QString>& expected) const
{
    QSet<QString> selection;
    fsf_view.selectAll();
    auto sel = fsf_view.selectionModel()->selectedRows();
    for (auto i : sel) {
        selection.insert(fsf_model->filePath(i));
    }
    EXPECT_EQ(selection, expected);
}

TEST_F(FS_Filter_Test, auto_expand)
{
    mkfile("f.1");
    mkfile("f.2");
    mkpath("1/1_1");
    mkfile("1/1_1/f1_1.1");
    mkpath("2/2_1");
    processEvents(processEvent_repetitions);
    compare_selection(fsset);
}

TEST_F(FS_Filter_Test, no_hide_empty_without_search_text)
{
    fsf.set_hide_empty_dirs(true);
    mkfile("f.1");
    mkfile("f.2");
    mkpath("1/1_1");
    mkfile("1/1_1/f1_1.1");
    mkpath("2/2_1");
    processEvents(processEvent_repetitions);
    compare_selection(fsset); // does not hide
}

TEST_F(FS_Filter_Test, hide_empty)
{
    fsf.set_hide_empty_dirs(true);
    mkfile("f.1");
    mkfile("f.2");
    mkpath("1/1_1");
    mkfile("1/1_1/f1_1.1");
    mkpath("2/2_1");
    mkpath("2/2_2");
    mkfile("2/2_2/f2_2.1");
    mkfile("2/2_2/f2_2.2");
    mkfile("2/2_2/f2_2.3");
    mkfile("2/2_2/f2_2.4");
    mkpath("3/3_1");
    mkpath("4/4_1");
    mkfile("4/4_1/f4_1.1");
    fsf.set_search_text("1");
    processEvents(processEvent_repetitions);
    QSet<QString> expected = fsset;
    EXPECT_TRUE(expected.remove(hierarchy.absolutePath() + "/f.2"));
    EXPECT_TRUE(expected.remove(hierarchy.absolutePath() + "/2/2_2/f2_2.2"));
    EXPECT_TRUE(expected.remove(hierarchy.absolutePath() + "/2/2_2/f2_2.3"));
    EXPECT_TRUE(expected.remove(hierarchy.absolutePath() + "/2/2_2/f2_2.4"));
    compare_selection(expected);
    fsf.set_search_text("");
    processEvents();
    fsf.set_search_text("3");
    processEvents();
    expected.clear();
    expected.insert(hierarchy.absolutePath() + "/2");
    expected.insert(hierarchy.absolutePath() + "/2/2_2");
    expected.insert(hierarchy.absolutePath() + "/2/2_2/f2_2.3");
    expected.insert(hierarchy.absolutePath() + "/3");
    expected.insert(hierarchy.absolutePath() + "/3/3_1");
    compare_selection(expected);
}
