#include "fs_history_qt/fs_history_ui.h"
#include "fs_history_search.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    const int history_size = 5;
    const int default_width = 800;
    const int default_height = 600;

    const QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("fs_history_search");

    QWidget widget;
    QVBoxLayout layout;
    QSettings settings;
    FS_History_Search historySearch(settings.value("filesystem/recent_dirs").toStringList(), history_size);
    layout.addWidget(&historySearch, FS_History_Search::main_stretch_factor);
    layout.addStretch();
    widget.setLayout(&layout);
    widget.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    widget.show();
    auto ret = QApplication::exec();
    settings.setValue("filesystem/recent_dirs", historySearch.get_dirs());
    settings.setValue("main_window/size", widget.size());
    return ret;
}
