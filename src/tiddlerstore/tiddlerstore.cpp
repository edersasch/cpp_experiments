#include "tiddlerstore.h"

#include <algorithm>
#include <fstream>

namespace
{

auto map_value = [](const auto& map, const auto& key)
{
    auto it = map.find(key);
    return it == map.end() ? typename std::decay_t<decltype(map)>::mapped_type() : it->second;
};

auto set_map_value = [](auto& map, const auto& key, const auto& value)
{
    if (!key.empty()) {
        if (value.empty()) {
            if (map.erase(key) != 0) {
                return Tiddlerstore::Set_Field_List_Change::Remove;
            }
        } else {
            auto& val = map[key];
            if (val != value) {
                auto ret = val.empty() ? Tiddlerstore::Set_Field_List_Change::Add : Tiddlerstore::Set_Field_List_Change::Value;
                val = value;
                return ret;
            }
        }
    }
    return Tiddlerstore::Set_Field_List_Change::None;
};

auto extract_map_keys = [](const auto& src, auto& dest)
{
    for (const auto& elem : src) {
        dest.insert(elem.first);
    }
};

bool string_find_case_insensitive(const std::string& haystack, const std::string& needle)
{
    auto it = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(), [](char c1, char c2) {
        return std::tolower(c1) == std::tolower(c2);
    });
    return it != haystack.end();
}

}

namespace Tiddlerstore
{

std::string Tiddler::title() const
{
    return tiddler_title;
}

bool Tiddler::set_title(const std::string &new_title)
{
    if (!new_title.empty() && tiddler_title != new_title) {
        tiddler_title = new_title;
        return true;
    }
    return false;
}

std::int32_t Tiddler::history_size() const
{
    return text_history_size;
}

bool Tiddler::set_history_size(std::int32_t new_history_size)
{
    if (new_history_size > 0) {
        static constexpr int max_history_size = 100;
        if (new_history_size > max_history_size) {
            new_history_size = max_history_size;
        }
        if (new_history_size != text_history_size) {
            text_history_size = new_history_size > max_history_size ? max_history_size : new_history_size;
            if (static_cast<std::size_t>(text_history_size) < tiddler_text_history.size()) {
                tiddler_text_history.resize(static_cast<std::size_t>(text_history_size));
            }
            return true;
        }
    }
    return false;
}

std::string Tiddler::text() const
{
    return tiddler_text_history.empty() ? std::string() : tiddler_text_history.front();
}

std::deque<std::string> Tiddler::text_history() const
{
    return tiddler_text_history;
}

bool Tiddler::set_text(const std::string &text)
{
    if (!text.empty()) {
        auto sz = static_cast<int32_t>(tiddler_text_history.size());
        if (sz == 0) {
            tiddler_text_history.push_front(text);
            return true;
        }
        if (text_history_size == 1) {
            if (tiddler_text_history.front() != text) {
                tiddler_text_history.front() = text;
                return true;
            }
        } else {
            auto it = std::find(tiddler_text_history.begin(), tiddler_text_history.end(), text);
            if (it != tiddler_text_history.begin()) {
                if (it == tiddler_text_history.end()) {
                    if (sz == text_history_size) {
                        tiddler_text_history.pop_back();
                    }
                    tiddler_text_history.push_front(text);
                } else {
                    std::rotate(tiddler_text_history.begin(), it, it + 1);
                }
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> Tiddler::tags() const
{
    return tiddler_tags;
}

bool Tiddler::has_tag(const std::string& tag) const
{
    return std::find(tiddler_tags.begin(), tiddler_tags.end(), tag) != tiddler_tags.end();
}

bool Tiddler::set_tag(const std::string& tag)
{
    if (!tag.empty() && !has_tag(tag)) {
        tiddler_tags.push_back(tag);
        return true;
    }
    return false;
}

bool Tiddler::remove_tag(const std::string& tag)
{
    auto it = std::find(tiddler_tags.begin(), tiddler_tags.end(), tag);
    if (it != tiddler_tags.end()) {
        tiddler_tags.erase(it);
        return true;
    }
    return false;
}

std::string Tiddler::field_value(const std::string& field_name) const
{
    return map_value(tiddler_fields, field_name);
}

std::unordered_map<std::string, std::string> Tiddler::fields() const
{
    return tiddler_fields;
}

Set_Field_List_Change Tiddler::set_field(const std::string& field_name, const std::string& value)
{
    return set_map_value(tiddler_fields, field_name, value);
}

bool  Tiddler::remove_field(const std::string& field_name)
{
    return tiddler_fields.erase(field_name);
}

std::vector<std::string> Tiddler::list(const std::string &list_name) const
{
    return map_value(tiddler_lists, list_name);
}

std::unordered_map<std::string, std::vector<std::string>> Tiddler::lists() const
{
    return tiddler_lists;
}

Set_Field_List_Change Tiddler::set_list(const std::string &list_name, std::vector<std::string> values)
{
    values.erase(std::remove_if(values.begin(), values.end(), [](const std::string& element) {
        return element.empty();
    }), values.end());
    return set_map_value(tiddler_lists, list_name, values);
}

bool  Tiddler::remove_list(const std::string &list_name)
{
    return tiddler_lists.erase(list_name);
}

bool Tiddler::is_empty()
{
    return  tiddler_title.empty() &&
            tiddler_text_history.empty() &&
            tiddler_tags.empty() &&
            tiddler_fields.empty() &&
            tiddler_lists.empty();
}

void to_json(nlohmann::json& j, const Tiddler& t)
{
    j = nlohmann::json {{ version_key, version_value }};
    auto push_optional = [&j](const std::string& key, const auto& value) {
        if (!value.empty()) {
            j.push_back({key, value});
        }
    };
    if (t.history_size() != Tiddler::default_text_history_size) {
        j.push_back({ Tiddler::history_size_key, t.history_size() });
    }
    push_optional(Tiddler::title_key, t.title());
    push_optional(Tiddler::text_history_key, t.text_history());
    push_optional(Tiddler::tags_key, t.tags());
    push_optional(Tiddler::fields_key, t.fields());
    push_optional(Tiddler::lists_key, t.lists());
}

void from_json(const nlohmann::json& j, Tiddler& t)
{
    auto title_v1 = [&j, &t] {
        try {
            t.set_title(j.at(Tiddler::title_key).get<std::string>());
        } catch (const nlohmann::json::exception&) {
        }
    };
    auto history_size_v1 = [&j, &t] {
        try {
            t.set_history_size(j.at(Tiddler::history_size_key).get<int>());
        } catch (const nlohmann::json::exception&) {
        }
    };
    auto text_history_v1 = [&j, &t] {
        try {
            auto jtext_history = j.at(Tiddler::text_history_key).get<std::vector<std::string>>();
            auto textit = jtext_history.rbegin();
            while (textit != jtext_history.rend()) {
                t.set_text(*textit);
                textit += 1;
            }
        } catch (const nlohmann::json::exception&) {
        }
    };
    auto tags_v1 = [&j, &t] {
        try {
            auto jtags = j.at(Tiddler::tags_key).get<std::vector<std::string>>();
            for (const auto& jtag : jtags) {
                t.set_tag(jtag);
            }
        } catch (const nlohmann::json::exception&) {
        }
    };
    auto fields_v1 = [&j, &t] {
        try {
            auto jfields = j.at(Tiddler::fields_key).get<std::unordered_map<std::string, std::string>>();
            for (const auto& jfield : jfields) {
                t.set_field(jfield.first, jfield.second);
            }
        } catch (const nlohmann::json::exception&) {
        }
    };
    auto lists_v1 = [&j, &t] {
        try {
            auto jlists = j.at(Tiddler::lists_key).get<std::unordered_map<std::string, std::vector<std::string>>>();
            for (const auto& jlist : jlists) {
                t.set_list(jlist.first, jlist.second);
            }
        } catch (const nlohmann::json::exception&) {
        }
    };
    try {
        switch(j.at(version_key).get<int>()) {
        case 1:
            title_v1();
            history_size_v1();
            text_history_v1();
            tags_v1();
            fields_v1();
            lists_v1();
            break;
        default:
            break;
        }
    } catch (const nlohmann::json::exception&) {
    }
}

void to_json(nlohmann::json& j, const Store& s)
{
    for (const auto& t : s) {
        if (!t->is_empty()) {
            j.push_back(*t);
        }
    }
}

void from_json(const nlohmann::json& j, Store& s)
{
    for (const auto& elem : j) {
        auto t = s.emplace_back(new Tiddler).get();
        from_json(elem, *t);
        if (t->is_empty()) {
            s.pop_back();
        }
    }
}

Store open_store_from_file(const std::string& path)
{
    nlohmann::json j;
    std::ifstream in(path);
    Store s;
    in >> j;
    from_json(j, s);
    return s;
}

bool save_store_to_file(const Store& store, const std::string& path)
{
    std::ofstream out(path);
    nlohmann::json j(store);
    out << j.dump();
    return out.good();
}

std::unordered_set<std::string> store_tags(const Store& store)
{
    std::unordered_set<std::string> ret;
    for (const auto& t : store) {
        auto taglist = t->tags();
        ret.insert(taglist.begin(), taglist.end());
    }
    return ret;
}

std::unordered_set<std::string> store_fields(const Store& store)
{
    std::unordered_set<std::string> ret;
    for (const auto& t : store) {
        extract_map_keys(t->fields(), ret);
    }
    return ret;
}

std::unordered_set<std::string> store_lists(const Store& store)
{
    std::unordered_set<std::string> ret;
    for (const auto& t : store) {
        extract_map_keys(t->lists(), ret);
    }
    return ret;
}

auto tiddler_pos_in_store(const Tiddler& tiddler, const Store& store) -> decltype(store.begin())
{
    return std::find_if(store.begin(), store.end(), [&tiddler](const auto& tref) {
        auto taddr = &tiddler;
        auto trefaddr = tref.get();
        return taddr == trefaddr;
    });
}

void erase_tiddler_from_store(const Tiddler& tiddler, Store& store)
{
    auto it = tiddler_pos_in_store(tiddler, store);
    if (it != store.end()) {
        store.erase(it);
    }
}

bool is_tiddler_in_store(const Tiddler& tiddler, const Store& store)
{
    return tiddler_pos_in_store(tiddler, store) != store.end();
}

Store_Filter::Store_Filter(const Store& all)
    : s(all)
{
    for (std::size_t i = 0; i < s.size(); i += 1) {
        idx.push_back(i);
    }
}

Store_Filter& Store_Filter::title(const std::string& title_value)
{
    if (!title_value.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, title_value](const std::size_t& i) {
            return s[i]->title() != title_value;
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::n_title(const std::string& title_value)
{
    if (!title_value.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, title_value](const std::size_t& i) {
            return s[i]->title() == title_value;
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::title_contains(const std::string& title_value, bool case_sensitive)
{
    if (!title_value.empty()) {
        if (case_sensitive) {
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, title_value](const std::size_t& i) {
                return s[i]->title().find(title_value) == std::string::npos;
            }), idx.end());
        } else {
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, title_value](const std::size_t& i) {
                return !string_find_case_insensitive(s[i]->title(), title_value);
            }), idx.end());
        }
    }
    return *this;
}

Store_Filter& Store_Filter::n_title_contains(const std::string& title_value, bool case_sensitive)
{
    if (!title_value.empty()) {
        if (case_sensitive) {
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, title_value](const std::size_t& i) {
                return s[i]->title().find(title_value) != std::string::npos;
            }), idx.end());
        } else {
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, title_value](const std::size_t& i) {
                return string_find_case_insensitive(s[i]->title(), title_value);
            }), idx.end());
        }
    }
    return *this;
}

Store_Filter& Store_Filter::text(const std::string& text_value)
{
    if (!text_value.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, text_value](const std::size_t& i) {
            return s[i]->text() != text_value;
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::n_text(const std::string& text_value)
{
    if (!text_value.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, text_value](const std::size_t& i) {
            return s[i]->text() == text_value;
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::text_contains(const std::string& text_value, bool case_sensitive)
{
    if (!text_value.empty()) {
        if (case_sensitive) {
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, text_value](const std::size_t& i) {
                return s[i]->text().find(text_value) == std::string::npos;
            }), idx.end());
        } else {
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, text_value](const std::size_t& i) {
                return !string_find_case_insensitive(s[i]->text(), text_value);
            }), idx.end());
        }
    }
    return *this;
}

Store_Filter& Store_Filter::n_text_contains(const std::string& text_value, bool case_sensitive)
{
    if (!text_value.empty()) {
        if (case_sensitive) {
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, text_value](const std::size_t& i) {
                return s[i]->text().find(text_value) != std::string::npos;
            }), idx.end());
        } else {
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, text_value](const std::size_t& i) {
                return string_find_case_insensitive(s[i]->text(), text_value);
            }), idx.end());
        }
    }
    return *this;
}

Store_Filter& Store_Filter::tag(const std::string& tag_value)
{
    if (!tag_value.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, tag_value](const std::size_t& i) {
            return !s[i]->has_tag(tag_value);
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::n_tag(const std::string& tag_value)
{
    if (!tag_value.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, tag_value](const std::size_t& i) {
            return s[i]->has_tag(tag_value);
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::tagged()
{
    idx.erase(std::remove_if(idx.begin(), idx.end(), [this](const std::size_t& i) {
        return s[i]->tags().empty();
    }), idx.end());
    return *this;
}

Store_Filter& Store_Filter::n_tagged()
{
    idx.erase(std::remove_if(idx.begin(), idx.end(), [this](const std::size_t& i) {
        return !s[i]->tags().empty();
    }), idx.end());
    return *this;
}

Store_Filter& Store_Filter::field(const std::string& field_name, const std::string& value)
{
    if (!field_name.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, field_name, value](const std::size_t& i) {
            auto fv = s[i]->field_value(field_name);
            return fv.empty() || (!value.empty() && fv != value);
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::n_field(const std::string& field_name, const std::string& value)
{
    if (!field_name.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, field_name, value](const std::size_t& i) {
            auto fv = s[i]->field_value(field_name);
            return !fv.empty() && (value.empty() || fv == value);
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::list(const std::string& list_name, const std::vector<std::string>& contains)
{
    if (!list_name.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, list_name, contains](const std::size_t& i) {
            auto l = s[i]->list(list_name);
            if (l.empty()) {
                return true;
            }
            for (const auto& c : contains) {
                if (std::find(l.begin(), l.end(), c) == l.end()) {
                    return true;
                }
            }
            return false;
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::n_list(const std::string& list_name, const std::vector<std::string>& contains)
{
    if (!list_name.empty()) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [this, list_name, contains](const std::size_t& i) {
            auto l = s[i]->list(list_name);
            if (l.empty()) {
                return false;
            }
            for (const auto& c : contains) {
                if (std::find(l.begin(), l.end(), c) == l.end()) {
                    return false;
                }
            }
            return true;
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::intersect(const Store_Filter& other)
{
    if (&s == &other.s) {
        idx.erase(std::remove_if(idx.begin(), idx.end(), [other](const std::size_t& i) {
            return std::find(other.idx.begin(), other.idx.end(), i) == other.idx.end();
        }), idx.end());
    }
    return *this;
}

Store_Filter& Store_Filter::join(const Store_Filter& other)
{
    if (&s == &other.s) {
        for (const auto& i : other.filtered_idx()) {
            if (std::find(idx.begin(), idx.end(), i) == idx.end()) {
                idx.push_back(i);
            }
        }
    }
    return *this;
}

Store_Indexes Store_Filter::filtered_idx() const
{
    return idx;
}

std::vector<Tiddler*> Store_Filter::filtered_tiddlers() const
{
    std::vector<Tiddler*> ret;
    for (const auto& i : idx) {
        ret.push_back(s[i].get());
    }
    return ret;
}

Tiddler* Store_Filter::first_filtered_tiddler() const
{
    return idx.empty() ? nullptr : s[idx.front()].get();
}

Store_Filter apply_filter(const Store& store, const Filter_Groups& filter_groups)
{
    bool first_filter = true;
    auto all_filter = Store_Filter(store);
    for (auto& single_group : filter_groups) {
        if (!single_group->empty()) {
            bool filtered = false;
            auto grfi = Store_Filter(store);
            for (auto& filter_data : *single_group) {
                if (!filter_data->key.empty()) {
                    filtered = true;
                    switch (filter_data->filter_type) {
                    case Filter_Type::Title:
                        filter_data->negate ? grfi.n_title_contains(filter_data->key, filter_data->case_sensitive) : grfi.title_contains(filter_data->key, filter_data->case_sensitive);
                        break;
                    case Filter_Type::Text:
                        filter_data->negate ? grfi.n_text_contains(filter_data->key, filter_data->case_sensitive) : grfi.text_contains(filter_data->key, filter_data->case_sensitive);
                        break;
                    case Filter_Type::Tag:
                        filter_data->negate ? grfi.n_tag(filter_data->key) : grfi.tag(filter_data->key);
                        break;
                    case Filter_Type::Field:
                        filter_data->negate ? grfi.n_field(filter_data->key, filter_data->field_value) : grfi.field(filter_data->key, filter_data->field_value);
                        break;
                    case Filter_Type::List:
                        filter_data->negate ? grfi.n_list(filter_data->key, filter_data->list_value) : grfi.list(filter_data->key, filter_data->list_value);
                        break;
                    }
                    if (grfi.filtered_idx().empty()) {
                        break;
                    }
                }
            }
            if (filtered) {
                first_filter ? all_filter.intersect(grfi) : all_filter.join(grfi);
                first_filter = false;
            }
        }
    }
    return all_filter;
}

}
