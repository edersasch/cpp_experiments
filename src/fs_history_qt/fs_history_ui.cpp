#include "fs_history_ui.h"

#include <QComboBox>
#include <QFileDialog>
#include <QMenu>
#include <QToolButton>

FS_History_UI::FS_History_UI(FsHistory::Operation_Mode mode, const QString& fallback, std::int32_t history_size,
    const QStringList& initial_elements, QObject* parent)
: QObject(parent)
, mFsHistory(mode, fallback, history_size, initial_elements)
{
    mModel.setStringList(mFsHistory.get_elements());
    connect(&mFsHistory, &FsHistory::elements_changed, this, [this](const QStringList& list) {
        mModel.setStringList(list);
        if (mChooserComboBox) {
            mChooserComboBox->setCurrentIndex(0);
        }
        if (mFsMenu) {
            update_menu();
        }
        emit current_element_changed(list.isEmpty() ? QString() : list.first());
    });
}

FS_History_UI::FS_History_UI(const QStringList& initial_elements, QObject* parent)
: FS_History_UI(FsHistory::Operation_Mode::OP_FILE, "", FsHistory::defaultHistorySize, initial_elements, parent)
{
}

FS_History_UI::FS_History_UI(std::int32_t history_size, const QStringList& initial_elements, QObject* parent)
: FS_History_UI(FsHistory::Operation_Mode::OP_FILE, "", history_size, initial_elements, parent)
{
}

FS_History_UI::FS_History_UI(
    const QString& fallback_dir, std::int32_t history_size, const QStringList& initial_elements, QObject* parent)
: FS_History_UI(FsHistory::Operation_Mode::OP_DIR, fallback_dir, history_size, initial_elements, parent)
{
}

FS_History_UI::~FS_History_UI() = default;

QComboBox* FS_History_UI::combobox()
{
    if (mChooserComboBox == nullptr) {
        mChooserComboBox = new QComboBox;
        mChooserComboBox->setModel(&mModel);
        connect(mChooserComboBox, &QComboBox::currentTextChanged, &mFsHistory, &FsHistory::set_current_element);
    }
    return mChooserComboBox;
}

QAction* FS_History_UI::browse_action(
    const QString& action_text, const QString& dialog_caption, const QString& file_filter)
{
    if (mFileDialogAction == nullptr) {
        mFileDialogAction = new QAction(action_text);
        connect(mFileDialogAction, &QAction::triggered, this, [this, dialog_caption, file_filter] {
            auto location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
            auto list = mFsHistory.get_elements();
            if (!list.isEmpty()) {
                location = list.first();
            }
            switch (mFsHistory.operation_mode()) {
            case FsHistory::Operation_Mode::OP_FILE:
                mFsHistory.set_current_element(
                    QFileDialog::getOpenFileName(nullptr, dialog_caption, location, file_filter));
                break;
            case FsHistory::Operation_Mode::OP_DIR:
                mFsHistory.set_current_element(QFileDialog::getExistingDirectory(nullptr, dialog_caption, location));
                break;
            }
        });
    }
    return mFileDialogAction;
}

QToolButton* FS_History_UI::browse_button(
    const QString& action_text, const QString& dialog_caption, const QString& file_filter)
{
    if (mFileDialogButton == nullptr) {
        mFileDialogButton = new QToolButton;
        mFileDialogButton->setDefaultAction(browse_action(action_text, dialog_caption, file_filter));
    }
    return mFileDialogButton;
}

QMenu* FS_History_UI::menu(bool use_hotkey, const QString& name, bool append_browse_action, const QString& action_text,
    const QString& dialog_caption, const QString& file_filter)
{
    if (mFsMenu == nullptr) {
        mFsMenu = new QMenu(name);
        mMenuUsesHotkey = use_hotkey;
        mMenuAppendsBrowseAction = append_browse_action;
        if (mMenuAppendsBrowseAction && mFileDialogAction == nullptr) {
            browse_action(action_text, dialog_caption, file_filter);
        }
        update_menu();
    }
    return mFsMenu;
}

// private

void FS_History_UI::update_menu()
{
    mFsMenu->clear();
    auto elms = mFsHistory.get_elements();
    const bool prepend_hotkey = elms.size() <= FsHistory::defaultHistorySize;
    std::int32_t hotkey = 1;
    for (auto element : elms) {
        auto* action = mFsMenu->addAction(element);
        connect(action, &QAction::triggered, this, [this, element] { mFsHistory.set_current_element(element); });
        if (prepend_hotkey && mMenuUsesHotkey) {
            element.prepend("&" + QString::number(hotkey) + " ");
            action->setText(element);
            hotkey += 1;
        }
    }
    if (mMenuAppendsBrowseAction) {
        mFsMenu->addAction(mFileDialogAction);
    }
}
