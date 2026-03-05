#include "fs_filter.h"

#include <QDesktopServices>
#include <QDirListing>
#include <QHeaderView>
#include <QUrl>
#include <QVBoxLayout>

namespace
{

constexpr int uiDelayIntervalInMs = 30;

}

FsFilter::FsFilter(const QString& rootPath, QWidget* parent)
: QWidget(parent)
{
    mUiDelayTimer.setInterval(uiDelayIntervalInMs);
    mUiDelayTimer.setSingleShot(true);
    connect(&mUiDelayTimer, &QTimer::timeout, this, &FsFilter::updateUi);

    auto idx = mFsModel.setRootPath(rootPath);
    mFsModel.setNameFilterDisables(false);
    mFsModel.setIconProvider(&mIconProvider);
    connect(&mFsModel, &QFileSystemModel::directoryLoaded, this, [this](const QString& /*dir*/) {
        mUiDelayTimer.start();
    });

    mFsFilterModel.setSourceModel(&mFsModel);

    auto* mainLayout = new QVBoxLayout(this);

    mSearchTextEdit.setClearButtonEnabled(true);
    connect(&mSearchTextEdit, &QLineEdit::textChanged, this, [this] {
        mFsFilterModel.setHideEmptyDirs(!mSearchTextEdit.text().isEmpty());
        if (mSearchTextEdit.text().isEmpty()) {
            mFsModel.setNameFilters({});
            mFsFilterModel.setWhitelistedName({});
        } else {
            auto filterText = mSearchTextEdit.text();
            mFsFilterModel.setWhitelistedName(filterText);
            filterText = filterText.replace('[', "[[]"); // behave like windows explorer
            mFsModel.setNameFilters({ "*" + filterText + "*" });
        }
        updateUi();
    });
    mainLayout->addWidget(&mSearchTextEdit);

    mFsView.setModel(&mFsFilterModel);
    mFsView.setRootIndex(mFsFilterModel.mapFromSource(idx));
    mFsView.setDragEnabled(true);
    mFsView.setExpandsOnDoubleClick(false);
    mFsView.setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(&mFsView, &QTreeView::doubleClicked, this, [this](const QModelIndex& index) {
        auto url = QUrl::fromLocalFile(mFsModel.filePath(mFsFilterModel.mapToSource(index)));
        QDesktopServices::openUrl(url);
    });
    mainLayout->addWidget(&mFsView);

    QTimer::singleShot(0, this, &FsFilter::loadPath);
}

void FsFilter::set_auto_expand(bool do_auto_expand)
{
    if (do_auto_expand != mAutoExpand) {
        mAutoExpand = do_auto_expand;
        if (mAutoExpand) {
            mUiDelayTimer.start();
        }
    }
}

void FsFilter::set_hide_empty_dirs(bool do_hide)
{
    mFsFilterModel.setHideEmptyDirs(do_hide);
}

// private

void FsFilter::loadPath()
{
    const QDirListing dirlist(
        mFsModel.rootPath(), QDirListing::IteratorFlag::DirsOnly | QDirListing::IteratorFlag::Recursive);
    for (const auto& dir : dirlist) {
        const auto dirIndex = mFsModel.index(dir.filePath());
        if (dirIndex.isValid()) {
            hide_expand(dirIndex);
        }
    }
}

void FsFilter::hide_expand(const QModelIndex& index)
{
    if (mFsModel.canFetchMore(index)) {
        mFsModel.fetchMore(index);
    } else {
        mUiDelayTimer.start();
    }
}

void FsFilter::updateUi()
{
    if (mAutoExpand) {
        mFsView.expandAll();
        QTimer::singleShot(0, this, [this] {
            auto* header = mFsView.header();
            // calling resizeSections() repeatedly instead of setSectionResizeMode() once keeps the colums user
            // adjustable
            header->resizeSections(QHeaderView::ResizeToContents);
        });
    }
}
