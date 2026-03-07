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
    auto firstIndex = QQmlProperty(vis.get(), "first_index").read().toInt();
    auto lastIndex = QQmlProperty(vis.get(), "last_index").read().toInt();
    auto step = QQmlProperty(vis.get(), "step").read().toInt();
    auto oerationId = QQmlProperty(vis.get(), "operation_id").read().toInt();
    auto offs = QQmlProperty(vis.get(), "index_to_value_offset").read().toInt();
    for (int i = 0; i <= lastIndex; i += 1) {
        set_color(i, "white");
    }
    if (oerationId == QQmlProperty(vis.get(), "plus_id").read().toInt()) {
        const int limit = lastIndex - firstIndex;
        auto color = set_limits("plus_color", lastIndex - 1, limit >= 0, limit, step > limit);
        while (!color.isEmpty() && firstIndex <= lastIndex && step > 0) {
            set_color(firstIndex, color);
            firstIndex += step;
        }
    } else if (oerationId == QQmlProperty(vis.get(), "minus_id").read().toInt()) {
        auto color = set_limits("minus_color", lastIndex, firstIndex > lastIndex, firstIndex, step > firstIndex);
        while (!color.isEmpty() && firstIndex >= 0 && step > 0) {
            set_color(firstIndex, color);
            firstIndex -= step;
        }
    } else { // multiply
        auto step_max = lastIndex;
        if (firstIndex > 0) {
            step_max /= firstIndex;
        }
        auto mcolor = set_limits("multiply_color", lastIndex / 2, firstIndex > lastIndex / 2, step_max, step > step_max);
        auto pcolor = QQmlProperty(vis.get(), "plus_color").read().toString();
        auto currentIndex = firstIndex;
        int last = 0;
        while (!mcolor.isEmpty() && currentIndex <= lastIndex && step > 0) {
            if ((currentIndex + offs - last) / step == firstIndex + offs && (currentIndex + offs - last) % step == 0) {
                last = currentIndex + offs;
                set_color(currentIndex, mcolor);
            } else {
                set_color(currentIndex, pcolor);
            }
            currentIndex += firstIndex + offs;
            if (currentIndex == 0) {
                break;
            }
        }
    }
}

// private

QString Vis_1000::set_limits(
    const QString& color_to_get, int first_max, bool first_limit_reached, int step_max, bool step_limit_reached)
{
    QQmlProperty(vis.get(), "first_max").write(first_max);
    if (first_limit_reached) {
        return {};
    }
    QQmlProperty(vis.get(), "step_max").write(step_max);
    if (step_limit_reached) {
        return {};
    }
    return QQmlProperty(vis.get(), color_to_get).read().toString();
}

void Vis_1000::set_color(int item_pos, const QString& color)
{
    auto* repeater = QQmlProperty(vis.get(), "number_repeater").read().value<QQuickItem*>();
    QQuickItem* item = nullptr;
    QMetaObject::invokeMethod(repeater, "itemAt", Q_RETURN_ARG(QQuickItem*, item), Q_ARG(int, item_pos));
    auto* bgrect = QQmlProperty(item, "background").read().value<QQuickItem*>();
    QQmlProperty(bgrect, "color").write(color);
}
