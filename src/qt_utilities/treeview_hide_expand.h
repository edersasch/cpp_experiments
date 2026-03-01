#ifndef SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE
#define SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE

#include <QFileSystemModel>
#include <QSet>
#include <QSortFilterProxyModel>

class QTreeView;
class QRegularExpression;
class QModelIndex;
class QPersistentModelIndex;

namespace Qt_Utilities
{

class HideEmptyParentsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    HideEmptyParentsProxyModel(QObject* parent = nullptr)
    : QSortFilterProxyModel(parent)
    {
    }

    ~HideEmptyParentsProxyModel() override = default;

    void setHideEmptyDirs(bool hide) { mHideEmptyDirs = hide; }
    void setNameFilter(const QString& filter) { mFilter = filter; }
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override
    {
        if (!mHideEmptyDirs) {
            return true;
        }
        auto* fsModel = qobject_cast<const QFileSystemModel*>(sourceModel());
        if (fsModel) {
            auto idx = fsModel->index(source_row, 0, source_parent);
            if (!fsModel->isDir(idx) ||
                idx == fsModel->index(fsModel->rootPath()) ||
                fsModel->canFetchMore(idx) ||
                fsModel->fileName(idx).contains(mFilter, Qt::CaseInsensitive)) {
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

private:
    bool mHideEmptyDirs {false};
    QString mFilter;
};

bool treeview_hide_expand(QTreeView* view, const QModelIndex& index, const QRegularExpression& pattern, bool hide_empty_parents, bool expand, QSet<QPersistentModelIndex>* incomplete_parents = nullptr);

}

#endif // SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE
