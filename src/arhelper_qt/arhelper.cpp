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
    auto* sevenZip = sevenzip();
    if (sevenZip == nullptr) {
        return;
    }
    connect(sevenZip, &QProcess::readyReadStandardOutput, this, [this, sevenZip] {
        static const QRegularExpression regex(R"(^\d+-\d+-\d+\s+\d+:\d+:\d+\s+\S+\s+\d*\s+\d+\s+(.*)$)");
        const QFileIconProvider fip;
        QFileInfo fileInfo;
        while (sevenZip->canReadLine()) {
            const QString out = sevenZip->readLine();
            auto match = regex.match(out);
            if (match.hasMatch()) {
                const QString line = match.captured(1);
                auto splitted = line.split('/');
                if (!splitted.isEmpty() && sevenZip->canReadLine()) { // don't process last line
                    auto* root = archive_directory_model.invisibleRootItem();
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
                            fileInfo.setFile(splitted.first());
                            root->appendRow(new QStandardItem(fip.icon(fileInfo), splitted.first()));
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
    sevenZip->setArguments(QStringList() << "l" << archive_path);
    sevenZip->start();
}

void Arhelper::open_path(const QModelIndex& index, const QString& archive_path)
{
    auto* item = archive_directory_model.itemFromIndex(index);
    if (item == nullptr) {
        return;
    }
    auto* sevenZip = sevenzip();
    if (sevenZip == nullptr) {
        return;
    }
    QString path = item->text();
    while (item->parent() != nullptr) {
        item = item->parent();
        path.prepend(item->text() + "/");
    }
    connect(sevenZip, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this, path](int exit_code, QProcess::ExitStatus exit_status) {
        if (exit_status == QProcess::NormalExit && exit_code == 0) {
            auto url = QUrl::fromLocalFile(tempdir.path() + "/" + path);
            QDesktopServices::openUrl(url);
        }
    });
    sevenZip->setWorkingDirectory(tempdir.path());
    sevenZip->setArguments(QStringList() << "x" << "-y" << archive_path << path);
    sevenZip->start();
}

// private

QProcess* Arhelper::sevenzip()
{
    auto sevenzippath = QStandardPaths::findExecutable("7z");
    if (sevenzippath.isEmpty()) {
        emit ar_error("No 7z executable found");
        return nullptr;
    }
    auto* sevenZip = new QProcess(this);
    sevenZip->setProgram(sevenzippath);
    connect(sevenZip, &QProcess::errorOccurred, this, [this, sevenZip](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            sevenZip->deleteLater();
        }
        emit ar_error("sevenzip error " + QString::number(error));
    });
    connect(sevenZip, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this, sevenZip](int exit_code, QProcess::ExitStatus exit_status) {
        if (exit_status != QProcess::NormalExit || exit_code != 0) {
            emit ar_error("sevenzip exit " + QString::number(exit_status));
        }
        sevenZip->deleteLater();
    });
    return sevenZip;
}
