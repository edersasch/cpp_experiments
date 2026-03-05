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

    const QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("ar_history_search");

    QWidget widget;
    QVBoxLayout layout;
    QSettings settings;
    AR_History_Search historySearch(settings.value("filesystem/recent_archives").toStringList(), history_size);
    layout.addWidget(&historySearch, AR_History_Search::main_stretch_factor);
    layout.addStretch();
    widget.setLayout(&layout);
    widget.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    widget.show();
    auto ret = QApplication::exec();
    settings.setValue("filesystem/recent_archives", historySearch.get_archives());
    settings.setValue("main_window/size", widget.size());
    return ret;
}
