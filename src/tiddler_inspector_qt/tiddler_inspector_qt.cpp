#include "tiddler_inspector_qt.h"
#include "tiddlerstore_qt/tiddler_model_qt.h"
#include "qt_utilities/flowlayout.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QStyle>
#include <QGroupBox>
#include <QScrollArea>
#include <QListView>
#include <QMenu>
#include <QStackedLayout>
#include <QTextBrowser>

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

Tiddler_Inspector::Tiddler_Inspector(const QString& tiddlerstore_json, QWidget* parent)
    : QWidget(parent)
{
    auto main_layout(new QVBoxLayout(this));
    auto text_tiddler_ui = new Text_Tiddler_ui(this);
    auto tiddler_scroll = new QScrollArea(this);
    auto bastelmodel = new Tiddler_Model(this);
    tiddler_scroll->setWidgetResizable(true);
    tiddler_scroll->setWidget(text_tiddler_ui);
    main_layout->addWidget(tiddler_scroll);
    text_tiddler_ui->set_tiddler_model(bastelmodel);
    if (!tiddlerstore_json.isEmpty()) {
        //
    }
}

QString Tiddler_Inspector::get_store()
{
    return {};
}

Text_Tiddler_ui::Text_Tiddler_ui(QWidget* parent)
    : QWidget(parent)
    , edit_tag_add(new QToolButton)
    , edit_tag_lineedit(new QLineEdit)
    , edit_tags_layout(new FlowLayout)
    , edit_field_name_lineedit(new QLineEdit)
    , edit_field_value_lineedit(new QLineEdit)
    , present_fields_layout(new QVBoxLayout)
    , edit_list_name_lineedit(new QLineEdit)
    , edit_list_value_lineedit(new QLineEdit)
    , present_lists_layout(new QVBoxLayout)
{
    auto view_widget = new QWidget;
    auto view_layout = new QVBoxLayout(view_widget);

    auto view_title_layout(new QHBoxLayout);
    auto view_title_label(new QLabel);
    view_title_label->setText("Title");
    view_title_layout->addWidget(view_title_label);
    auto start_edit_button(new QToolButton);
    start_edit_button->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    view_title_layout->addWidget(start_edit_button);
    view_layout->addLayout(view_title_layout);

    auto view_text_browser(new QTextBrowser);
    view_layout->addWidget(view_text_browser);

    auto edit_widget(new QWidget);
    auto edit_layout(new QVBoxLayout(edit_widget));

    auto edit_title_layout(new QHBoxLayout);
    auto edit_title_lineedit(new QLineEdit);
    edit_title_lineedit->setPlaceholderText("Title");
    edit_title_layout->addWidget(edit_title_lineedit);
    auto edit_confirm_button(new QToolButton);
    edit_confirm_button->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    edit_title_layout->addWidget(edit_confirm_button);
    auto edit_discard_button(new QToolButton);
    edit_title_layout->addWidget(edit_discard_button);
    edit_discard_button->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    edit_discard_button->setPopupMode(QToolButton::InstantPopup);
    auto discard_edit_menu(new QMenu(this));
    edit_discard_button->setMenu(discard_edit_menu);
    auto edit_discard_action = discard_edit_menu->addAction(style()->standardIcon(QStyle::SP_DialogCloseButton), "discard");
    edit_layout->addLayout(edit_title_layout);

    auto edit_text_edit(new QTextEdit);
    edit_layout->addWidget(edit_text_edit);
    edit_text_edit->setPlaceholderText("Text content");

    auto edit_tags_group(new QGroupBox("Tags"));
    edit_tags_group->setLayout(edit_tags_layout);
    edit_layout->addWidget(edit_tags_group);
    edit_tag_lineedit->setPlaceholderText("Tag name");
    edit_tags_layout->addWidget(edit_tag_lineedit);
    edit_tag_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    edit_tags_layout->addWidget(edit_tag_add);
    connect(edit_tag_add, &QToolButton::clicked, this, [this] {
        if (tm) {
            tm->request_set_tag(edit_tag_lineedit->text().toStdString());
        }
    });
    connect(edit_tag_lineedit, &QLineEdit::returnPressed, edit_tag_add, &QToolButton::click);

    auto edit_add_field_layout(new QHBoxLayout);
    auto edit_fields_group(new QGroupBox("Fields"));
    auto edit_fields_layout(new QVBoxLayout(edit_fields_group));
    auto edit_field_add(new QToolButton);
    edit_add_field_layout->addWidget(edit_field_add);
    edit_field_name_lineedit->setPlaceholderText("Field name");
    edit_add_field_layout->addWidget(edit_field_name_lineedit);
    edit_field_value_lineedit->setPlaceholderText("Field value");
    edit_add_field_layout->addWidget(edit_field_value_lineedit);
    edit_field_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    edit_add_field_layout->addStretch(field_stretch_factor);
    edit_fields_layout->addLayout(present_fields_layout);
    edit_fields_layout->addLayout(edit_add_field_layout);
    edit_layout->addWidget(edit_fields_group);
    connect(edit_field_add, &QToolButton::clicked, this, [this] {
        if (tm && !edit_field_name_lineedit->text().isEmpty() && !edit_field_value_lineedit->text().isEmpty()) {
            tm->request_set_field(edit_field_name_lineedit->text().toStdString(), edit_field_value_lineedit->text().toStdString());
            edit_field_name_lineedit->setFocus();
        }
    });
    connect(edit_field_name_lineedit, &QLineEdit::returnPressed, edit_field_add, &QToolButton::click);
    connect(edit_field_value_lineedit, &QLineEdit::returnPressed, edit_field_add, &QToolButton::click);

    auto edit_add_list_layout(new QHBoxLayout);
    auto edit_lists_group(new QGroupBox("Lists"));
    auto edit_lists_layout(new QVBoxLayout(edit_lists_group));
    auto edit_list_add(new QToolButton);
    edit_list_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    edit_list_name_lineedit->setPlaceholderText("List name");
    edit_list_value_lineedit->setPlaceholderText("First list value");
    edit_add_list_layout->addWidget(edit_list_add);
    edit_add_list_layout->addWidget(edit_list_name_lineedit);
    edit_add_list_layout->addWidget(edit_list_value_lineedit);
    edit_add_list_layout->addStretch(list_stretch_factor);
    edit_lists_layout->addLayout(present_lists_layout);
    edit_lists_layout->addLayout(edit_add_list_layout);
    edit_layout->addWidget(edit_lists_group);
    connect(edit_list_add, &QToolButton::clicked, this, [this] {
        if (tm && !edit_list_name_lineedit->text().isEmpty() && !edit_list_value_lineedit->text().isEmpty()) {
            tm->request_set_list(edit_list_name_lineedit->text().toStdString(), {edit_list_value_lineedit->text().toStdString()});
            edit_list_name_lineedit->setFocus();
        }
    });
    connect(edit_list_name_lineedit, &QLineEdit::returnPressed, edit_list_add, &QToolButton::click);
    connect(edit_list_value_lineedit, &QLineEdit::returnPressed, edit_list_add, &QToolButton::click);

    auto main_layout(new QStackedLayout(this));
    auto view_index = main_layout->addWidget(view_widget);
    auto edit_index = main_layout->addWidget(edit_widget);
    connect(start_edit_button, &QToolButton::clicked, this, [main_layout, edit_index] {
        main_layout->setCurrentIndex(edit_index);
    });
    connect(edit_confirm_button, &QToolButton::clicked, this, [edit_discard_button] {
        edit_discard_button->setMenu(nullptr);
    });
    connect(edit_discard_button, &QToolButton::clicked, this, [edit_discard_button, edit_discard_action] {
        if (!edit_discard_button->menu()) {
            edit_discard_action->trigger();
        }
    });
    connect(edit_discard_action, &QAction::triggered, this, [main_layout, view_index] {
        main_layout->setCurrentIndex(view_index);
    });
}

void Text_Tiddler_ui::set_tiddler_model(Tiddler_Model* new_tiddler_model)
{
    if (tm != new_tiddler_model) {
        if (tm) {
            disconnect(tm);
        }
        tm = new_tiddler_model;
        if (tm) {
            //connect(tm, &Tiddler_Model::title_changed, this, woasned);
            //connect(tm, &Tiddler_Model::text_changed, this, woasned);
            //connect(tm, &Tiddler_Model::history_size_changed, this, woasned);
            connect(tm, &Tiddler_Model::tags_changed, this, &Text_Tiddler_ui::update_present_tags);
            connect(tm, &Tiddler_Model::fields_changed, this, &Text_Tiddler_ui::update_present_fields);
            connect(tm, &Tiddler_Model::lists_changed, this, &Text_Tiddler_ui::update_present_lists);
            connect(tm, &Tiddler_Model::single_list_changed, this, &Text_Tiddler_ui::update_present_list);
            update_present_tags();
            update_present_fields();
            update_present_lists();
        }
    }
}

// private

void Text_Tiddler_ui::update_present_tags()
{
    edit_tags_layout->removeWidget(edit_tag_add);
    edit_tags_layout->removeWidget(edit_tag_lineedit);
    edit_tag_lineedit->clear();
    clear_layout(edit_tags_layout);
    for (const auto& tag : tm->tags()) {
        connect(deletable_value(tag, edit_tags_layout), &QToolButton::clicked, this, [this, tag] {
            tm->request_remove_tag(tag);
        });
    }
    edit_tags_layout->addWidget(edit_tag_add);
    edit_tags_layout->addWidget(edit_tag_lineedit);
}

void Text_Tiddler_ui::update_present_fields()
{
    edit_field_name_lineedit->clear();
    edit_field_value_lineedit->clear();
    clear_layout(present_fields_layout);
    for (const auto& field : tm->fields()) {
        auto l = new QHBoxLayout;
        auto del = new QToolButton;
        auto fieldnamelabel = new QLabel(QString(field.first.c_str()) + ": ");
        auto fieldvaluelabel = new QLabel(QString(field.second.c_str()));
        del->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
        l->addWidget(del);
        l->addWidget(fieldnamelabel);
        l->addWidget(fieldvaluelabel, field_stretch_factor);
        present_fields_layout->addLayout(l);
        connect(del, &QToolButton::clicked, this, [this, field] {
            tm->request_remove_field(field.first);
        });
    }
}

void Text_Tiddler_ui::update_present_lists()
{
    edit_list_name_lineedit->clear();
    edit_list_value_lineedit->clear();
    clear_layout(present_lists_layout);
    lists_ui.clear();
    for (const auto& list : tm->lists()) {
        auto l = new QHBoxLayout;
        auto present_values_layout = new FlowLayout;
        auto del = new QToolButton;
        auto listnamelabel = new QLabel(QString(list.first.c_str()) + ": ");
        auto listvalues = list.second;
        auto ladd = new QToolButton;
        auto lval = new QLineEdit;
        del->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
        ladd->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
        lval->setPlaceholderText("add ...");
        l->addWidget(del);
        l->addWidget(listnamelabel);
        present_values_layout->addWidget(ladd);
        present_values_layout->addWidget(lval);
        l->addLayout(present_values_layout, list_stretch_factor);
        lists_ui[list.first] = {present_values_layout, ladd, lval};
        update_present_list(list.first);
        present_lists_layout->addLayout(l);
        connect(del, &QToolButton::clicked, this, [this, list] {
            tm->request_remove_list(list.first);
        });
        connect(ladd, &QToolButton::clicked, this, [this, list, lval] {
            if (!lval->text().isEmpty()) {
                auto values = tm->list(list.first);
                values.push_back(lval->text().toStdString());
                tm->request_set_list(list.first, values);
            }
        });
        connect(lval, &QLineEdit::returnPressed, ladd, &QToolButton::click);
    }
}

void Text_Tiddler_ui::update_present_list(const std::string &list_name)
{
    auto listvalues = tm->list(list_name);
    auto lui = lists_ui[list_name];
    if (!lui.l || !lui.add || !lui.val) {
        return;
    }
    lui.l->removeWidget(lui.add);
    lui.l->removeWidget(lui.val);
    clear_layout(lui.l);
    if (listvalues.empty()) {
        delete(lui.add);
        delete(lui.val);
        update_present_lists();
    } else {
        for (const auto& lv : listvalues) {
            connect(deletable_value(lv, lui.l), &QToolButton::clicked, this, [this, list_name, listvalues, lv] {
                auto values = tm->list(list_name);
                auto it = std::find(values.begin(), values.end(), lv);
                if (it != values.end()) {
                    values.erase(it);
                    tm->request_set_list(list_name, values);
                }
            });
        }
        lui.val->clear();
        lui.l->addWidget(lui.add);
        lui.l->addWidget(lui.val);
    }
}

QToolButton* Text_Tiddler_ui::deletable_value(const std::string& text, FlowLayout* parent_layout)
{
    auto w = new QWidget;
    auto l = new QHBoxLayout(w);
    auto del = new QToolButton;
    auto label = new QLabel(text.c_str());
    l->setSpacing(0);
    l->setMargin(0);
    del->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setLineWidth(1);
    l->addWidget(del);
    l->addWidget(label);
    parent_layout->addWidget(w);
    return del;
}
