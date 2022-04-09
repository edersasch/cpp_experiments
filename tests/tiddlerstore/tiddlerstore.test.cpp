#include "tiddlerstore.test.h"
#include "tiddlerstore/tiddlerstore.h"

#include <cstdlib>

TEST_F(Tiddlerstore_Test, title)
{
    Tiddlerstore::Tiddler t;
    auto check_title = [&t](const std::string& expected_title) {
        EXPECT_EQ(true, t.set_title(expected_title));
        EXPECT_EQ(expected_title, t.title());
        Tiddlerstore::Tiddler copy(t);
        EXPECT_EQ(expected_title, t.title());
        EXPECT_EQ(expected_title, copy.title());
        Tiddlerstore::Tiddler assign;
        EXPECT_EQ(true, assign.set_title("something unexpected"));
        assign = t;
        EXPECT_EQ(expected_title, t.title());
        EXPECT_EQ(expected_title, assign.title());
        nlohmann::json j(t);
        auto clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_title, t.title());
        EXPECT_EQ(expected_title, clone.title());
    };
    std::string testtitle = "testtitle";
    std::string othertitle = "othertitle";
    check_title(testtitle);
    check_title(othertitle);
    EXPECT_EQ(false, t.set_title(""));
    EXPECT_EQ(othertitle, t.title());
}

TEST_F(Tiddlerstore_Test, text_history)
{
    Tiddlerstore::Tiddler t;
    auto check_text = [&t](const std::deque<std::string>& expected_text_history, int expected_history_size) {
        EXPECT_EQ(expected_text_history, t.text_history());
        EXPECT_EQ(expected_history_size, t.history_size());
        Tiddlerstore::Tiddler copy(t);
        EXPECT_EQ(expected_text_history, t.text_history());
        EXPECT_EQ(expected_text_history, copy.text_history());
        EXPECT_EQ(expected_history_size, t.history_size());
        EXPECT_EQ(expected_history_size, copy.history_size());
        Tiddlerstore::Tiddler assign;
        EXPECT_EQ(true, assign.set_text("something unexpected"));
        assign = t;
        EXPECT_EQ(expected_text_history, t.text_history());
        EXPECT_EQ(expected_text_history, assign.text_history());
        EXPECT_EQ(expected_history_size, t.history_size());
        EXPECT_EQ(expected_history_size, assign.history_size());
        nlohmann::json j(t);
        auto clone = j.get<Tiddlerstore::Tiddler>();
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
    EXPECT_EQ(true, t.set_text(will_disappear_text));
    check_text({will_disappear_text}, 1);
    EXPECT_EQ(false, t.set_history_size(0)); // shall stay at 1
    check_text({will_disappear_text}, 1);
    EXPECT_EQ(true, t.set_history_size(4));
    EXPECT_EQ(true, t.set_text(a_text));
    check_text({a_text, will_disappear_text}, 4);
    EXPECT_EQ(false, t.set_text(a_text));
    check_text({a_text, will_disappear_text}, 4);
    EXPECT_EQ(false, t.set_text(""));
    check_text({a_text, will_disappear_text}, 4);
    EXPECT_EQ(true, t.set_text(b_text));
    check_text({b_text, a_text, will_disappear_text}, 4);
    EXPECT_EQ(true, t.set_text(c_text));
    check_text({c_text, b_text, a_text, will_disappear_text}, 4);
    EXPECT_EQ(true, t.set_text(a_text));
    check_text({a_text, c_text, b_text, will_disappear_text}, 4);
    EXPECT_EQ(true, t.set_history_size(3));
    check_text({a_text, c_text, b_text}, 3);
    EXPECT_EQ(true, t.set_text(b_text));
    check_text({b_text, a_text, c_text}, 3);
    EXPECT_EQ(true, t.set_text(d_text));
    check_text({d_text, b_text, a_text}, 3);
    EXPECT_EQ(false, t.set_text(""));
    check_text({d_text, b_text, a_text}, 3);
}

TEST_F(Tiddlerstore_Test, tags)
{
    Tiddlerstore::Tiddler t;
    auto check_tags = [&t](const std::vector<std::string>& expected_tags) {
        EXPECT_EQ(expected_tags, t.tags());
        Tiddlerstore::Tiddler copy(t);
        EXPECT_EQ(expected_tags, t.tags());
        EXPECT_EQ(expected_tags, copy.tags());
        Tiddlerstore::Tiddler assign;
        EXPECT_EQ(true, assign.set_tag("something unexpected"));
        assign = t;
        EXPECT_EQ(expected_tags, t.tags());
        EXPECT_EQ(expected_tags, assign.tags());
        nlohmann::json j(t);
        auto clone = j.get<Tiddlerstore::Tiddler>();
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
    EXPECT_EQ(true, t.set_tag(a_tag));
    check_tags({a_tag});
    EXPECT_EQ(false, t.set_tag(""));
    check_tags({a_tag});
    EXPECT_EQ(true, t.set_tag(b_tag));
    check_tags({a_tag, b_tag});
    EXPECT_EQ(false, t.set_tag(b_tag));
    check_tags({a_tag, b_tag});
    EXPECT_EQ(false, t.set_tag(""));
    EXPECT_EQ(true, t.set_tag(c_tag));
    check_tags({a_tag, b_tag, c_tag});
    EXPECT_EQ(false, t.remove_tag("nonexisting"));
    EXPECT_EQ(true, t.remove_tag(b_tag));
    EXPECT_EQ(false, t.remove_tag(b_tag)); // no error
    check_tags({a_tag, c_tag});
    EXPECT_EQ(false, t.set_tag(""));
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
        EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, assign.set_field("something unexpected", "1"));
        assign = t;
        EXPECT_EQ(expected_fields, t.fields());
        EXPECT_EQ(expected_fields, assign.fields());
        nlohmann::json j(t);
        auto clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_fields, t.fields());
        EXPECT_EQ(expected_fields, clone.fields());
        for (const auto& entry : expected_fields) {
            EXPECT_EQ(entry.second, t.field_value(entry.first));
        }
        EXPECT_EQ("", t.field_value("never ever such a field"));
        EXPECT_EQ("", clone.field_value("never ever such a field"));
    };
    expected_fields["a_key"] = "a_val";
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_field("a_key", "a_val"));
    check_fields();
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, t.set_field("", "vanish"));
    check_fields();
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, t.set_field("vanish", ""));
    check_fields();
    expected_fields["b_key"] = "b_val";
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_field("b_key", "b_val"));
    check_fields();
    expected_fields["c_key"] = "c_val";
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_field("c_key", "c_val"));
    check_fields();
    expected_fields.erase("b_key");
    EXPECT_EQ(false, t.remove_field("nonexisting"));
    EXPECT_EQ(true, t.remove_field("b_key"));
    EXPECT_EQ(false, t.remove_field("b_key"));
    check_fields();
    expected_fields["c_key"] = "ccc";
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Value, t.set_field("c_key", "ccc"));
    check_fields();
    expected_fields.erase("a_key");
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Remove, t.set_field("a_key", "")); // setting to empty erases
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
        EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, assign.set_list("something unexpected", {"1", "2", "3"}));
        assign = t;
        EXPECT_EQ(expected_lists, t.lists());
        EXPECT_EQ(expected_lists, assign.lists());
        nlohmann::json j(t);
        auto clone = j.get<Tiddlerstore::Tiddler>();
        EXPECT_EQ(expected_lists, t.lists());
        EXPECT_EQ(expected_lists, clone.lists());
        for (const auto& entry : expected_lists) {
            EXPECT_EQ(entry.second, t.list(entry.first));
        }
        EXPECT_EQ(std::vector<std::string>(), t.list("never ever such a list"));
        EXPECT_EQ(std::vector<std::string>(), clone.list("never ever such a list"));
    };
    expected_lists["a_key"] = {"a_val", "aa_val", "a", "a"}; // duplicate entries are allowed in lists
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_list("a_key", {"a_val", "aa_val", "a", "a"}));
    check_lists();
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, t.set_list("", {"vanish", "vanish"}));
    check_lists();
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, t.set_list("vanish", {}));
    check_lists();
    expected_lists["b_key"] = {"b_val", "bb_val", "b"};
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_list("b_key", {"b_val", "bb_val", "b"}));
    check_lists();
    expected_lists["c_key"] = {"c_val", "c"};
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_list("c_key", {"c_val", "c"}));
    check_lists();
    expected_lists.erase("b_key");
    EXPECT_EQ(false, t.remove_list("nonexisting"));
    EXPECT_EQ(true, t.remove_list("b_key"));
    EXPECT_EQ(false, t.remove_list("b_key"));
    check_lists();
    expected_lists["c_key"] = {"ccc", "caa", "cbb"};
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Value, t.set_list("c_key", {"ccc", "caa", "cbb"}));
    check_lists();
    expected_lists.erase("a_key");
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Remove, t.set_list("a_key", {})); // setting to empty erases
    check_lists();
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, t.set_list("a_key", {""})); // must stay empty
    check_lists();
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, t.set_list("a_key", {"", "", ""})); // must stay empty
    check_lists();
    expected_lists["a_key"] = {"aaa"};
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_list("a_key", {"", "", "aaa", ""})); // only "aaa" stays
    check_lists();
}

TEST_F(Tiddlerstore_Test, store)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    EXPECT_EQ(true, t1->is_empty());
    t1->set_title("t1");
    EXPECT_EQ(false, t1->is_empty());
    t1->set_text("... t1 ...");
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_title("t2");
    t2->set_text("... t2 ...");
    nlohmann::json j(s);
    auto clone = j.get<Tiddlerstore::Store>();
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

TEST_F(Tiddlerstore_Test, store_tags)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t1->set_tag("tag1");
    t1->set_tag("tag2");
    t1->set_tag("tag3");
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_tag("tag2");
    t2->set_tag("tag3");
    t2->set_tag("tag4");
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t3->set_tag("tag3");
    t3->set_tag("tag4");
    t3->set_tag("tag5");
    EXPECT_EQ(Tiddlerstore::store_tags(s), std::unordered_set<std::string>({"tag1", "tag2", "tag3", "tag4", "tag5"}));
}

TEST_F(Tiddlerstore_Test, store_fields)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t1->set_field("f1", "1_1");
    t1->set_field("f2", "1_2");
    t1->set_field("f3", "1_3");
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_field("f2", "2_2");
    t2->set_field("f3", "2_3");
    t2->set_field("f4", "2_4");
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t3->set_field("f3", "3_3");
    t3->set_field("f4", "3_4");
    t3->set_field("f5", "3_5");
    EXPECT_EQ(Tiddlerstore::store_fields(s), std::unordered_set<std::string>({"f1", "f2", "f3", "f4", "f5"}));
}

TEST_F(Tiddlerstore_Test, store_lists)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t1->set_list("l1", {"1_1"});
    t1->set_list("l2", {"1_2"});
    t1->set_list("l3", {"1_3"});
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_list("l2", {"2_2"});
    t2->set_list("l3", {"2_3"});
    t2->set_list("l4", {"2_4"});
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t3->set_list("l3", {"3_3"});
    t3->set_list("l4", {"3_4"});
    t3->set_list("l5", {"3_5"});
    EXPECT_EQ(Tiddlerstore::store_lists(s), std::unordered_set<std::string>({"l1", "l2", "l3", "l4", "l5"}));
}

TEST_F(Tiddlerstore_Test, store_pos_erase)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    Tiddlerstore::Tiddler f1;
    EXPECT_EQ(s.begin(), Tiddlerstore::tiddler_pos_in_store(*t1, s));
    EXPECT_EQ(true, Tiddlerstore::is_tiddler_in_store(*t1, s));
    EXPECT_EQ(true, Tiddlerstore::is_tiddler_in_store(*t2, s));
    EXPECT_EQ(true, Tiddlerstore::is_tiddler_in_store(*t3, s));
    EXPECT_EQ(s.end(), Tiddlerstore::tiddler_pos_in_store(f1, s));
    EXPECT_EQ(false, Tiddlerstore::is_tiddler_in_store(f1, s));
    EXPECT_EQ(3, s.size());
    Tiddlerstore::erase_tiddler_from_store(*t2, s);
    EXPECT_EQ(2, s.size());
    Tiddlerstore::erase_tiddler_from_store(f1, s);
    EXPECT_EQ(2, s.size());
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
    auto t4 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t4->set_title("ab");
    auto t5 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t5->set_title("bc");
    auto t6 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t6->set_title("abc");
    auto t7 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t7->set_title("ABC");
    auto r1 = Tiddlerstore::Store_Filter(s).title("a").filtered_idx();
    EXPECT_EQ(r1, Tiddlerstore::Store_Indexes{0});
    auto r2 = Tiddlerstore::Store_Filter(s).title("b").filtered_idx();
    EXPECT_EQ(r2, Tiddlerstore::Store_Indexes{1});
    auto r3 = Tiddlerstore::Store_Filter(s).n_title("a").filtered_idx();
    EXPECT_EQ(r3, Tiddlerstore::Store_Indexes({1, 2, 3, 4, 5, 6}));
    auto r4 = Tiddlerstore::Store_Filter(s).title_contains("a").filtered_idx();
    EXPECT_EQ(r4, Tiddlerstore::Store_Indexes({0, 3, 5, 6}));
    auto r5 = Tiddlerstore::Store_Filter(s).n_title_contains("a").filtered_idx();
    EXPECT_EQ(r5, Tiddlerstore::Store_Indexes({1, 2, 4}));
    auto r6 = Tiddlerstore::Store_Filter(s).title_contains("A", true).filtered_idx();
    EXPECT_EQ(r6, Tiddlerstore::Store_Indexes({6}));
    auto r7 = Tiddlerstore::Store_Filter(s).n_title_contains("A", true).filtered_idx();
    EXPECT_EQ(r7, Tiddlerstore::Store_Indexes({0, 1, 2, 3, 4, 5}));
    Tiddlerstore::Store_Filter f1(s);
    f1.title_contains("c");
    EXPECT_EQ(f1.filtered_idx(), Tiddlerstore::Store_Indexes({2, 4, 5, 6}));
    Tiddlerstore::Store_Filter f2(s);
    f2.title_contains("b", true);
    EXPECT_EQ(f2.filtered_idx(), Tiddlerstore::Store_Indexes({1, 3, 4, 5}));
    f1.join(f2);
    EXPECT_EQ(f1.filtered_idx(), Tiddlerstore::Store_Indexes({2, 4, 5, 6, 1, 3}));
    Tiddlerstore::Store_Filter f3(s);
    f3.title_contains("C", true);
    f1.intersect(f3);
    EXPECT_EQ(f1.filtered_idx(), Tiddlerstore::Store_Indexes({6}));
}

TEST_F(Tiddlerstore_Test, filter_text)
{
    Tiddlerstore::Store s;
    auto t1 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t1->set_text("a");
    auto t2 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t2->set_text("b");
    auto t3 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t3->set_text("c");
    auto t4 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t4->set_text("ab");
    auto t5 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t5->set_text("bc");
    auto t6 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t6->set_text("abc");
    auto t7 = s.emplace_back(new Tiddlerstore::Tiddler).get();
    t7->set_text("ABC");
    auto r1 = Tiddlerstore::Store_Filter(s).text("a").filtered_idx();
    EXPECT_EQ(r1, Tiddlerstore::Store_Indexes{0});
    auto r2 = Tiddlerstore::Store_Filter(s).text("b").filtered_idx();
    EXPECT_EQ(r2, Tiddlerstore::Store_Indexes{1});
    auto r3 = Tiddlerstore::Store_Filter(s).n_text("a").filtered_idx();
    EXPECT_EQ(r3, Tiddlerstore::Store_Indexes({1, 2, 3, 4, 5, 6}));
    auto r4 = Tiddlerstore::Store_Filter(s).text_contains("a").filtered_idx();
    EXPECT_EQ(r4, Tiddlerstore::Store_Indexes({0, 3, 5, 6}));
    auto r5 = Tiddlerstore::Store_Filter(s).n_text_contains("a").filtered_idx();
    EXPECT_EQ(r5, Tiddlerstore::Store_Indexes({1, 2, 4}));
    auto r6 = Tiddlerstore::Store_Filter(s).text_contains("A", true).filtered_idx();
    EXPECT_EQ(r6, Tiddlerstore::Store_Indexes({6}));
    auto r7 = Tiddlerstore::Store_Filter(s).n_text_contains("A", true).filtered_idx();
    EXPECT_EQ(r7, Tiddlerstore::Store_Indexes({0, 1, 2, 3, 4, 5}));
    Tiddlerstore::Store_Filter f1(s);
    f1.text_contains("c");
    EXPECT_EQ(f1.filtered_idx(), Tiddlerstore::Store_Indexes({2, 4, 5, 6}));
    Tiddlerstore::Store_Filter f2(s);
    f2.text_contains("b", true);
    EXPECT_EQ(f2.filtered_idx(), Tiddlerstore::Store_Indexes({1, 3, 4, 5}));
    f1.join(f2);
    EXPECT_EQ(f1.filtered_idx(), Tiddlerstore::Store_Indexes({2, 4, 5, 6, 1, 3}));
    Tiddlerstore::Store_Filter f3(s);
    f3.text_contains("C", true);
    f1.intersect(f3);
    EXPECT_EQ(f1.filtered_idx(), Tiddlerstore::Store_Indexes({6}));
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

TEST_F(Tiddlerstore_Test, apply_filter)
{
    std::string store_json = R"(
                             [
                             {"ti":"aaTidABC","v":1},
                             {"ti":"bbTidBCD","ta":["bbT1"],"v":1},
                             {"ti":"ccTidCDE","v":1}
                             ])";
    Tiddlerstore::Store s1 = nlohmann::json::parse(store_json);
    EXPECT_EQ(3, s1.size());
    Tiddlerstore::Filter_Groups fg1;
    auto check_s1_fg1 = [&s1, &fg1](Tiddlerstore::Store_Indexes si) {
        EXPECT_EQ(si, Tiddlerstore::apply_filter(s1, fg1).filtered_idx());
        std::vector<Tiddlerstore::Tiddler*> st;
        for (const auto& i : si) {
            st.push_back(s1[i].get());
        }
        EXPECT_EQ(st, Tiddlerstore::apply_filter(s1, fg1).filtered_tiddlers());
        EXPECT_EQ(st[0], Tiddlerstore::apply_filter(s1, fg1).first_filtered_tiddler());
    };
    check_s1_fg1({0, 1, 2});
    auto sg1 = fg1.emplace_back(new Tiddlerstore::Single_Group).get();
    check_s1_fg1({0, 1, 2});
    auto fd1_1 = sg1->emplace_back(new Tiddlerstore::Filter_Data).get();
    check_s1_fg1({0, 1, 2});
    fd1_1->key = "c";
    check_s1_fg1({0, 1, 2});
    fd1_1->case_sensitive = true;
    check_s1_fg1({2});
    fd1_1->negate = true;
    check_s1_fg1({0, 1});
    auto sg2 = fg1.emplace_back(new Tiddlerstore::Single_Group).get();
    check_s1_fg1({0, 1});
    auto fd2_1 = sg2->emplace_back(new Tiddlerstore::Filter_Data).get();
    check_s1_fg1({0, 1});
    fd2_1->key = "CC";
    check_s1_fg1({0, 1, 2});
    auto fd1_2 = sg1->emplace_back(new Tiddlerstore::Filter_Data).get();
    check_s1_fg1({0, 1, 2});
    fd1_2->filter_type = Tiddlerstore::Filter_Type::Tag;
    check_s1_fg1({0, 1, 2});
    fd1_2->key = "bbT1";
    check_s1_fg1({1, 2});
    fd1_2->negate = true;
    check_s1_fg1({0, 2});
}
