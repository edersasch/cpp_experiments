#ifndef SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE
#define SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE

#include <QSet>

class QTreeView;
class QRegularExpression;
class QModelIndex;
class QPersistentModelIndex;

namespace Qt_Utilities
{

bool treeview_hide_expand(QTreeView* view, const QModelIndex& index, const QRegularExpression& pattern, bool hide_empty_parents, bool expand, QSet<QPersistentModelIndex>* incomplete_parents = nullptr);

}

#endif // SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE
