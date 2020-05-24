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
    fs_view.setModel(&fs_model);
    fs_view.setRootIndex(idx);
    fs_view.setDragEnabled(true);
    fs_view.setExpandsOnDoubleClick(false);
    fs_view.setSelectionMode(QAbstractItemView::ExtendedSelection);
    filter_pattern.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    h->setSectionResizeMode(QHeaderView::ResizeToContents);

    main_layout->addWidget(&search_text_edit);
    main_layout->addWidget(&fs_view);

    connect(&search_text_edit, &QLineEdit::textChanged, this, [this] {
        if (search_text_edit.text().isEmpty()) {
            filter_pattern.setPattern(QString());
        } else {
            filter_pattern.setPattern(QRegularExpression::wildcardToRegularExpression("*" + search_text_edit.text() + "*"));
        }
        hide_expand();
    });
    connect(&fs_model, &QFileSystemModel::layoutChanged, this, &FS_Filter::hide_expand);
    connect(&fs_model, &QFileSystemModel::directoryLoaded, this, [this](const QString& dir) {
        if (wait_for_dirs.remove(QPersistentModelIndex(fs_model.index(dir)))) {
            hide_expand();
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
        hide_expand();
    }
}

void FS_Filter::set_hide_empty_dirs(bool do_hide)
{
    if (do_hide != hide_empty_dirs) {
        hide_empty_dirs = do_hide;
        hide_expand();
    }
}
