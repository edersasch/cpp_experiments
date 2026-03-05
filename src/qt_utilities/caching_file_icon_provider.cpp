#include "caching_file_icon_provider.hpp"

namespace QtUtilities
{

const QString CachingFileIconProvider::mFolder = "Folder";
const QString CachingFileIconProvider::mFile = "File";

CachingFileIconProvider::CachingFileIconProvider()
: mFolderIcon(QFileIconProvider::icon(QFileIconProvider::Folder))
, mFileIcon(QFileIconProvider::icon(QFileIconProvider::File))
{
}

CachingFileIconProvider::~CachingFileIconProvider() = default;

QIcon CachingFileIconProvider::icon(const QFileInfo& info) const
{
    return getCached(info, mIconCache, &CachingFileIconProvider::getBaseIcon, mFolderIcon, mFileIcon);
}

QString CachingFileIconProvider::type(const QFileInfo& info) const
{
    return getCached(info, mTypeCache, &CachingFileIconProvider::getBaseType, mFolder, mFile);
}

// private

template <typename T, typename Cache>
T CachingFileIconProvider::getCached(const QFileInfo& info, Cache& cache,
    T (CachingFileIconProvider::*getBase)(const QFileInfo&) const, const T& dirDefault, const T& fileDefault) const
{
    if (info.isDir()) {
        return dirDefault;
    }
    const QString key = info.suffix();
    if (key.isEmpty()) {
        return fileDefault;
    }
    {
        const QReadLocker locker(&mCacheLock);
        const auto itr = cache.find(key);
        if (itr != cache.end()) {
            return itr->second;
        }
    }
    const auto value = (this->*getBase)(info);
    const QWriteLocker locker(&mCacheLock);
    cache[key] = value;
    return value;
}

QIcon CachingFileIconProvider::getBaseIcon(const QFileInfo& info) const { return QFileIconProvider::icon(info); }

QString CachingFileIconProvider::getBaseType(const QFileInfo& info) const { return QFileIconProvider::type(info); }

}
