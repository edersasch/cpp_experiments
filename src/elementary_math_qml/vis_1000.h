#ifndef SRC_ELEMENTARY_MATH_QML_VIS_1000
#define SRC_ELEMENTARY_MATH_QML_VIS_1000

#include <QObject>
#include <QQmlComponent>
#include <QQuickItem>

#include <memory>

class QQmlEngine;

class Vis_1000
        : public QObject
{
    Q_OBJECT

public:
    Vis_1000(QQmlEngine* engine, QQuickItem* parentItem);
    virtual ~Vis_1000() = default;

private slots:
    void change();

private:
    QString set_limits(const QString& color_to_get, int first_max, bool first_limit_reached, int step_max, int step_limit_reaced);
    void set_color(int item_pos, const QString& color);

    QQmlComponent vis_component;
    std::unique_ptr<QQuickItem> vis;
};

#endif // SRC_ELEMENTARY_MATH_QML_VIS_1000
