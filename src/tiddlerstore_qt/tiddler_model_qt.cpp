#include "tiddler_model_qt.h"

Tiddler_Model::Tiddler_Model(QObject* parent)
    : QObject(parent)
{
}

Tiddlerstore::Tiddler Tiddler_Model::tiddler() const
{
    return t;
}

std::string Tiddler_Model::title() const
{
    return t.title();
}


void Tiddler_Model::request_set_title(const std::string& new_title)
{
    auto old = t.title();
    t.set_title(new_title);
    if (old != t.title()) {
        emit title_changed();
    }
}

int32_t Tiddler_Model::history_size() const
{
    return t.history_size();
}

void Tiddler_Model::request_set_history_size(int32_t new_history_size)
{
    auto old = t.history_size();
    t.set_history_size(new_history_size);
    if (old != t.history_size()) {
        emit history_size_changed();
    }
}

std::string Tiddler_Model::text() const
{
    return t.text();
}

std::deque<std::string> Tiddler_Model::text_history() const
{
    return t.text_history();
}

void Tiddler_Model::request_set_text(const std::string& text)
{
    auto old = t.text();
    t.set_text(text);
    if (old != t.text()) {
        emit text_changed();
    }
}

std::vector<std::string> Tiddler_Model::tags() const
{
    return t.tags();
}

bool Tiddler_Model::has_tag(const std::string& tag) const
{
    return t.has_tag(tag);
}

void Tiddler_Model::request_set_tag(const std::string& tag)
{
    auto old = t.tags();
    t.set_tag(tag);
    if (old != t.tags()) {
        emit tags_changed();
    }
}

void Tiddler_Model::request_remove_tag(const std::string& tag)
{
    auto old = t.tags();
    t.remove_tag(tag);
    if (old != t.tags()) {
        emit tags_changed();
    }
}

std::string Tiddler_Model::field_value(const std::string& field_name) const
{
    return t.field_value(field_name);
}

std::unordered_map<std::string, std::string> Tiddler_Model::fields() const
{
    return t.fields();
}

void Tiddler_Model::request_set_field(const std::string& field_name, const std::string& field_value)
{
    auto old = t.fields();
    t.set_field(field_name, field_value);
    if (old != t.fields()) {
        emit fields_changed();
    }
}

void Tiddler_Model::request_remove_field(const std::string& field_name)
{
    auto old = t.fields();
    t.remove_field(field_name);
    if (old != t.fields()) {
        emit fields_changed();
    }
}

std::vector<std::string> Tiddler_Model::list(const std::string& list_name) const
{
    return t.list(list_name);
}

std::unordered_map<std::string, std::vector<std::string>> Tiddler_Model::lists() const
{
    return t.lists();
}

void Tiddler_Model::request_set_list(const std::string& list_name, const std::vector<std::string>& values)
{
    auto old = t.list(list_name);
    t.set_list(list_name, values);
    auto current = t.list(list_name);
    if (old != current) {
        emit (old.empty() || current.empty()) ? lists_changed() : single_list_changed(list_name.c_str());
    }
}

void Tiddler_Model::request_remove_list(const std::string& list_name)
{
    auto old = t.lists();
    t.remove_list(list_name);
    if (old != t.lists()) {
        emit lists_changed();
    }
}
