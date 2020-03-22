#include "elementary_math_qml/vis_1000.h"

#include <QGuiApplication>
#include <QQuickView>
#include <QSettings>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName("Edersasch_Opensource");
    QGuiApplication::setApplicationName(QObject::tr("Visualize to 1000"));

    QSettings settings;
    auto size = settings.value("main_window/size", QSize(800, 600)).toSize();
    Q_INIT_RESOURCE(vis_1000);
    QQuickView v;
    Vis_1000 v1(v.engine(), v.contentItem());
    v.setWidth(size.width());
    v.setHeight(size.height());
    v.show();
    auto ret = QGuiApplication::exec();
    size.setWidth(v.width());
    size.setHeight(v.height());
    settings.setValue("main_window/size", size);
    return ret;
}
