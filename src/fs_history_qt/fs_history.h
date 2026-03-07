#ifndef SRC_FS_HISTORY_QT_FS_HISTORY
#define SRC_FS_HISTORY_QT_FS_HISTORY

#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QStringList>

#include <algorithm>

template <typename T> void move_to_front(T& sequence, std::int32_t pos)
{
    auto it = sequence.begin() + pos;
    std::rotate(sequence.begin(), it, it + 1);
};

class FsHistory
: public QObject
{
    Q_OBJECT

public:
    enum class Operation_Mode { OP_FILE, OP_DIR };

    /// the default is 9, so for example a single character shortcut in a menu is possible
    static constexpr std::int32_t defaultHistorySize { 9 };

    /**
     * @brief FsHistory        keep a history of the last selected files or dircetories and watch for change; emits
     *                          @see elemets_changed()
     * @param mode              select whether to accept directories or files, @see Operation_Mode
     * @param fallback          only relevant for Operation_Mode::OP_DIR: in case the history is empty, use the given
     *                          path as fallback entry if it is valid
     * @param history_size      how many entries shall be stored, 2 is minimum
     * @param initial_elements  populate the elements list with valid entries according to @see Operation_Mode,
     *                          duplicates are removed
     * @param parent            Qt object hierarchy
     */
    FsHistory(Operation_Mode mode, QString fallback, std::int32_t history_size, QStringList initial_elements,
        QObject* parent = nullptr);

    /// convenience constructor to set Operation_Mode::OP_FILE with @see defaultHistorySize
    FsHistory(const QStringList& initial_elements = {}, QObject* parent = nullptr)
    : FsHistory(Operation_Mode::OP_FILE, "", defaultHistorySize, initial_elements, parent)
    {
    }

    /// convenience constructor to set Operation_Mode::OP_FILE with custom history_size
    FsHistory(std::int32_t history_size, const QStringList& initial_elements = {}, QObject* parent = nullptr)
    : FsHistory(Operation_Mode::OP_FILE, "", history_size, initial_elements, parent)
    {
    }

    /// convenience constructor to set Operation_Mode::OP_DIR with fallback, history_size and initial_elements
    FsHistory(const QString& fallback, std::int32_t history_size = defaultHistorySize,
        const QStringList& initial_elements = {}, QObject* parent = nullptr)
    : FsHistory(Operation_Mode::OP_DIR, fallback, history_size, initial_elements, parent)
    {
    }

    virtual ~FsHistory() override;

    /**
     * @brief potentially remove last entry, emit @see elements_changed() on change
     * @param move or add to top of the list if it is valid according to @see Operation_Mode
     */
    void set_current_element(const QString& element);

    /// return the current list of valid mElements
    const QStringList& get_elements() const { return mElements; }

    /// return the selected mode
    Operation_Mode operation_mode() const { return mOpmode; }

signals:
    /// is emitted once after construction and on change
    void elements_changed(const QStringList&);

private:
    void check() { cleanup(false); }
    void cleanup(bool changed);
    bool is_valid(const QString& element) const;

    const Operation_Mode mOpmode;
    const std::int32_t mHistorySize;
    const QString mFallbackDir;
    QStringList mElements;
    QFileSystemWatcher mFsWatcher;
};

#endif // SRC_FS_HISTORY_QT_FS_HISTORY
