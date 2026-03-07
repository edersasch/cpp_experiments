#ifndef SRC_FS_HISTORY_QT_FS_HISTORY_UI
#define SRC_FS_HISTORY_QT_FS_HISTORY_UI

#include "fs_history.h"

#include <QObject>
#include <QStandardPaths>
#include <QStringListModel>

class QWidget;
class QComboBox;
class QToolButton;
class QMenu;
class QAction;

class FS_History_UI : public QObject
{
    Q_OBJECT

public:
    FS_History_UI(FsHistory::Operation_Mode mode, const QString& fallback, std::int32_t history_size,
        const QStringList& initial_elements, QObject* parent = nullptr);
    FS_History_UI(const QStringList& initial_elements = {}, QObject* parent = nullptr);
    FS_History_UI(std::int32_t history_size, const QStringList& initial_elements = {}, QObject* parent = nullptr);
    FS_History_UI(const QString& fallback_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        std::int32_t history_size = FsHistory::defaultHistorySize, const QStringList& initial_elements = {},
        QObject* parent = nullptr);
    virtual ~FS_History_UI() override;

    QComboBox* combobox();
    QAction* browse_action(const QString& action_text = "...", const QString& dialog_caption = tr("Please select"),
        const QString& file_filter = QString());
    QToolButton* browse_button(const QString& action_text = "...", const QString& dialog_caption = tr("Please select"),
        const QString& file_filter = QString());
    QMenu* menu(bool use_hotkey = true, const QString& name = "&History", bool append_browse_action = false,
        const QString& action_text = "...", const QString& dialog_caption = tr("Please select"),
        const QString& file_filter = QString());
    void set_current_element(const QString& element) { mFsHistory.set_current_element(element); }
    QStringList get_elements() const { return mModel.stringList(); }

signals:
    void current_element_changed(const QString&);

private:
    void update_menu();
    QComboBox* mChooserComboBox { nullptr };
    QAction* mFileDialogAction { nullptr };
    QToolButton* mFileDialogButton { nullptr };
    QMenu* mFsMenu { nullptr };
    QStringListModel mModel;
    FsHistory mFsHistory;
    bool mMenuUsesHotkey { false };
    bool mMenuAppendsBrowseAction { false };
};

#endif // SRC_FS_HISTORY_QT_FS_HISTORY_UI
