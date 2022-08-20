#ifndef SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
#define SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT

#include "tiddler_model_qt.h"
#include "fs_history_qt/fs_history.h"

#include <QWidget>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QListView>

#include <unordered_map>
#include <unordered_set>
#include <memory>

class QHBoxLayout;
class QVBoxLayout;
class QBoxLayout;
class QFormLayout;
class QToolButton;
class QMenu;
class QListView;
class QLineEdit;
class QAction;
class QGroupBox;
class QComboBox;

class Flow_List_View
        : public QListView
{
    Q_OBJECT

public:
    explicit Flow_List_View(QWidget* parent = nullptr);
    virtual ~Flow_List_View() override = default;
    void doItemsLayout() override;
protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
private:
    void delete_del_button();
    int hoverRow {-1};
    QToolButton* del_button {nullptr};
};

class Move_Only_StandardItemModel
        : public QStandardItemModel
{
public:
    Move_Only_StandardItemModel(const QStringList &strings, QObject *parent = nullptr);
    Move_Only_StandardItemModel(QObject* parent = nullptr);
    virtual ~Move_Only_StandardItemModel() override = default;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    Qt::DropActions supportedDropActions() const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
};

class Tiddlerstore_Handler
        : public QWidget
{
    Q_OBJECT

public:
    explicit Tiddlerstore_Handler(const QStringList& tiddlerstore_list = {}, QWidget* parent = nullptr);
    virtual ~Tiddlerstore_Handler() override;

    QStringList get_tiddlerstore_list() { return tiddlerstore_history.get_elements(); }

signals:
    void open_tiddler_model(Tiddler_Model*);

private:
    QHBoxLayout* setup_toolbar();
    void setup_load_button();
    QToolButton* setup_save_button();
    void setup_main_title_filter();
    void setup_filter();
    void add_single_group(Tiddlerstore::Store_Filter& filter);
    void add_negate_button(Tiddlerstore::Store_Filter& filter, std::size_t pos, QLayout* layout);
    QToolButton* add_label_del_row(const QString& text, QLayout* single_filter_functions, QFormLayout* filter_form_layout);
    QToolButton* add_title_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout);
    QToolButton* add_text_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout);
    QToolButton* add_tag_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout);
    QToolButton* add_field_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout);
    QToolButton* add_list_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout);
    void connect_model(Tiddler_Model* model);
    void apply_filter();
    void set_dirty() { adjust_dirty(true); }
    void adjust_dirty(bool dirty_value);
    void open_store(const QString& path);
    void save_store(const QString& path);
    void prepare_open(Tiddlerstore::Tiddler& t);

    Tiddlerstore::Store store {};
    Tiddlerstore_Model store_model {store};
    QStringListModel title_model {};
    FS_History tiddlerstore_history;
    QString currently_loaded_store;
    QToolButton* load_button;
    QMenu* load_safety_menu {nullptr};
    QMenu* load_history_menu;
    QVBoxLayout* filter_layout;
    QLineEdit* main_title_filter_edit {nullptr};
    QAction* main_title_filter_open_action {nullptr};
    QAction* main_title_filter_add_action {nullptr};
    QAction* main_title_filter_placeholder_action {nullptr};
    std::unordered_set<std::string> present_tags;
    std::unordered_set<std::string> present_fields;
    std::unordered_set<std::string> present_lists;
    bool is_dirty {false};
    std::unique_ptr<Tiddlerstore::Filter_Group> filter_group;
    std::unique_ptr<Tiddlerstore::Store_Filter> always_empty_filter;
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
