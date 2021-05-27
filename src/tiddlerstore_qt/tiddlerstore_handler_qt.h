#ifndef SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
#define SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT

#include "tiddlerstore/tiddlerstore.h"
#include "fs_history_qt/fs_history.h"

#include <QWidget>
#include <QStringListModel>
#include <QToolButton>

#include <unordered_map>
#include <memory>

class QMenu;
class Tiddler_Model;

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
    void update_title_model();
    void set_dirty() { adjust_dirty(true); }
    void adjust_dirty(bool dirty_value);
    void open_store(const QString& path);
    void save_store(const QString& path);
    void prepare_open(Tiddlerstore::Tiddler* t);

    Tiddlerstore::Store store {};
    QStringListModel title_model {};
    FS_History tiddlerstore_history;
    QToolButton* load_button;
    QMenu* load_safety_menu {nullptr};
    QMenu* load_history_menu;
    std::unordered_map<const Tiddlerstore::Tiddler*, std::unique_ptr<Tiddler_Model>> active_models;
    bool is_dirty {false};
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
