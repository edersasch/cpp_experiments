#include "fs_history.h"

#include <QFileInfo>
#include <QTimer>

#include <utility>

FS_History::FS_History(Operation_Mode mode, QString fallback, int history_size, const QStringList& initial_elements, QObject* parent)
    : QObject(parent)
    , opmode(mode)
    , hist_size(history_size < 2 ? 2 : history_size)
    , fb(std::move(fallback))
    , elements(initial_elements)
{
    elements.removeDuplicates();
    connect(&watcher, &QFileSystemWatcher::directoryChanged, this, &FS_History::check);
    connect(&watcher, &QFileSystemWatcher::fileChanged, this, &FS_History::check);
    QTimer::singleShot(0, this, [this, initial_elements] {
        cleanup(true);
    });
}

FS_History::FS_History(const QStringList& initial_elements, QObject* parent)
    : FS_History(Operation_Mode::OP_FILE, "", default_history_size, initial_elements, parent)
{
}

FS_History::FS_History(int history_size, const QStringList& initial_elements, QObject* parent)
    : FS_History(Operation_Mode::OP_FILE, "", history_size, initial_elements, parent)
{
}

FS_History::FS_History(const QString& fallback, int history_size, const QStringList& initial_elements, QObject* parent)
    : FS_History(Operation_Mode::OP_DIR, fallback, history_size, initial_elements, parent)
{
}

void FS_History::set_current_element(const QString &element)
{
    if (is_valid(element)) {
        auto idx = elements.indexOf(element);
        bool changed = false;
        if (idx == -1) {
            elements.prepend(element);
            idx = 0;
            changed = true;
        } else {
            if (idx > 0) {
                move_to_front(elements, elements.indexOf(element));
                changed = true;
            }
        }
        cleanup(changed);
    }
}

// private

void FS_History::cleanup(bool changed)
{
    auto prev_count = elements.count();
    elements.erase(std::remove_if(elements.begin(), elements.end(), [this](const QString& element) {
        return !is_valid(element);
    }), elements.end());
    if (elements.count() > hist_size) {
        elements.erase(elements.begin() + hist_size, elements.end()); // no resize() in QStringList
    }
    if (prev_count != elements.count()) {
        changed = true;
    }
    if (elements.isEmpty() && opmode == Operation_Mode::OP_DIR && !fb.isEmpty() && is_valid(fb)) {
        changed = true;
        elements << fb;
    }
    if (changed) {
        auto wdf = watcher.directories();
        wdf.append(watcher.files());
        if (!wdf.isEmpty()) {
            watcher.removePaths(wdf);
        }
        if (!elements.isEmpty()) {
            watcher.addPaths(elements);
        }
        emit elements_changed(elements);
    }
}

bool FS_History::is_valid(const QString& element) const
{
    QFileInfo i(element);
    bool ret = false;
    switch (opmode) {
    case Operation_Mode::OP_FILE:
        ret = i.isFile();
        break;
    case Operation_Mode::OP_DIR:
        ret = i.isDir();
        break;
    }
    return ret;
}
