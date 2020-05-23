#include "ar_history_search.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QToolButton>
#include <QTreeView>
#include <QStandardItemModel>

#include <QDebug>

AR_History_Search::AR_History_Search(const QStringList& start_archives, int history_size, QWidget* parent)
    : QWidget(parent)
    , ar_ui(history_size, start_archives)
{
    auto main_layout(new QVBoxLayout(this));
    auto dir_layout = new QHBoxLayout;
    ar_ui.combobox()->setStyle(new Combobox_Label_Elide_Left_Proxy_Style);
    search_text_edit.setClearButtonEnabled(true);
    dir_tree.setDragEnabled(true);
    dir_tree.setExpandsOnDoubleClick(false);
    dir_tree.setSelectionMode(QAbstractItemView::ExtendedSelection);
    dir_tree.setHeaderHidden(true);
    dir_tree.setModel(&arhelper.get_archive_directory_model());
    dir_layout->addWidget(ar_ui.combobox());
    dir_layout->addWidget(ar_ui.browse_button("...", tr("Please Select Archive"), tr("Archives (*.zip *.7z)")));
    main_layout->addLayout(dir_layout);
    main_layout->addWidget(&search_text_edit);
    main_layout->addWidget(&dir_tree, main_stretch_factor);
    main_layout->addStretch();

    connect(&search_text_edit, &QLineEdit::textChanged, this, [this] {
        expand_hide(dir_tree.model()->index(0, 0));
    });
    connect(dir_tree.model(), &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex& index) {
        expand_hide(index);
    });
    connect(&ar_ui, &FS_History_UI::current_element_changed, this, [this](const QString& archive_path) {
        arhelper.ls(archive_path);
        search_text_edit.setFocus();
    });
}

bool AR_History_Search::expand_hide(QModelIndex index)
{
    auto is_empty = !dir_tree.model()->canFetchMore(index);
    if (is_empty) {
        for (int i = 0; i < dir_tree.model()->rowCount(index); i += 1) {
            auto cidx = dir_tree.model()->index(i, 0, index);
            if (dir_tree.model()->hasChildren(cidx)) {
                auto can_hide = expand_hide(cidx);
                if (can_hide) {
                    // an empty text is always found, so nothing gets hidden
                    can_hide &= !dir_tree.model()->data(cidx).toString().contains(search_text_edit.text(), Qt::CaseInsensitive);
                }
                dir_tree.setRowHidden(i, index, can_hide);
                is_empty &= can_hide;
            } else {
                auto do_hide = !dir_tree.model()->data(cidx).toString().contains(search_text_edit.text(), Qt::CaseInsensitive);
                dir_tree.setRowHidden(i, index, do_hide);
                is_empty &= do_hide;
            }
        }
    }
    if (!is_empty && auto_expand) {
        dir_tree.expand(index);
    }
    return is_empty;
}
