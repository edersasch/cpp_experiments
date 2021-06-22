#include "tiddlerstore_handler_qt.h"
#include "tiddler_model_qt.h"

#include <QMenu>
#include <QLineEdit>
#include <QStyle>
#include <QToolButton>
#include <QListView>
#include <QStandardPaths>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QStringListModel>
#include <QCompleter>
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

Tiddlerstore_Handler::Tiddlerstore_Handler(const QStringList& tiddlerstore_list, QWidget* parent)
    : QWidget(parent)
    , tiddlerstore_history(tiddlerstore_list)
    , tiddler_list_view(new QListView)
    , load_button(new QToolButton)
    , load_history_menu(new QMenu(tr("Attention! Unsaved Changes")))
    , filter_layout(new QVBoxLayout)
{
    auto main_layout(new QVBoxLayout(this));
    main_layout->addLayout(setup_toolbar());
    main_layout->addLayout(filter_layout);
    setup_tiddler_list_view();
    main_layout->addWidget(tiddler_list_view);

    connect(&main_filter_model, &Tiddler_Model::field_changed, this, &Tiddlerstore_Handler::apply_filter);
    connect(&main_filter_model, &Tiddler_Model::field_added, this, &Tiddlerstore_Handler::apply_filter);
    connect(&main_filter_model, &Tiddler_Model::field_removed, this, &Tiddlerstore_Handler::apply_filter);
    connect(&store_model, &Tiddlerstore_Model::model_created, this, &Tiddlerstore_Handler::connect_model);
    connect(&store_model, &Tiddlerstore_Model::removed, this, &Tiddlerstore_Handler::update_titles_and_filters);

    auto elems = tiddlerstore_history.get_elements();
    if (!elems.isEmpty()) {
        open_store(elems.first());
    }
}

// private

QHBoxLayout* Tiddlerstore_Handler::setup_toolbar()
{
    auto layout = new QHBoxLayout;
    setup_load_button();
    layout->addWidget(load_button);
    layout->addWidget(setup_save_button());
    layout->addStretch();
    return layout;
}

void Tiddlerstore_Handler::setup_load_button()
{
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
    load_button->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    load_button->setPopupMode(QToolButton::InstantPopup);
    load_button->setMenu(load_history_menu);
}

QToolButton* Tiddlerstore_Handler::setup_save_button()
{
    auto save_menu(new QMenu);
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
    auto save_button(new QToolButton);
    save_button->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    save_button->setPopupMode(QToolButton::InstantPopup);
    save_button->setMenu(save_menu);
    return save_button;
}

void Tiddlerstore_Handler::setup_tiddler_list_view()
{
    title_sort_model.setSourceModel(&title_model);
    title_sort_model.sort(0);
    tiddler_list_view->setModel(&title_sort_model);
    tiddler_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(tiddler_list_view, &QListView::clicked, this, [this](const QModelIndex& index) {
        prepare_open(store[source_row(index.row())].get());
    });
}

QLineEdit* Tiddlerstore_Handler::setup_main_title_filter()
{
    auto tiddler_selector_lineedit(new QLineEdit);
    tiddler_selector_lineedit->setClearButtonEnabled(true);
    main_title_filter_open_action = tiddler_selector_lineedit->addAction(style()->standardIcon(QStyle::SP_DirOpenIcon), QLineEdit::LeadingPosition);
    main_title_filter_open_action->setVisible(false);
    connect(main_title_filter_open_action, &QAction::triggered, this, [this] {
        for (int i = 0; i < title_sort_model.rowCount(); i += 1) {
            if (!tiddler_list_view->isRowHidden(i)) {
                prepare_open(store[source_row(i)].get());
                return;
            }
        }
    });
    main_title_filter_add_action = tiddler_selector_lineedit->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), QLineEdit::LeadingPosition);
    main_title_filter_add_action->setVisible(false);
    connect(main_title_filter_add_action, &QAction::triggered, this, [this, tiddler_selector_lineedit] {
        auto t = store.emplace_back(new Tiddlerstore::Tiddler).get();
        adjust_dirty(true);
        t->set_title(tiddler_selector_lineedit->text().toStdString());
        if (t->title().empty()) {
            t->set_title(tr("New Entry ...").toStdString());
        }
        tiddler_selector_lineedit->clear();
        update_titles_and_filters();
        prepare_open(t);
    });
    main_title_filter_placeholder_action = tiddler_selector_lineedit->addAction(style()->standardIcon(QStyle::SP_FileIcon), QLineEdit::LeadingPosition);
    connect(tiddler_selector_lineedit, &QLineEdit::returnPressed, this, [this] {
        if (main_title_filter_open_action->isVisible()) {
            main_title_filter_open_action->trigger();
        } else {
            if (main_title_filter_add_action->isVisible()) {
                main_title_filter_add_action->trigger();
            }
        }
    });

    connect(tiddler_selector_lineedit, &QLineEdit::textChanged, this, [this] (const QString& text) {
        main_filter_model.request_set_field(main_filter_title_field_name, text.toStdString());
    });
    return tiddler_selector_lineedit;
}

void Tiddlerstore_Handler::update_filters()
{
    auto add_potential_next_filter = [this](QBoxLayout* l) {
        auto potential_next_layout = new QHBoxLayout;
        auto add_label = new QLabel;
        auto potential_next_input = new QComboBox;
        add_label->setPixmap(style()->standardPixmap(QStyle::SP_DialogApplyButton));
        potential_next_input->setModel(&filter_options);
        potential_next_input->setEditable(true);
        potential_next_input->setInsertPolicy(QComboBox::NoInsert);
        potential_next_input->setEditText(QString());
        potential_next_input->completer()->setFilterMode(Qt::MatchContains);
        potential_next_input->completer()->setCaseSensitivity(Qt::CaseInsensitive);
        potential_next_input->completer()->setCompletionMode(QCompleter::PopupCompletion);
        potential_next_layout->addWidget(add_label);
        potential_next_layout->addWidget(potential_next_input);
        potential_next_layout->addStretch();
        l->addLayout(potential_next_layout);
    };

    present_tags = Tiddlerstore::store_tags(store);
    present_fields = Tiddlerstore::store_fields(store);
    present_lists = Tiddlerstore::store_lists(store);

    QStringList opt;
    for (const auto& t : present_tags) {
        opt.append(QString("tag: ") + t.c_str());
    }
    for (const auto& t : present_fields) {
        opt.append(QString("field: ") + t.c_str());
    }
    for (const auto& t : present_lists) {
        opt.append(QString("list: ") + t.c_str());
    }
    filter_options.setStringList(opt);

    auto first_filter_group = new QGroupBox;
    first_filter_group->setContentsMargins(0, 0, 0, 0);
    auto first_filter_group_layout = new QVBoxLayout(first_filter_group);
    first_filter_group_layout->addWidget(setup_main_title_filter());

    clear_layout(filter_layout);
    add_filters_to_layout(first_filter_list_name, first_filter_group_layout);
    add_potential_next_filter(first_filter_group_layout);
    filter_layout->addWidget(first_filter_group);

    for (const auto& filter_list : main_filter_model.list(additional_filter_list_name)) {
        auto additional_filter_group = new QGroupBox;
        additional_filter_group->setContentsMargins(0, 0, 0, 0);
        auto additional_filter_layout = new QVBoxLayout(additional_filter_group);
        add_filters_to_layout(filter_list, additional_filter_layout);
        filter_layout->addWidget(additional_filter_group);
    }

    add_potential_next_filter(filter_layout);
}

void Tiddlerstore_Handler::update_titles_and_filters()
{
    QStringList titles;
    for (const auto& t : store) {
        titles << t->title().c_str();
    }
    title_model.setStringList(titles);
    update_filters();
}

void Tiddlerstore_Handler::connect_model(Tiddler_Model* model)
{
    connect(model, &Tiddler_Model::title_changed,           this, &Tiddlerstore_Handler::update_titles_and_filters);
    connect(model, &Tiddler_Model::title_changed,           this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::text_changed,            this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::history_size_changed,    this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::tags_changed,            this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::field_changed,           this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::field_added,             this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::field_removed,           this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::list_changed,            this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::list_added,              this, &Tiddlerstore_Handler::set_dirty);
    connect(model, &Tiddler_Model::list_removed,            this, &Tiddlerstore_Handler::set_dirty);
}

void Tiddlerstore_Handler::apply_filter()
{
    auto mtf = main_filter_model.field_value(main_filter_title_field_name);
    auto idx = Tiddlerstore::Store_Filter(store).title_contains(mtf).filtered_idx();
    for (int i = 0; i < title_sort_model.rowCount(); i += 1) {
        tiddler_list_view->setRowHidden(i, std::find(idx.begin(), idx.end(), source_row(i)) == idx.end());
    }
    bool show_open = !mtf.empty() && idx.size() == 1;
    bool show_add = !mtf.empty() && idx.empty();
    main_title_filter_open_action->setVisible(show_open);
    main_title_filter_add_action->setVisible(show_add);
    main_title_filter_placeholder_action->setVisible(!(show_open || show_add));
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
    update_titles_and_filters();
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
    auto model = store_model.model_for_tiddler(t);
    if (model) {
        emit open_tiddler_model(model);
    }
}

int Tiddlerstore_Handler::source_row(int filter_row)
{
    return title_sort_model.mapToSource(title_sort_model.index(filter_row, 0)).row();
}

void Tiddlerstore_Handler::add_filters_to_layout(const std::string& list_name, QBoxLayout* l)
{
    (void)l;
    for (const auto& id : main_filter_model.list(list_name)) {
        (void)id;
    }
}
