#ifndef SRC_TIDDLERSTORE_TIDDLERSTORE_TYPES
#define SRC_TIDDLERSTORE_TIDDLERSTORE_TYPES

#include <vector>
#include <memory>
#include <string>

namespace Tiddlerstore
{

class Tiddler;
using Store = std::vector<std::unique_ptr<Tiddler>>;
class Filter;
class Filter_Group;

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
    Title_Contains,
    Text,
    Text_Contains,
    Tag,
    Tagged,
    Field,
    List
};

struct Filter_Element
{
    Filter_Type filter_type {Filter_Type::Title};
    bool negate {false};
    /// for all types except Tagged
    std::string key {};
    /// for type Field
    std::string field_value {};
    /// for type List
    std::vector<std::string> list_value {};
    /// for types Title_Contains, Text_Contains
    bool case_sensitive {false};
};

}

#endif // SRC_TIDDLERSTORE_TIDDLERSTORE_TYPES
