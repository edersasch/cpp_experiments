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
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QStringListModel>
#include <QCompleter>
#include <QLabel>

Tiddlerstore_Handler::Tiddlerstore_Handler(const QStringList& tiddlerstore_list, QWidget* parent)
    : QWidget(parent)
    , tiddlerstore_history(tiddlerstore_list)
    , tiddler_list_view(new QListView(this))
    , load_button(new QToolButton(this))
    , load_history_menu(new QMenu(tr("Attention! Unsaved Changes"), this))
    , filter_layout(new QVBoxLayout)
{
    auto main_layout(new QVBoxLayout(this));
    main_layout->addLayout(setup_toolbar());
    setup_filter();
    main_layout->addLayout(filter_layout);
    setup_tiddler_list_view();
    main_layout->addWidget(tiddler_list_view);

    connect(&store_model, &Tiddlerstore_Model::model_created, this, &Tiddlerstore_Handler::connect_model);
    connect(&store_model, &Tiddlerstore_Model::removed, this, &Tiddlerstore_Handler::set_dirty);

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
    connect(load_history_menu, &QMenu::aboutToShow, load_history_menu, [this] {
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
    auto save_menu(new QMenu(this));
    connect(save_menu, &QMenu::aboutToShow, save_menu, [this, save_menu] {
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
        connect(save_other, &QAction::triggered, save_other, [this, default_location] {
            auto other_name = QFileDialog::getSaveFileName(nullptr, tr("Where do you want to save?"), default_location);
            if (!other_name.isEmpty()) {
                save_store(other_name);
            }
        });
    });
    auto save_button(new QToolButton(this));
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
    connect(tiddler_list_view, &QListView::clicked, tiddler_list_view, [this](const QModelIndex& index) {
        prepare_open(store[source_row(index.row())].get());
    });
}

void Tiddlerstore_Handler::setup_main_title_filter()
{
    if (!main_title_filter_edit) {
        main_title_filter_edit = new QLineEdit(this);
        main_title_filter_edit->setClearButtonEnabled(true);
        main_title_filter_open_action = main_title_filter_edit->addAction(style()->standardIcon(QStyle::SP_DirOpenIcon), QLineEdit::LeadingPosition);
        main_title_filter_open_action->setVisible(false);
        connect(main_title_filter_open_action, &QAction::triggered, main_title_filter_open_action, [this] {
            for (int i = 0; i < title_sort_model.rowCount(); i += 1) {
                if (!tiddler_list_view->isRowHidden(i)) {
                    prepare_open(store[source_row(i)].get());
                    return;
                }
            }
        });
        main_title_filter_add_action = main_title_filter_edit->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), QLineEdit::LeadingPosition);
        main_title_filter_add_action->setVisible(false);
        connect(main_title_filter_add_action, &QAction::triggered, main_title_filter_add_action, [this] {
            auto t = store.emplace_back(new Tiddlerstore::Tiddler).get();
            adjust_dirty(true);
            t->set_title(main_title_filter_edit->text().toStdString());
            if (t->title().empty()) {
                t->set_title(tr("New Entry ...").toStdString());
            }
            main_title_filter_edit->clear();
            prepare_open(t);
            set_dirty();
        });
        main_title_filter_placeholder_action = main_title_filter_edit->addAction(style()->standardIcon(QStyle::SP_FileIcon), QLineEdit::LeadingPosition);
        connect(main_title_filter_edit, &QLineEdit::returnPressed, main_title_filter_edit, [this] {
            if (main_title_filter_open_action->isVisible()) {
                main_title_filter_open_action->trigger();
            } else {
                if (main_title_filter_add_action->isVisible()) {
                    main_title_filter_add_action->trigger();
                }
            }
        });
    }
}

QStringList Tiddlerstore_Handler::next_filter_options(const QStringList& dont_list)
{
    QStringList opts;
    (void)dont_list;
    for (const auto& t : present_tags) {
        opts.append(QString("tag: ") + t.c_str());
    }
    for (const auto& t : present_fields) {
        opts.append(QString("field: ") + t.c_str());
    }
    for (const auto& t : present_lists) {
        opts.append(QString("list: ") + t.c_str());
    }
    return opts;
}

QComboBox* Tiddlerstore_Handler::select_next_filter_combobox(const QStringList& dont_list)
{
    auto box = new QComboBox(this);
    box->insertItems(0, next_filter_options(dont_list));
    box->setEditable(true);
    box->setInsertPolicy(QComboBox::NoInsert);
    box->setEditText(QString());
    box->completer()->setFilterMode(Qt::MatchContains);
    box->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    box->completer()->setCompletionMode(QCompleter::PopupCompletion);
    connect(this, &Tiddlerstore_Handler::store_changed, box->model(), [this, box, dont_list] {
        box->clear();
        box->insertItems(0, next_filter_options(dont_list));
    });
    return box;
}

void Tiddlerstore_Handler::setup_filter()
{
    if (filter_groups.empty()) {
        filter_groups.emplace_back(new Single_Group);
    }
    for (auto& single_group : filter_groups) {
        add_single_group(*single_group);
    }
    Single_Group next_group;
    add_single_group(next_group);
}

void Tiddlerstore_Handler::add_single_group(Single_Group& single_group)
{
    auto group_box = new QGroupBox(this);
    group_box->setContentsMargins(0, 0, 0, 0);
    group_box->setFlat(true);
    auto group_layout = new QVBoxLayout(group_box);
    auto filter_form_layout = new QFormLayout;
    QStringList used_keys;
    if (filter_layout->isEmpty()) {
        if (!main_title_filter_edit) {
            setup_main_title_filter();
            connect(main_title_filter_edit, &QLineEdit::textChanged, main_title_filter_edit, [this, &single_group] (const QString& text) {
                single_group[Title].key = text.toStdString();
                apply_filter();
            });
        }
        group_layout->addWidget(main_title_filter_edit);
        used_keys << Tiddlerstore::Tiddler::title_key;
    }
    group_layout->addLayout(filter_form_layout);
    for (auto& [key, value] : single_group) {
        switch (key) {
        case Title:
            if (!filter_layout->isEmpty()) {
                add_title_filter(value, filter_form_layout);
                used_keys << Tiddlerstore::Tiddler::title_key;
            }
            break;
        case Text:
            add_text_filter(value, filter_form_layout);
            used_keys << Tiddlerstore::Tiddler::text_history_key;
            break;
        case Tag:
            add_tag_filter(value, filter_form_layout);
            used_keys << Tiddlerstore::Tiddler::tags_key;
            break;
        case Field:
            add_field_filter(value, filter_form_layout);
            used_keys << Tiddlerstore::Tiddler::fields_key;
            break;
        case List:
            add_list_filter(value, filter_form_layout);
            used_keys << Tiddlerstore::Tiddler::lists_key;
            break;
        }
    }
    group_layout->addWidget(select_next_filter_combobox(used_keys));
    filter_layout->addWidget(group_box);
}

void Tiddlerstore_Handler::add_title_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    single_filter_functions->addWidget(negate_button);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.key.c_str());
    single_filter_functions->addWidget(line_edit);
    auto label = new QLabel(tr("Title"), this);
    auto delete_button = new QToolButton(this);
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
}

void Tiddlerstore_Handler::add_text_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    single_filter_functions->addWidget(negate_button);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.key.c_str());
    single_filter_functions->addWidget(line_edit);
    auto label = new QLabel(tr("Text"), this);
    auto delete_button = new QToolButton(this);
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
}

void Tiddlerstore_Handler::add_tag_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    single_filter_functions->addWidget(negate_button);
    auto tag_value = new QLabel(filter_data.key.c_str(), this);
    single_filter_functions->addWidget(tag_value);
    auto label = new QLabel(tr("Tag") + filter_data.key.c_str(), this);
    auto delete_button = new QToolButton(this);
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
}

void Tiddlerstore_Handler::add_field_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    single_filter_functions->addWidget(negate_button);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.field_value.c_str());
    single_filter_functions->addWidget(line_edit);
    auto label = new QLabel(tr("Field: ") + filter_data.key.c_str(), this);
    auto delete_button = new QToolButton(this);
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
}

void Tiddlerstore_Handler::add_list_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    single_filter_functions->addWidget(negate_button);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.field_value.c_str()); // TODO: add list handling
    single_filter_functions->addWidget(line_edit);
    auto label = new QLabel(tr("List: ") + filter_data.key.c_str(), this);
    auto delete_button = new QToolButton(this);
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
}

void Tiddlerstore_Handler::connect_model(Tiddler_Model* model)
{
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
    auto all_filter = Tiddlerstore::Store_Filter(store).clear();
    for (auto& single_group : filter_groups) {
        auto grfi = Tiddlerstore::Store_Filter(store);
        for (auto& [k, v] : *single_group) {
            switch (k) {
            case Title:
                v.negate ? grfi.n_title_contains(v.key) : grfi.title_contains(v.key);
                break;
            case Text:
                //data.negate ? group_filter
                break;
            case Tag:
                v.negate ? grfi.n_tag(v.key) : grfi.tag(v.key);
                break;
            case Field:
                v.negate ? grfi.n_field(v.key, v.field_value) : grfi.field(v.key, v.field_value);
                break;
            case List:
                v.negate ? grfi.n_list(v.key, v.list_value) : grfi.list(v.key, v.list_value);
                break;
            }
            if (grfi.filtered_idx().empty()) {
                break;
            }
        }
        all_filter.join(grfi);
    }
    auto idx = all_filter.filtered_idx();
    for (int i = 0; i < title_sort_model.rowCount(); i += 1) {
        tiddler_list_view->setRowHidden(i, std::find(idx.begin(), idx.end(), source_row(i)) == idx.end());
    }
    bool show_open = idx.size() == 1;
    bool show_add = idx.empty();
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
    present_tags = Tiddlerstore::store_tags(store);
    present_fields = Tiddlerstore::store_fields(store);
    present_lists = Tiddlerstore::store_lists(store);
    QStringList titles;
    for (const auto& t : store) {
        titles << t->title().c_str();
    }
    title_model.setStringList(titles);
    apply_filter();
    emit store_changed();
}

void Tiddlerstore_Handler::open_store(const QString& path)
{
    store = Tiddlerstore::open_store_from_file(path.toStdString());
    adjust_dirty(false);
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
