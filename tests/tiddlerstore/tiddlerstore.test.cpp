#include "tiddlerstore.test.h"

Tiddlerstore_Test::Tiddlerstore_Test()
{
}

TEST_F(Tiddlerstore_Test, title)
{
    Tiddlerstore::Tiddler t;
    auto check_title = [&t](const std::string& expected_title) {
        t.set_title(expected_title);
        EXPECT_EQ(expected_title, t.title());
        nlohmann::json j(t);
        Tiddlerstore::Tiddler clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_title, t.title());
        EXPECT_EQ(expected_title, clone.title());
    };
    std::string testtitle = "testtitle";
    std::string othertitle = "othertitle";
    check_title(testtitle);
    check_title(othertitle);
    t.set_title("");
    EXPECT_EQ(othertitle, t.title());
}

TEST_F(Tiddlerstore_Test, text_history)
{
    Tiddlerstore::Tiddler t;
    auto check_text = [&t](std::deque<std::string> expected_text_history, int expected_history_size) {
        EXPECT_EQ(expected_text_history, t.text_history());
        EXPECT_EQ(expected_history_size, t.history_size());
        nlohmann::json j(t);
        Tiddlerstore::Tiddler clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_text_history, t.text_history());
        EXPECT_EQ(expected_text_history, clone.text_history());
        EXPECT_EQ(expected_history_size, t.history_size());
        EXPECT_EQ(expected_history_size, clone.history_size());
    };
    std::string will_disappear_text = "bye";
    std::string a_text = "aaa";
    std::string b_text = "bbb";
    std::string c_text = "ccc";
    std::string d_text = "ddd";
    t.set_text(will_disappear_text);
    check_text({will_disappear_text}, 1);
    t.set_history_size(0); // shall stay at 1
    check_text({will_disappear_text}, 1);
    t.set_history_size(4);
    t.set_text(a_text);
    check_text({a_text, will_disappear_text}, 4);
    t.set_text(a_text);
    check_text({a_text, will_disappear_text}, 4);
    t.set_text("");
    check_text({a_text, will_disappear_text}, 4);
    t.set_text(b_text);
    check_text({b_text, a_text, will_disappear_text}, 4);
    t.set_text(c_text);
    check_text({c_text, b_text, a_text, will_disappear_text}, 4);
    t.set_text(a_text);
    check_text({a_text, c_text, b_text, will_disappear_text}, 4);
    t.set_history_size(3);
    check_text({a_text, c_text, b_text}, 3);
    t.set_text(b_text);
    check_text({b_text, a_text, c_text}, 3);
    t.set_text(d_text);
    check_text({d_text, b_text, a_text}, 3);
    t.set_text("");
    check_text({d_text, b_text, a_text}, 3);
}

TEST_F(Tiddlerstore_Test, tags)
{
    Tiddlerstore::Tiddler t;
    auto check_tags = [&t](std::vector<std::string> expected_tags) {
        EXPECT_EQ(expected_tags, t.tags());
        nlohmann::json j(t);
        Tiddlerstore::Tiddler clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_tags, t.tags());
        EXPECT_EQ(expected_tags, clone.tags());
        for (const auto& exptag : expected_tags) {
            EXPECT_EQ(true, t.has_tag(exptag));
            EXPECT_EQ(true, clone.has_tag(exptag));
        }
        EXPECT_EQ(false, t.has_tag("never ever such a tag"));
        EXPECT_EQ(false, clone.has_tag("never ever such a tag"));
    };
    std::string a_tag = "aaa";
    std::string b_tag = "bbb";
    std::string c_tag = "ccc";
    t.set_tag(a_tag);
    check_tags({a_tag});
    t.set_tag("");
    check_tags({a_tag});
    t.set_tag(b_tag);
    check_tags({a_tag, b_tag});
    t.set_tag(b_tag);
    check_tags({a_tag, b_tag});
    t.set_tag("");
    t.set_tag(c_tag);
    check_tags({a_tag, b_tag, c_tag});
    t.remove_tag("nonexisting");
    t.remove_tag(b_tag);
    t.remove_tag(b_tag); // no error
    check_tags({a_tag, c_tag});
    t.set_tag("");
    check_tags({a_tag, c_tag});
}

TEST_F(Tiddlerstore_Test, fields)
{
    Tiddlerstore::Tiddler t;
    std::unordered_map<std::string, std::string> expected_fields;
    auto check_fields = [&t, &expected_fields]() {
        EXPECT_EQ(expected_fields, t.fields());
        nlohmann::json j(t);
        Tiddlerstore::Tiddler clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_fields, t.fields());
        EXPECT_EQ(expected_fields, clone.fields());
        for (const auto& entry : expected_fields) {
            EXPECT_EQ(entry.second, t.field_value(entry.first));
        }
        EXPECT_EQ("", t.field_value("never ever such a field"));
        EXPECT_EQ("", clone.field_value("never ever such a field"));
    };
    expected_fields["a_key"] = "a_val";
    t.set_field("a_key", "a_val");
    check_fields();
    t.set_field("", "vanish");
    check_fields();
    t.set_field("vanish", "");
    check_fields();
    expected_fields["b_key"] = "b_val";
    t.set_field("b_key", "b_val");
    check_fields();
    expected_fields["c_key"] = "c_val";
    t.set_field("c_key", "c_val");
    check_fields();
    expected_fields.erase("b_key");
    t.remove_field("nonexisting");
    t.remove_field("b_key");
    t.remove_field("b_key");
    check_fields();
    expected_fields["c_key"] = "ccc";
    t.set_field("c_key", "ccc");
    check_fields();
    expected_fields.erase("a_key");
    t.set_field("a_key", ""); // setting to empty erases
    check_fields();
}

TEST_F(Tiddlerstore_Test, lists)
{
    Tiddlerstore::Tiddler t;
    std::unordered_map<std::string, std::vector<std::string>> expected_lists;
    auto check_lists = [&t, &expected_lists]() {
        EXPECT_EQ(expected_lists, t.lists());
        nlohmann::json j(t);
        Tiddlerstore::Tiddler clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_lists, t.lists());
        EXPECT_EQ(expected_lists, clone.lists());
        for (const auto& entry : expected_lists) {
            EXPECT_EQ(entry.second, t.list(entry.first));
        }
        EXPECT_EQ(std::vector<std::string>(), t.list("never ever such a list"));
        EXPECT_EQ(std::vector<std::string>(), clone.list("never ever such a list"));
    };
    expected_lists["a_key"] = {"a_val", "aa_val", "a", "a"}; // duplicate entries are allowed in lists
    t.set_list("a_key", {"a_val", "aa_val", "a", "a"});
    check_lists();
    t.set_list("", {"vanish", "vanish"});
    check_lists();
    t.set_list("vanish", {});
    check_lists();
    expected_lists["b_key"] = {"b_val", "bb_val", "b"};
    t.set_list("b_key", {"b_val", "bb_val", "b"});
    check_lists();
    expected_lists["c_key"] = {"c_val", "c"};
    t.set_list("c_key", {"c_val", "c"});
    check_lists();
    expected_lists.erase("b_key");
    t.remove_list("nonexisting");
    t.remove_list("b_key");
    t.remove_list("b_key");
    check_lists();
    expected_lists["c_key"] = {"ccc", "caa", "cbb"};
    t.set_list("c_key", {"ccc", "caa", "cbb"});
    check_lists();
    expected_lists.erase("a_key");
    t.set_list("a_key", {}); // setting to empty erases
    check_lists();
    t.set_list("a_key", {""}); // must stay empty
    check_lists();
    t.set_list("a_key", {"", "", ""}); // must stay empty
    check_lists();
    expected_lists["a_key"] = {"aaa"};
    t.set_list("a_key", {"", "", "aaa", ""}); // only "aaa" stays
    check_lists();
}
