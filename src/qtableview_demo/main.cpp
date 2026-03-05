#include "qtableview_demo.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    const int default_width = 800;
    const int default_height = 600;

    const QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("qtableview_demo");

    QWidget widget;
    QVBoxLayout layout;
    QSettings settings;
    QTableView_Demo tvd;
    layout.addWidget(&tvd);
    layout.addStretch();
    widget.setLayout(&layout);
    widget.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    widget.show();
    auto ret = QApplication::exec();
    settings.setValue("main_window/size", widget.size());
    return ret;
}
