#include "tiddlerstore_handler_qt.h"
#include "tiddler_model_qt.h"

#include <QMenu>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QStyle>
#include <QListView>
#include <QStandardPaths>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

Tiddlerstore_Handler::Tiddlerstore_Handler(const QStringList& tiddlerstore_list, QWidget* parent)
    : QWidget(parent)
    , tiddlerstore_history(tiddlerstore_list)
    , load_button(new QToolButton)
    , load_history_menu(new QMenu(tr("Attention! Unsaved Changes")))
{
    auto title_filter_model(new QSortFilterProxyModel(this));
    title_filter_model->setSourceModel(&title_model);
    title_filter_model->sort(0);

    auto tiddler_selector_lineedit(new QLineEdit);
    tiddler_selector_lineedit->setClearButtonEnabled(true);
    auto open_tiddler_action = tiddler_selector_lineedit->addAction(style()->standardIcon(QStyle::SP_DirOpenIcon), QLineEdit::LeadingPosition);
    open_tiddler_action->setVisible(false);
    connect(open_tiddler_action, &QAction::triggered, this, [this, title_filter_model] {
        prepare_open(store[title_filter_model->mapToSource(title_filter_model->index(0, 0)).row()].get());
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
        update_title_model();
        prepare_open(t);
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
    tiddler_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(tiddler_list_view, &QListView::clicked, this, [this, title_filter_model](const QModelIndex& index) {
        prepare_open(store[title_filter_model->mapToSource(index).row()].get());
    });

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

    auto main_layout(new QVBoxLayout(this));
    main_layout->addLayout(file_handling_layout);
    main_layout->addWidget(tiddler_selector_lineedit);
    main_layout->addWidget(tiddler_list_view);

    auto elems = tiddlerstore_history.get_elements();
    if (!elems.isEmpty()) {
        open_store(elems.first());
    }
}

// private

void Tiddlerstore_Handler::update_title_model()
{
    QStringList titles;
    for (const auto& t : store) {
        titles << t->title().c_str();
    }
    title_model.setStringList(titles);
}

void Tiddlerstore_Handler::adjust_dirty(bool dirty_value)
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

void Tiddlerstore_Handler::open_store(const QString& path)
{
    store = Tiddlerstore::open_store_from_file(path.toStdString());
    update_title_model();
}

void Tiddlerstore_Handler::save_store(const QString& path)
{
    if (Tiddlerstore::save_store_to_file(store, path.toStdString())) {
        tiddlerstore_history.set_current_element(path);
        adjust_dirty(false);
    }
}

void Tiddlerstore_Handler::prepare_open(Tiddlerstore::Tiddler* t)
{
    auto it = active_models.find(t);
    if (it == active_models.end()) {
        active_models[t] = std::make_unique<Tiddler_Model>(t);
        it = active_models.insert({t, std::make_unique<Tiddler_Model>(t)}).first;
        auto model = it->second.get();
        connect(model, &Tiddler_Model::title_changed,           this, &Tiddlerstore_Handler::update_title_model);

        connect(model, &Tiddler_Model::title_changed,           this, &Tiddlerstore_Handler::set_dirty);
        connect(model, &Tiddler_Model::text_changed,            this, &Tiddlerstore_Handler::set_dirty);
        connect(model, &Tiddler_Model::history_size_changed,    this, &Tiddlerstore_Handler::set_dirty);
        connect(model, &Tiddler_Model::tags_changed,            this, &Tiddlerstore_Handler::set_dirty);
        connect(model, &Tiddler_Model::fields_changed,          this, &Tiddlerstore_Handler::set_dirty);
        connect(model, &Tiddler_Model::single_list_changed,     this, &Tiddlerstore_Handler::set_dirty);
        connect(model, &Tiddler_Model::lists_changed,           this, &Tiddlerstore_Handler::set_dirty);

        connect(model, &Tiddler_Model::remove_request,          this, [this, t] {
            active_models.erase(t);
            auto store_it = std::find_if(store.begin(), store.end(), [t](const std::unique_ptr<Tiddlerstore::Tiddler>& tref) {
                return t == tref.get();
            });
            if (store_it != store.end()) {
                store.erase(store_it);
                update_title_model();
            }
        });
    }
    emit open_tiddler_model(it->second.get());
}
