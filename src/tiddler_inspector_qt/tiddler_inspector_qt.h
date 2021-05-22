#ifndef SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
#define SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT

#include "tiddlerstore_qt/tiddler_model_qt.h"
#include "fs_history_qt/fs_history_ui.h"

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
    explicit Tiddler_Inspector(const QString& tiddlerstore_json = {}, QWidget* parent = nullptr);
    virtual ~Tiddler_Inspector() override = default;

    QString get_store();

private:
    void update_tiddler_list();
    void adjust_dirty(bool dirty_value);

    Tiddler_Pure_View* pure_view;
    Tiddler_Pure_Edit* pure_edit;
    std::vector<std::unique_ptr<Tiddler_Model>> tiddler_models;
    QStringListModel title_model {};
    bool is_dirty {false};
    FS_History_UI store_chooser;
    QToolButton* load_button;
    QMenu* load_safety_menu {nullptr};
    QMenu* load_history_menu {nullptr};
};

#endif // SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
