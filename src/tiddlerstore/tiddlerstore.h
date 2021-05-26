#ifndef SRC_TIDDLERSTORE_TIDDLERSTORE
#define SRC_TIDDLERSTORE_TIDDLERSTORE

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <deque>
#include <unordered_map>

namespace Tiddlerstore
{

static constexpr auto version_key           = "version";

class Tiddler
{
public:
    /// Create empty tiddler
    Tiddler() = default;
    Tiddler(const Tiddler& other) = default;
    Tiddler& operator=(const Tiddlerstore::Tiddler& rhs) = default;
    virtual ~Tiddler() = default;

    static constexpr auto version_value         = 1;
    static constexpr auto title_key             = "title";
    static constexpr auto history_size_key      = "history_size";
    static constexpr auto text_history_key      = "text_history";
    static constexpr auto tags_key              = "tags";
    static constexpr auto fields_key            = "fields";
    static constexpr auto lists_key             = "lists";

    std::string title() const;

    /// If new_title is empty, the old title is preserved.
    void set_title(const std::string& new_title);

    int32_t history_size() const;

    /// If new_history_size < 1, the old size is preserved. On values > 100 the max history size 100 is used.
    void set_history_size(int32_t new_history_size);
    std::string text() const;
    std::deque<std::string> text_history() const;

    /// If text is not empty, it is put in front of the existing history. The oldest entry will be discarded if history_size() would be exceeded.
    void set_text(const std::string& text);

    std::vector<std::string> tags() const;
    bool has_tag(const std::string& tag) const;

    /// The tag will be added if it is not empty and not already present.
    void set_tag(const std::string& tag);

    /// Removing a nonexisting tag does nothing
    void remove_tag(const std::string& tag);

    std::string field_value(const std::string& field_name) const;
    std::unordered_map<std::string, std::string> fields() const;

    /// Nothing will be done if field_name is empty. An empty field_value removes the field.
    void set_field(const std::string& field_name, const std::string& field_value);

    /// Removing a nonexisting field does nothing
    void remove_field(const std::string& field_name);

    std::vector<std::string> list(const std::string& list_name) const;
    std::unordered_map<std::string, std::vector<std::string>> lists() const;

    /// Nothing will be done if list_name is empty. Empty strings get removed from values paratemer. An empty values parameter removes the list. Duplicate entries in the values parameter are preserved.
    void set_list(const std::string& list_name, std::vector<std::string> values);

    /// Removing a nonexisting list does nothing
    void remove_list(const std::string& list_name);

private:
    std::string tiddler_title {};
    int32_t text_history_size {1};
    std::deque<std::string> tiddler_text_history {};
    std::vector<std::string> tiddler_tags {};
    std::unordered_map<std::string, std::string> tiddler_fields {};
    std::unordered_map<std::string, std::vector<std::string>> tiddler_lists {};
};

/// Does not export empty data
void to_json(nlohmann::json& j, const Tiddler& t);

/// Every json key except version is optional. Passing an already filled tiddler will add text_history, tags, fields and lists.
void from_json(const nlohmann::json& j, Tiddler& t);

using Store = std::vector<std::unique_ptr<Tiddler>>;
void to_json(nlohmann::json& j, const Store& s);
void from_json(const nlohmann::json& j, Store& s);
Store open_store_from_file(const std::string& path);
bool save_store_to_file(const Store& store, const std::string& path);

class Store_Filter
{
public:
    Store_Filter(const Store& all);
    ~Store_Filter() = default;

    Store_Filter& title(const std::string& title_value);
    Store_Filter& n_title(const std::string& title_value);
    Store_Filter& tag(const std::string& tag_value);
    Store_Filter& n_tag(const std::string& tag_value);
    Store_Filter& tagged();
    Store_Filter& n_tagged();
    Store_Filter& field(const std::string& field_name, const std::string& value = {});
    Store_Filter& n_field(const std::string& field_name, const std::string& value = {});
    Store_Filter& list(const std::string& list_name, const std::vector<std::string>& contains = {});
    Store_Filter& n_list(const std::string& list_name, const std::vector<std::string>& contains = {});
    std::vector<std::size_t> filtered_idx();

private:
    const Store& s;
    std::vector<std::size_t> idx;
};

}

#endif // SRC_TIDDLERSTORE_TIDDLERSTORE
