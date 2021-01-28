#ifndef SRC_TIDDLERSTORE_QT_TIDDLER_MODEL_QT
#define SRC_TIDDLERSTORE_QT_TIDDLER_MODEL_QT

#include "tiddlerstore/tiddlerstore.h"

#include <QObject>

class Tiddler_Model
        : public QObject
{
    Q_OBJECT

public:
    explicit Tiddler_Model(QObject* parent = nullptr);
    virtual ~Tiddler_Model() = default;

signals:
    void title_changed();

    /// can also mean that text_history has changed if changed via request_set_tiddler_data()
    void text_changed();

    void history_size_changed();
    void tags_changed();
    void fields_changed();
    void lists_changed();
    void single_list_changed(const char* list_name);

public:
    Tiddlerstore::Tiddler tiddler() const;
    void request_set_tiddler_data(const Tiddlerstore::Tiddler& other);

    /// Tiddler API, modifier methods are prefixed with "request_", because they check for change and eventually don't emit the corresponding signal
    std::string title() const;
    void request_set_title(const std::string& new_title);
    int32_t history_size() const;
    void request_set_history_size(int32_t new_history_size);
    std::string text() const;
    std::deque<std::string> text_history() const;
    void request_set_text(const std::string& text);
    std::vector<std::string> tags() const;
    bool has_tag(const std::string& tag) const;
    void request_set_tag(const std::string& tag);
    void request_remove_tag(const std::string& tag);
    std::string field_value(const std::string& field_name) const;
    std::unordered_map<std::string, std::string> fields() const;
    void request_set_field(const std::string& field_name, const std::string& field_value);
    void request_remove_field(const std::string& field_name);
    std::vector<std::string> list(const std::string& list_name) const;
    std::unordered_map<std::string, std::vector<std::string>> lists() const;
    void request_set_list(const std::string& list_name, const std::vector<std::string>& values);
    void request_remove_list(const std::string& list_name);

private:
    Tiddlerstore::Tiddler t;
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLER_MODEL_QT
