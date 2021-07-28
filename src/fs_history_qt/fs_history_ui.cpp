#include "fs_history_ui.h"

#include <QToolButton>
#include <QComboBox>
#include <QFileDialog>
#include <QMenu>

FS_History_UI::FS_History_UI(FS_History::Operation_Mode mode, const QString& fallback, std::int32_t history_size, const QStringList& initial_elements, QObject* parent)
    : QObject(parent)
    , history(mode, fallback, history_size, initial_elements)
{
    model.setStringList(history.get_elements());
    connect(&history, &FS_History::elements_changed, this, [this](const QStringList& list) {
        model.setStringList(list);
        if (chooser) {
            chooser->setCurrentIndex(0);
        }
        if (fs_menu) {
            update_menu();
        }
        emit current_element_changed(list.isEmpty() ? QString() : list.first());
    });
}

FS_History_UI::FS_History_UI(const QStringList& initial_elements, QObject* parent)
    : FS_History_UI(FS_History::Operation_Mode::OP_FILE, "", FS_History::default_history_size, initial_elements, parent)
{
}

FS_History_UI::FS_History_UI(std::int32_t history_size, const QStringList& initial_elements, QObject* parent)
    : FS_History_UI(FS_History::Operation_Mode::OP_FILE, "", history_size, initial_elements, parent)
{
}

FS_History_UI::FS_History_UI(const QString& fallback_dir, std::int32_t history_size, const QStringList& initial_elements, QObject* parent)
    : FS_History_UI(FS_History::Operation_Mode::OP_DIR, fallback_dir, history_size, initial_elements, parent)
{
}

QComboBox* FS_History_UI::combobox()
{
    if (!chooser) {
        chooser = new QComboBox;
        chooser->setModel(&model);
        connect(chooser, &QComboBox::currentTextChanged, &history, &FS_History::set_current_element);
    }
    return chooser;
}

QAction* FS_History_UI::browse_action(const QString& action_text, const QString& dialog_caption, const QString& file_filter)
{
    if (!file_dialog_action) {
        file_dialog_action = new QAction(action_text);
        connect(file_dialog_action, &QAction::triggered, this, [this, dialog_caption, file_filter] {
            auto location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
            auto list = history.get_elements();
            if (!list.isEmpty()) {
                location = list.first();
            }
            switch (history.operation_mode()) {
            case FS_History::Operation_Mode::OP_FILE:
                history.set_current_element(QFileDialog::getOpenFileName(nullptr, dialog_caption, location, file_filter));
                break;
            case FS_History::Operation_Mode::OP_DIR:
                history.set_current_element(QFileDialog::getExistingDirectory(nullptr, dialog_caption, location));
                break;
            }
        });
    }
    return file_dialog_action;
}

QToolButton* FS_History_UI::browse_button(const QString& action_text, const QString& dialog_caption, const QString& file_filter)
{
    if (!file_dialog_button) {
        file_dialog_button = new QToolButton;
        file_dialog_button->setDefaultAction(browse_action(action_text, dialog_caption, file_filter));
    }
    return file_dialog_button;
}

QMenu* FS_History_UI::menu(bool use_hotkey, const QString& name, bool append_browse_action, const QString& action_text, const QString& dialog_caption, const QString& file_filter)
{
    if (!fs_menu) {
        fs_menu = new QMenu(name);
        menu_uses_hotkey = use_hotkey;
        menu_appends_browse_action = append_browse_action;
        if (menu_appends_browse_action && !file_dialog_action) {
            browse_action(action_text, dialog_caption, file_filter);
        }
        update_menu();
    }
    return fs_menu;
}

// private

void FS_History_UI::update_menu()
{
    fs_menu->clear();
    auto elms = history.get_elements();
    bool prepend_hotkey = elms.size() <= FS_History::default_history_size;
    std::int32_t hotkey = 1;
    for (auto element : elms) {
        auto a = fs_menu->addAction(element);
        connect(a, &QAction::triggered, this, [this, element] {
            history.set_current_element(element);
        });
        if (prepend_hotkey && menu_uses_hotkey) {
            element.prepend("&" + QString::number(hotkey) + " ");
            a->setText(element);
            hotkey += 1;
        }
    }
    if (menu_appends_browse_action) {
        fs_menu->addAction(file_dialog_action);
    }
}
