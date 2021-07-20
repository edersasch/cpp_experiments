#include "tiddlerstore_handler_qt.h"
#include "tiddlerstore/tiddlerstore.h"

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

#include <utility>

#include <QDebug>

Flow_List_View::Flow_List_View(QWidget* parent)
    : QListView(parent)
{
    setFlow(QListView::LeftToRight);
    setWrapping(true);
    setResizeMode(QListView::Adjust);
    setSpacing(2);
}

void Flow_List_View::doItemsLayout()
{
    QListView::doItemsLayout();
    setMaximumSize(QWIDGETSIZE_MAX, contentsSize().height() + 2);
}

Move_Only_StringListModel::Move_Only_StringListModel(const QStringList& strings, QObject* parent)
    : QStringListModel(strings, parent)
{
}

Move_Only_StringListModel::Move_Only_StringListModel(QObject* parent)
    : Move_Only_StringListModel({}, parent)
{
}

Qt::ItemFlags Move_Only_StringListModel::flags(const QModelIndex& index) const
{
    return index.isValid() ? QStringListModel::flags(index) & ~Qt::ItemIsDropEnabled : QStringListModel::flags(index);
}

Qt::DropActions Move_Only_StringListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool Move_Only_StringListModel::dropMimeData(const QMimeData* data, Qt::DropAction action,
                  int row, int column, const QModelIndex& parent)
{
    int maxrow = rowCount(parent) - 1;
    if (row == -1 || row > maxrow) {
        row = maxrow; // don't drop behind the add button
    }
    return QStringListModel::dropMimeData(data, action, row, column, parent);
}

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

Tiddlerstore_Handler::~Tiddlerstore_Handler() = default;

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
        prepare_open(store[static_cast<std::size_t>(source_row(index.row()))].get());
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
                    prepare_open(store[static_cast<std::size_t>(source_row(i))].get());
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
        auto single_group = filter_groups.emplace_back(new Tiddlerstore::Single_Group).get();
        single_group->emplace_back(new Tiddlerstore::Filter_Data);
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
    auto empty_group = filter_groups.emplace_back(new Tiddlerstore::Single_Group).get();
    add_single_group(*empty_group);
}

void Tiddlerstore_Handler::add_single_group(Tiddlerstore::Single_Group& single_group)
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
        case Tiddlerstore::Filter_Type::Title:
            if (filter_layout->count() > 0 || filter_data != single_group.front()) {
                del = add_title_filter(*filter_data, filter_form_layout);
            }
            break;
        case Tiddlerstore::Filter_Type::Text:
            del = add_text_filter(*filter_data, filter_form_layout);
            break;
        case Tiddlerstore::Filter_Type::Tag:
            del = add_tag_filter(*filter_data, filter_form_layout);
            tag_opts.removeAll(filter_data->key.c_str());
            break;
        case Tiddlerstore::Filter_Type::Field:
            del = add_field_filter(*filter_data, filter_form_layout);
            break;
        case Tiddlerstore::Filter_Type::List:
            del = add_list_filter(*filter_data, filter_form_layout);
            break;
        }
        if (del) {
            const auto fd_ptr = filter_data.get();
            connect(del, &QToolButton::clicked, del, [this, &single_group, fd_ptr] {
                auto it = std::find_if(single_group.begin(), single_group.end(), [fd_ptr](const auto& f_data) {
                    return f_data.get() == fd_ptr;
                });
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
        auto add_to_group = [this, &index, &single_group](const QStringList& opts, Tiddlerstore::Filter_Data filter_data) {
            if (index < opts.size()) {
                single_group.emplace_back(new Tiddlerstore::Filter_Data(std::move(filter_data)));
                setup_filter();
                apply_filter();
                return true;
            }
            index -= opts.size();
            return false;
        };
        add_to_group(title_opt, {Tiddlerstore::Filter_Type::Title}) ||
        add_to_group(text_opt, {Tiddlerstore::Filter_Type::Text}) ||
        add_to_group(tag_opts, {Tiddlerstore::Filter_Type::Tag, false, index < tag_opts.size() ? tag_opts[index].toStdString() : std::string()}) ||
        add_to_group(field_opts, {Tiddlerstore::Filter_Type::Field, false, index < field_opts.size() ? field_opts[index].toStdString() : std::string()}) ||
        add_to_group(list_opts, {Tiddlerstore::Filter_Type::List, false, index < list_opts.size() ? list_opts[index].toStdString() : std::string()});
    });
    group_layout->addWidget(box);
    filter_layout->addWidget(group_box);
}

void Tiddlerstore_Handler::add_negate_button(Tiddlerstore::Filter_Data& filter_data, QLayout* layout)
{
    auto negate_button = new QToolButton(this);
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    connect(negate_button, &QToolButton::toggled, negate_button, [this, &filter_data](bool checked) {
        filter_data.negate = checked;
        apply_filter();
    });
    layout->addWidget(negate_button);
}

QToolButton* Tiddlerstore_Handler::add_label_del_row(const QString& text, QLayout* single_filter_functions, QFormLayout* filter_form_layout)
{
    auto label = new QLabel(text, this);
    auto delete_button = new QToolButton(this);
    delete_button->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    single_filter_functions->addWidget(delete_button);
    filter_form_layout->addRow(label, single_filter_functions);
    return delete_button;
}

QToolButton* Tiddlerstore_Handler::add_title_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter_data, single_filter_functions);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.key.c_str());
    connect(line_edit, &QLineEdit::textChanged, line_edit, [this, &filter_data](const QString& text) {
        filter_data.key = text.toStdString();
        apply_filter();
    });
    single_filter_functions->addWidget(line_edit);
    return add_label_del_row(tr("Title"), single_filter_functions, filter_form_layout);
}

QToolButton* Tiddlerstore_Handler::add_text_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter_data, single_filter_functions);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.key.c_str());
    connect(line_edit, &QLineEdit::textChanged, line_edit, [this, &filter_data](const QString& text) {
        filter_data.key = text.toStdString();
        apply_filter();
    });
    single_filter_functions->addWidget(line_edit);
    return add_label_del_row(tr("Text"), single_filter_functions, filter_form_layout);
}

QToolButton* Tiddlerstore_Handler::add_tag_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter_data, single_filter_functions);
    auto tag_value = new QLabel(filter_data.key.c_str(), this);
    single_filter_functions->addWidget(tag_value);
    return add_label_del_row(tr("Tag"), single_filter_functions, filter_form_layout);
}

QToolButton* Tiddlerstore_Handler::add_field_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter_data, single_filter_functions);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.field_value.c_str());
    connect(line_edit, &QLineEdit::textChanged, line_edit, [this, &filter_data](const QString& text) {
        filter_data.field_value = text.toStdString();
        apply_filter();
    });
    single_filter_functions->addWidget(line_edit);
    return add_label_del_row(tr("Field: ") + filter_data.key.c_str(), single_filter_functions, filter_form_layout);
}

void Tiddlerstore_Handler::add_list_append_button(QListView* list_view, QStringListModel* list_model)
{
    auto btn = new QToolButton(this);
    btn->setText("+");
    list_view->setIndexWidget(list_model->index(list_model->rowCount() - 1), btn);
    connect(btn, &QToolButton::clicked, btn, [this, list_model, list_view, btn] {
        auto dlg = new QDialog(btn, Qt::Popup);
        auto dlgl = new QHBoxLayout(dlg);
        auto le = new QLineEdit(dlg);
        dlgl->addWidget(le);
        dlg->move(btn->mapToGlobal(QPoint(0, btn->height())));
        dlg->open();
        le->setFocus();
        connect(le, &QLineEdit::returnPressed, dlg, &QDialog::accept);
        connect(dlg, &QDialog::finished, dlg, [this, le, dlg, list_model, list_view](int result) {
            if (result == QDialog::Accepted) {
                auto text = le->text();
                if (!text.isEmpty()) {
                    auto list = list_model->stringList();
                    list.back() = text;
                    list.append("+");
                    list_model->setStringList(list);
                    add_list_append_button(list_view, list_model);
                }
            }
            dlg->deleteLater();
        });
    });
}

QToolButton* Tiddlerstore_Handler::add_list_filter(Tiddlerstore::Filter_Data& filter_data, QFormLayout* filter_form_layout)
{
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter_data, single_filter_functions);
    auto list_view = new Flow_List_View(this);
    list_view->setDropIndicatorShown(true);
    list_view->setDragDropMode(QAbstractItemView::InternalMove);
    auto list_model = new Move_Only_StringListModel({"+"});
    list_view->setModel(list_model);
    add_list_append_button(list_view, list_model);
    single_filter_functions->addWidget(list_view);
    connect(list_model, &QStringListModel::dataChanged, list_model, [this, list_view, list_model](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
        auto list = list_model->stringList();
        int i = topLeft.row() > list.size() - 1 ? list.size() - 1 : topLeft.row();
        int j = bottomRight.row() > list.size() - 1 ? list.size() - 1 : bottomRight.row();
        int orig_sz = list.size();
        while (i <= j) {
            if (list[i].isEmpty()) {
                list.removeAt(i);
                j -= 1;
            } else {
                i += 1;
            }
        }
        if (list.size() != orig_sz) {
            list_model->setStringList(list);
            add_list_append_button(list_view, list_model);
        }
    });
    return add_label_del_row(tr("List: ") + filter_data.key.c_str(), single_filter_functions, filter_form_layout);
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
    auto idx = Tiddlerstore::apply_filter(store, filter_groups);
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
