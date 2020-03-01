#include "fs_history_search.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QComboBox>
#include <QToolButton>

FS_History_Search::FS_History_Search(const QStringList& start_dirs, int history_size, QWidget* parent)
    : QWidget(parent)
    , dir_ui("", history_size, start_dirs)
{
    auto main_layout(new QVBoxLayout(this));
    auto dir_layout = new QHBoxLayout;
    dir_ui.combobox()->setStyle(new Combobox_Label_Elide_Left_Proxy_Style);
    dir_layout->addWidget(dir_ui.combobox());
    dir_layout->addWidget(dir_ui.browse_button());
    main_layout->addLayout(dir_layout);
    main_layout->addStretch();

    connect(&dir_ui, &FS_History_UI::current_element_changed, this, [this, main_layout] (const QString& dir) {
        std::unique_ptr<FS_Filter> old(fs_filter.release());
        QFileInfo i(dir);
        if (i.isDir()) {
            fs_filter = std::make_unique<FS_Filter>(dir, this);
            fs_filter->set_auto_expand(true);
            fs_filter->set_hide_empty_dirs(true);
            main_layout->insertWidget(1, fs_filter.get(), main_stretch_factor);
            fs_filter->set_search_text(old ? old->get_search_text() : QString());
            fs_filter->set_focus();
        }
    });
}
