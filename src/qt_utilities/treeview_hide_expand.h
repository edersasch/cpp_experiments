#ifndef SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE
#define SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE

#include <QFileSystemModel>
#include <QSet>
#include <QSortFilterProxyModel>

class QTreeView;
class QRegularExpression;
class QModelIndex;
class QPersistentModelIndex;

namespace QtUtilities
{

class HideEmptyParentsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    HideEmptyParentsProxyModel(QObject* parent = nullptr);
    ~HideEmptyParentsProxyModel() override;

    void setHideEmptyDirs(bool hide);
    void setWhitelistedName(const QString& whitelistedName);
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
    bool mHideEmptyDirs {false};
    QString mWhitelistedName;
};

bool treeview_hide_expand(QTreeView* view, const QModelIndex& index, const QRegularExpression& pattern, bool hide_empty_parents, bool expand);

}

#endif // SRC_QT_UTILITIES_TREEVIEW_EXPAND_HIDE
