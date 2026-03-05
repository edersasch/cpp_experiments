#ifndef SRC_FS_FILTER_QT_FS_FILTER
#define SRC_FS_FILTER_QT_FS_FILTER

#include "qt_utilities/caching_file_icon_provider.hpp"
#include "qt_utilities/treeview_hide_expand.h"

#include <QWidget>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QTimer>
#include <QTreeView>
#include <QRegularExpression>

class FsFilter
        : public QWidget
{
    Q_OBJECT

public:
    FsFilter(const QString& rootPath, QWidget* parent = nullptr);
    virtual ~FsFilter() override = default;

    void set_auto_expand(bool do_auto_expand);
    void set_hide_empty_dirs(bool do_hide);
    void set_focus() { mSearchTextEdit.setFocus(); }
    QString get_search_text() const { return mSearchTextEdit.text(); }
    void set_search_text(const QString& search_text) { mSearchTextEdit.setText(search_text); }
    QTreeView& get_view() { return mFsView; }

private:
    void loadPath();
    void hide_expand(const QModelIndex& index);
    void updateUi();

    QFileSystemModel mFsModel;
    QtUtilities::HideEmptyParentsProxyModel mFsFilterModel;
    QLineEdit mSearchTextEdit;
    QTreeView mFsView;
    bool mAutoExpand {false};
    QRegularExpression mFilterPattern;
    QtUtilities::CachingFileIconProvider mIconProvider;
    QTimer mUiDelayTimer;
};

#endif // SRC_FS_FILTER_QT_FS_FILTER
