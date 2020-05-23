#ifndef SRC_FS_HISTORY_QT_FS_HISTORY_UI
#define SRC_FS_HISTORY_QT_FS_HISTORY_UI

#include "fs_history.h"

#include <QObject>
#include <QStringListModel>
#include <QStandardPaths>
#include <QProxyStyle>

class QWidget;
class QComboBox;
class QToolButton;
class QTextOption;
class QPainter;
class QMenu;

class Combobox_Label_Elide_Left_Proxy_Style : public QProxyStyle
{
  public:
    virtual void drawControl(QStyle::ControlElement element, const QStyleOption* combobox_option, QPainter* painter, const QWidget* widget = nullptr) const override;
};

class FS_History_UI
        : public QObject
{
    Q_OBJECT

public:
    FS_History_UI(FS_History::Operation_Mode mode, const QString& fallback, int history_size, const QStringList& initial_elements, QObject* parent = nullptr);
    FS_History_UI(const QStringList& initial_elements = {}, QObject* parent = nullptr);
    FS_History_UI(int history_size, const QStringList& initial_elements = {}, QObject* parent = nullptr);
    FS_History_UI(const QString& fallback_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation), int history_size = FS_History::default_history_size, const QStringList& initial_elements = {}, QObject* parent = nullptr);
    virtual ~FS_History_UI() override = default;

    QComboBox* combobox();
    QToolButton* browse_button(const QString& button_text = "...", const QString& dialog_caption = tr("Please select"), const QString& file_filter = QString());
    QMenu* menu(bool use_hotkey = true, const QString& name = "&History");
    QStringListModel& get_model() { return model; }
    void set_current_element(const QString& element) { history.set_current_element(element); }
    QStringList get_elements() const { return model.stringList(); }

signals:
    void current_element_changed(const QString&);

private:
    QComboBox* chooser {nullptr};
    QToolButton* file_dialog_button {nullptr};
    QMenu* fs_menu {nullptr};
    QStringListModel model;
    FS_History history;
};

#endif // SRC_FS_HISTORY_QT_FS_HISTORY_UI
