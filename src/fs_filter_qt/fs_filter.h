#ifndef SRC_FS_FILTER_QT_FS_FILTER
#define SRC_FS_FILTER_QT_FS_FILTER

#include "qt_utilities/treeview_hide_expand.h"

#include <QWidget>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QTreeView>
#include <QRegularExpression>

class FS_Filter
        : public QWidget
{
    Q_OBJECT

public:
    FS_Filter(const QString& root_path, QWidget* parent = nullptr);
    virtual ~FS_Filter() override = default;

    void set_auto_expand(bool do_auto_expand);
    void set_hide_empty_dirs(bool do_hide);
    void set_focus() { search_text_edit.setFocus(); }
    QString get_search_text() const { return search_text_edit.text(); }
    void set_search_text(const QString& search_text) { search_text_edit.setText(search_text); }
    QTreeView& get_view() { return fs_view; }

private:
    void hide_expand() { Qt_Utilities::treeview_hide_expand(&fs_view, fs_model.index(fs_model.rootPath()), filter_pattern, hide_empty_dirs, auto_expand, &wait_for_dirs); }

    QFileSystemModel fs_model;
    QLineEdit search_text_edit;
    QTreeView fs_view;

    /**
      Prevent left over empty dirs

      If fs_model.canFetchMore(index) is true, but directory is empty, no fs_model.layoutChanged() is
      emitted after expand() and an empty dir could stay in view.
      */
    QSet<QPersistentModelIndex> wait_for_dirs {};
    bool auto_expand {false};
    bool hide_empty_dirs {false};
    QRegularExpression filter_pattern;
};

#endif // SRC_FS_FILTER_QT_FS_FILTER
