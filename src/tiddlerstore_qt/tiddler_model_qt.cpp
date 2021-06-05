#include "tiddler_model_qt.h"
#include "tiddlerstore/tiddlerstore.h"

Tiddler_Model::Tiddler_Model(Tiddlerstore::Tiddler* tiddler, QObject* parent)
    : QObject(parent)
    , t(tiddler)
{
}

Tiddlerstore::Tiddler* Tiddler_Model::tiddler() const
{
    return t;
}

void Tiddler_Model::request_set_tiddler_data(const Tiddlerstore::Tiddler &other)
{
    if (t) {
        bool title_will_change          = t->title()        != other.title();
        bool history_size_will_change   = t->history_size() != other.history_size();
        bool text_will_change           = t->text_history() != other.text_history();
        bool tags_will_change           = t->tags()         != other.tags();
        bool fields_will_change         = t->fields()       != other.fields();
        bool lists_will_change          = t->lists()        != other.lists();
        *t = other;
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
            emit fields_changed();
        }
        if (lists_will_change) {
            emit lists_changed();
        }
    }
}

std::string Tiddler_Model::title() const
{
    return t ? t->title() : std::string();
}

void Tiddler_Model::request_set_title(const std::string& new_title)
{
    if (t && t->set_title(new_title)) {
        emit title_changed();
    }
}

int32_t Tiddler_Model::history_size() const
{
    return t ? t->history_size() : 0;
}

void Tiddler_Model::request_set_history_size(int32_t new_history_size)
{
    if (t && t->set_history_size(new_history_size)) {
        emit history_size_changed();
    }
}

std::string Tiddler_Model::text() const
{
    return t ? t->text() : std::string();
}

std::deque<std::string> Tiddler_Model::text_history() const
{
    return t ? t->text_history() : std::deque<std::string>();
}

void Tiddler_Model::request_set_text(const std::string& text)
{
    if (t && t->set_text(text)) {
        emit text_changed();
    }
}

std::vector<std::string> Tiddler_Model::tags() const
{
    return t ? t->tags() : std::vector<std::string>();
}

bool Tiddler_Model::has_tag(const std::string& tag) const
{
    return t ? t->has_tag(tag) : false;
}

void Tiddler_Model::request_set_tag(const std::string& tag)
{
    if (t && t->set_tag(tag)) {
        emit tags_changed();
    }
}

void Tiddler_Model::request_remove_tag(const std::string& tag)
{
    if (t && t->remove_tag(tag)) {
        emit tags_changed();
    }
}

std::string Tiddler_Model::field_value(const std::string& field_name) const
{
    return t ? t->field_value(field_name) : std::string();
}

std::unordered_map<std::string, std::string> Tiddler_Model::fields() const
{
    return t ? t->fields() : std::unordered_map<std::string, std::string>();
}

void Tiddler_Model::request_set_field(const std::string& field_name, const std::string& field_val)
{
    if (t && t->set_field(field_name, field_val)) {
        emit fields_changed();
    }
}

void Tiddler_Model::request_remove_field(const std::string& field_name)
{
    if (t && t->remove_field(field_name)) {
        emit fields_changed();
    }
}

std::vector<std::string> Tiddler_Model::list(const std::string& list_name) const
{
    return t ? t->list(list_name) : std::vector<std::string>();
}

std::unordered_map<std::string, std::vector<std::string>> Tiddler_Model::lists() const
{
    return t ? t->lists() : std::unordered_map<std::string, std::vector<std::string>>();
}

void Tiddler_Model::request_set_list(const std::string& list_name, const std::vector<std::string>& values)
{
    if (t) {
        switch (t->set_list(list_name, values)) {
        case Tiddlerstore::Tiddler::List_Change_Value::Single_List_Changed:
            emit single_list_changed(list_name.c_str());
            break;
        case Tiddlerstore::Tiddler::List_Change_Value::Lists_Changed:
            emit lists_changed();
            break;
        default:
            break;
        }
    }
}

void Tiddler_Model::request_remove_list(const std::string& list_name)
{
    if (t && t->remove_list(list_name)) {
        emit lists_changed();
    }
}
