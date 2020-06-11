#include "qtableview_demo.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    const int default_width = 800;
    const int default_height = 600;

    QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("qtableview_demo");

    QWidget w;
    QVBoxLayout l;
    QSettings settings;
    QTableView_Demo tvd;
    l.addWidget(&tvd);
    l.addStretch();
    w.setLayout(&l);
    w.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    w.show();
    auto ret = QApplication::exec();
    settings.setValue("main_window/size", w.size());
    return ret;
}
