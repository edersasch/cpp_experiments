#include "fs_history_qt/fs_history_ui.h"
#include "fs_history_search.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QVBoxLayout>

namespace
{

constexpr int history_size = 5;
constexpr int default_width = 800;
constexpr int default_height = 600;

}

int main(int argc, char *argv[])
{
    const QApplication app(argc, argv);
    QApplication::setOrganizationName("Edersasch_Opensource");
    QApplication::setApplicationName("fs_history_search");

    QWidget widget;
    QVBoxLayout layout;
    QSettings settings;
    const FsHistorySearch::FsHistorySearchConfig config {
        .recentDirs = settings.value("filesystem/recent_dirs").toStringList(),
        .historySize = history_size
    };
    FsHistorySearch historySearch(config);
    layout.addWidget(&historySearch, FsHistorySearch::mainStretchFactor);
    layout.addStretch();
    widget.setLayout(&layout);
    widget.resize(settings.value("main_window/size", QSize(default_width, default_height)).toSize());
    widget.show();
    const auto ret = QApplication::exec();
    settings.setValue("filesystem/recent_dirs", historySearch.getRecentDirs());
    settings.setValue("main_window/size", widget.size());
    return ret;
}
