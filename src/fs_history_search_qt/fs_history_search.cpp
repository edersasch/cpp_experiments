#include "fs_history_search.h"
#include "qt_utilities/combobox_label_elide_left_proxy_style.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QVBoxLayout>

FsHistorySearch::FsHistorySearch(const FsHistorySearchConfig& config, QWidget* parent)
: QWidget(parent)
, mDirUi("", config.historySize, config.recentDirs)
{
    auto* main_layout(new QVBoxLayout(this));
    auto* dir_layout = new QHBoxLayout;
    mDirUi.combobox()->setStyle(new Combobox_Label_Elide_Left_Proxy_Style);
    dir_layout->addWidget(mDirUi.combobox());
    dir_layout->addWidget(mDirUi.browse_button());
    main_layout->addLayout(dir_layout);
    main_layout->addStretch();

    connect(&mDirUi, &FS_History_UI::current_element_changed, this, [this, main_layout](const QString& dir) {
        const QFileInfo fileInfo(dir);
        if (fileInfo.isDir()) {
            const auto searchText = mFsFilter == nullptr ? QString() : mFsFilter->get_search_text();
            mFsFilter = std::make_unique<FsFilter>(dir, this);
            mFsFilter->set_auto_expand(true);
            mFsFilter->set_hide_empty_dirs(true);
            mFsFilter->set_search_text(searchText);
            mFsFilter->set_focus();
            main_layout->insertWidget(1, mFsFilter.get(), mainStretchFactor);
        }
    });
}

FsHistorySearch::~FsHistorySearch() = default;
