#include "tiddlerstore_handler_qt.h"
#include "tiddlerstore/tiddlerstore.h"

#include <QMenu>
#include <QLineEdit>
#include <QStyle>
#include <QToolButton>
#include <QStandardPaths>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QCompleter>
#include <QLabel>
#include <QMouseEvent>
#include <QSortFilterProxyModel>

#include <utility>

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

void Flow_List_View::mouseMoveEvent(QMouseEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.row() != hoverRow || state() != QAbstractItemView::NoState) {
        delete_del_button();
        if (index.row() != -1 && state() == QAbstractItemView::NoState) {
            hoverRow = index.row();
            del_button = new QToolButton(this);
            del_button->setStyleSheet("background-color: rgba(255, 255, 255, 0); border: none;");
            auto rect = visualRect(index);
            rect.setWidth(rect.height());
            del_button->setGeometry(rect);
            del_button->show();
            connect(del_button, &QToolButton::clicked, del_button, [this, index] {
                delete_del_button();
                model()->removeRow(index.row());
            });
        }
    }
    QListView::mouseMoveEvent(event);
}

void Flow_List_View::leaveEvent(QEvent* event)
{
    delete_del_button();
    QListView::leaveEvent(event);
}

// private

void Flow_List_View::delete_del_button()
{
    if (del_button) {
        del_button->deleteLater();
    }
    del_button = nullptr;
    hoverRow = -1;
}

Move_Only_StandardItemModel::Move_Only_StandardItemModel(QObject* parent)
    : QStandardItemModel(parent)
{
}

Qt::ItemFlags Move_Only_StandardItemModel::flags(const QModelIndex& index) const
{
    return index.isValid() ? QStandardItemModel::flags(index) & ~Qt::ItemIsDropEnabled : QStandardItemModel::flags(index);
}

Qt::DropActions Move_Only_StandardItemModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool Move_Only_StandardItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action,
                  int row, int column, const QModelIndex& parent)
{
    if (parent.isValid()) {
        return false;
    }
    return QStandardItemModel::dropMimeData(data, action, row, column, parent);
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
    , load_button(new QToolButton(this))
    , load_history_menu(new QMenu(tr("Attention! Unsaved Changes"), this))
    , filter_layout(new QVBoxLayout)
    , always_empty_filter(new Tiddlerstore::Store_Filter(store))
{
    connect(&store_model, &Tiddlerstore_Model::model_created, this, &Tiddlerstore_Handler::connect_model);
    connect(&store_model, &Tiddlerstore_Model::removed, this, &Tiddlerstore_Handler::set_dirty);

    auto elems = tiddlerstore_history.get_elements();
    open_store(elems.isEmpty() ? QString() : elems.first());

    auto title_sort_model = new QSortFilterProxyModel(this);
    title_sort_model->setSourceModel(&title_model);
    title_sort_model->sort(0);

    auto tiddler_list_view = new QListView(this);
    tiddler_list_view->setModel(title_sort_model);
    tiddler_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(tiddler_list_view, &QListView::clicked, tiddler_list_view, [this, title_sort_model](const QModelIndex& index) {
        prepare_open(*filter_group->filtered_tiddlers()[title_sort_model->mapToSource(index).row()]);
    });

    auto main_layout(new QVBoxLayout(this));
    main_layout->addLayout(setup_toolbar());
    main_layout->addLayout(filter_layout);
    main_layout->addWidget(tiddler_list_view);
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

void Tiddlerstore_Handler::setup_main_title_filter()
{
    if (!main_title_filter_edit) {
        main_title_filter_edit = new QLineEdit(this);
        main_title_filter_edit->setClearButtonEnabled(true);
        main_title_filter_open_action = main_title_filter_edit->addAction(style()->standardIcon(QStyle::SP_DirOpenIcon), QLineEdit::LeadingPosition);
        main_title_filter_open_action->setVisible(false);
        connect(main_title_filter_open_action, &QAction::triggered, main_title_filter_open_action, [this] {
            prepare_open(*filter_group->first_filtered_tiddler());
        });
        main_title_filter_add_action = main_title_filter_edit->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), QLineEdit::LeadingPosition);
        main_title_filter_add_action->setVisible(false);
        connect(main_title_filter_add_action, &QAction::triggered, main_title_filter_add_action, [this] {
            auto& t = *store.emplace_back(new Tiddlerstore::Tiddler);
            t.set_title(main_title_filter_edit->text().toStdString());
            if (t.title().empty()) {
                t.set_title(tr("New Entry ...").toStdString());
            }
            main_title_filter_edit->clear();
            prepare_open(t);
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
    if (filter_group->filters().empty()) {
        setup_main_title_filter();
        filter_group->append();
        connect(main_title_filter_edit, &QLineEdit::textChanged, main_title_filter_edit, [this] (const QString& text) {
            filter_group->at(0).clear();
            filter_group->at(0).append(Tiddlerstore::title_contains(text.toStdString()));
            apply_filter();
        });
    } else {
        main_title_filter_edit->parentWidget()->layout()->removeWidget(main_title_filter_edit);
        main_title_filter_edit->setParent(this);
    }
    clear_layout(filter_layout);
    auto filters = filter_group->filters();
    for (auto it = filters.begin() + 1; it != filters.end(); it += 1) {
        add_single_group(**it);
    }
    add_single_group(*always_empty_filter.get());
}

void Tiddlerstore_Handler::add_single_group(Tiddlerstore::Store_Filter& filter)
{
    auto group_box = new QGroupBox(this);
    group_box->setContentsMargins(0, 0, 0, 0);
    //group_box->setFlat(true);
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
    auto elements = filter.elements();
    for (std::size_t i = 0; i < elements.size(); i += 1) {
        QToolButton* del = nullptr;
        switch (elements[i].filter_type) {
        case Tiddlerstore::Filter_Type::Title:
            break;
        case Tiddlerstore::Filter_Type::Title_Contains:
            del = add_title_filter(filter, i, filter_form_layout);
            break;
        case Tiddlerstore::Filter_Type::Text:
            break;
        case Tiddlerstore::Filter_Type::Text_Contains:
            del = add_text_filter(filter, i, filter_form_layout);
            break;
        case Tiddlerstore::Filter_Type::Tag:
            del = add_tag_filter(filter, i, filter_form_layout);
            tag_opts.removeAll(elements[i].key.c_str());
            break;
        case Tiddlerstore::Filter_Type::Tagged:
            break;
        case Tiddlerstore::Filter_Type::Field:
            del = add_field_filter(filter, i, filter_form_layout);
            break;
        case Tiddlerstore::Filter_Type::List:
            del = add_list_filter(filter, i, filter_form_layout);
            break;
        }
        if (del) {
            connect(del, &QToolButton::clicked, del, [this, &filter, i] {
                filter.remove(i);
                if (filter.elements().empty()) {
                    filter_group->remove(filter);
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
    connect(box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), box, [this, title_opt, text_opt, tag_opts, field_opts, list_opts, &filter](int index) {
        auto add_to_group = [this, &index, &filter](const QStringList& opts, Tiddlerstore::Filter_Element filter_data) {
            if (index < opts.size()) {
                &filter == always_empty_filter.get() ? filter_group->append().append(filter_data) : filter.append(filter_data);
                setup_filter();
                apply_filter();
                return true;
            }
            index -= opts.size();
            return false;
        };
        add_to_group(title_opt, {Tiddlerstore::Filter_Type::Title_Contains}) ||
        add_to_group(text_opt, {Tiddlerstore::Filter_Type::Text_Contains}) ||
        add_to_group(tag_opts, {Tiddlerstore::Filter_Type::Tag, false, index < tag_opts.size() ? tag_opts[index].toStdString() : std::string()}) ||
        add_to_group(field_opts, {Tiddlerstore::Filter_Type::Field, false, index < field_opts.size() ? field_opts[index].toStdString() : std::string()}) ||
        add_to_group(list_opts, {Tiddlerstore::Filter_Type::List, false, index < list_opts.size() ? list_opts[index].toStdString() : std::string()});
    });
    group_layout->addWidget(box);
    filter_layout->addWidget(group_box);
}

void Tiddlerstore_Handler::add_negate_button(Tiddlerstore::Store_Filter& filter, std::size_t pos, QLayout* layout)
{
    auto filter_data = filter.elements()[pos];
    auto negate_button = new QToolButton(this);
    negate_button->setText("!");
    negate_button->setCheckable(true);
    negate_button->setChecked(filter_data.negate);
    connect(negate_button, &QToolButton::toggled, negate_button, [this, &filter, pos, filter_data](bool checked) {
        auto cpy = filter_data;
        cpy.negate = checked;
        filter.set_element(pos, cpy);
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

QToolButton* Tiddlerstore_Handler::add_title_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout)
{
    auto filter_data = filter.elements()[pos];
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter, pos, single_filter_functions);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.key.c_str());
    connect(line_edit, &QLineEdit::textChanged, line_edit, [this, &filter, pos](const QString& text) {
        filter.set_element(pos, Tiddlerstore::title_contains(text.toStdString()));
        apply_filter();
    });
    single_filter_functions->addWidget(line_edit);
    single_filter_functions->addStretch();
    return add_label_del_row(tr("Title"), single_filter_functions, filter_form_layout);
}

QToolButton* Tiddlerstore_Handler::add_text_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout)
{
    auto filter_data = filter.elements()[pos];
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter, pos, single_filter_functions);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.key.c_str());
    connect(line_edit, &QLineEdit::textChanged, line_edit, [this, &filter, pos](const QString& text) {
        filter.set_element(pos, Tiddlerstore::text_contains(text.toStdString()));
        apply_filter();
    });
    single_filter_functions->addWidget(line_edit);
    single_filter_functions->addStretch();
    return add_label_del_row(tr("Text"), single_filter_functions, filter_form_layout);
}

QToolButton* Tiddlerstore_Handler::add_tag_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout)
{
    auto filter_data = filter.elements()[pos];
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter, pos, single_filter_functions);
    auto tag_value = new QLabel(filter_data.key.c_str(), this);
    single_filter_functions->addWidget(tag_value);
    single_filter_functions->addStretch();
    return add_label_del_row(tr("Tag"), single_filter_functions, filter_form_layout);
}

QToolButton* Tiddlerstore_Handler::add_field_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout)
{
    auto filter_data = filter.elements()[pos];
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter, pos, single_filter_functions);
    auto line_edit = new QLineEdit(this);
    line_edit->setText(filter_data.field_value.c_str());
    connect(line_edit, &QLineEdit::textChanged, line_edit, [this, &filter, filter_data, pos](const QString& text) {
        filter.set_element(pos, Tiddlerstore::field(filter_data.key, text.toStdString()));
        apply_filter();
    });
    single_filter_functions->addWidget(line_edit);
    single_filter_functions->addStretch();
    return add_label_del_row(tr("Field: ") + filter_data.key.c_str(), single_filter_functions, filter_form_layout);
}

QToolButton* Tiddlerstore_Handler::add_list_filter(Tiddlerstore::Store_Filter& filter, std::size_t pos, QFormLayout* filter_form_layout)
{
    auto filter_data = filter.elements()[pos];
    auto single_filter_functions = new QHBoxLayout;
    add_negate_button(filter, pos, single_filter_functions);
    auto list_view = new Flow_List_View(this);
    list_view->setDropIndicatorShown(true);
    list_view->setDragDropMode(QAbstractItemView::InternalMove);
    list_view->setMouseTracking(true);
    auto list_model = new Move_Only_StandardItemModel;
    list_view->setModel(list_model);
    auto list_handler = new QWidget(this);
    auto list_handler_layout = new QVBoxLayout(list_handler);
    list_handler_layout->addWidget(list_view);
    auto entry_enter_edit = new QLineEdit(this);
    entry_enter_edit->setClearButtonEnabled(true);
    entry_enter_edit->addAction(style()->standardIcon(QStyle::SP_DialogApplyButton), QLineEdit::LeadingPosition);
    auto add_to_list_model = [this, list_model, list_view](const std::string& text) {
        auto item = new QStandardItem(text.c_str());
        list_model->appendRow(item);
        auto rect = list_view->visualRect(item->index());
        if (list_view->iconSize().height() > rect.height()) {
            list_view->setIconSize(QSize(rect.height(), rect.height()));
        }
        item->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    };
    for (const auto& text : filter_data.list_value) {
        add_to_list_model(text);
    }
    connect(entry_enter_edit, &QLineEdit::returnPressed, entry_enter_edit, [add_to_list_model, entry_enter_edit]() {
        auto text = entry_enter_edit->text().toStdString();
        if (!text.empty()) {
            add_to_list_model(text);
            entry_enter_edit->clear();
        }
    });
    list_handler_layout->addWidget(entry_enter_edit);
    // try to make the empty list view as high as a single line
    list_view->setMinimumSize(QSize(1, entry_enter_edit->height() - entry_enter_edit->contentsMargins().top() - entry_enter_edit->contentsMargins().bottom()));
    single_filter_functions->addWidget(list_handler);
    single_filter_functions->addStretch();
    connect(list_model, &QStandardItemModel::dataChanged, list_model, [&filter, pos, list_model](const QModelIndex& topLeft, const QModelIndex& bottomRight) {
        auto data_changed_filter_data = filter.elements().at(pos);
        int i = topLeft.row();
        int j = bottomRight.row();
        while (i <= j) {
            auto text = list_model->item(i)->text();
            if (text.isEmpty()) {
                list_model->removeRow(i);
                j -= 1;
            } else {
                if (static_cast<std::size_t>(i) < data_changed_filter_data.list_value.size()) {
                    data_changed_filter_data.list_value[static_cast<std::size_t>(i)] = text.toStdString();
                }
                i += 1;
            }
        }
        filter.set_element(pos, data_changed_filter_data);
        //apply_filter()
    });
    connect(list_model, &QStandardItemModel::rowsRemoved, list_model, [&filter, pos](const QModelIndex&, int first, int last) {
        auto rows_removed_filter_data = filter.elements().at(pos);
        for (int i = first; i <= last && i < static_cast<int>(rows_removed_filter_data.list_value.size()); i += 1) {
            rows_removed_filter_data.list_value.erase(rows_removed_filter_data.list_value.begin() + i);
        }
        filter.set_element(pos, rows_removed_filter_data);
        //apply_filter()
    });
    connect(list_model, &QStandardItemModel::rowsInserted, list_model, [&filter, pos, list_model](const QModelIndex&, int first, int last) {
        auto rows_inserted_filter_data = filter.elements().at(pos);
        for (int i = first; i <= last; i += 1) {
            auto item = list_model->item(i);
            rows_inserted_filter_data.list_value.insert(rows_inserted_filter_data.list_value.begin() + i, item ? list_model->item(i)->text().toStdString() : std::string());
        }
        filter.set_element(pos, rows_inserted_filter_data);
        //apply_filter()
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
    QStringList titles;
    const auto& tiddlers = filter_group->filtered_tiddlers();
    for (const auto& t : tiddlers) {
        titles << t->title().c_str();
    }
    title_model.setStringList(titles);
    bool show_open = tiddlers.size() == 1;
    bool show_add = tiddlers.empty();
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
    if (is_dirty) {
        filter_group->invalidate();
    }
    setup_filter();
    apply_filter();
}

void Tiddlerstore_Handler::open_store(const QString& path)
{
    if (is_dirty || path != currently_loaded_store) {
        currently_loaded_store = path;
        if (!path.isEmpty()) {
            store = Tiddlerstore::open_store_from_file(path.toStdString());
        }
        filter_group.reset(new Tiddlerstore::Filter_Group(store));
        adjust_dirty(false);
    }
}

void Tiddlerstore_Handler::save_store(const QString& path)
{
    if (Tiddlerstore::save_store_to_file(store, path.toStdString())) {
        tiddlerstore_history.set_current_element(path);
        adjust_dirty(false);
    }
}

void Tiddlerstore_Handler::prepare_open(Tiddlerstore::Tiddler& t)
{
    auto model = store_model.model_for_tiddler(t);
    if (model) {
        emit open_tiddler_model(model);
    }
}
