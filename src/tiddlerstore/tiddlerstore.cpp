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

Tiddlerstore::Filter_Element title_text(Tiddlerstore::Filter_Type type, const std::string& key, bool negate = false, bool case_sensitive = false)
{
    return  {type, negate, key, {}, {}, case_sensitive};
}

Tiddlerstore::Filter_Element tag_tagged(Tiddlerstore::Filter_Type type, const std::string& key = {}, bool negate = false)
{
    return  {type, negate, key};
}

Tiddlerstore::Filter_Element field_n_field(const std::string& key = {}, const std::string& value = {}, bool negate = false)
{
    return  {Tiddlerstore::Filter_Type::Field, negate, key, value};
}

Tiddlerstore::Filter_Element list_n_list(const std::string& key = {}, const std::vector<std::string>& value = {}, bool negate = false)
{
    return  {Tiddlerstore::Filter_Type::List, negate, key, {}, value};
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
    if (tiddler_title != new_title) {
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

std::vector<std::string> Tiddler::text_history() const
{
    return tiddler_text_history;
}

bool Tiddler::set_text(const std::string &text)
{
    auto it = std::find(tiddler_text_history.begin(), tiddler_text_history.end(), text);
    if (it != tiddler_text_history.begin() || tiddler_text_history.empty()) {
        if (it == tiddler_text_history.end()) {
            if (static_cast<std::int32_t>(tiddler_text_history.size()) == text_history_size) {
                tiddler_text_history.back() = text;
            } else {
                tiddler_text_history.push_back(text);
            }
            it = tiddler_text_history.end() - 1;
        }
        std::rotate(tiddler_text_history.begin(), it, it + 1);
        return true;
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
            (tiddler_text_history.empty() ||
             (tiddler_text_history.front().empty() && tiddler_text_history.size() == 1)) &&
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

Filter_Element title(const std::string& title_value)
{
    return title_text(Filter_Type::Title, title_value);
}

Filter_Element n_title(const std::string& title_value)
{
    return title_text(Filter_Type::Title, title_value, true);
}

Filter_Element title_contains(const std::string& title_value, bool case_sensitive)
{
    return title_text(Filter_Type::Title_Contains, title_value, false, case_sensitive);
}

Filter_Element n_title_contains(const std::string& title_value, bool case_sensitive)
{
    return title_text(Filter_Type::Title_Contains, title_value, true, case_sensitive);
}

Filter_Element text(const std::string& text_value)
{
    return title_text(Filter_Type::Text, text_value);
}

Filter_Element n_text(const std::string& text_value)
{
    return title_text(Filter_Type::Text, text_value, true);
}

Filter_Element text_contains(const std::string& text_value, bool case_sensitive)
{
    return title_text(Filter_Type::Text_Contains, text_value, false, case_sensitive);
}

Filter_Element n_text_contains(const std::string& text_value, bool case_sensitive)
{
    return title_text(Filter_Type::Text_Contains, text_value, true, case_sensitive);
}

Filter_Element tag(const std::string& tag_value)
{
    return tag_tagged(Filter_Type::Tag, tag_value);
}

Filter_Element n_tag(const std::string& tag_value)
{
    return tag_tagged(Filter_Type::Tag, tag_value, true);
}

Filter_Element tagged()
{
    return tag_tagged(Filter_Type::Tagged);
}

Filter_Element n_tagged()
{
    return tag_tagged(Filter_Type::Tagged, {}, true);
}

Filter_Element field(const std::string& field_name, const std::string& value)
{
    return field_n_field(field_name, value);
}

Filter_Element n_field(const std::string& field_name, const std::string& value)
{
    return field_n_field(field_name, value, true);
}

Filter_Element list(const std::string& list_name, const std::vector<std::string>& contains)
{
    return list_n_list(list_name, contains);
}

Filter_Element n_list(const std::string& list_name, const std::vector<std::string>& contains)
{
    return list_n_list(list_name, contains, true);
}

Filter::Filter(const Store& store)
    : s(store)
{
}

Filter::Filter(const Filter& other)
    : s(other.s)
{
    assign(other);
}

bool Filter::assign(const Filter& other)
{
    if (&s == &other.s) {
        idx = other.idx;
        filter_steps_done = other.filter_steps_done;
        fe = other.fe;
        return true;
    }
    return false;
}

void Filter::clear()
{
    assign(Filter(s));
}

Filter& Filter::append(const Filter_Element& element)
{
    fe.emplace_back(element);
    return *this;
}

void Filter::set_element(std::size_t pos, const Filter_Element& element)
{
    fe[pos] = element;
    invalidate();
}

void Filter::remove(std::size_t pos)
{
    fe.erase(fe.begin() + pos);
    invalidate();
}

std::vector<Filter_Element> Filter::elements()
{
    return fe;
}

void Filter::invalidate()
{
    filter_steps_done = std::numeric_limits<std::size_t>::max();
    idx.clear();
}

std::vector<std::size_t> Filter::filtered_idx()
{
    apply();
    return idx;
}

std::vector<Tiddler*> Filter::filtered_tiddlers()
{
    std::vector<Tiddler*> ret;
    for (const auto& i : filtered_idx()) {
        ret.push_back(s[i].get());
    }
    return ret;
}

Tiddler* Filter::first_filtered_tiddler()
{
    apply();
    return idx.empty() ? nullptr : s[idx.front()].get();
}

// private

void Filter::apply()
{
    if (filter_steps_done == std::numeric_limits<std::size_t>::max()) {
        std::generate_n(std::back_inserter(idx), s.size(), [n = 0]() mutable {
            return n++;
        });
        filter_steps_done = 0;
    }
    if (filter_steps_done == fe.size()) {
        return;
    }
    for (; filter_steps_done < fe.size(); filter_steps_done += 1) {
        auto& filter_data = fe[filter_steps_done];
        switch (filter_data.filter_type) {
        case Filter_Type::Title:
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                return filter_data.negate ? s[i]->title() == filter_data.key : s[i]->title() != filter_data.key;
            }), idx.end());
            break;
        case Filter_Type::Title_Contains:
            if (filter_data.case_sensitive) {
                idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                    return filter_data.negate ? s[i]->title().find(filter_data.key) != std::string::npos : s[i]->title().find(filter_data.key) == std::string::npos;
                }), idx.end());
            } else {
                idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                    return filter_data.negate ? string_find_case_insensitive(s[i]->title(), filter_data.key) : !string_find_case_insensitive(s[i]->title(), filter_data.key);
                }), idx.end());
            }
            break;
        case Filter_Type::Text:
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                return filter_data.negate ? s[i]->text() == filter_data.key : s[i]->text() != filter_data.key;
            }), idx.end());
            break;
        case Filter_Type::Text_Contains:
            if (filter_data.case_sensitive) {
                idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                    return filter_data.negate ? s[i]->text().find(filter_data.key) != std::string::npos : s[i]->text().find(filter_data.key) == std::string::npos;
                }), idx.end());
            } else {
                idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                    return filter_data.negate ? string_find_case_insensitive(s[i]->text(), filter_data.key) : !string_find_case_insensitive(s[i]->text(), filter_data.key);
                }), idx.end());
            }
            break;
        case Filter_Type::Tag:
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                return filter_data.negate ? s[i]->has_tag(filter_data.key) : !s[i]->has_tag(filter_data.key);
            }), idx.end());
            break;
        case Filter_Type::Tagged:
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                return filter_data.negate ? !s[i]->tags().empty() : s[i]->tags().empty();
            }), idx.end());
            break;
        case Filter_Type::Field:
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                auto fv = s[i]->field_value(filter_data.key);
                return filter_data.negate ? (!fv.empty() && (filter_data.field_value.empty() || fv == filter_data.field_value)) : (fv.empty() || (!filter_data.field_value.empty() && fv != filter_data.field_value));
            }), idx.end());
            break;
        case Filter_Type::List:
            idx.erase(std::remove_if(idx.begin(), idx.end(), [this, &filter_data](const std::size_t& i) {
                auto l = s[i]->list(filter_data.key);
                if (l.empty()) {
                    return !filter_data.negate;
                }
                for (const auto& c : filter_data.list_value) {
                    if (std::find(l.begin(), l.end(), c) == l.end()) {
                        return !filter_data.negate;
                    }
                }
                return filter_data.negate;
            }), idx.end());
            break;
        }
    }
}

Filter_Group::Filter_Group(Store& store)
    : s(store)
{
}

Filter& Filter_Group::append()
{
    return *f.emplace_back(new Filter(s)).get();
}

void Filter_Group::remove(const Filter& to_remove)
{
    auto it = std::find_if(f.begin(), f.end(), [&to_remove](const auto& filter) {
        return filter.get() == &to_remove;
    });
    if (it != f.end()) {
        f.erase(it);
    }
}

Filter& Filter_Group::at(std::size_t pos)
{
    return *f[pos].get();
}

std::vector<Filter*> Filter_Group::filters()
{
    std::vector<Filter*> ret;
    for (const auto& filter : f) {
        ret.push_back(filter.get());
    }
    return ret;
}

void Filter_Group::invalidate()
{
    for (auto& filter : f) {
        filter->invalidate();
    }
}

std::vector<Tiddler*> Filter_Group::filtered_tiddlers()
{
    std::vector<std::size_t> dest;
    for (auto& filter : f) {
        std::vector<std::size_t> idx;
        idx.swap(dest);
        auto fidx = filter->filtered_idx();
        std::set_union(idx.begin(), idx.end(), fidx.begin(), fidx.end(), std::back_inserter(dest));
    }
    std::vector<Tiddler*> ret;
    for (auto i : dest) {
        ret.push_back(s[i].get());
    }
    return ret;
}

Tiddler* Filter_Group::first_filtered_tiddler()
{
    auto t = filtered_tiddlers();
    return t.empty() ? nullptr : t.front();
}

}
