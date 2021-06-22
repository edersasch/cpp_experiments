#include "tiddler_model_qt.test.h"

#include "qt_gtest_helper/qt_gtest_helper.h"

Tiddler_Model_Test::Tiddler_Model_Test()
{
    connect(&tm, &Tiddler_Model::title_changed, &tmt_slots, &Tiddler_Model_Test_Slots::title_changed);
    connect(&tm, &Tiddler_Model::text_changed, &tmt_slots, &Tiddler_Model_Test_Slots::text_changed);
    connect(&tm, &Tiddler_Model::history_size_changed, &tmt_slots, &Tiddler_Model_Test_Slots::history_size_changed);
    connect(&tm, &Tiddler_Model::tags_changed, &tmt_slots, &Tiddler_Model_Test_Slots::tags_changed);
    connect(&tm, &Tiddler_Model::field_changed, &tmt_slots, &Tiddler_Model_Test_Slots::field_changed);
    connect(&tm, &Tiddler_Model::field_added, &tmt_slots, &Tiddler_Model_Test_Slots::field_added);
    connect(&tm, &Tiddler_Model::field_removed, &tmt_slots, &Tiddler_Model_Test_Slots::field_removed);
    connect(&tm, &Tiddler_Model::fields_reset, &tmt_slots, &Tiddler_Model_Test_Slots::fields_reset);
    connect(&tm, &Tiddler_Model::list_changed, &tmt_slots, &Tiddler_Model_Test_Slots::list_changed);
    connect(&tm, &Tiddler_Model::list_added, &tmt_slots, &Tiddler_Model_Test_Slots::list_added);
    connect(&tm, &Tiddler_Model::list_removed, &tmt_slots, &Tiddler_Model_Test_Slots::list_removed);
    connect(&tm, &Tiddler_Model::lists_reset, &tmt_slots, &Tiddler_Model_Test_Slots::lists_reset);
}

TEST_F(Tiddler_Model_Test, title)
{
    EXPECT_CALL(tmt_slots, title_changed());
    tm.request_set_title("one");
    processEvents();
    EXPECT_EQ("one", tm.title());
    tm.request_set_title("");
    processEvents();
    EXPECT_EQ("one", tm.title());
    tm.request_set_title("one");
    processEvents();
    EXPECT_EQ("one", tm.title());
    EXPECT_CALL(tmt_slots, title_changed());
    tm.request_set_title("two");
    processEvents();
    EXPECT_EQ("two", tm.title());
    EXPECT_CALL(tmt_slots, title_changed());
    Tiddlerstore::Tiddler t;
    t.set_title("set data");
    tm.request_set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.title(), tm.title());
    tm.request_set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, text)
{
    EXPECT_CALL(tmt_slots, text_changed());
    tm.request_set_text("one");
    processEvents();
    EXPECT_EQ("one", tm.text());
    tm.request_set_text("");
    processEvents();
    EXPECT_EQ("one", tm.text());
    tm.request_set_text("one");
    processEvents();
    EXPECT_EQ("one", tm.text());
    EXPECT_CALL(tmt_slots, text_changed());
    tm.request_set_text("two");
    processEvents();
    EXPECT_EQ("two", tm.text());
    EXPECT_CALL(tmt_slots, text_changed());
    Tiddlerstore::Tiddler t;
    t.set_text("set data");
    tm.request_set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.text(), tm.text());
    tm.request_set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, history_size)
{
    EXPECT_CALL(tmt_slots, history_size_changed());
    tm.request_set_history_size(5);
    processEvents();
    EXPECT_EQ(5, tm.history_size());
    tm.request_set_history_size(0);
    processEvents();
    EXPECT_EQ(5, tm.history_size());
    tm.request_set_history_size(5);
    processEvents();
    EXPECT_EQ(5, tm.history_size());
    EXPECT_CALL(tmt_slots, history_size_changed());
    tm.request_set_history_size(500);
    processEvents();
    EXPECT_EQ(100, tm.history_size());
    EXPECT_CALL(tmt_slots, history_size_changed());
    Tiddlerstore::Tiddler t;
    t.set_history_size(59);
    tm.request_set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.history_size(), tm.history_size());
    tm.request_set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, tags)
{
    EXPECT_CALL(tmt_slots, tags_changed());
    tm.request_set_tag("one");
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"one"}), tm.tags());
    tm.request_set_tag("");
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"one"}), tm.tags());
    tm.request_set_tag("one");
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"one"}), tm.tags());
    EXPECT_CALL(tmt_slots, tags_changed());
    tm.request_set_tag("two");
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"one", "two"}), tm.tags());
    EXPECT_CALL(tmt_slots, tags_changed());
    tm.request_remove_tag("one");
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"two"}), tm.tags());
    tm.request_remove_tag("one");
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"two"}), tm.tags());
    tm.request_remove_tag("not there");
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"two"}), tm.tags());
    EXPECT_CALL(tmt_slots, tags_changed());
    Tiddlerstore::Tiddler t;
    t.set_tag("set data");
    tm.request_set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.tags(), tm.tags());
    tm.request_set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, fields)
{
    EXPECT_CALL(tmt_slots, field_added(testing::StrEq("one")));
    tm.request_set_field("one", "1");
    processEvents();
    std::unordered_map<std::string, std::string> expected;
    expected["one"] = "1";
    EXPECT_EQ(expected, tm.fields());
    tm.request_set_field("", "vanish");
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    tm.request_set_field("vanish", "");
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, field_added(testing::StrEq("two")));
    tm.request_set_field("two", "2");
    expected["two"] = "2";
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, field_removed(testing::StrEq("two")));
    tm.request_set_field("two", "");
    expected.erase("two");
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, field_added(testing::StrEq("three")));
    tm.request_set_field("three", "3");
    expected["three"] = "3";
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, field_removed(testing::StrEq("one")));
    tm.request_remove_field("one");
    expected.erase("one");
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    tm.request_remove_field("one");
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, fields_reset());
    Tiddlerstore::Tiddler t;
    t.set_field("set data", "set data");
    tm.request_set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.fields(), tm.fields());
    tm.request_set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, lists)
{
    EXPECT_CALL(tmt_slots, list_added(testing::StrEq("one")));
    tm.request_set_list("one", {"1"});
    processEvents();
    std::unordered_map<std::string, std::vector<std::string>> expected;
    expected["one"] = {"1"};
    EXPECT_EQ(expected, tm.lists());
    tm.request_set_list("", {"vanish"});
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    tm.request_set_list("vanish", {});
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, list_changed(testing::StrEq("one")));
    tm.request_set_list("one", {"1", "1_1"});
    expected["one"] = {"1", "1_1"};
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_EQ(expected["one"], tm.list("one"));
    tm.request_set_list("one", {"1", "1_1"});
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_EQ(expected["one"], tm.list("one"));
    EXPECT_CALL(tmt_slots, list_added(testing::StrEq("two")));
    tm.request_set_list("two", {"2"});
    expected["two"] = {"2"};
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, list_removed(testing::StrEq("two")));
    tm.request_set_list("two", {""});
    expected.erase("two");
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, list_added(testing::StrEq("three")));
    tm.request_set_list("three", {"3"});
    expected["three"] = {"3"};
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, list_removed(testing::StrEq("one")));
    tm.request_remove_list("one");
    expected.erase("one");
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    tm.request_remove_list("one");
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, lists_reset());
    Tiddlerstore::Tiddler t;
    t.set_list("set data", {"set data"});
    tm.request_set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.lists(), tm.lists());
    tm.request_set_tiddler_data(t);
    processEvents();
}

Tiddlerstore_Model_Test::Tiddlerstore_Model_Test()
    : tsm(ts)
{
    connect(&tsm, &Tiddlerstore_Model::added, &tsm_slots, &Tiddlerstore_Model_Test_Slots::added);
    connect(&tsm, &Tiddlerstore_Model::model_created, &tsm_slots, &Tiddlerstore_Model_Test_Slots::model_created);
    connect(&tsm, &Tiddlerstore_Model::removed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::removed);
}

TEST_F(Tiddlerstore_Model_Test, add_model_remove)
{
    using testing::_;
    EXPECT_CALL(tsm_slots, added(0));
    EXPECT_CALL(tsm_slots, model_created(_));
    auto tm1 = tsm.add();
    auto tm1_same = tsm.model_for_index(0);
    auto tm1_still_same = tsm.model_for_tiddler(tm1->tiddler());
    EXPECT_EQ(tm1, tm1_same);
    EXPECT_EQ(tm1, tm1_still_same);
    EXPECT_EQ(nullptr, tsm.model_for_index(1));
    EXPECT_CALL(tsm_slots, added(1));
    EXPECT_CALL(tsm_slots, model_created(_));
    auto tm2 = tsm.add();
    auto tm2_same = tsm.model_for_index(1);
    auto tm2_still_same = tsm.model_for_tiddler(tm2->tiddler());
    EXPECT_EQ(tm2, tm2_same);
    EXPECT_EQ(tm2, tm2_still_same);
    EXPECT_EQ(nullptr, tsm.model_for_index(2));
    auto t1 = ts.emplace_back(new Tiddlerstore::Tiddler).get();
    EXPECT_CALL(tsm_slots, model_created(_));
    auto tm3 = tsm.model_for_tiddler(t1);
    Tiddlerstore::Tiddler f1;
    EXPECT_EQ(nullptr, tsm.model_for_tiddler(&f1));
    EXPECT_CALL(tsm_slots, removed);
    tm1->request_remove();
    auto tm2_still_there = tsm.model_for_index(0);
    EXPECT_EQ(tm2, tm2_still_there);
    EXPECT_CALL(tsm_slots, removed);
    tm2->request_remove();
    EXPECT_CALL(tsm_slots, removed);
    tm3->request_remove();
    EXPECT_EQ(true, ts.empty());
}
