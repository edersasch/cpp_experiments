#include "tiddlerstore.test.h"

#include <cstdlib>

Tiddlerstore_Test::Tiddlerstore_Test()
{
}

TEST_F(Tiddlerstore_Test, title)
{
    Tiddlerstore::Tiddler t;
    auto check_title = [&t](const std::string& expected_title) {
        t.set_title(expected_title);
        EXPECT_EQ(expected_title, t.title());
        Tiddlerstore::Tiddler copy(t);
        EXPECT_EQ(expected_title, t.title());
        EXPECT_EQ(expected_title, copy.title());
        Tiddlerstore::Tiddler assign;
        assign.set_title("something unexpected");
        assign = t;
        EXPECT_EQ(expected_title, t.title());
        EXPECT_EQ(expected_title, assign.title());
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
        Tiddlerstore::Tiddler copy(t);
        EXPECT_EQ(expected_text_history, t.text_history());
        EXPECT_EQ(expected_text_history, copy.text_history());
        EXPECT_EQ(expected_history_size, t.history_size());
        EXPECT_EQ(expected_history_size, copy.history_size());
        Tiddlerstore::Tiddler assign;
        assign.set_text("something unexpected");
        assign = t;
        EXPECT_EQ(expected_text_history, t.text_history());
        EXPECT_EQ(expected_text_history, assign.text_history());
        EXPECT_EQ(expected_history_size, t.history_size());
        EXPECT_EQ(expected_history_size, assign.history_size());
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
        Tiddlerstore::Tiddler copy(t);
        EXPECT_EQ(expected_tags, t.tags());
        EXPECT_EQ(expected_tags, copy.tags());
        Tiddlerstore::Tiddler assign;
        assign.set_tag("something unexpected");
        assign = t;
        EXPECT_EQ(expected_tags, t.tags());
        EXPECT_EQ(expected_tags, assign.tags());
        nlohmann::json j(t);
        Tiddlerstore::Tiddler clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_tags, t.tags());
        EXPECT_EQ(expected_tags, clone.tags());
        for (const auto& exptag : expected_tags) {
            EXPECT_EQ(true, t.has_tag(exptag));
            EXPECT_EQ(true, clone.has_tag(exptag));
            EXPECT_EQ(true, assign.has_tag(exptag));
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
        Tiddlerstore::Tiddler copy(t);
        EXPECT_EQ(expected_fields, t.fields());
        EXPECT_EQ(expected_fields, copy.fields());
        Tiddlerstore::Tiddler assign;
        assign.set_field("something unexpected", "1");
        assign = t;
        EXPECT_EQ(expected_fields, t.fields());
        EXPECT_EQ(expected_fields, assign.fields());
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
        Tiddlerstore::Tiddler copy(t);
        EXPECT_EQ(expected_lists, t.lists());
        EXPECT_EQ(expected_lists, copy.lists());
        Tiddlerstore::Tiddler assign;
        assign.set_list("something unexpected", {"1", "2", "3"});
        assign = t;
        EXPECT_EQ(expected_lists, t.lists());
        EXPECT_EQ(expected_lists, assign.lists());
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

TEST_F(Tiddlerstore_Test, store)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    EXPECT_EQ(true, t1->isEmpty());
    t1->set_title("t1");
    EXPECT_EQ(false, t1->isEmpty());
    t1->set_text("... t1 ...");
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_title("t2");
    t2->set_text("... t2 ...");
    nlohmann::json j(s);
    Tiddlerstore::Store clone = j.get<Tiddlerstore::Store>();
    EXPECT_EQ(s.size(), clone.size());
    EXPECT_EQ(s[0]->title(), clone[0]->title());
    EXPECT_EQ(s[0]->text(), clone[0]->text());
    EXPECT_EQ(s[1]->title(), clone[1]->title());
    EXPECT_EQ(s[1]->text(), clone[1]->text());
    char tname[] = "Tidderstore_Test_storeXXXXXX";
    ::mkstemp(tname);
    EXPECT_EQ(true, Tiddlerstore::save_store_to_file(s, tname));
    auto fclone = Tiddlerstore::open_store_from_file(tname);
    EXPECT_EQ(s.size(), fclone.size());
    EXPECT_EQ(s[0]->title(), fclone[0]->title());
    EXPECT_EQ(s[0]->text(), fclone[0]->text());
    EXPECT_EQ(s[1]->title(), fclone[1]->title());
    EXPECT_EQ(s[1]->text(), fclone[1]->text());
    s.emplace_back(new Tiddlerstore::Tiddler).get();
    EXPECT_EQ(3, s.size());
    EXPECT_EQ(true, Tiddlerstore::save_store_to_file(s, tname)); // empty tiddler is not saved ...
    auto fclone2 = Tiddlerstore::open_store_from_file(tname); // ... but even if it was it would not be added to the store
    EXPECT_EQ(s.size() - 1, fclone2.size());
    std::string store_json = R"([{"ti":"aa","v":1},{"ti":"bb","v":1},{"th":[""],"ti":"","v":1}])"; // last tiddler object is empty
    Tiddlerstore::Store s2 = nlohmann::json::parse(store_json);
    from_json(store_json, s2);
    EXPECT_EQ(2, s2.size());
}

TEST_F(Tiddlerstore_Test, filter_title)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t1->set_title("a");
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_title("b");
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t3->set_title("c");
    auto r1 = Tiddlerstore::Store_Filter(s).title("a").filtered_idx();
    EXPECT_EQ(1, r1.size());
    auto r2 = Tiddlerstore::Store_Filter(s).title("b").filtered_idx();
    EXPECT_EQ(1, r2.size());
    auto r3 = Tiddlerstore::Store_Filter(s).n_title("a").filtered_idx();
    EXPECT_EQ(2, r3.size());
}

TEST_F(Tiddlerstore_Test, filter_tags)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t1->set_tag("a");
    t1->set_tag("b");
    t1->set_tag("c");
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_tag("b");
    t2->set_tag("c");
    t2->set_tag("d");
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t3->set_tag("c");
    t3->set_tag("d");
    t3->set_tag("e");
    auto r1 = Tiddlerstore::Store_Filter(s).tag("a").filtered_idx();
    EXPECT_EQ(1, r1.size());
    auto r2 = Tiddlerstore::Store_Filter(s).tag("b").filtered_idx();
    EXPECT_EQ(2, r2.size());
    auto r3 = Tiddlerstore::Store_Filter(s).tag("c").filtered_idx();
    EXPECT_EQ(3, r3.size());
    auto r4 = Tiddlerstore::Store_Filter(s).tag("d").filtered_idx();
    EXPECT_EQ(2, r4.size());
    auto r5 = Tiddlerstore::Store_Filter(s).n_tag("d").filtered_idx();
    EXPECT_EQ(1, r5.size());
    auto r6 = Tiddlerstore::Store_Filter(s).n_tag("e").filtered_idx();
    EXPECT_EQ(2, r6.size());
    auto r7 = Tiddlerstore::Store_Filter(s).n_tagged().filtered_idx();
    EXPECT_EQ(0, r7.size());
    s.emplace_back(new Tiddlerstore::Tiddler).get();
    auto r8 = Tiddlerstore::Store_Filter(s).n_tagged().filtered_idx();
    EXPECT_EQ(1, r8.size());
    auto r9 = Tiddlerstore::Store_Filter(s).tagged().filtered_idx();
    EXPECT_EQ(3, r9.size());
}

TEST_F(Tiddlerstore_Test, filter_fields)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t1->set_field("a", "a_field");
    t1->set_field("b", "b_field");
    t1->set_field("c", "c_field");
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_field("b", "b_field");
    t2->set_field("c", "c_field");
    t2->set_field("d", "d_field2");
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t3->set_field("c", "c_field");
    t3->set_field("d", "d_field3");
    t3->set_field("e", "e_field");
    auto r1 = Tiddlerstore::Store_Filter(s).field("a").filtered_idx();
    EXPECT_EQ(1, r1.size());
    auto r2 = Tiddlerstore::Store_Filter(s).field("b").filtered_idx();
    EXPECT_EQ(2, r2.size());
    auto r3 = Tiddlerstore::Store_Filter(s).field("c").filtered_idx();
    EXPECT_EQ(3, r3.size());
    auto r4 = Tiddlerstore::Store_Filter(s).field("d").filtered_idx();
    EXPECT_EQ(2, r4.size());
    auto r5 = Tiddlerstore::Store_Filter(s).n_field("d").filtered_idx();
    EXPECT_EQ(1, r5.size());
    auto r6 = Tiddlerstore::Store_Filter(s).n_field("e").filtered_idx();
    EXPECT_EQ(2, r6.size());
    auto r7 = Tiddlerstore::Store_Filter(s).field("d", "d_field2").filtered_idx();
    EXPECT_EQ(1, r7.size());
    auto r8 = Tiddlerstore::Store_Filter(s).field("d", "not there").filtered_idx();
    EXPECT_EQ(0, r8.size());
}

TEST_F(Tiddlerstore_Test, filter_lists)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t1->set_list("a", {"a1_1", "a1_2", "a1_3"});
    t1->set_list("b", {"b1_1", "b1_2", "b1_3"});
    t1->set_list("c", {"c1_1", "c1_2", "c1_3"});
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_list("b", {"b2_1", "b2_2", "b2_3"});
    t2->set_list("c", {"c2_1", "c2_2", "c2_3"});
    t2->set_list("d", {"d2_1", "d2_2", "d2_3"});
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t3->set_list("c", {"c3_1", "c3_2", "c3_3"});
    t3->set_list("d", {"d3_1", "d3_2", "d3_3"});
    t3->set_list("e", {"e3_1", "e3_2", "e3_3"});
    auto r1 = Tiddlerstore::Store_Filter(s).list("a").filtered_idx();
    EXPECT_EQ(1, r1.size());
    auto r2 = Tiddlerstore::Store_Filter(s).list("b").filtered_idx();
    EXPECT_EQ(2, r2.size());
    auto r3 = Tiddlerstore::Store_Filter(s).list("c").filtered_idx();
    EXPECT_EQ(3, r3.size());
    auto r4 = Tiddlerstore::Store_Filter(s).list("d").filtered_idx();
    EXPECT_EQ(2, r4.size());
    auto r5 = Tiddlerstore::Store_Filter(s).n_list("d").filtered_idx();
    EXPECT_EQ(1, r5.size());
    auto r6 = Tiddlerstore::Store_Filter(s).n_list("e").filtered_idx();
    EXPECT_EQ(2, r6.size());
    auto r7 = Tiddlerstore::Store_Filter(s).list("d", {"d2_1", "d2_3"}).filtered_idx();
    EXPECT_EQ(1, r7.size());
    auto r8 = Tiddlerstore::Store_Filter(s).list("d", {"d3_2", "not there"}).filtered_idx();
    EXPECT_EQ(0, r8.size());
}
