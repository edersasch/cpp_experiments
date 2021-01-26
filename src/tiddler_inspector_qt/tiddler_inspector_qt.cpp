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

NoDnDOverwriteStringListModel::NoDnDOverwriteStringListModel(QObject* parent)
    : QStringListModel(parent)
{
}

Qt::ItemFlags NoDnDOverwriteStringListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QStringListModel::flags(index);
    if (index.isValid()) {
        flags &= ~Qt::ItemIsDropEnabled;
    }
    return flags;
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
    , tag_add(new QToolButton)
    , tag_edit(new QLineEdit)
    , tags_layout(new FlowLayout)
    , field_name_edit(new QLineEdit)
    , field_value_edit(new QLineEdit)
    , present_fields_layout(new QVBoxLayout)
    , list_name_edit(new QLineEdit)
    , list_value_edit(new QLineEdit)
    , present_lists_layout(new QVBoxLayout)
{
    auto main_layout(new QVBoxLayout(this));
    auto add_field_layout(new QHBoxLayout);
    auto add_list_layout(new QHBoxLayout);

    auto title_edit(new QLineEdit);
    auto text_edit(new QTextEdit);
    auto tags_group(new QGroupBox("Tags"));
    auto fields_group(new QGroupBox("Fields"));
    auto fields_layout(new QVBoxLayout(fields_group));
    auto field_add(new QToolButton);
    auto lists_group(new QGroupBox("Lists"));
    auto lists_layout(new QVBoxLayout(lists_group));
    auto list_add(new QToolButton);

    title_edit->setPlaceholderText("Title");
    text_edit->setPlaceholderText("Text content");
    tag_edit->setPlaceholderText("Tag name");
    field_name_edit->setPlaceholderText("Field name");
    field_value_edit->setPlaceholderText("Field value");
    list_name_edit->setPlaceholderText("List name");
    list_value_edit->setPlaceholderText("First list value");
    tag_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    field_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    list_add->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));

    tags_layout->addWidget(tag_add);
    tags_layout->addWidget(tag_edit);
    tags_group->setLayout(tags_layout);
    add_field_layout->addWidget(field_add);
    add_field_layout->addWidget(field_name_edit);
    add_field_layout->addWidget(field_value_edit);
    add_field_layout->addStretch(field_stretch_factor);
    fields_layout->addLayout(present_fields_layout);
    fields_layout->addLayout(add_field_layout);
    add_list_layout->addWidget(list_add);
    add_list_layout->addWidget(list_name_edit);
    add_list_layout->addWidget(list_value_edit);
    add_list_layout->addStretch(list_stretch_factor);
    lists_layout->addLayout(present_lists_layout);
    lists_layout->addLayout(add_list_layout);
    main_layout->addWidget(title_edit);
    main_layout->addWidget(text_edit);
    main_layout->addWidget(tags_group);
    main_layout->addWidget(fields_group);
    main_layout->addWidget(lists_group);

    connect(tag_add, &QToolButton::clicked, this, [this] {
        if (tm) {
            tm->request_set_tag(tag_edit->text().toStdString());
        }
    });
    connect(tag_edit, &QLineEdit::returnPressed, tag_add, &QToolButton::click);
    connect(field_add, &QToolButton::clicked, this, [this] {
        if (tm && !field_name_edit->text().isEmpty() && !field_value_edit->text().isEmpty()) {
            tm->request_set_field(field_name_edit->text().toStdString(), field_value_edit->text().toStdString());
            field_name_edit->setFocus();
        }
    });
    connect(field_name_edit, &QLineEdit::returnPressed, field_add, &QToolButton::click);
    connect(field_value_edit, &QLineEdit::returnPressed, field_add, &QToolButton::click);
    connect(list_add, &QToolButton::clicked, this, [this] {
        if (tm && !list_name_edit->text().isEmpty() && !list_value_edit->text().isEmpty()) {
            tm->request_set_list(list_name_edit->text().toStdString(), {list_value_edit->text().toStdString()});
            list_name_edit->setFocus();
        }
    });
    connect(list_name_edit, &QLineEdit::returnPressed, list_add, &QToolButton::click);
    connect(list_value_edit, &QLineEdit::returnPressed, list_add, &QToolButton::click);
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
    tags_layout->removeWidget(tag_add);
    tags_layout->removeWidget(tag_edit);
    tag_edit->clear();
    clear_layout(tags_layout);
    for (const auto& tag : tm->tags()) {
        connect(deletable_value(tag, tags_layout), &QToolButton::clicked, this, [this, tag] {
            tm->request_remove_tag(tag);
        });
    }
    tags_layout->addWidget(tag_add);
    tags_layout->addWidget(tag_edit);
}

void Text_Tiddler_ui::update_present_fields()
{
    field_name_edit->clear();
    field_value_edit->clear();
    clear_layout(present_fields_layout);
    for (const auto& field : tm->fields()) {
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
            tm->request_remove_field(field.first);
        });
    }
}

void Text_Tiddler_ui::update_present_lists()
{
    list_name_edit->clear();
    list_value_edit->clear();
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
        del->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
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
