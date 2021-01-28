#ifndef SRC_TIDDLERSTORE_QT_TIDDLER_PURE_VIEW_QT
#define SRC_TIDDLERSTORE_QT_TIDDLER_PURE_VIEW_QT

#include <QWidget>

class Tiddler_Model;
class QLabel;
class QTextBrowser;
class QToolButton;

class Tiddler_Pure_View
        : public QWidget
{
    Q_OBJECT

signals:
    void trigger_edit();

public:
    explicit Tiddler_Pure_View(QWidget* parent = nullptr);
    virtual ~Tiddler_Pure_View() override = default;

    Tiddler_Model* tiddler_model();
    void set_tiddler_model(Tiddler_Model* model);

protected:
    void showEvent(QShowEvent* event) override;

private:
    Tiddler_Model* tm {nullptr};
    QLabel* title_label;
    QTextBrowser* text_browser;
    QToolButton* edit_button;
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLER_PURE_VIEW_QT
