#ifndef SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
#define SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT

#include <QWidget>

class Tiddler_Pure_View;
class Tiddler_Pure_Edit;
class Tiddler_Model;
class FlowLayout;
class QHBoxLayout;
class QVBoxLayout;
class QBoxLayout;
class QToolButton;
class QLineEdit;
class QLabel;
class QTextBrowser;

class Tiddler_Inspector
        : public QWidget
{
    Q_OBJECT

public:
    explicit Tiddler_Inspector(const QString& tiddlerstore_json = {}, QWidget* parent = nullptr);
    virtual ~Tiddler_Inspector() override = default;

    QString get_store();
};

class Text_Tiddler_ui
        : public QWidget
{
    Q_OBJECT

public:
    explicit Text_Tiddler_ui(QWidget* parent = nullptr);
    virtual ~Text_Tiddler_ui() override = default;

    void set_tiddler_model(Tiddler_Model* tiddler_model);

private:
    Tiddler_Pure_View* pure_view;
    Tiddler_Pure_Edit* pure_edit;
    bool is_edit_mode_active {false};
};

#endif // SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
