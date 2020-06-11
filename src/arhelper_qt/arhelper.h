#ifndef SRC_ARHELPER_QT_ARHELPER
#define SRC_ARHELPER_QT_ARHELPER

#include <QWidget>
#include <QStandardItemModel>
#include <QProcess>
#include <QTemporaryDir>

class Arhelper
        : public QWidget
{
    Q_OBJECT

public:
    Arhelper(QWidget* parent = nullptr);
    virtual ~Arhelper() override = default;

    QStandardItemModel& get_archive_directory_model() { return archive_directory_model; }
    void ls(const QString& archive_path);
    void open_path(const QModelIndex& index, const QString& archive_path);

signals:
    void ar_error(const QString&);

private:
    QProcess* sevenzip();
    QStandardItemModel archive_directory_model;
    QTemporaryDir tempdir {};
};

#endif // SRC_ARHELPER_QT_ARHELPER
