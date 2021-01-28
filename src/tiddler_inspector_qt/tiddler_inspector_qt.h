#ifndef SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
#define SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT

#include <QWidget>

class Tiddler_Pure_View;
class Tiddler_Pure_Edit;

class Tiddler_Inspector
        : public QWidget
{
    Q_OBJECT

public:
    explicit Tiddler_Inspector(const QString& tiddlerstore_json = {}, QWidget* parent = nullptr);
    virtual ~Tiddler_Inspector() override = default;

    QString get_store();

private:
    Tiddler_Pure_View* pure_view;
    Tiddler_Pure_Edit* pure_edit;
    bool is_edit_mode_active {false};
};

#endif // SRC_TIDDLER_INSPECTOR_QT_TIDDLER_INSPECTOR_QT
