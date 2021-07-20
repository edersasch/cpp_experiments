#ifndef SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
#define SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT

#include "tiddler_model_qt.h"
#include "fs_history_qt/fs_history.h"

#include <QWidget>
#include <QStringListModel>
#include <QSortFilterProxyModel>
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
};

class Move_Only_StringListModel
        : public QStringListModel
{
public:
    Move_Only_StringListModel(const QStringList &strings, QObject *parent = nullptr);
    Move_Only_StringListModel(QObject* parent = nullptr);
    virtual ~Move_Only_StringListModel() override = default;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    Qt::DropActions supportedDropActions() const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                      int row, int column, const QModelIndex& parent) override;
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
    void setup_tiddler_list_view();
    void setup_main_title_filter();
    void setup_filter();
    void add_single_group(Tiddlerstore::Single_Group& single_group);
    void add_negate_button(Tiddlerstore::Filter_Data& filter_data, QLayout* layout);
    QToolButton* add_label_del_row(const QString& text, QLayout* single_filter_functions, QFormLayout* filter_form_layout);
    QToolButton* add_title_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout);
    QToolButton* add_text_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout);
    QToolButton* add_tag_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout);
    QToolButton* add_field_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout);
    void add_list_append_button(QListView* list_view, QStringListModel* list_model);
    QToolButton* add_list_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout);
    void connect_model(Tiddler_Model* model);
    void apply_filter();
    void set_dirty() { adjust_dirty(true); }
    void adjust_dirty(bool dirty_value);
    void open_store(const QString& path);
    void save_store(const QString& path);
    void prepare_open(Tiddlerstore::Tiddler* t);
    int source_row(int filter_row);

    Tiddlerstore::Store store {};
    Tiddlerstore_Model store_model {store};
    QStringListModel title_model {};
    QSortFilterProxyModel title_sort_model {};
    FS_History tiddlerstore_history;
    QListView* tiddler_list_view;
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
    Tiddlerstore::Filter_Groups filter_groups;
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
