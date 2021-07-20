#include "tiddler_model_qt.h"
#include "tiddlerstore/tiddlerstore.h"

Tiddler_Model::Tiddler_Model(Tiddlerstore::Tiddler& tiddler, QObject* parent)
    : QObject(parent)
    , t(tiddler)
{
}

Tiddlerstore::Tiddler& Tiddler_Model::tiddler() const
{
    return t;
}

void Tiddler_Model::request_set_tiddler_data(const Tiddlerstore::Tiddler &other)
{
    bool title_will_change          = t.title()        != other.title();
    bool history_size_will_change   = t.history_size() != other.history_size();
    bool text_will_change           = t.text_history() != other.text_history();
    bool tags_will_change           = t.tags()         != other.tags();
    bool fields_will_change         = t.fields()       != other.fields();
    bool lists_will_change          = t.lists()        != other.lists();
    t = other;
    if (title_will_change) {
        emit title_changed();
    }
    if (history_size_will_change) {
        emit history_size_changed();
    }
    if (text_will_change) {
        emit text_changed();
    }
    if (tags_will_change) {
        emit tags_changed();
    }
    if (fields_will_change) {
        emit fields_reset();
    }
    if (lists_will_change) {
        emit lists_reset();
    }
}

std::string Tiddler_Model::title() const
{
    return t.title();
}

bool Tiddler_Model::request_set_title(const std::string& new_title)
{
    if (t.set_title(new_title)) {
        emit title_changed();
        return true;
    }
    return false;
}

std::int32_t Tiddler_Model::history_size() const
{
    return t.history_size();
}

bool Tiddler_Model::request_set_history_size(std::int32_t new_history_size)
{
    if (t.set_history_size(new_history_size)) {
        emit history_size_changed();
        return true;
    }
    return false;
}

std::string Tiddler_Model::text() const
{
    return t.text();
}

std::deque<std::string> Tiddler_Model::text_history() const
{
    return t.text_history();
}

bool Tiddler_Model::request_set_text(const std::string& text)
{
    if (t.set_text(text)) {
        emit text_changed();
        return true;
    }
    return false;
}

std::vector<std::string> Tiddler_Model::tags() const
{
    return t.tags();
}

bool Tiddler_Model::has_tag(const std::string& tag) const
{
    return t.has_tag(tag);
}

bool Tiddler_Model::request_set_tag(const std::string& tag)
{
    if (t.set_tag(tag)) {
        emit tags_changed();
        return true;
    }
    return false;
}

bool Tiddler_Model::request_remove_tag(const std::string& tag)
{
    if (t.remove_tag(tag)) {
        emit tags_changed();
        return true;
    }
    return false;
}

std::string Tiddler_Model::field_value(const std::string& field_name) const
{
    return t.field_value(field_name);
}

std::unordered_map<std::string, std::string> Tiddler_Model::fields() const
{
    return t.fields();
}

Tiddlerstore::Set_Field_List_Change Tiddler_Model::request_set_field(const std::string& field_name, const std::string& field_val)
{
    switch(t.set_field(field_name, field_val)) {
    case Tiddlerstore::Set_Field_List_Change::Add:
        emit field_added(field_name.c_str());
        return Tiddlerstore::Set_Field_List_Change::Add;
    case Tiddlerstore::Set_Field_List_Change::Remove:
        emit field_removed(field_name.c_str());
        return Tiddlerstore::Set_Field_List_Change::Remove;
    case Tiddlerstore::Set_Field_List_Change::Value:
        emit field_changed(field_name.c_str());
        return Tiddlerstore::Set_Field_List_Change::Value;
    default:
        break;
    }
    return Tiddlerstore::Set_Field_List_Change::None;
}

bool Tiddler_Model::request_remove_field(const std::string& field_name)
{
    if (t.remove_field(field_name)) {
        emit field_removed(field_name.c_str());
        return true;
    }
    return false;
}

std::vector<std::string> Tiddler_Model::list(const std::string& list_name) const
{
    return t.list(list_name);
}

std::unordered_map<std::string, std::vector<std::string>> Tiddler_Model::lists() const
{
    return t.lists();
}

Tiddlerstore::Set_Field_List_Change Tiddler_Model::request_set_list(const std::string& list_name, const std::vector<std::string>& values)
{
    switch(t.set_list(list_name, values)) {
    case Tiddlerstore::Set_Field_List_Change::Add:
        emit list_added(list_name.c_str());
        return Tiddlerstore::Set_Field_List_Change::Add;
    case Tiddlerstore::Set_Field_List_Change::Remove:
        emit list_removed(list_name.c_str());
        return Tiddlerstore::Set_Field_List_Change::Remove;
    case Tiddlerstore::Set_Field_List_Change::Value:
        emit list_changed(list_name.c_str());
        return Tiddlerstore::Set_Field_List_Change::Value;
    default:
        break;
    }
    return Tiddlerstore::Set_Field_List_Change::None;
}

bool Tiddler_Model::request_remove_list(const std::string& list_name)
{
    if (t.remove_list(list_name)) {
        emit list_removed(list_name.c_str());
        return true;
    }
    return false;
}

Tiddlerstore_Model::Tiddlerstore_Model(Tiddlerstore::Store& s, QObject* parent)
    : QObject(parent)
    , data(s)
{
}

Tiddler_Model* Tiddlerstore_Model::add()
{
    auto t = data.emplace_back(new Tiddlerstore::Tiddler).get();
    emit added(static_cast<int>(data.size() - 1));
    return model_for_tiddler(t);
}

Tiddler_Model* Tiddlerstore_Model::model_for_index(std::int32_t index)
{
    if (index >= 0 && static_cast<std::size_t>(index) < data.size()) {
        return model_for_tiddler(data[static_cast<std::size_t>(index)].get());
    }
    return nullptr;
}

Tiddler_Model* Tiddlerstore_Model::model_for_tiddler(Tiddlerstore::Tiddler* t)
{
    auto it = active_models.find(t);
    if (it == active_models.end()) {
        if (Tiddlerstore::is_tiddler_in_store(*t, data)) {
            it = active_models.insert({t, std::make_unique<Tiddler_Model>(*t)}).first;
            connect(it->second.get(), &Tiddler_Model::remove_request, this, [this, t] {
                active_models.erase(t);
                Tiddlerstore::erase_tiddler_from_store(*t, data);
                emit removed();
            });
            emit model_created(it->second.get());
        } else {
            return nullptr;
        }
    }
    return it->second.get();
}
