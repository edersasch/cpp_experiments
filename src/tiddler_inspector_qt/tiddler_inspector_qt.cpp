#include "tiddler_inspector_qt.h"
#include "tiddlerstore_qt/tiddler_model_qt.h"
#include "tiddlerstore_qt/tiddler_pure_view_qt.h"
#include "tiddlerstore_qt/tiddler_pure_edit_qt.h"

#include "fs_history_qt/fs_history_ui.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QStackedWidget>
#include <QSplitter>
#include <QLineEdit>
#include <QListView>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QToolButton>
#include <QMenu>

Tiddler_Inspector::Tiddler_Inspector(const QString& tiddlerstore_json, QWidget* parent)
    : QWidget(parent)
    , pure_view(new Tiddler_Pure_View)
    , pure_edit(new Tiddler_Pure_Edit)
    , load_button(new QToolButton)
    , load_safety_menu(new QMenu)
    , load_history_menu(new QMenu)
{
    auto tiddler_view_edit_stack(new QStackedWidget(this));
    auto view_index = tiddler_view_edit_stack->addWidget(pure_view);
    auto edit_index = tiddler_view_edit_stack->addWidget(pure_edit);
    connect(pure_view, &Tiddler_Pure_View::trigger_edit, this, [this, tiddler_view_edit_stack, edit_index] {
        pure_edit->set_tiddler_model(pure_view->tiddler_model());
        tiddler_view_edit_stack->setCurrentIndex(edit_index);
    });
    connect(pure_edit, &Tiddler_Pure_Edit::accept_edit, this, [this, tiddler_view_edit_stack, view_index] {
        tiddler_view_edit_stack->setCurrentIndex(view_index);
        adjust_dirty(true);
    });
    connect(pure_edit, &Tiddler_Pure_Edit::discard_edit, this, [tiddler_view_edit_stack, view_index] {
        tiddler_view_edit_stack->setCurrentIndex(view_index);
    });

    auto title_filter_model(new QSortFilterProxyModel(this));
    title_filter_model->setSourceModel(&title_model);

    auto tiddler_selector_lineedit(new QLineEdit);
    tiddler_selector_lineedit->setClearButtonEnabled(true);
    auto open_tiddler_action = tiddler_selector_lineedit->addAction(style()->standardIcon(QStyle::SP_DirOpenIcon), QLineEdit::LeadingPosition);
    open_tiddler_action->setVisible(false);
    connect(open_tiddler_action, &QAction::triggered, this, [this, title_filter_model, tiddler_selector_lineedit] {
        for (const auto& tm : tiddler_models) {
            if (tm->title() == title_filter_model->data(title_filter_model->index(0, 0)).toString().toStdString()) {
                tiddler_selector_lineedit->clear();
                pure_view->set_tiddler_model(tm.get());
                return;
            }
        }
    });
    auto add_tiddler_action = tiddler_selector_lineedit->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), QLineEdit::LeadingPosition);
    add_tiddler_action->setVisible(false);
    connect(add_tiddler_action, &QAction::triggered, this, [this, tiddler_selector_lineedit] {
        tiddler_models.push_back(std::make_unique<Tiddler_Model>());
        adjust_dirty(true);
        auto tm = tiddler_models.back().get();
        tm->request_set_title(tiddler_selector_lineedit->text().toStdString());
        tiddler_selector_lineedit->clear();
        if (tm->title().empty()) {
            tm->request_set_title("New Entry ...");
        }
        pure_view->set_tiddler_model(tm);
        update_tiddler_list();
        connect(tm, &Tiddler_Model::title_changed, this, &Tiddler_Inspector::update_tiddler_list);
    });
    auto placeholder_tiddler_action = tiddler_selector_lineedit->addAction(style()->standardIcon(QStyle::SP_FileIcon), QLineEdit::LeadingPosition);
    connect(tiddler_selector_lineedit, &QLineEdit::returnPressed, this, [open_tiddler_action, add_tiddler_action] {
        if (open_tiddler_action->isVisible()) {
            open_tiddler_action->trigger();
        } else {
            if (add_tiddler_action->isVisible()) {
                add_tiddler_action->trigger();
            }
        }
    });

    auto tiddler_list_view(new QListView);
    tiddler_list_view->setModel(title_filter_model);

    connect(tiddler_selector_lineedit, &QLineEdit::textChanged, this, [open_tiddler_action, add_tiddler_action, placeholder_tiddler_action, title_filter_model] (const QString& text) {
        title_filter_model->setFilterWildcard("*" + text + "*");
        auto rc = title_filter_model->rowCount();
        bool show_open = !text.isEmpty() && rc == 1;
        bool show_add = !text.isEmpty() && rc == 0;
        open_tiddler_action->setVisible(show_open);
        add_tiddler_action->setVisible(show_add);
        placeholder_tiddler_action->setVisible(!(show_open || show_add));
    });

    load_button->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    load_button->setPopupMode(QToolButton::InstantPopup);
    load_history_menu->setTitle("attention: usaved changes");
    load_button->setMenu(load_history_menu);

    load_history_menu->addAction("bastel 1");
    load_history_menu->addAction("bastel 2");
    load_history_menu->addAction("bastel 3");

    auto save_button(new QToolButton);
    save_button->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    save_button->setPopupMode(QToolButton::InstantPopup);
    auto save_menu(new QMenu);
    save_button->setMenu(save_menu);

    auto file_handling_layout(new QHBoxLayout);
    file_handling_layout->addWidget(load_button);
    file_handling_layout->addWidget(save_button);
    file_handling_layout->addStretch();

    auto tiddler_selector(new QWidget);
    auto tiddler_selector_layout(new QVBoxLayout(tiddler_selector));
    tiddler_selector_layout->addLayout(file_handling_layout);
    tiddler_selector_layout->addWidget(tiddler_selector_lineedit);
    tiddler_selector_layout->addWidget(tiddler_list_view);

    auto tiddler_scroll(new QScrollArea);
    tiddler_scroll->setWidgetResizable(true);
    tiddler_scroll->setWidget(tiddler_view_edit_stack);

    auto main_splitter(new QSplitter);
    main_splitter->addWidget(tiddler_selector);
    main_splitter->addWidget(tiddler_scroll);
    main_splitter->setStretchFactor(1, 2);

    auto main_layout(new QVBoxLayout(this));
    main_layout->addWidget(main_splitter, 1);

    if (!tiddlerstore_json.isEmpty()) {
        nlohmann::json tj = tiddlerstore_json.toStdString();
        for (auto& t : tj) {
            tiddler_models.push_back(std::make_unique<Tiddler_Model>());
            auto tm = tiddler_models.back().get();
            tm->request_set_tiddler_data(t.get<Tiddlerstore::Tiddler>());
            connect(tm, &Tiddler_Model::title_changed, this, &Tiddler_Inspector::update_tiddler_list);
        }
        update_tiddler_list();
    }
}

QString Tiddler_Inspector::get_store()
{
    nlohmann::json j;
    for (const auto& tm : tiddler_models) {
        j.push_back(tm->tiddler());
    }
    return QString(j.dump().c_str());
}

// private

void Tiddler_Inspector::update_tiddler_list()
{
    QStringList titles;
    for (const auto& tm : tiddler_models) {
        titles << tm->title().c_str();
    }
    title_model.setStringList(titles);
}

void Tiddler_Inspector::adjust_dirty(bool dirty_value)
{
    if (dirty_value != is_dirty) {
        is_dirty = dirty_value;
        if (is_dirty) {
            is_dirty = true;
            load_button->setMenu(load_safety_menu);
            load_safety_menu->addMenu(load_history_menu);
        } else {
            load_button->setMenu(load_history_menu);
        }
    }
}
