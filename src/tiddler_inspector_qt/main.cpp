#include "tiddler_inspector_qt.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    constexpr int32_t default_width = 800;
    constexpr int32_t default_height = 600;

    const QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("Tiddler Inspector");

    QWidget widget;
    QVBoxLayout layout;
    QSettings settings;
    Tiddler_Inspector tiddlerInspector(settings.value("tiddlers/store_list").toStringList());
    layout.addWidget(&tiddlerInspector);
    widget.setLayout(&layout);
    widget.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    widget.show();
    auto ret = QApplication::exec();
    settings.setValue("tiddlers/store_list", tiddlerInspector.get_tiddlerstore_list());
    settings.setValue("main_window/size", widget.size());
    return ret;
}
