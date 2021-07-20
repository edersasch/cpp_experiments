#ifndef SRC_TIDDLERSTORE_TIDDLERSTORE_TYPES
#define SRC_TIDDLERSTORE_TIDDLERSTORE_TYPES

#include <vector>
#include <memory>
#include <string>

namespace Tiddlerstore
{

class Tiddler;
using Store = std::vector<std::unique_ptr<Tiddler>>;
using Store_Indexes = std::vector<std::size_t>;
class Store_Filter;

enum class Set_Field_List_Change
{
    None,   /// nothing changed
    Value,  /// an already present element got a different value
    Add,    /// a new element was added
    Remove  /// an elment was removed
};

enum class Filter_Type
{
    Title,
    Text,
    Tag,
    Field,
    List
};

struct Filter_Data
{
    Filter_Type filter_type {Filter_Type::Title};
    bool negate {false};
    /// title, text, tag, field name, list name
    std::string key {};
    /// for type Field
    std::string field_value {};
    /// for type List
    std::vector<std::string> list_value {};
    /// for title, text
    bool case_sensitive {false};
};

using Single_Group = std::vector<std::unique_ptr<Filter_Data>>;
using Filter_Groups = std::vector<std::unique_ptr<Single_Group>>;

}

#endif // SRC_TIDDLERSTORE_TIDDLERSTORE_TYPES
