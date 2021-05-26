#ifndef SRC_FS_HISTORY_QT_FS_HISTORY_UI
#define SRC_FS_HISTORY_QT_FS_HISTORY_UI

#include "fs_history.h"

#include <QObject>
#include <QStringListModel>
#include <QStandardPaths>

class QWidget;
class QComboBox;
class QToolButton;
class QMenu;
class QAction;

class FS_History_UI
        : public QObject
{
    Q_OBJECT

public:
    FS_History_UI(FS_History::Operation_Mode mode, const QString& fallback, std::int32_t history_size, const QStringList& initial_elements, QObject* parent = nullptr);
    FS_History_UI(const QStringList& initial_elements = {}, QObject* parent = nullptr);
    FS_History_UI(std::int32_t history_size, const QStringList& initial_elements = {}, QObject* parent = nullptr);
    FS_History_UI(const QString& fallback_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation), std::int32_t history_size = FS_History::default_history_size, const QStringList& initial_elements = {}, QObject* parent = nullptr);
    virtual ~FS_History_UI() override = default;

    QComboBox* combobox();
    QAction* browse_action(const QString& action_text = "...", const QString& dialog_caption = tr("Please select"), const QString& file_filter = QString());
    QToolButton* browse_button(const QString& action_text = "...", const QString& dialog_caption = tr("Please select"), const QString& file_filter = QString());
    QMenu* menu(bool use_hotkey = true, const QString& name = "&History", bool append_browse_action = false, const QString& action_text = "...", const QString& dialog_caption = tr("Please select"), const QString& file_filter = QString());
    void set_current_element(const QString& element) { history.set_current_element(element); }
    QStringList get_elements() const { return model.stringList(); }

signals:
    void current_element_changed(const QString&);

private:
    void update_menu();
    QComboBox* chooser {nullptr};
    QAction* file_dialog_action {nullptr};
    QToolButton* file_dialog_button {nullptr};
    QMenu* fs_menu {nullptr};
    QStringListModel model;
    FS_History history;
    bool menu_uses_hotkey {false};
    bool menu_appends_browse_action {false};
};

#endif // SRC_FS_HISTORY_QT_FS_HISTORY_UI
