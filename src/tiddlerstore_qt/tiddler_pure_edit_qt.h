#ifndef SRC_TIDDLERSTORE_QT_TIDDLER_PURE_EDIT_QT
#define SRC_TIDDLERSTORE_QT_TIDDLER_PURE_EDIT_QT

#include "tiddlerstore/tiddlerstore.h"
#include "tiddler_model_qt.h"

#include <QWidget>

#include <unordered_map>

class Tiddler_Model;
class FlowLayout;
class QLineEdit;
class QTextEdit;
class QToolButton;
class QMenu;
class QVBoxLayout;

class Tiddler_Pure_Edit
        : public QWidget
{
    Q_OBJECT

signals:
    void accept_edit();
    void discard_edit();
    void unsaved_edit(bool);

public:
    explicit Tiddler_Pure_Edit(QWidget* parent = nullptr);
    virtual ~Tiddler_Pure_Edit() override = default;

    Tiddler_Model* tiddler_model();
    void set_tiddler_model(Tiddler_Model* model);

protected:
    void showEvent(QShowEvent* event) override;

private:
    static constexpr int field_stretch_factor = 2;
    static constexpr int list_stretch_factor = field_stretch_factor;

    /// if tm is valid and its content differs from work_tm content, the discard_button gets a menu so you have to click twice
    void update_dirty();

    void update_present_tags();
    void update_present_fields();
    void update_present_lists();
    void update_present_list(const std::string& list_name);
    QToolButton* deletable_value(const std::string& text, FlowLayout* parent_layout);

    /// accept_button will only be shown if the model tm is valid
    void present_accept_button();

    Tiddler_Model* tm {nullptr};
    Tiddlerstore::Tiddler work;
    Tiddler_Model work_tm {work};
    QLineEdit* title_lineedit;
    QTextEdit* text_edit;
    QToolButton* accept_button;
    QToolButton* discard_button;
    QMenu* discard_menu;
    QLineEdit* tag_lineedit;
    FlowLayout* tags_layout;
    QLineEdit* field_name_lineedit;
    QLineEdit* field_value_lineedit;
    QVBoxLayout* present_fields_layout;
    QLineEdit* list_name_lineedit;
    QLineEdit* list_value_lineedit;
    QVBoxLayout* present_lists_layout;
    struct Single_List_Element
    {
        FlowLayout* l {nullptr};
        QLineEdit* val {nullptr};
    };
    std::unordered_map<std::string, Single_List_Element> single_list_elements;
    bool current_dirty {false};
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLER_PURE_EDIT_QT
