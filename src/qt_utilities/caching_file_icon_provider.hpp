#ifndef SRC_QT_UTILITIES_CACHING_FILE_ICON_PROVIDER_HPP
#define SRC_QT_UTILITIES_CACHING_FILE_ICON_PROVIDER_HPP

#include <QFileIconProvider>
#include <QReadWriteLock>

#include <unordered_map>

namespace QtUtilities
{

class CachingFileIconProvider : public QFileIconProvider
{
public:
    CachingFileIconProvider();
    ~CachingFileIconProvider() override;

    QIcon icon(const QFileInfo& info) const override;
    QString type(const QFileInfo& info) const override;

private:
    template <typename T, typename Cache>
    T getCached(const QFileInfo& info, Cache& cache, T (CachingFileIconProvider::*getBase)(const QFileInfo&) const,
        const T& dirDefault, const T& fileDefault) const;
    QIcon getBaseIcon(const QFileInfo& info) const;
    QString getBaseType(const QFileInfo& info) const;

    static const QString mFolder;
    static const QString mFile;
    const QIcon mFolderIcon;
    const QIcon mFileIcon;
    mutable std::unordered_map<QString, QIcon> mIconCache;
    mutable std::unordered_map<QString, QString> mTypeCache;
    mutable QReadWriteLock mCacheLock; // one lock for both caches is enough
};

}

#endif
