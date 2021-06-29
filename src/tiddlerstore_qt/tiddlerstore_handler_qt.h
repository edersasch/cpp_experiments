#ifndef SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
#define SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT

#include "tiddlerstore/tiddlerstore.h"
#include "tiddler_model_qt.h"
#include "fs_history_qt/fs_history.h"

#include <QWidget>
#include <QStringListModel>
#include <QSortFilterProxyModel>

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
    void store_changed();

private:
    enum Filter_Type
    {
        Title,
        Text,
        Tag,
        Field,
        List
    };
    struct Filter_Data
    {
        bool negate {false};
        std::string key {}; // title, text, tag, field name, list name
        std::string field_value {};
        std::vector<std::string> list_value {};
    };
    using Single_Group = std::unordered_map<Filter_Type, Filter_Data>;
    using Filter_Groups = std::vector<std::unique_ptr<Single_Group>>;

    QHBoxLayout* setup_toolbar();
    void setup_load_button();
    QToolButton* setup_save_button();
    void setup_tiddler_list_view();
    void setup_main_title_filter();
    QStringList next_filter_options(const QStringList& dont_list);
    QComboBox* select_next_filter_combobox(const QStringList& dont_list);
    void setup_filter();
    void add_single_group(Single_Group& single_group);
    void add_title_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout);
    void add_text_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout);
    void add_tag_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout);
    void add_field_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout);
    void add_list_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout);
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
    Filter_Groups filter_groups;
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLERSTORE_HANDLER_QT
