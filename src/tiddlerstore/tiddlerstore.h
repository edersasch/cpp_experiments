#ifndef SRC_TIDDLERSTORE_TIDDLERSTORE
#define SRC_TIDDLERSTORE_TIDDLERSTORE

#include "tiddlerstore_types.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Tiddlerstore
{

static constexpr auto version_key           = "v";
static constexpr auto version_value         = 1;

class Tiddler
{
public:
    /// Create empty tiddler
    Tiddler() = default;
    Tiddler(const Tiddler& other) = default;
    Tiddler& operator=(const Tiddlerstore::Tiddler& rhs) = default;
    virtual ~Tiddler() = default;

    static constexpr auto           title_key                       = "ti";
    static constexpr auto           history_size_key                = "hs";
    static constexpr auto           text_history_key                = "th";
    static constexpr auto           tags_key                        = "ta";
    static constexpr auto           fields_key                      = "f";
    static constexpr auto           lists_key                       = "l";
    static constexpr std::int32_t   default_text_history_size       = 1;

    std::string title() const;

    /**
     * @brief set_title set a new title
     * @param new_title may also be empty
     * @return true on change
     */
    bool set_title(const std::string& new_title);

    std::int32_t history_size() const;

    /**
     * @brief set_history_size set a new history size between 1 and 100
     * @param new_history_size old size is preserved if < 1, 100 is used if > 100
     * @return true on change
     */
    bool set_history_size(std::int32_t new_history_size);
    std::string text() const;
    std::vector<std::string> text_history() const;

    /**
     * @brief set_text set a new text, potentially altering the history
     * @param text put in front of the existing history, may also be empty, oldest entry will be discarded if @see history_size() would be exceeded
     * @return true on change
     */
    bool set_text(const std::string& text);

    std::vector<std::string> tags() const;
    bool has_tag(const std::string& tag) const;

    /**
     * @brief set_tag set a new non-empty tag
     * @param tag will be added if it is not empty and not already present
     * @return true on add
     */
    bool set_tag(const std::string& tag);

    /**
     * @brief remove_tag remove a present non-empty tag
     * @param tag removing a nonexisting or empty tag does nothing
     * @return true on remove
     */
    bool remove_tag(const std::string& tag);

    std::string field_value(const std::string& field_name) const;
    std::unordered_map<std::string, std::string> fields() const;

    /**
     * @brief set_field add / remove / modify a field
     * @param field_name if empty nothing will be done
     * @param field_value removes the field if empty
     * @return @see Set_Field_List_Change
     */
    Set_Field_List_Change set_field(const std::string& field_name, const std::string& field_value);

    /**
     * @brief remove_field remove a present field
     * @param field_name will not remove if empty or field is nonexisting
     * @return true on remove
     */
    bool remove_field(const std::string& field_name);

    std::vector<std::string> list(const std::string& list_name) const;
    std::unordered_map<std::string, std::vector<std::string>> lists() const;

    /**
     * @brief add / remove / modify a list
     * @param list_name nothing will be done if list_name is empty.
     * @param values empty strings get removed, an empty parameter removes the list, duplicate entries in the values parameter are preserved
     * @return @see Set_Field_List_Change
     */
    Set_Field_List_Change set_list(const std::string& list_name, std::vector<std::string> values);

    /**
     * @brief remove_list remove a present list
     * @param list_name will not remove if empty or list is nonexisting
     * @return true on remove
     */
    bool remove_list(const std::string& list_name);

    /// true if everything is empty, text_history_size does not matter
    bool is_empty();

private:
    std::string tiddler_title {};
    std::int32_t text_history_size {default_text_history_size};
    std::vector<std::string> tiddler_text_history {};
    std::vector<std::string> tiddler_tags {};
    std::unordered_map<std::string, std::string> tiddler_fields {};
    std::unordered_map<std::string, std::vector<std::string>> tiddler_lists {};
};

/// Does not export empty data
void to_json(nlohmann::json& j, const Tiddler& t);

/// Every json key except version is optional. Passing an already filled tiddler will add text_history, tags, fields and lists.
void from_json(const nlohmann::json& j, Tiddler& t);

/// no separate versioning for Store, version_value is included in every tiddler (negligible size increase of 5 chars per tiddler)
void to_json(nlohmann::json& j, const Store& s);
void from_json(const nlohmann::json& j, Store& s);
Store open_store_from_file(const std::string& path);
bool save_store_to_file(const Store& store, const std::string& path);

/// returns all tags used in a store
std::unordered_set<std::string> store_tags(const Store& store);

/// returns all field names used in a store
std::unordered_set<std::string> store_fields(const Store& store);

/// returns all list names used in a store
std::unordered_set<std::string> store_lists(const Store& store);

/// returns an iterator to the position in the store
auto tiddler_pos_in_store(const Tiddler& tiddler, const Store& store) -> decltype(store.begin());

/// erases a tiddler from the store, it's of course invalid afterwards
void erase_tiddler_from_store(const Tiddler& tiddler, Store& store);

/// returns true if a tiddler is in the store
bool is_tiddler_in_store(const Tiddler& tiddler, const Store& store);

/// create a Filter_Element to filter for title with the given title_value. If title_value is empty it will match all empty titles.
Filter_Element title(const std::string& title_value);

/// create a Filter_Element to filter for title where the given title_value does not match. If title_value is empty it will match all non-empty titles.
Filter_Element n_title(const std::string& title_value);

/// create a Filter_Element to filter for title that contains the given title_value. If title_value is empty it will match all titles.
Filter_Element title_contains(const std::string& title_value, bool case_sensitive = false);

/// create a Filter_Element to filter for title that does not contain the given title_value. If title_value is empty it will match nothing.
Filter_Element n_title_contains(const std::string& title_value, bool case_sensitive = false);

/// create a Filter_Element to filter for text with the given text_value. If text_value is empty it will match all empty texts.
Filter_Element text(const std::string& text_value);

/// create a Filter_Element to filter for text where the given text_value does not match. If text_value is empty it will match all non-empty titles.
Filter_Element n_text(const std::string& text_value);

/// create a Filter_Element to filter for text that contains the given text_value. If text_value is empty it will match all texts.
Filter_Element text_contains(const std::string& text_value, bool case_sensitive = false);

/// create a Filter_Element to filter for text that does not contain the given text_value. If text_value is empty it will match nothing.
Filter_Element n_text_contains(const std::string& text_value, bool case_sensitive = false);

/// create a Filter_Element to filter for the given tag_value. If tag_value is empty it will match nothing.
Filter_Element tag(const std::string& tag_value);

/// create a Filter_Element to filter for tiddlers not having the given tag_value. If tag_value is empty it will match everything.
Filter_Element n_tag(const std::string& tag_value);

/// create a Filter_Element to filter for tiddlers that have at least one tag
Filter_Element tagged();

/// create a Filter_Element to filter for tiddlers that have no tags
Filter_Element n_tagged();

/// create a Filter_Element to filter for the field given in field_name, optionally checking for matching value. If field_name is empty it will match nothing.
Filter_Element field(const std::string& field_name, const std::string& value = {});

/// create a Filter_Element to filter for tiddlers that do not have the field given in field_name, or if not empty do not have the matching value. If field_name is empty it will match everything.
Filter_Element n_field(const std::string& field_name, const std::string& value = {});

/// create a Filter_Element to filter for the list given in list_name, optionally checking for matching entries in contains. If list_name is empty it will match nothing.
Filter_Element list(const std::string& list_name, const std::vector<std::string>& contains = {});

/// create a Filter_Element to filter for tiddlers that do not have the list given in list_name, or if not empty do not have the mathing entries in contains. If list_name is empty it will match everything.
Filter_Element n_list(const std::string& list_name, const std::vector<std::string>& contains = {});

/// a Filter without a Filter_Element matches all tiddlers in a store
class Filter
{
public:
    /// a Filter is always connected to a Ftore
    explicit Filter(const Store& store);
    Filter(const Filter& other);
    virtual ~Filter() = default;
    Filter& operator=(const Filter& other) = delete;

    /// no operator=, returns false if Store s is different
    bool assign(const Filter& other);

    /// clear filter elements
    void clear();

    /// append a Filter_Element
    Filter& append(const Filter_Element& element);

    /// replace the content of a Filter_Element, invalidates the filter, no bounds checking
    void set_element(std::size_t pos, const Filter_Element& element);

    /// remove a Filter_Element, invalidates the filter, no bounds checking
    void remove(std::size_t pos);

    /// returns a copy of the present filter elements
    std::vector<Filter_Element> elements();

    /// Filter deletes its cached result, to be called if store data has changed
    void invalidate();

    /// returns store indexes where the underlying tiddler matches every Filter_Element
    std::vector<std::size_t> filtered_idx();

    /// returns tiddlers that match every Filter_Element
    std::vector<Tiddler*> filtered_tiddlers();

    /// if you are only inerested in one entry, returns nullptr if there is none
    Tiddler* first_filtered_tiddler();

private:
    void apply();

    const Store& s;
    std::vector<std::size_t> idx;
    std::vector<Filter_Element> fe;
    std::size_t filter_steps_done {std::numeric_limits<std::size_t>::max()};
};

class Filter_Group
{
public:
    /// a Filter_Group is always connected to a Store
    Filter_Group(Store& store);
    virtual ~Filter_Group() = default;

    /// append a Filter for the same Store as the Filter_Group
    Filter& append();

    /// remove the Filter given in to_remove if it is part of the Filter_Group
    void remove(const Filter& to_remove);

    /// returns the Filter at the position given in pos, no bounds checking
    Filter& at(std::size_t pos);

    /// returns the current filters
    std::vector<Filter*> filters();

    /// all filters delete their cached result, to be called if store data has changed
    void invalidate();

    /// returns tiddlers that match any, not every, Filter
    std::vector<Tiddler*> filtered_tiddlers();

    /// if you are only inerested in one entry, returns nullptr if there is none
    Tiddler* first_filtered_tiddler();

private:
    const Store& s;
    std::vector<std::unique_ptr<Filter>> f;
};

}

#endif // SRC_TIDDLERSTORE_TIDDLERSTORE
