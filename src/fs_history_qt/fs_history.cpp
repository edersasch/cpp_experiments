#include "fs_history.h"

#include <QFileInfo>
#include <QTimer>

#include <utility>

FsHistory::FsHistory(
    Operation_Mode mode, QString fallback, std::int32_t history_size, QStringList initial_elements, QObject* parent)
: QObject(parent)
, mOpmode(mode)
, mHistorySize(std::max(2, history_size))
, mFallbackDir(std::move(fallback))
, mElements(std::move(initial_elements))
{
    mElements.removeDuplicates();
    connect(&mFsWatcher, &QFileSystemWatcher::directoryChanged, this, &FsHistory::check);
    connect(&mFsWatcher, &QFileSystemWatcher::fileChanged, this, &FsHistory::check);
    QTimer::singleShot(0, this, [this] { cleanup(true); });
}

FsHistory::~FsHistory() = default;

void FsHistory::set_current_element(const QString& element)
{
    if (is_valid(element)) {
        const auto idx = mElements.indexOf(element);
        if (idx == -1) {
            mElements.prepend(element);
            cleanup(true);
        } else {
            if (idx > 0) {
                move_to_front(mElements, static_cast<std::int32_t>(idx));
                cleanup(true);
            }
        }
    }
}

// private

void FsHistory::cleanup(bool changed)
{
    const auto prev_count = mElements.count();
    mElements.erase(std::remove_if(mElements.begin(), mElements.end(),
                        [this](const QString& element) { return !is_valid(element); }),
        mElements.end());
    changed |= prev_count != mElements.count();
    if (mElements.count() > mHistorySize) {
        mElements.resize(mHistorySize);
    }
    if (mElements.isEmpty() && mOpmode == Operation_Mode::OP_DIR && is_valid(mFallbackDir)) {
        changed = true;
        mElements << mFallbackDir;
    }
    if (changed) {
        auto wdf = mFsWatcher.directories();
        wdf.append(mFsWatcher.files());
        if (!wdf.isEmpty()) {
            mFsWatcher.removePaths(wdf);
        }
        if (!mElements.isEmpty()) {
            mFsWatcher.addPaths(mElements);
        }
        emit elements_changed(mElements);
    }
}

bool FsHistory::is_valid(const QString& element) const
{
    if (!element.isEmpty()) {
        const QFileInfo fileInfo(element);
        switch (mOpmode) {
        case Operation_Mode::OP_FILE:
            return fileInfo.isFile();
        case Operation_Mode::OP_DIR:
            return fileInfo.isDir();
        }
    }
    return false;
}
