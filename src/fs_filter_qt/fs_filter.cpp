#include "fs_filter.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>

FS_Filter::FS_Filter(const QString& root_path, QWidget* parent)
    : QWidget(parent)
{
    auto main_layout = new QVBoxLayout(this);
    auto h = fs_view.header();

    search_text_edit.setClearButtonEnabled(true);
    auto idx = fs_model.setRootPath(root_path);
    fs_model.setNameFilterDisables(false);
    fs_view.setModel(&fs_model);
    fs_view.setRootIndex(idx);
    fs_view.setDragEnabled(true);
    fs_view.setExpandsOnDoubleClick(false);
    fs_view.setSelectionMode(QAbstractItemView::ExtendedSelection);
    h->setSectionResizeMode(QHeaderView::ResizeToContents);

    main_layout->addWidget(&search_text_edit);
    main_layout->addWidget(&fs_view);

    connect(&search_text_edit, &QLineEdit::textChanged, this, [this](const QString& text) {
        if (text.isEmpty()) {
            return fs_model.setNameFilters({});
        }
        fs_model.setNameFilters({"*" + text + "*"});
    });
    connect(&fs_model, &QFileSystemModel::layoutChanged, this, &FS_Filter::check);
    connect(&fs_model, &QFileSystemModel::directoryLoaded, this, [this](const QString& dir) {
        if (wait_for_dirs.remove(QPersistentModelIndex(fs_model.index(dir)))) {
            check();
        }
    });
    connect(&fs_view, &QTreeView::doubleClicked, this, [this](const QModelIndex& index) {
        auto url = QUrl::fromLocalFile(fs_model.filePath(index));
        QDesktopServices::openUrl(url);
    });
}

void FS_Filter::set_auto_expand(bool do_auto_expand)
{
    if (do_auto_expand != auto_expand) {
        auto_expand = do_auto_expand;
        expand_hide(fs_model.index(fs_model.rootPath()));
    }
}

void FS_Filter::set_hide_empty_dirs(bool do_hide)
{
    if (do_hide != hide_empty_dirs) {
        hide_empty_dirs = do_hide;
        expand_hide(fs_model.index(fs_model.rootPath()));
    }
}

// private

void FS_Filter::check()
{
    if (auto_expand || hide_empty_dirs) {
        expand_hide(fs_model.index(fs_model.rootPath()));
    }
}

bool FS_Filter::expand_hide(QModelIndex index)
{
    auto is_empty = !fs_model.canFetchMore(index);
    if (is_empty) {
        for (int i = 0; i < fs_model.rowCount(index); i += 1) {
            auto cidx = fs_model.index(i, 0, index);
            if (fs_model.isDir(cidx)) {
                wait_for_dirs.remove(QPersistentModelIndex(index));
                auto can_hide = expand_hide(cidx);
                if (can_hide) {
                    // an empty text is always found, so nothing gets hidden
                    can_hide &= !fs_model.fileName(cidx).contains(search_text_edit.text(), Qt::CaseInsensitive);
                }
                fs_view.setRowHidden(i, index, can_hide && hide_empty_dirs);
                is_empty &= can_hide;
            } else {
                is_empty = false;
            }
        }
    } else {
        wait_for_dirs.insert(QPersistentModelIndex(index));
    }
    if (!is_empty && auto_expand) {
        fs_view.expand(index);
    }
    return is_empty;
}
