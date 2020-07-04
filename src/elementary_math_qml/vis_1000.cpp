#include "vis_1000.h"

#include <QTimer>
#include <QQmlProperty>

Vis_1000::Vis_1000(QQmlEngine* engine, QQuickItem* parentItem)
    : vis_component(engine, QUrl(QStringLiteral("qrc:/Vis_1000.qml")))
    , vis(qobject_cast<QQuickItem*>(vis_component.create()))
{
    vis->setParentItem(parentItem);
    connect(vis.get(), SIGNAL(change()), this, SLOT(change()));
    QTimer::singleShot(0, this, &Vis_1000::change);
}

// private slots

void Vis_1000::change()
{
    auto fi = QQmlProperty(vis.get(), "first_index").read().toInt();
    auto li = QQmlProperty(vis.get(), "last_index").read().toInt();
    auto step = QQmlProperty(vis.get(), "step").read().toInt();
    auto op = QQmlProperty(vis.get(), "operation_id").read().toInt();
    auto offs = QQmlProperty(vis.get(), "index_to_value_offset").read().toInt();
    for (int i = 0; i <= li; i += 1) {
        set_color(i, "white");
    }
    if (op == QQmlProperty(vis.get(), "plus_id").read().toInt()) {
        auto color = set_limits("plus_color", li - 1, fi >= li, li - fi, step > li -fi);
        while (!color.isEmpty() && fi <= li && step > 0) {
            set_color(fi, color);
            fi += step;
        }
    } else if (op == QQmlProperty(vis.get(), "minus_id").read().toInt()) {
        auto color = set_limits("minus_color", li, fi > li, fi, step > fi);
        while (!color.isEmpty() && fi >= 0 && step > 0) {
            set_color(fi, color);
            fi -= step;
        }
    } else { // multiply
        auto step_max = li;
        if (fi > 0) {
            step_max /= fi;
        }
        auto mcolor = set_limits("multiply_color", li / 2, fi > li / 2, step_max, step > step_max);
        auto pcolor = QQmlProperty(vis.get(), "plus_color").read().toString();
        auto i = fi;
        int last = 0;
        while (!mcolor.isEmpty() && i <= li && step > 0) {
            if ((i + offs - last) / step == fi + offs && (i + offs - last) % step == 0) {
                last = i + offs;
                set_color(i, mcolor);
            } else {
                set_color(i, pcolor);
            }
            i += fi + offs;
            if (i == 0) {
                break;
            }
        }
    }
}

// private

QString Vis_1000::set_limits(const QString &color_to_get, int first_max, bool first_limit_reached, int step_max, int step_limit_reaced)
{
    QQmlProperty(vis.get(), "first_max").write(first_max);
    if (first_limit_reached) {
        return {};
    }
    QQmlProperty(vis.get(), "step_max").write(step_max);
    if (step_limit_reaced) {
        return {};
    }
    return QQmlProperty(vis.get(), color_to_get).read().toString();
}

void Vis_1000::set_color(int item_pos, const QString& color)
{
    auto repeater = QQmlProperty(vis.get(), "number_repeater").read().value<QQuickItem*>();
    QQuickItem* item = nullptr;
    QMetaObject::invokeMethod(repeater, "itemAt", Q_RETURN_ARG(QQuickItem*, item), Q_ARG(int, item_pos));
    auto bgrect = QQmlProperty(item, "background").read().value<QQuickItem*>();
    QQmlProperty(bgrect, "color").write(color);
}
