#include "tiddlerstore.h"

#include <algorithm>
#include <iostream>

namespace
{

auto map_value = [](const auto& map, const auto& key)
{
    auto it = map.find(key);
    return it == map.end() ? typename std::decay_t<decltype(map)>::mapped_type() : it->second;
};

auto set_map_value = [](auto& map, const auto& key, const auto& value)
{
    if (value.empty()) {
        map.erase(key);
    } else {
        map[key] = value;
    }
};

}

namespace Tiddlerstore
{

std::string Tiddler::title() const
{
    return tiddler_title;
}

void Tiddler::set_title(const std::string &new_title)
{
    if (!new_title.empty()) {
        tiddler_title = new_title;
    }
}

int32_t Tiddler::history_size() const
{
    return text_history_size;
}

void Tiddler::set_history_size(int32_t new_history_size)
{
    if (new_history_size > 0) {
        static constexpr int max_history_size = 100;
        text_history_size = new_history_size > max_history_size ? max_history_size : new_history_size;
        if (static_cast<std::size_t>(text_history_size) < tiddler_text_history.size()) {
            tiddler_text_history.resize(static_cast<std::size_t>(text_history_size));
        }
    }
}

std::string Tiddler::text() const
{
    return tiddler_text_history.empty() ? std::string() : tiddler_text_history.front();
}

std::deque<std::string> Tiddler::text_history() const
{
    return tiddler_text_history;
}

void Tiddler::set_text(const std::string &text)
{
    if (!text.empty()) {
        auto sz = static_cast<int32_t>(tiddler_text_history.size());
        if (sz == 0) {
            tiddler_text_history.push_front(text);
        } else if (text_history_size == 1) {
            tiddler_text_history.front() = text;
        } else {
            auto it = std::find(tiddler_text_history.begin(), tiddler_text_history.end(), text);
            if (it == tiddler_text_history.end()) {
                if (sz == text_history_size) {
                    tiddler_text_history.pop_back();
                }
                tiddler_text_history.push_front(text);
            } else {
                std::rotate(tiddler_text_history.begin(), it, it + 1);
            }
        }
    }
}

std::vector<std::string> Tiddler::tags() const
{
    return tiddler_tags;
}

bool Tiddler::has_tag(const std::string& tag) const
{
    return std::find(tiddler_tags.begin(), tiddler_tags.end(), tag) != tiddler_tags.end();
}

void Tiddler::set_tag(const std::string& tag)
{
    if (!tag.empty() && !has_tag(tag)) {
        tiddler_tags.push_back(tag);
    }
}

void Tiddler::remove_tag(const std::string& tag)
{
    auto it = std::find(tiddler_tags.begin(), tiddler_tags.end(), tag);
    if (it != tiddler_tags.end()) {
        tiddler_tags.erase(it);
    }
}

std::string Tiddler::field_value(const std::string& field_name) const
{
    return map_value(tiddler_fields, field_name);
}

std::unordered_map<std::string, std::string> Tiddler::fields() const
{
    return tiddler_fields;
}

void Tiddler::set_field(const std::string& field_name, const std::string& value)
{
    set_map_value(tiddler_fields, field_name, value);
}

void Tiddler::remove_field(const std::string& field_name)
{
    tiddler_fields.erase(field_name);
}

std::vector<std::string> Tiddler::list(const std::string &list_name) const
{
    return map_value(tiddler_lists, list_name);
}

std::unordered_map<std::string, std::vector<std::string>> Tiddler::lists() const
{
    return tiddler_lists;
}

void Tiddler::set_list(const std::string &list_name, std::vector<std::string> values)
{
    values.erase(std::remove_if(values.begin(), values.end(), [](const std::string& element) {
        return element.empty();
    }), values.end());
    set_map_value(tiddler_lists, list_name, values);
}

void Tiddler::remove_list(const std::string &list_name)
{
    tiddler_lists.erase(list_name);
}

void to_json(nlohmann::json& j, const Tiddler& t)
{
    j = nlohmann::json {{ version_key, Tiddler::version_value }, { Tiddler::history_size_key, t.history_size() }};
    auto push_optional = [&j](const std::string& key, const auto& value) {
        if (!value.empty()) {
            j.push_back({key, value});
        }
    };
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

}
