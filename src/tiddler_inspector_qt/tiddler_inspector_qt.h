#ifndef SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
#define SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT

#include <QWidget>
#include <QStringListModel>

#include <unordered_map>

class Tiddler_Model;
class FlowLayout;
class QHBoxLayout;
class QVBoxLayout;
class QBoxLayout;
class QToolButton;
class QLineEdit;

class NoDnDOverwriteStringListModel
        : public QStringListModel
{
    Q_OBJECT

public:
    NoDnDOverwriteStringListModel(QObject* parent = nullptr);
    virtual ~NoDnDOverwriteStringListModel() override = default;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
};

class Tiddler_Inspector
        : public QWidget
{
    Q_OBJECT

public:
    Tiddler_Inspector(const QString& tiddlerstore_json = {}, QWidget* parent = nullptr);
    virtual ~Tiddler_Inspector() override = default;

    QString get_store();
};

class Text_Tiddler_ui
        : public QWidget
{
    Q_OBJECT

public:
    Text_Tiddler_ui(QWidget* parent = nullptr);
    virtual ~Text_Tiddler_ui() override = default;

    void set_tiddler_model(Tiddler_Model* new_tiddler_model);

private:
    void update_present_tags();
    void update_present_fields();
    void update_present_lists();
    void update_present_list(const std::string& list_name);
    QToolButton* deletable_value(const std::string& text, FlowLayout* parent_layout);

    static constexpr int field_stretch_factor = 2;
    static constexpr int list_stretch_factor = field_stretch_factor;

    Tiddler_Model* tm {nullptr};
    QToolButton* tag_add;
    QLineEdit* tag_edit;
    FlowLayout* tags_layout;
    QLineEdit* field_name_edit;
    QLineEdit* field_value_edit;
    QVBoxLayout* present_fields_layout;
    QLineEdit* list_name_edit;
    QLineEdit* list_value_edit;
    QVBoxLayout* present_lists_layout;
    struct Single_List_Element
    {
        FlowLayout* l {nullptr};
        QToolButton* add {nullptr};
        QLineEdit* val {nullptr};
    };
    std::unordered_map<std::string, Single_List_Element> lists_ui;
};

#endif // SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
