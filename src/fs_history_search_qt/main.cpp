#include "fs_history_qt/fs_history_ui.h"
#include "fs_history_search.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>
#include <QComboBox>
#include <QToolButton>
#include <QTemporaryDir>

int main(int argc, char *argv[])
{
    const int history_size = 5;
    const int default_width = 800;
    const int default_height = 600;

    QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("fs_history_search");

    QWidget w;
    QVBoxLayout l;
    QSettings settings;
    FS_History_Search hs(settings.value("filesystem/recent_dirs").toStringList(), history_size);
    l.addWidget(&hs, FS_History_Search::main_stretch_factor);
    l.addStretch();
    w.setLayout(&l);
    w.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    w.show();
    auto ret = QApplication::exec();
    settings.setValue("filesystem/recent_dirs", hs.get_dirs());
    settings.setValue("main_window/size", w.size());
    return ret;
}
