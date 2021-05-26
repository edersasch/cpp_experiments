#ifndef SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
#define SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT

#include "tiddlerstore_qt/tiddler_model_qt.h"
#include "tiddlerstore/tiddlerstore.h"
#include "fs_history_qt/fs_history.h"

#include <QWidget>
#include <QStringListModel>

#include <vector>
#include <memory>

class Tiddler_Pure_View;
class Tiddler_Pure_Edit;
class QToolButton;
class QMenu;

class Tiddler_Inspector
        : public QWidget
{
    Q_OBJECT

public:
    explicit Tiddler_Inspector(const QStringList& tiddlerstore_list = {}, QWidget* parent = nullptr);
    virtual ~Tiddler_Inspector() override = default;

    QStringList get_tiddlerstore_list() { return tiddlerstore_history.get_elements(); }

private:
    void update_tiddler_list();
    void adjust_dirty(bool dirty_value);
    void open_store(const QString& path);
    void save_store(const QString& path);

    Tiddler_Pure_View* pure_view;
    Tiddler_Pure_Edit* pure_edit;
    Tiddlerstore::Store store {};
    std::unique_ptr<Tiddler_Model> tiddler_model{};
    QStringListModel title_model {};
    FS_History tiddlerstore_history;
    QToolButton* load_button;
    QMenu* load_safety_menu {nullptr};
    QMenu* load_history_menu;
    bool is_dirty {false};
};

#endif // SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
