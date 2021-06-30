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

#include <QDebug>

namespace
{

void clear_layout(QLayout* l)
{
    QLayoutItem* child;
    while ((child = l->takeAt(0) ) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
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

void Tiddlerstore_Handler::setup_filter()
{
    if (filter_groups.empty()) {
        auto single_group = filter_groups.emplace_back(new Single_Group).get();
        single_group->emplace_back(new Filter_Data{Title});
        setup_main_title_filter();
        connect(main_title_filter_edit, &QLineEdit::textChanged, main_title_filter_edit, [this, single_group] (const QString& text) {
            single_group->front()->key = text.toStdString();
            apply_filter();
        });
    } else {
        main_title_filter_edit->parentWidget()->layout()->removeWidget(main_title_filter_edit);
        main_title_filter_edit->setParent(this);
        clear_layout(filter_layout);
    }
    filter_groups.erase(std::remove_if(filter_groups.begin(), filter_groups.end(), [this](auto& single_group) {
        if (single_group->empty()) {
            return true;
        }
        add_single_group(*single_group);
        return false;
    }), filter_groups.end());
    auto empty_group = filter_groups.emplace_back(new Single_Group).get();
    add_single_group(*empty_group);
}

void Tiddlerstore_Handler::add_single_group(Single_Group& single_group)
{
    auto group_box = new QGroupBox(this);
    group_box->setContentsMargins(0, 0, 0, 0);
    group_box->setFlat(true);
    auto group_layout = new QVBoxLayout(group_box);
    auto filter_form_layout = new QFormLayout;
    QStringList title_opt {"title"};
    QStringList text_opt {"text"};
    QStringList tag_opts;
    QStringList field_opts;
    QStringList list_opts;
    for (const auto& t : present_tags) {
        tag_opts.append(t.c_str());
    }
    for (const auto& t : present_fields) {
        field_opts.append(t.c_str());
    }
    for (const auto& t : present_lists) {
        list_opts.append(t.c_str());
    }
    if (filter_layout->count() == 0) {
        group_layout->addWidget(main_title_filter_edit);
    }
    group_layout->addLayout(filter_form_layout);
    for (auto& filter_data : single_group) {
        QToolButton* del = nullptr;
        switch (filter_data->filter_type) {
        case Title:
            if (filter_layout->count() > 0) {
                del = add_title_filter(*filter_data, filter_form_layout);
            }
            title_opt.clear();
            break;
        case Text:
            add_text_filter(*filter_data, filter_form_layout);
            text_opt.clear();
            break;
        case Tag:
            del = add_tag_filter(*filter_data, filter_form_layout);
            tag_opts.removeAll(filter_data->key.c_str());
            break;
        case Field:
            del = add_field_filter(*filter_data, filter_form_layout);
            field_opts.removeAll(filter_data->key.c_str());
            break;
        case List:
            add_list_filter(*filter_data, filter_form_layout);
            list_opts.removeAll(filter_data->key.c_str());
            break;
        }
        if (del) {
            connect(del, &QToolButton::clicked, del, [this, &single_group, &filter_data] {
                auto it = std::find(single_group.begin(), single_group.end(), filter_data);
                if(it != single_group.end()) {
                    single_group.erase(it);
                }
                setup_filter();
                apply_filter();
            });
        }
    }
    QStringList remaining_opts;
    remaining_opts << title_opt;
    remaining_opts << text_opt;
    for (const auto& o : tag_opts) {
        remaining_opts.append(QString("tag: ") + o);
    }
    for (const auto& o : field_opts) {
        remaining_opts.append(QString("field: ") + o);
    }
    for (const auto& o : list_opts) {
        remaining_opts.append(QString("list: ") + o);
    }
    auto box = new QComboBox(this);
    box->insertItems(0, remaining_opts);
    box->setEditable(true);
    box->setInsertPolicy(QComboBox::NoInsert);
    box->setEditText(QString());
    box->setCurrentIndex(-1);
    box->completer()->setFilterMode(Qt::MatchContains);
    box->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    box->completer()->setCompletionMode(QCompleter::PopupCompletion);
    connect(box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), box, [this, title_opt, text_opt, tag_opts, field_opts, list_opts, &single_group](int index) {
        int offs = 0;
        int sz = title_opt.size();
        if (index < offs + sz) {
            single_group.emplace_back(new Filter_Data{Title});
            setup_filter();
            apply_filter();
            qDebug() << "title " << title_opt[index - offs];
            return;
        }
        offs += sz;
        sz = text_opt.size();
        if (index < offs + sz) {
            qDebug() << "text " << tag_opts[index - offs];
            return;
        }
        offs += sz;
        sz = tag_opts.size();
        if (index < offs + sz) {
            single_group.emplace_back(new Filter_Data{Tag, false, tag_opts[index - offs].toStdString()});
            setup_filter();
            apply_filter();
            qDebug() << "tag " << tag_opts[index - offs];
            return;
        }
        offs += sz;
        sz = field_opts.size();
        if (index < offs + sz) {
            single_group.emplace_back(new Filter_Data{Field, false, field_opts[index - offs].toStdString()});
            setup_filter();
            apply_filter();
            qDebug() << "field " << field_opts[index - offs];
            return;
        }
        offs += sz;
        sz = list_opts.size();
        if (index < offs + sz) {
            qDebug() << "list " << list_opts[index - offs];
            return;
        }
    });
    group_layout->addWidget(box);
    filter_layout->addWidget(group_box);
}

QToolButton* Tiddlerstore_Handler::add_title_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    connect(negate_button, &QToolButton::toggled, negate_button, [this, &filter_data](bool checked) {
        filter_data.negate = checked;
        apply_filter();
    });
    single_filter_functions->addWidget(negate_button);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.key.c_str());
    connect(line_edit, &QLineEdit::textChanged, line_edit, [this, &filter_data](const QString& text) {
        filter_data.key = text.toStdString();
        apply_filter();
    });
    single_filter_functions->addWidget(line_edit);
    auto label = new QLabel(tr("Title"), this);
    auto delete_button = new QToolButton(this);
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
    return delete_button;
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

QToolButton* Tiddlerstore_Handler::add_tag_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    connect(negate_button, &QToolButton::toggled, negate_button, [this, &filter_data](bool checked) {
        filter_data.negate = checked;
        apply_filter();
    });
    single_filter_functions->addWidget(negate_button);
    auto tag_value = new QLabel(filter_data.key.c_str(), this);
    single_filter_functions->addWidget(tag_value);
    auto label = new QLabel(tr("Tag"), this);
    auto delete_button = new QToolButton(this);
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
    return delete_button;
}

QToolButton* Tiddlerstore_Handler::add_field_filter(Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    connect(negate_button, &QToolButton::toggled, negate_button, [this, &filter_data](bool checked) {
        filter_data.negate = checked;
        apply_filter();
    });
    single_filter_functions->addWidget(negate_button);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.field_value.c_str());
    connect(line_edit, &QLineEdit::textChanged, line_edit, [this, &filter_data](const QString& text) {
        filter_data.field_value = text.toStdString();
        apply_filter();
    });
    single_filter_functions->addWidget(line_edit);
    auto label = new QLabel(tr("Field: ") + filter_data.key.c_str(), this);
    auto delete_button = new QToolButton(this);
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
    return delete_button;
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
    set_dirty();
}

void Tiddlerstore_Handler::apply_filter()
{
    auto all_filter = Tiddlerstore::Store_Filter(store).clear();
    for (auto& single_group : filter_groups) {
        if (!single_group->empty()) {
            auto grfi = Tiddlerstore::Store_Filter(store);
            for (auto& filter_data : *single_group) {
                switch (filter_data->filter_type) {
                case Title:
                    filter_data->negate ? grfi.n_title_contains(filter_data->key) : grfi.title_contains(filter_data->key);
                    break;
                case Text:
                    //data.negate ? group_filter
                    break;
                case Tag:
                    filter_data->negate ? grfi.n_tag(filter_data->key) : grfi.tag(filter_data->key);
                    break;
                case Field:
                    filter_data->negate ? grfi.n_field(filter_data->key, filter_data->field_value) : grfi.field(filter_data->key, filter_data->field_value);
                    break;
                case List:
                    filter_data->negate ? grfi.n_list(filter_data->key, filter_data->list_value) : grfi.list(filter_data->key, filter_data->list_value);
                    break;
                }
                if (grfi.filtered_idx().empty()) {
                    break;
                }
            }
            all_filter.join(grfi);
        }
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
    setup_filter();
    apply_filter();
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
