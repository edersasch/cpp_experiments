#include "treeview_hide_expand.h"

#include <QTreeView>
#include <QRegularExpression>

namespace Qt_Utilities

{

bool treeview_hide_expand(QTreeView* view, const QModelIndex& index, const QRegularExpression& pattern, bool hide_empty_parents, bool expand, QSet<QPersistentModelIndex>* incomplete_parents)
{
    auto is_empty = !view->model()->canFetchMore(index);
    if (is_empty) {
        if (incomplete_parents) {
            incomplete_parents->remove(QPersistentModelIndex(index));
        }
        for (int i = 0; i < view->model()->rowCount(index); i += 1) {
            auto cidx = view->model()->index(i, 0, index);
            bool can_hide = !pattern.pattern().isEmpty(); // skip contains(pattern) if nothing will be hidden
            bool do_hide = true;
            if (view->model()->hasChildren(cidx)) {
                do_hide = hide_empty_parents;
                can_hide &= treeview_hide_expand(view, cidx, pattern, hide_empty_parents, expand, incomplete_parents);
            }
            if (can_hide) {
                can_hide = !view->model()->data(cidx).toString().contains(pattern);
            }
            if ((can_hide && do_hide) != view->isRowHidden(i, index)) { // checking for change is faster than always calling setRowHidden()
                view->setRowHidden(i, index, can_hide && do_hide);
            }
            is_empty &= can_hide;
        }
    } else {
        if (incomplete_parents) {
            incomplete_parents->insert(QPersistentModelIndex(index));
        }
    }
    if (!is_empty && expand) {
        view->expand(index);
    }
    return is_empty;
}

}
