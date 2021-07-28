#ifndef SRC_TIDDLERSTORE_QT_TIDDLER_MODEL_QT
#define SRC_TIDDLERSTORE_QT_TIDDLER_MODEL_QT

#include "tiddlerstore/tiddlerstore_types.h"

#include <QObject>

#include <deque>
#include <unordered_map>
#include <memory>

class Tiddler_Model
        : public QObject
{
    Q_OBJECT

public:
    explicit Tiddler_Model(Tiddlerstore::Tiddler& tiddler, QObject* parent = nullptr);
    virtual ~Tiddler_Model() = default;

signals:
    void title_changed();

    /// can also mean that text_history has changed if changed via request_set_tiddler_data()
    void text_changed();

    void history_size_changed();
    void tags_changed();
    void field_changed(const char* field_name);
    void field_added(const char* field_name);
    void field_removed(const char* field_name);
    void fields_reset();
    void list_changed(const char* list_name);
    void list_added(const char* list_name);
    void list_removed(const char* list_name);
    void lists_reset();

    /**
     * @brief remove_request object giving out the model shall delete it and the tiddler, removing it from a store if necessary
     *
     * use the destroyed() signal to react on deletion
     */
    void remove_request();

public:
    const Tiddlerstore::Tiddler& tiddler() const;
    void request_set_tiddler_data(const Tiddlerstore::Tiddler& other);
    void request_set_tiddler_data(const Tiddler_Model& other);

    /// Tiddler API, modifier methods are prefixed with "request_", because they check for change and eventually don't emit the corresponding signal
    std::string title() const;
    bool request_set_title(const std::string& new_title);
    std::int32_t history_size() const;
    bool request_set_history_size(std::int32_t new_history_size);
    std::string text() const;
    std::deque<std::string> text_history() const;
    bool request_set_text(const std::string& text);
    std::vector<std::string> tags() const;
    bool has_tag(const std::string& tag) const;
    bool request_set_tag(const std::string& tag);
    bool request_remove_tag(const std::string& tag);
    std::string field_value(const std::string& field_name) const;
    std::unordered_map<std::string, std::string> fields() const;
    Tiddlerstore::Set_Field_List_Change request_set_field(const std::string& field_name, const std::string& field_val);
    bool request_remove_field(const std::string& field_name);
    std::vector<std::string> list(const std::string& list_name) const;
    std::unordered_map<std::string, std::vector<std::string>> lists() const;
    Tiddlerstore::Set_Field_List_Change request_set_list(const std::string& list_name, const std::vector<std::string>& values);
    bool request_remove_list(const std::string& list_name);
    void request_remove() { emit remove_request(); }

private:
    Tiddlerstore::Tiddler& t;
};

class Tiddlerstore_Model
        : public QObject
{
    Q_OBJECT

public:
    explicit Tiddlerstore_Model(Tiddlerstore::Store& s, QObject* parent = nullptr);
    virtual ~Tiddlerstore_Model() = default;

    Tiddler_Model& add();
    Tiddler_Model* model_for_index(std::size_t index);
    Tiddler_Model* model_for_tiddler(const Tiddlerstore::Tiddler& t);
    Tiddlerstore::Store_Filter filter();
    std::vector<Tiddler_Model*> filtered_models(const Tiddlerstore::Store_Filter& filter);
    Tiddler_Model* first_filtered_model(const Tiddlerstore::Store_Filter& filter);

signals:
    /// a tiddler with the corresponding index in the store was created with the @see add() method; @see model_created() will be emitted before
    void added(std::size_t index);

    /// a model for a tiddler existing in the store was created
    void model_created(Tiddler_Model* model);

    /// a model and its tiddler existing in the store will be removed and deleted
    void begin_remove(Tiddler_Model* model);

    /// a model and its corresponding tiddler were removed
    void removed();

    /// forward all signals a Tiddler_Model has except remove_request which results in the store_model's begin_remove and removed signals
    void title_changed(Tiddler_Model* model);
    void text_changed(Tiddler_Model* model);
    void history_size_changed(Tiddler_Model* model);
    void tags_changed(Tiddler_Model* model);
    void field_changed(Tiddler_Model* model, const char* field_name);
    void field_added(Tiddler_Model* model, const char* field_name);
    void field_removed(Tiddler_Model* model, const char* field_name);
    void fields_reset(Tiddler_Model* model);
    void list_changed(Tiddler_Model* model, const char* list_name);
    void list_added(Tiddler_Model* model, const char* list_name);
    void list_removed(Tiddler_Model* model, const char* list_name);
    void lists_reset(Tiddler_Model* model);

private:
    Tiddlerstore::Store& data;
    std::unordered_map<const Tiddlerstore::Tiddler*, std::unique_ptr<Tiddler_Model>> active_models;
};

#endif // SRC_TIDDLERSTORE_QT_TIDDLER_MODEL_QT
