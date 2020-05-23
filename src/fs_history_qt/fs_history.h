#ifndef SRC_FS_HISTORY_QT_FS_HISTORY
#define SRC_FS_HISTORY_QT_FS_HISTORY

#include <QObject>
#include <QFileSystemWatcher>

#include <algorithm>

template <typename T>
void move_to_front(T& sequence, int pos)
{
    auto it = sequence.begin() + pos;
    std::rotate(sequence.begin(), it, it + 1);
}

class FS_History
        : public QObject
{
    Q_OBJECT

public:
    enum class Operation_Mode
    {
        OP_FILE,
        OP_DIR
    };

    static constexpr int default_history_size {9};

    FS_History(Operation_Mode mode, QString fallback, int history_size, const QStringList& initial_elements, QObject* parent = nullptr);
    FS_History(const QStringList& initial_elements = {}, QObject* parent = nullptr);
    FS_History(int history_size, const QStringList& initial_elements = {}, QObject* parent = nullptr);
    FS_History(const QString& fallback, int history_size = default_history_size, const QStringList& initial_elements = {}, QObject* parent = nullptr);
    virtual ~FS_History() = default;

    void set_current_element(const QString& element);
    const QStringList& get_elements() const { return elements; }
    Operation_Mode operation_mode() const { return opmode; }

signals:
    void elements_changed(const QStringList&);

private:
    void check() { cleanup(false); }
    void cleanup(bool changed);
    bool is_valid(const QString& element) const;

    const Operation_Mode opmode;
    const int hist_size;
    const QString fb;
    QStringList elements;
    QFileSystemWatcher watcher;
};

#endif // SRC_FS_HISTORY_QT_FS_HISTORY
