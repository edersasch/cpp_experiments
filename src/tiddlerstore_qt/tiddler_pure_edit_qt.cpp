#include "tiddler_pure_edit_qt.h"
#include "tiddlerstore/tiddlerstore.h"
#include "qt_utilities/flowlayout.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QStyle>
#include <QMenu>
#include <QGroupBox>
#include <QLabel>

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

Tiddler_Pure_Edit::Tiddler_Pure_Edit(QWidget* parent)
    : QWidget(parent)
    , work(new Tiddlerstore::Tiddler)
    , title_lineedit(new QLineEdit)
    , text_edit(new QTextEdit)
    , accept_button(new QToolButton)
    , discard_button(new QToolButton)
    , discard_menu(new QMenu(this))
    , tag_lineedit(new QLineEdit)
    , tags_layout(new FlowLayout)
    , field_name_lineedit(new QLineEdit)
    , field_value_lineedit(new QLineEdit)
    , present_fields_layout(new QVBoxLayout)
    , list_name_lineedit(new QLineEdit)
    , list_value_lineedit(new QLineEdit)
    , present_lists_layout(new QVBoxLayout)
{
    connect(&work_tm, &Tiddler_Model::tags_changed, this, &Tiddler_Pure_Edit::update_present_tags);
    connect(&work_tm, &Tiddler_Model::field_changed, this, &Tiddler_Pure_Edit::update_present_fields);
    connect(&work_tm, &Tiddler_Model::field_added, this, &Tiddler_Pure_Edit::update_present_fields);
    connect(&work_tm, &Tiddler_Model::field_removed, this, &Tiddler_Pure_Edit::update_present_fields);
    connect(&work_tm, &Tiddler_Model::list_changed, this, &Tiddler_Pure_Edit::update_present_list);
    connect(&work_tm, &Tiddler_Model::list_added, this, &Tiddler_Pure_Edit::update_present_lists);
    connect(&work_tm, &Tiddler_Model::list_removed, this, &Tiddler_Pure_Edit::update_present_lists);

    auto main_layout = new QVBoxLayout(this);

    auto title_layout(new QHBoxLayout);
    title_lineedit->setPlaceholderText("Title");
    title_layout->addWidget(title_lineedit);
    accept_button->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    title_layout->addWidget(accept_button);
    title_layout->addWidget(discard_button);
    discard_button->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    discard_button->setPopupMode(QToolButton::InstantPopup);
    auto discard_action = discard_menu->addAction(style()->standardIcon(QStyle::SP_DialogCloseButton), tr("really discard?"));
    main_layout->addLayout(title_layout);
    connect(title_lineedit, &QLineEdit::textChanged, this, &Tiddler_Pure_Edit::update_dirty);
    connect(accept_button, &QToolButton::clicked, this, [this, discard_action] {
        if (tm) {
            if (current_dirty) {
                tm->request_set_tiddler_data(work_tm);
                tm->request_set_title(title_lineedit->text().toStdString());
                tm->request_set_text(text_edit->toPlainText().toStdString());
                emit accept_edit();
            } else {
                emit discard_action->trigger();
            }
        }
    });
    connect(discard_button, &QToolButton::clicked, this, [this, discard_action] {
        if (!current_dirty) {
            discard_action->trigger();
        }
    });
    connect(discard_action, &QAction::triggered, this, &Tiddler_Pure_Edit::discard_edit);

    auto remove_button(new QToolButton);
    auto remove_menu(new QMenu);
    auto remove_action = remove_menu->addAction(style()->standardIcon(QStyle::SP_MessageBoxCritical), tr("remove"));
    remove_button->setIcon(style()->standardIcon(QStyle::SP_MessageBoxCritical));
    remove_button->setMenu(remove_menu);
    remove_button->setPopupMode(QToolButton::InstantPopup);
    connect(remove_action, &QAction::triggered, this, [this] {
        if (tm) {
            tm->request_remove();
        }
    });
    title_layout->addWidget(remove_button);

    text_edit->setPlaceholderText("Text content");
    main_layout->addWidget(text_edit);
    connect(text_edit, &QTextEdit::textChanged, this, [this] {
        update_dirty();
    });

    auto tags_group(new QGroupBox("Tags"));
    tags_group->setLayout(tags_layout);
    main_layout->addWidget(tags_group);
    tag_lineedit->setPlaceholderText("Tag name");
    tags_layout->addWidget(tag_lineedit);
    auto tag_add_action = tag_lineedit->addAction(style()->standardIcon(QStyle::SP_DialogApplyButton), QLineEdit::LeadingPosition);
    connect(tag_add_action, &QAction::triggered, this, [this] {
        work_tm.request_set_tag(tag_lineedit->text().toStdString());
    });
    connect(tag_lineedit, &QLineEdit::returnPressed, tag_add_action, &QAction::trigger);

    auto add_field_layout(new QHBoxLayout);
    auto fields_group(new QGroupBox("Fields"));
    auto fields_layout(new QVBoxLayout(fields_group));
    auto field_add_button(new QToolButton);
    add_field_layout->addWidget(field_add_button);
    field_name_lineedit->setPlaceholderText("Field name");
    add_field_layout->addWidget(field_name_lineedit);
    field_value_lineedit->setPlaceholderText("Field value");
    add_field_layout->addWidget(field_value_lineedit);
    field_add_button->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    add_field_layout->addStretch(field_stretch_factor);
    fields_layout->addLayout(present_fields_layout);
    fields_layout->addLayout(add_field_layout);
    main_layout->addWidget(fields_group);
    connect(field_add_button, &QToolButton::clicked, this, [this] {
        if (!field_name_lineedit->text().isEmpty() && !field_value_lineedit->text().isEmpty()) {
            work_tm.request_set_field(field_name_lineedit->text().toStdString(), field_value_lineedit->text().toStdString());
            field_name_lineedit->setFocus();
        }
    });
    connect(field_name_lineedit, &QLineEdit::returnPressed, field_add_button, &QToolButton::click);
    connect(field_value_lineedit, &QLineEdit::returnPressed, field_add_button, &QToolButton::click);

    auto add_list_layout(new QHBoxLayout);
    auto lists_group(new QGroupBox("Lists"));
    auto lists_layout(new QVBoxLayout(lists_group));
    auto list_add_button(new QToolButton);
    list_add_button->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    list_name_lineedit->setPlaceholderText("List name");
    list_value_lineedit->setPlaceholderText("First list value");
    add_list_layout->addWidget(list_add_button);
    add_list_layout->addWidget(list_name_lineedit);
    add_list_layout->addWidget(list_value_lineedit);
    add_list_layout->addStretch(list_stretch_factor);
    lists_layout->addLayout(present_lists_layout);
    lists_layout->addLayout(add_list_layout);
    main_layout->addWidget(lists_group);
    connect(list_add_button, &QToolButton::clicked, this, [this] {
        if (!list_name_lineedit->text().isEmpty() && !list_value_lineedit->text().isEmpty()) {
            work_tm.request_set_list(list_name_lineedit->text().toStdString(), {list_value_lineedit->text().toStdString()});
            list_name_lineedit->setFocus();
        }
    });
    connect(list_name_lineedit, &QLineEdit::returnPressed, list_add_button, &QToolButton::click);
    connect(list_value_lineedit, &QLineEdit::returnPressed, list_add_button, &QToolButton::click);

    accept_button->hide();
}

Tiddler_Pure_Edit::~Tiddler_Pure_Edit() = default;

Tiddler_Model* Tiddler_Pure_Edit::tiddler_model()
{
    return tm;
}

void Tiddler_Pure_Edit::set_tiddler_model(Tiddler_Model* model)
{
    tm = model;
    work_tm.request_set_tiddler_data(tm ? tm->tiddler(): Tiddlerstore::Tiddler());
    title_lineedit->setText(work_tm.title().c_str());
    text_edit->setText(work_tm.text().c_str());
    update_present_tags();
    update_present_fields();
    update_present_lists();
    present_accept_button();
    if (tm) {
        connect(tm, &Tiddler_Model::destroyed, this, [this]{
            set_tiddler_model(nullptr);
        });
    }
}

// private

void Tiddler_Pure_Edit::update_dirty()
{
    bool dirty = (tm && title_lineedit->text().toStdString() != tm->title());
    dirty |= (tm && text_edit->toPlainText().toStdString() != tm->text());
    dirty |= (tm && work_tm.tags() != tm->tags());
    dirty |= (tm && work_tm.fields() != tm->fields());
    dirty |= (tm && work_tm.lists() != tm->lists());
    discard_button->setMenu(dirty ? discard_menu : nullptr);
    if (current_dirty != dirty) {
        current_dirty = dirty;
        emit unsaved_edit(dirty);
    }
}

void Tiddler_Pure_Edit::update_present_tags()
{
    tags_layout->removeWidget(tag_lineedit);
    tag_lineedit->clear();
    clear_layout(tags_layout);
    for (const auto& tag : work_tm.tags()) {
        connect(deletable_value(tag, tags_layout), &QToolButton::clicked, this, [this, tag] {
            work_tm.request_remove_tag(tag);
        });
    }
    tags_layout->addWidget(tag_lineedit);
    update_dirty();
}

void Tiddler_Pure_Edit::update_present_fields()
{
    field_name_lineedit->clear();
    field_value_lineedit->clear();
    clear_layout(present_fields_layout);
    for (const auto& field : work_tm.fields()) {
        auto l(new QHBoxLayout);
        auto del(new QToolButton);
        auto fieldnamelabel(new QLabel(QString(field.first.c_str()) + ": "));
        auto fieldvaluelabel(new QLabel(QString(field.second.c_str())));
        del->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
        l->addWidget(del);
        l->addWidget(fieldnamelabel);
        l->addWidget(fieldvaluelabel, field_stretch_factor);
        present_fields_layout->addLayout(l);
        connect(del, &QToolButton::clicked, this, [this, field] {
            work_tm.request_remove_field(field.first);
        });
    }
    update_dirty();
}

void Tiddler_Pure_Edit::update_present_lists()
{
    list_name_lineedit->clear();
    list_value_lineedit->clear();
    clear_layout(present_lists_layout);
    single_list_elements.clear();
    for (const auto& list : work_tm.lists()) {
        auto l(new QHBoxLayout);
        auto present_values_layout(new FlowLayout);
        auto del(new QToolButton);
        auto listnamelabel(new QLabel(QString(list.first.c_str()) + ": "));
        auto listvalues = list.second;
        auto lval(new QLineEdit);
        del->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
        lval->setPlaceholderText("add ...");
        auto ladd_action = lval->addAction(style()->standardIcon(QStyle::SP_DialogApplyButton), QLineEdit::LeadingPosition);
        l->addWidget(del);
        l->addWidget(listnamelabel);
        present_values_layout->addWidget(lval);
        l->addLayout(present_values_layout, list_stretch_factor);
        single_list_elements[list.first] = {present_values_layout, lval};
        update_present_list(list.first);
        present_lists_layout->addLayout(l);
        connect(del, &QToolButton::clicked, this, [this, list] {
            work_tm.request_remove_list(list.first);
        });
        connect(ladd_action, &QAction::triggered, this, [this, list, lval] {
            if (!lval->text().isEmpty()) {
                auto values = work_tm.list(list.first);
                values.push_back(lval->text().toStdString());
                work_tm.request_set_list(list.first, values);
            }
        });
        connect(lval, &QLineEdit::returnPressed, ladd_action, &QAction::trigger);
    }
    update_dirty();
}

void Tiddler_Pure_Edit::update_present_list(const std::string& list_name)
{
    auto listvalues = work_tm.list(list_name);
    auto lui = single_list_elements[list_name];
    if (!lui.l || !lui.val) {
        return;
    }
    lui.l->removeWidget(lui.val);
    clear_layout(lui.l);
    if (listvalues.empty()) {
        delete(lui.val);
        update_present_lists();
    } else {
        for (const auto& lv : listvalues) {
            connect(deletable_value(lv, lui.l), &QToolButton::clicked, this, [this, list_name, listvalues, lv] {
                auto values = work_tm.list(list_name);
                auto it = std::find(values.begin(), values.end(), lv);
                if (it != values.end()) {
                    values.erase(it);
                    work_tm.request_set_list(list_name, values);
                }
            });
        }
        lui.val->clear();
        lui.l->addWidget(lui.val);
        update_dirty();
    }
}

QToolButton* Tiddler_Pure_Edit::deletable_value(const std::string& text, QLayout* parent_layout, bool button_after_label)
{
    auto w(new QWidget(this));
    auto l(new QHBoxLayout(w));
    auto del(new QToolButton(this));
    auto label(new QLabel(text.c_str(), this));
    l->setSpacing(0);
    l->setMargin(0);
    del->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setLineWidth(1);
    l->addWidget(label);
    l->insertWidget(button_after_label ? 1 : 0, del);
    parent_layout->addWidget(w);
    return del;
}

void Tiddler_Pure_Edit::present_accept_button()
{
    tm ? accept_button->show() : accept_button->hide();
}
