#ifndef SRC_AR_HISTORY_SEARCH_QT_AR_HISTORY_SEARCH
#define SRC_AR_HISTORY_SEARCH_QT_AR_HISTORY_SEARCH

#include "fs_history_qt/fs_history_ui.h"
#include "arhelper_qt/arhelper.h"

#include <QWidget>
#include <QLineEdit>
#include <QTreeView>

#include <memory>

class QVBoxLayout;

class AR_History_Search
        : public QWidget
{
    Q_OBJECT

public:
    static constexpr int main_stretch_factor {10};

    AR_History_Search(const QStringList& start_archives = {}, int history_size = FS_History::default_history_size, QWidget* parent = nullptr);
    virtual ~AR_History_Search() override = default;

    QStringList get_archives() { return ar_ui.get_elements(); }

private:
    bool expand_hide(QModelIndex index);

    FS_History_UI ar_ui;
    Arhelper arhelper;
    QLineEdit search_text_edit;
    QTreeView dir_tree;
    bool auto_expand {true};
};

#endif // SRC_FS_HSEARCH_QT_FS_HSEARCH
