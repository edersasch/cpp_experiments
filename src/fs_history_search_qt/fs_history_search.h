#ifndef SRC_FS_HISTORY_SEARCH_QT_FS_HISTORY_SEARCH
#define SRC_FS_HISTORY_SEARCH_QT_FS_HISTORY_SEARCH

#include "fs_filter_qt/fs_filter.h"
#include "fs_history_qt/fs_history_ui.h"

#include <QWidget>

#include <memory>

class FsHistorySearch : public QWidget
{
    Q_OBJECT

public:
    struct FsHistorySearchConfig
    {
        QStringList recentDirs {};
        int historySize { FsHistory::defaultHistorySize };
    };
    static constexpr int mainStretchFactor {10};
    FsHistorySearch(const FsHistorySearchConfig& config, QWidget* parent = nullptr);
    virtual ~FsHistorySearch() override;

    QStringList getRecentDirs() { return mDirUi.get_elements(); }

private:
    FS_History_UI mDirUi;
    std::unique_ptr<FsFilter> mFsFilter { nullptr };
};

#endif // SRC_FS_HSEARCH_QT_FS_HSEARCH
