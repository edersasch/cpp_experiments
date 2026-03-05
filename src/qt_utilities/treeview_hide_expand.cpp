#include "treeview_hide_expand.h"

#include <QRegularExpression>
#include <QTreeView>

namespace QtUtilities
{

HideEmptyParentsProxyModel::HideEmptyParentsProxyModel(QObject* parent)
: QSortFilterProxyModel(parent)
{
}

HideEmptyParentsProxyModel::~HideEmptyParentsProxyModel() = default;

void HideEmptyParentsProxyModel::setHideEmptyDirs(bool hide)
{
    if (hide != mHideEmptyDirs) {
        mHideEmptyDirs = hide;
        invalidateFilter();
    }
}

void HideEmptyParentsProxyModel::setWhitelistedName(const QString& whitelistedName)
{
    if (whitelistedName != mWhitelistedName) {
        mWhitelistedName = whitelistedName;
        invalidateFilter();
    }
}

bool HideEmptyParentsProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (!mHideEmptyDirs) {
        return true;
    }
    const auto* fsModel = qobject_cast<const QFileSystemModel*>(sourceModel());
    if (fsModel != nullptr) {
        auto idx = fsModel->index(source_row, 0, source_parent);
        if (!fsModel->isDir(idx) ||
            idx == fsModel->index(fsModel->rootPath()) ||
            fsModel->canFetchMore(idx) ||
            fsModel->fileName(idx).contains(mWhitelistedName, Qt::CaseInsensitive)) {
            return true;
        }
        for (int i = 0; i < fsModel->rowCount(idx); i += 1) {
            if (filterAcceptsRow(i, idx)) {
                return true;
            }
        }
        return false;
    }
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool treeview_hide_expand(
    QTreeView* view, const QModelIndex& index, const QRegularExpression& pattern, bool hide_empty_parents, bool expand)
{
    auto is_empty = !view->model()->canFetchMore(index);
    if (is_empty) {
        for (int i = 0; i < view->model()->rowCount(index); i += 1) {
            auto cidx = view->model()->index(i, 0, index);
            bool can_hide = true;
            bool do_hide = true;
            if (view->model()->hasChildren(cidx)) {
                do_hide = hide_empty_parents;
                can_hide &= treeview_hide_expand(view, cidx, pattern, hide_empty_parents, expand);
            }
            if (can_hide) {
                can_hide = !pattern.pattern().isEmpty() && !view->model()->data(cidx).toString().contains(pattern);
            }
            if ((can_hide && do_hide)
                != view->isRowHidden(i, index)) { // checking for change is faster than always calling setRowHidden()
                view->setRowHidden(i, index, can_hide && do_hide);
            }
            is_empty &= can_hide;
        }
    }
    return is_empty;
}
}
