#include "elementary_math_qml/vis_1000.h"

#include <QGuiApplication>
#include <QQuickView>
#include <QSettings>

int main(int argc, char *argv[])
{
    const int default_width = 800;
    const int default_height = 600;

    const QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName("Edersasch_Opensource");
    QGuiApplication::setApplicationName(QObject::tr("Visualize to 1000"));

    QSettings settings;
    auto size = settings.value("main_window/size", QSize(default_width, default_height)).toSize();
    Q_INIT_RESOURCE(vis_1000);
    QQuickView view;
    const Vis_1000 vis1000(view.engine(), view.contentItem());
    view.setWidth(size.width());
    view.setHeight(size.height());
    view.show();
    auto ret = QGuiApplication::exec();
    size.setWidth(view.width());
    size.setHeight(view.height());
    settings.setValue("main_window/size", size);
    return ret;
}
