#ifndef SRC_FS_HISTORY_SEARCH_QT_FS_HISTORY_SEARCH
#define SRC_FS_HISTORY_SEARCH_QT_FS_HISTORY_SEARCH

#include "fs_history_qt/fs_history_ui.h"
#include "fs_filter_qt/fs_filter.h"

#include <QWidget>

#include <memory>

class QVBoxLayout;

class FS_History_Search
        : public QWidget
{
    Q_OBJECT

public:
    static constexpr int main_stretch_factor {10};

    FS_History_Search(const QStringList& start_dirs = {}, int history_size = FS_History::default_history_size, QWidget* parent = nullptr);
    virtual ~FS_History_Search() override = default;

    QStringList get_dirs() { return dir_ui.get_elements(); }

private:
    FS_History_UI dir_ui;
    std::unique_ptr<FS_Filter> fs_filter {nullptr};
};

#endif // SRC_FS_HSEARCH_QT_FS_HSEARCH
