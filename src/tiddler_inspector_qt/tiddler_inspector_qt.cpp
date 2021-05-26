#include "tiddler_inspector_qt.h"
#include "tiddlerstore_qt/tiddler_model_qt.h"
#include "tiddlerstore_qt/tiddler_pure_view_qt.h"
#include "tiddlerstore_qt/tiddler_pure_edit_qt.h"

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
#include <QFileDialog>
#include <QStandardPaths>

Tiddler_Inspector::Tiddler_Inspector(const QStringList& tiddlerstore_list, QWidget* parent)
    : QWidget(parent)
    , pure_view(new Tiddler_Pure_View)
    , pure_edit(new Tiddler_Pure_Edit)
    , tiddlerstore_history(tiddlerstore_list)
    , load_button(new QToolButton)
    , load_history_menu(new QMenu(tr("Attention! Unsaved Changes")))
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
        for (const auto& t : store) {
            if (t->title() == title_filter_model->data(title_filter_model->index(0, 0)).toString().toStdString()) {
                tiddler_selector_lineedit->clear();
                tiddler_model = std::make_unique<Tiddler_Model>(t.get());
                connect(tiddler_model.get(), &Tiddler_Model::title_changed, this, &Tiddler_Inspector::update_tiddler_list);
                pure_view->set_tiddler_model(tiddler_model.get());
                return;
            }
        }
    });
    auto add_tiddler_action = tiddler_selector_lineedit->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), QLineEdit::LeadingPosition);
    add_tiddler_action->setVisible(false);
    connect(add_tiddler_action, &QAction::triggered, this, [this, tiddler_selector_lineedit] {
        auto t = store.emplace_back(new Tiddlerstore::Tiddler).get();
        adjust_dirty(true);
        t->set_title(tiddler_selector_lineedit->text().toStdString());
        if (t->title().empty()) {
            t->set_title(tr("New Entry ...").toStdString());
        }
        tiddler_selector_lineedit->clear();
        update_tiddler_list();
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
    load_button->setMenu(load_history_menu);

    connect(load_history_menu, &QMenu::aboutToShow, this, [this] {
        auto elems = tiddlerstore_history.get_elements();
        auto default_location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        load_history_menu->clear();
        for (auto it = elems.begin(); it != elems.end(); it += 1) {
            auto path = *it;
            auto load = load_history_menu->addAction(path);
            connect(load, &QAction::triggered, this, [this, path] {
                open_store(path);
            });
        }
        auto load_other = load_history_menu->addAction("...");
        connect(load_other, &QAction::triggered, this, [this, default_location] {
            auto path = QFileDialog::getOpenFileName(nullptr, tr("Select Tiddlerstore"), default_location);
            if (!path.isEmpty()) {
                open_store(path);
            }
        });
    });

    auto save_button(new QToolButton);
    save_button->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    save_button->setPopupMode(QToolButton::InstantPopup);
    auto save_menu(new QMenu);
    save_button->setMenu(save_menu);

    connect(save_menu, &QMenu::aboutToShow, this, [this, save_menu] {
        auto elems = tiddlerstore_history.get_elements();
        auto default_location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        save_menu->clear();
        if (!elems.isEmpty()) {
            auto save_current = save_menu->addAction(elems.first());
            connect(save_current, &QAction::triggered, this, [this, save_current]{
                save_store(save_current->text());
            });
            QFileInfo fi(elems.first());
            default_location = fi.path();
        }
        auto save_other = save_menu->addAction("...");
        connect(save_other, &QAction::triggered, this, [this, default_location] {
            auto other_name = QFileDialog::getSaveFileName(nullptr, tr("Where do you want to save?"), default_location);
            if (!other_name.isEmpty()) {
                save_store(other_name);
            }
        });
    });

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
}

// private

void Tiddler_Inspector::update_tiddler_list()
{
    QStringList titles;
    for (const auto& t : store) {
        titles << t->title().c_str();
    }
    title_model.setStringList(titles);
}

void Tiddler_Inspector::adjust_dirty(bool dirty_value)
{
    if (dirty_value != is_dirty) {
        is_dirty = dirty_value;
        if (is_dirty) {
            if (!load_safety_menu) {
                load_safety_menu = new QMenu;
            }
            load_button->setMenu(load_safety_menu);
            load_safety_menu->addMenu(load_history_menu);
        } else {
            load_button->setMenu(load_history_menu);
        }
    }
}

void Tiddler_Inspector::open_store(const QString& path)
{
    store = Tiddlerstore::open_store_from_file(path.toStdString());
    update_tiddler_list();
}

void Tiddler_Inspector::save_store(const QString& path)
{
    if (Tiddlerstore::save_store_to_file(store, path.toStdString())) {
        tiddlerstore_history.set_current_element(path);
        adjust_dirty(false);
    }
}
