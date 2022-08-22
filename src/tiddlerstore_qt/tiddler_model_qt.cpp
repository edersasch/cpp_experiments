#include "tiddler_model_qt.h"
#include "tiddlerstore/tiddlerstore.h"

Tiddler_Model::Tiddler_Model(Tiddlerstore::Tiddler& tiddler, QObject* parent)
    : QObject(parent)
    , t(tiddler)
{
}

const Tiddlerstore::Tiddler& Tiddler_Model::tiddler() const
{
    return t;
}

void Tiddler_Model::set_tiddler_data(const Tiddlerstore::Tiddler &other)
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

void Tiddler_Model::set_tiddler_data(const Tiddler_Model &other)
{
    set_tiddler_data(other.t);
}

std::string Tiddler_Model::title() const
{
    return t.title();
}

bool Tiddler_Model::set_title(const std::string& new_title)
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

bool Tiddler_Model::set_history_size(std::int32_t new_history_size)
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

std::vector<std::string> Tiddler_Model::text_history() const
{
    return t.text_history();
}

bool Tiddler_Model::set_text(const std::string& text)
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

bool Tiddler_Model::set_tag(const std::string& tag)
{
    if (t.set_tag(tag)) {
        emit tags_changed();
        return true;
    }
    return false;
}

bool Tiddler_Model::remove_tag(const std::string& tag)
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

Tiddlerstore::Set_Field_List_Change Tiddler_Model::set_field(const std::string& field_name, const std::string& field_val)
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

bool Tiddler_Model::remove_field(const std::string& field_name)
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

Tiddlerstore::Set_Field_List_Change Tiddler_Model::set_list(const std::string& list_name, const std::vector<std::string>& values)
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

bool Tiddler_Model::remove_list(const std::string& list_name)
{
    if (t.remove_list(list_name)) {
        emit list_removed(list_name.c_str());
        return true;
    }
    return false;
}

bool Tiddler_Model::is_empty()
{
    return t.is_empty();
}

Tiddlerstore_Model::Tiddlerstore_Model(Tiddlerstore::Store& s, QObject* parent)
    : QObject(parent)
    , data(s)
{
}

Tiddler_Model& Tiddlerstore_Model::add()
{
    auto& t = *data.emplace_back(new Tiddlerstore::Tiddler);
    auto tm = model_for_tiddler(t);
    emit added(data.size() - 1);
    return *tm;
}

Tiddler_Model* Tiddlerstore_Model::model_for_index(std::size_t index)
{
    if (index < data.size()) {
        return model_for_tiddler(*data[static_cast<std::size_t>(index)]);
    }
    return nullptr;
}

Tiddler_Model* Tiddlerstore_Model::model_for_tiddler(const Tiddlerstore::Tiddler& t)
{
    auto it = active_models.find(&t);
    if (it == active_models.end()) {
        auto store_it = Tiddlerstore::tiddler_pos_in_store(t, data);
        if (store_it != data.end()) {
            auto tiddler = store_it->get();
            it = active_models.insert({tiddler, std::make_unique<Tiddler_Model>(*tiddler)}).first;
            auto tm = it->second.get();
            connect(tm, &Tiddler_Model::title_changed,          tm, [this, tm]                          { emit title_changed(tm); });
            connect(tm, &Tiddler_Model::text_changed,           tm, [this, tm]                          { emit text_changed(tm); });
            connect(tm, &Tiddler_Model::history_size_changed,   tm, [this, tm]                          { emit history_size_changed(tm); });
            connect(tm, &Tiddler_Model::tags_changed,           tm, [this, tm]                          { emit tags_changed(tm); });
            connect(tm, &Tiddler_Model::field_changed,          tm, [this, tm](const char* field_name)  { emit field_changed(tm, field_name); });
            connect(tm, &Tiddler_Model::field_added,            tm, [this, tm](const char* field_name)  { emit field_added(tm, field_name); });
            connect(tm, &Tiddler_Model::field_removed,          tm, [this, tm](const char* field_name)  { emit field_removed(tm, field_name); });
            connect(tm, &Tiddler_Model::fields_reset,           tm, [this, tm]                          { emit fields_reset(tm); });
            connect(tm, &Tiddler_Model::list_changed,           tm, [this, tm](const char* list_name)   { emit list_changed(tm, list_name); });
            connect(tm, &Tiddler_Model::list_added,             tm, [this, tm](const char* list_name)   { emit list_added(tm, list_name); });
            connect(tm, &Tiddler_Model::list_removed,           tm, [this, tm](const char* list_name)   { emit list_removed(tm, list_name); });
            connect(tm, &Tiddler_Model::lists_reset,            tm, [this, tm]                          { emit lists_reset(tm); });
            connect(tm, &Tiddler_Model::remove_request,         tm, [this, tm, tiddler] {
                emit begin_remove(tm);
                active_models.erase(tiddler);
                Tiddlerstore::erase_tiddler_from_store(*tiddler, data);
                emit removed();
            });
            emit model_created(tm);
        } else {
            return nullptr;
        }
    }
    return it->second.get();
}

Tiddlerstore::Filter Tiddlerstore_Model::filter()
{
    return Tiddlerstore::Filter(data);
}

std::vector<Tiddler_Model*> Tiddlerstore_Model::filtered_models(Tiddlerstore::Filter& filter)
{
    std::vector<Tiddler_Model*> ret;
    for (const auto& i : filter.filtered_idx()) {
        auto model = model_for_index(i);
        if (model) {
            ret.push_back(model);
        }
    }
    return ret;
}

Tiddler_Model* Tiddlerstore_Model::first_filtered_model(Tiddlerstore::Filter& filter)
{
    auto idx = filter.filtered_idx();
    return idx.empty() ? nullptr : model_for_index(idx.front());
}
