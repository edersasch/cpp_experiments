#ifndef SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
#define SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT

#include "tiddlerstore/tiddlerstore.h"
#include "tiddler_model_qt.h"
#include "fs_history_qt/fs_history.h"

#include <QWidget>
#include <QStringListModel>
#include <QSortFilterProxyModel>

#include <unordered_map>
#include <memory>

class QHBoxLayout;
class QToolButton;
class QMenu;
class QListView;
class QLineEdit;
class QAction;

class Tiddlerstore_Handler
        : public QWidget
{
    Q_OBJECT

public:
    explicit Tiddlerstore_Handler(const QStringList& tiddlerstore_list = {}, QWidget* parent = nullptr);
    virtual ~Tiddlerstore_Handler() = default;

    QStringList get_tiddlerstore_list() { return tiddlerstore_history.get_elements(); }

signals:
    void open_tiddler_model(Tiddler_Model*);

private:
    QHBoxLayout* setup_toolbar();
    void setup_load_button();
    QToolButton* setup_save_button();
    void setup_tiddler_list_view();
    QLineEdit* setup_main_title_filter();
    void update_title_model();
    void apply_filter();
    void set_dirty() { adjust_dirty(true); }
    void adjust_dirty(bool dirty_value);
    void open_store(const QString& path);
    void save_store(const QString& path);
    void prepare_open(Tiddlerstore::Tiddler* t);
    int source_row(int filter_row);

    static constexpr auto main_filter_title_field_name = "main_title";
    Tiddlerstore::Store store {};
    Tiddlerstore::Store filters {}; // TODO: use
    QStringListModel title_model {};
    QSortFilterProxyModel title_filter_model {};
    FS_History tiddlerstore_history;
    QListView* tiddler_list_view;
    QToolButton* load_button;
    QMenu* load_safety_menu {nullptr};
    QMenu* load_history_menu;
    QAction* main_title_filter_open_action {nullptr};
    QAction* main_title_filter_add_action {nullptr};
    QAction* main_title_filter_placeholder_action {nullptr};
    std::unordered_map<const Tiddlerstore::Tiddler*, std::unique_ptr<Tiddler_Model>> active_models;
    std::unordered_map<const Tiddlerstore::Tiddler*, std::unique_ptr<Tiddler_Model>> filter_models; // TODO: use
    Tiddlerstore::Tiddler main_filter;
    Tiddler_Model main_filter_model {&main_filter};
    bool is_dirty {false};
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
