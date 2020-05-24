#include "arhelper.h"

#include <QStandardPaths>
#include <QRegularExpression>
#include <QFileIconProvider>
#include <QFileInfo>

Arhelper::Arhelper(QWidget* parent)
    : QWidget(parent)
{
}

void Arhelper::ls(const QString& archive_path)
{
    auto sevenzippath = QStandardPaths::findExecutable("7z");
    if (sevenzippath.isEmpty()) {
        emit ar_error("No 7z executable found");
        return;
    }
    connect(&sevenzip, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        emit ar_error("sevenzip error " + QString::number(error));
    });
    connect(&sevenzip, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this](int exit_code, QProcess::ExitStatus exit_status) {
        if (exit_status != QProcess::NormalExit || exit_code != 0) {
            emit ar_error("sevenzip exit " + QString::number(exit_status));
        }
    });
    connect(&sevenzip, &QProcess::readyReadStandardOutput, this, [this] {
        QRegularExpression re(R"(^\d+-\d+-\d+\s+\d+:\d+:\d+\s+\S+\s+\d*\s+\d+\s+(.*)$)");
        QFileIconProvider fip;
        QFileInfo fi;
        while (sevenzip.canReadLine()) {
            QString out = sevenzip.readLine();
            auto m = re.match(out);
            if (m.hasMatch()) {
                QString line = m.captured(1);
                auto splitted = line.split('/');
                if (!splitted.isEmpty() && sevenzip.canReadLine()) { // don't process last line
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
    sevenzip.start(sevenzippath, QStringList() << "l" << archive_path);
}
