#include "arhelper.h"

#include <QStandardPaths>
#include <QRegularExpression>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>

Arhelper::Arhelper(QWidget* parent)
    : QWidget(parent)
{
}

void Arhelper::ls(const QString& archive_path)
{
    auto sz = sevenzip();
    if (!sz) {
        return;
    }
    connect(sz, &QProcess::readyReadStandardOutput, this, [this, sz] {
        static QRegularExpression re(R"(^\d+-\d+-\d+\s+\d+:\d+:\d+\s+\S+\s+\d*\s+\d+\s+(.*)$)");
        QFileIconProvider fip;
        QFileInfo fi;
        while (sz->canReadLine()) {
            QString out = sz->readLine();
            auto m = re.match(out);
            if (m.hasMatch()) {
                QString line = m.captured(1);
                auto splitted = line.split('/');
                if (!splitted.isEmpty() && sz->canReadLine()) { // don't process last line
                    auto root = archive_directory_model.invisibleRootItem();
                    while (!splitted.isEmpty()) {
                        bool found = false;
                        for (int i = 0; i < root->rowCount(); i += 1) {
                            if (root->child(i)->text() == splitted.first()) {
                                found = true;
                                root = root->child(i);
                                break;
                            }
                        }
                        if (!found) {
                            if (splitted.size() == 1 && root->rowCount() == 0) {
                                root->setIcon(fip.icon(QFileIconProvider::Folder));
                            }
                            fi.setFile(splitted.first());
                            root->appendRow(new QStandardItem(fip.icon(fi), splitted.first()));
                            root = root->child(root->rowCount() - 1);
                            root->setFlags(root->flags() & ~Qt::ItemIsEditable);
                        }
                        splitted.pop_front();
                    }
                }
            }
        }
    });
    archive_directory_model.clear();
    sz->setArguments(QStringList() << "l" << archive_path);
    sz->start();
}

void Arhelper::open_path(const QModelIndex& index, const QString& archive_path)
{
    auto item = archive_directory_model.itemFromIndex(index);
    if (!item) {
        return;
    }
    auto sz = sevenzip();
    if (!sz) {
        return;
    }
    QString path = item->text();
    while (item->parent()) {
        item = item->parent();
        path.prepend(item->text() + "/");
    }
    connect(sz, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this, path](int exit_code, QProcess::ExitStatus exit_status) {
        if (exit_status == QProcess::NormalExit && exit_code == 0) {
            auto url = QUrl::fromLocalFile(tempdir.path() + "/" + path);
            QDesktopServices::openUrl(url);
        }
    });
    sz->setWorkingDirectory(tempdir.path());
    sz->setArguments(QStringList() << "x" << "-y" << archive_path << path);
    sz->start();
}

// private

QProcess* Arhelper::sevenzip()
{
    auto sevenzippath = QStandardPaths::findExecutable("7z");
    if (sevenzippath.isEmpty()) {
        emit ar_error("No 7z executable found");
        return nullptr;
    }
    auto sz = new QProcess(this);
    sz->setProgram(sevenzippath);
    connect(sz, &QProcess::errorOccurred, this, [this, sz](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            sz->deleteLater();
        }
        emit ar_error("sevenzip error " + QString::number(error));
    });
    connect(sz, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this, sz](int exit_code, QProcess::ExitStatus exit_status) {
        if (exit_status != QProcess::NormalExit || exit_code != 0) {
            emit ar_error("sevenzip exit " + QString::number(exit_status));
        }
        sz->deleteLater();
    });
    return sz;
}
