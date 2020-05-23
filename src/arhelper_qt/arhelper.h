#ifndef SRC_ARHELPER_QT_ARHELPER
#define SRC_ARHELPER_QT_ARHELPER

#include <QWidget>
#include <QStandardItemModel>
#include <QProcess>

class Arhelper
        : public QWidget
{
    Q_OBJECT

public:
    Arhelper(QWidget* parent = nullptr);
    virtual ~Arhelper() override = default;

    void set_sevenzip_custom_path(const QString& path) { sevenzip_custom_path = path; }
    QStandardItemModel& get_archive_directory_model() { return archive_directory_model; }
    void ls(const QString& archive_path);

signals:
    void ar_error(const QString&);

private:
    QString sevenzip_custom_path;
    QStandardItemModel archive_directory_model;
    QProcess sevenzip;
};

#endif // SRC_ARHELPER_QT_ARHELPER
