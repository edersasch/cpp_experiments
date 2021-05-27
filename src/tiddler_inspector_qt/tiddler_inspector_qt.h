#ifndef SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
#define SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT

#include <QWidget>

class Tiddler_Pure_View;
class Tiddler_Pure_Edit;
class Tiddlerstore_Handler;

class Tiddler_Inspector
        : public QWidget
{
    Q_OBJECT

public:
    explicit Tiddler_Inspector(const QStringList& tiddlerstore_list = {}, QWidget* parent = nullptr);
    virtual ~Tiddler_Inspector() override = default;

    QStringList get_tiddlerstore_list();

private:
    Tiddlerstore_Handler* store_handler;
    Tiddler_Pure_View* pure_view;
    Tiddler_Pure_Edit* pure_edit;
};

#endif // SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
