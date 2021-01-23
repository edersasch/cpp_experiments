#ifndef SRC_TIDDLERSTORE_UI_QT_TIDDLERSTORE_UI_QT
#define SRC_TIDDLERSTORE_UI_QT_TIDDLERSTORE_UI_QT

#include "tiddlerstore/tiddlerstore.h"

#include <QWidget>

class QHBoxLayout;
class QVBoxLayout;
class QBoxLayout;
class QToolButton;

class Tiddlerstore_ui
        : public QWidget
{
    Q_OBJECT

public:
    Tiddlerstore_ui(const QString& tiddlerstore_json = {}, QWidget* parent = nullptr);
    virtual ~Tiddlerstore_ui() override = default;

    QString get_store();
};

class Text_Tiddler_ui
        : public QWidget
{
    Q_OBJECT

public:
    Text_Tiddler_ui(QWidget* parent = nullptr);
    virtual ~Text_Tiddler_ui() override = default;

    Tiddlerstore::Tiddler tiddler() const;
    void set_tiddler(const Tiddlerstore::Tiddler& new_tiddler);

private:
    void update_present_tags();
    void update_present_fields();
    void update_present_lists();
    void update_present_list(const std::string& list_name, QBoxLayout* parent_layout);
    QToolButton* deletable_value(const std::string& text, QBoxLayout* parent_layout);

    static constexpr int tags_stretch_factor = 2;
    static constexpr int field_stretch_factor = tags_stretch_factor;
    static constexpr int list_stretch_factor = field_stretch_factor;

    Tiddlerstore::Tiddler t;
    QHBoxLayout* present_tags_layout;
    QVBoxLayout* present_fields_layout;
    QVBoxLayout* present_lists_layout;
};

#endif // SRC_TIDDLERSTORE_UI_QT_TIDDLERSTORE_UI_QT
