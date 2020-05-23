#include "fs_history_qt/fs_history_ui.h"
#include "ar_history_search.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    const int history_size = 5;
    const int default_width = 800;
    const int default_height = 600;

    QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("ar_history_search");

    QWidget w;
    QVBoxLayout l;
    QSettings settings;
    AR_History_Search hs(settings.value("filesystem/recent_archives").toStringList(), history_size);
    l.addWidget(&hs, AR_History_Search::main_stretch_factor);
    l.addStretch();
    w.setLayout(&l);
    w.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    w.show();
    auto ret = QApplication::exec();
    settings.setValue("filesystem/recent_archives", hs.get_archives());
    settings.setValue("main_window/size", w.size());
    return ret;
}
