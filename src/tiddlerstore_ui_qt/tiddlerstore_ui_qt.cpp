#include "tiddlerstore_ui_qt.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QStyle>

namespace
{

void clear_layout(QLayout* l)
{
    QLayoutItem* child;
    while ((child = l->takeAt(0) ) != nullptr) {
        delete child->widget();
        auto cl = child->layout();
        if (cl) {
            clear_layout(cl);
        }
        delete child;
    }
}

}

Tiddlerstore_ui::Tiddlerstore_ui(const QString& tiddlerstore_json, QWidget* parent)
    : QWidget(parent)
{
    auto main_layout(new QVBoxLayout(this));
    auto text_tiddler_ui = new Text_Tiddler_ui(this);
    main_layout->addWidget(text_tiddler_ui);
    main_layout->addStretch();
    if (!tiddlerstore_json.isEmpty()) {
        //
    }
}

QString Tiddlerstore_ui::get_store()
{
    return {};
}

Text_Tiddler_ui::Text_Tiddler_ui(QWidget* parent)
    : QWidget(parent)
    , present_tags_layout(new QHBoxLayout)
    , present_fields_layout(new QVBoxLayout)
    , present_lists_layout(new QVBoxLayout)
{
    auto main_layout(new QVBoxLayout(this));
    auto tags_layout(new QHBoxLayout);
    auto add_field_layout(new QHBoxLayout);
    auto add_list_layout(new QHBoxLayout);

    auto title_edit(new QLineEdit);
    auto text_edit(new QTextEdit);
    auto tags_edit(new QLineEdit);
    auto tags_add(new QToolButton);
    auto field_name_edit(new QLineEdit);
    auto field_value_edit(new QLineEdit);
    auto field_add(new QToolButton);
    auto list_name_edit(new QLineEdit);
    auto list_value_edit(new QLineEdit);
    auto list_add(new QToolButton);

    title_edit->setPlaceholderText("Title");
    text_edit->setPlaceholderText("Text content");
    tags_edit->setPlaceholderText("Tag name");
    field_name_edit->setPlaceholderText("Field name");
    field_value_edit->setPlaceholderText("Field value");
    list_name_edit->setPlaceholderText("List name");
    list_value_edit->setPlaceholderText("First list value");
    tags_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    field_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    list_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));

    tags_layout->addLayout(present_tags_layout);
    tags_layout->addWidget(tags_add);
    tags_layout->addWidget(tags_edit);
    tags_layout->addStretch(tags_stretch_factor);
    add_field_layout->addWidget(field_add);
    add_field_layout->addWidget(field_name_edit);
    add_field_layout->addWidget(field_value_edit);
    add_field_layout->addStretch(field_stretch_factor);
    add_list_layout->addWidget(list_add);
    add_list_layout->addWidget(list_name_edit);
    add_list_layout->addWidget(list_value_edit);
    add_list_layout->addStretch(list_stretch_factor);
    main_layout->addWidget(title_edit);
    main_layout->addWidget(text_edit);
    main_layout->addLayout(tags_layout);
    main_layout->addLayout(present_fields_layout);
    main_layout->addLayout(add_field_layout);
    main_layout->addLayout(present_lists_layout);
    main_layout->addLayout(add_list_layout);

    connect(tags_add, &QToolButton::clicked, this, [this, tags_edit] {
        t.set_tag(tags_edit->text().toStdString());
        tags_edit->clear();
        update_present_tags();
    });
    connect(tags_edit, &QLineEdit::returnPressed, tags_add, &QToolButton::click);
    connect(field_add, &QToolButton::clicked, this, [this, field_name_edit, field_value_edit] {
        if (!field_name_edit->text().isEmpty() && !field_value_edit->text().isEmpty()) {
            t.set_field(field_name_edit->text().toStdString(), field_value_edit->text().toStdString());
            field_name_edit->clear();
            field_value_edit->clear();
            update_present_fields();
        }
    });
    connect(field_name_edit, &QLineEdit::returnPressed, field_add, &QToolButton::click);
    connect(field_value_edit, &QLineEdit::returnPressed, field_add, &QToolButton::click);
    connect(list_add, &QToolButton::clicked, this, [this, list_name_edit, list_value_edit] {
        if (!list_name_edit->text().isEmpty() && !list_value_edit->text().isEmpty()) {
            t.set_list(list_name_edit->text().toStdString(), {list_value_edit->text().toStdString()});
            list_name_edit->clear();
            list_value_edit->clear();
        update_present_lists();
        }
    });
    connect(list_name_edit, &QLineEdit::returnPressed, list_add, &QToolButton::click);
    connect(list_value_edit, &QLineEdit::returnPressed, list_add, &QToolButton::click);
}

Tiddlerstore::Tiddler Text_Tiddler_ui::tiddler() const
{
    return t;
}

void Text_Tiddler_ui::set_tiddler(const Tiddlerstore::Tiddler &new_tiddler)
{
    t = new_tiddler;
    update_present_tags();
    update_present_fields();
    update_present_lists();
}

// private

void Text_Tiddler_ui::update_present_tags()
{
    clear_layout(present_tags_layout);
    for (const auto& tag : t.tags()) {
        connect(deletable_value(tag.c_str(), present_tags_layout), &QToolButton::clicked, this, [this, tag] {
            t.remove_tag(tag);
            update_present_tags();
        });
    }
}

void Text_Tiddler_ui::update_present_fields()
{
    clear_layout(present_fields_layout);
    for (const auto& field : t.fields()) {
        auto l = new QHBoxLayout;
        auto del = new QToolButton;
        auto fieldnamelabel = new QLabel(QString(field.first.c_str()) + ": ");
        auto fieldvaluelabel = new QLabel(QString(field.second.c_str()));
        del->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
        l->addWidget(del);
        l->addWidget(fieldnamelabel);
        l->addWidget(fieldvaluelabel, field_stretch_factor);
        present_fields_layout->addLayout(l);
        connect(del, &QToolButton::clicked, this, [this, field] {
            t.remove_field(field.first);
            update_present_fields();
        });
    }
}

void Text_Tiddler_ui::update_present_lists()
{
    clear_layout(present_lists_layout);
    for (const auto& list : t.lists()) {
        auto l = new QHBoxLayout;
        auto present_values_layout = new QHBoxLayout;
        auto del = new QToolButton;
        auto listnamelabel = new QLabel(QString(list.first.c_str()) + ": ");
        auto listvalues = list.second;
        auto ladd = new QToolButton;
        auto lval = new QLineEdit;
        del->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
        ladd->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
        lval->setPlaceholderText("add ...");
        l->addWidget(del);
        l->addWidget(listnamelabel);
        l->addLayout(present_values_layout);
        l->addWidget(ladd);
        l->addWidget(lval);
        l->addStretch(list_stretch_factor);
        update_present_list(list.first, present_values_layout);
        present_lists_layout->addLayout(l);
        connect(del, &QToolButton::clicked, this, [this, list] {
            t.remove_list(list.first);
            update_present_lists();
        });
        connect(ladd, &QToolButton::clicked, this, [this, list, lval, present_values_layout] {
            if (!lval->text().isEmpty()) {
                auto values = t.list(list.first);
                values.push_back(lval->text().toStdString());
                lval->clear();
                t.set_list(list.first, values);
                update_present_list(list.first, present_values_layout);
            }
        });
        connect(lval, &QLineEdit::returnPressed, ladd, &QToolButton::click);
    }
}

void Text_Tiddler_ui::update_present_list(const std::string &list_name, QBoxLayout* parent_layout)
{
    auto listvalues = t.list(list_name);
    clear_layout(parent_layout);
    if (listvalues.empty()) {
        update_present_lists();
    }
    for (const auto& lv : listvalues) {
        connect(deletable_value(lv, parent_layout), &QToolButton::clicked, this, [this, list_name, listvalues, lv, parent_layout] {
            auto values = t.list(list_name);
            auto it = std::find(values.begin(), values.end(), lv);
            if (it != values.end()) {
                values.erase(it);
                t.set_list(list_name, values);
                update_present_list(list_name, parent_layout);
            }
        });
    }
}

QToolButton* Text_Tiddler_ui::deletable_value(const std::string& text, QBoxLayout* parent_layout)
{
    auto l = new QHBoxLayout;
    auto del = new QToolButton;
    auto label = new QLabel(text.c_str());
    l->setSpacing(0);
    del->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setLineWidth(1);
    l->addWidget(del);
    l->addWidget(label);
    parent_layout->addLayout(l);
    return del;
}
