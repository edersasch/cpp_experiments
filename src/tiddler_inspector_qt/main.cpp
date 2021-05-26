#include "tiddler_inspector_qt.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    constexpr int32_t default_width = 800;
    constexpr int32_t default_height = 600;

    QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("Tiddler Inspector");

    QWidget w;
    QVBoxLayout l;
    QSettings settings;
    Tiddler_Inspector ti(settings.value("tiddlers/store_list").toStringList());
    l.addWidget(&ti);
    w.setLayout(&l);
    w.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    w.show();
    auto ret = QApplication::exec();
    settings.setValue("tiddlers/store_list", ti.get_tiddlerstore_list());
    settings.setValue("main_window/size", w.size());
    return ret;
}
