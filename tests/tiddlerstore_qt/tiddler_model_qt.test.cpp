#include "tiddler_model_qt.test.h"
#include "tiddlerstore/tiddlerstore.h"

#include "qt_gtest_helper/qt_gtest_helper.h"

Tiddler_Model_Test::Tiddler_Model_Test()
    : tiddler_for_model(new Tiddlerstore::Tiddler)
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

Tiddler_Model_Test::~Tiddler_Model_Test() = default;

TEST_F(Tiddler_Model_Test, title)
{
    EXPECT_CALL(tmt_slots, title_changed());
    EXPECT_EQ(true, tm.set_title("one"));
    processEvents();
    EXPECT_EQ("one", tm.title());
    EXPECT_EQ(false, tm.set_title(""));
    processEvents();
    EXPECT_EQ("one", tm.title());
    EXPECT_EQ(false, tm.set_title("one"));
    processEvents();
    EXPECT_EQ("one", tm.title());
    EXPECT_CALL(tmt_slots, title_changed());
    EXPECT_TRUE(tm.set_title("two"));
    processEvents();
    EXPECT_EQ("two", tm.title());
    EXPECT_CALL(tmt_slots, title_changed());
    Tiddlerstore::Tiddler t;
    EXPECT_EQ(true, t.set_title("set data"));
    tm.set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.title(), tm.title());
    tm.set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, text)
{
    EXPECT_CALL(tmt_slots, text_changed());
    EXPECT_EQ(true, tm.set_text("one"));
    processEvents();
    EXPECT_EQ("one", tm.text());
    EXPECT_EQ(false, tm.set_text(""));
    processEvents();
    EXPECT_EQ("one", tm.text());
    EXPECT_EQ(false, tm.set_text("one"));
    processEvents();
    EXPECT_EQ("one", tm.text());
    EXPECT_CALL(tmt_slots, text_changed());
    EXPECT_EQ(true, tm.set_text("two"));
    processEvents();
    EXPECT_EQ("two", tm.text());
    EXPECT_CALL(tmt_slots, text_changed());
    Tiddlerstore::Tiddler t;
    EXPECT_EQ(true, t.set_text("set data"));
    tm.set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.text(), tm.text());
    tm.set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, history_size)
{
    EXPECT_CALL(tmt_slots, history_size_changed());
    EXPECT_EQ(true, tm.set_history_size(5));
    processEvents();
    EXPECT_EQ(5, tm.history_size());
    EXPECT_EQ(false, tm.set_history_size(0));
    processEvents();
    EXPECT_EQ(5, tm.history_size());
    EXPECT_EQ(false, tm.set_history_size(5));
    processEvents();
    EXPECT_EQ(5, tm.history_size());
    EXPECT_CALL(tmt_slots, history_size_changed());
    EXPECT_EQ(true, tm.set_history_size(500));
    processEvents();
    EXPECT_EQ(100, tm.history_size());
    EXPECT_CALL(tmt_slots, history_size_changed());
    Tiddlerstore::Tiddler t;
    EXPECT_EQ(true, t.set_history_size(59));
    tm.set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.history_size(), tm.history_size());
    tm.set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, tags)
{
    EXPECT_CALL(tmt_slots, tags_changed());
    EXPECT_EQ(true, tm.set_tag("one"));
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"one"}), tm.tags());
    EXPECT_EQ(false, tm.set_tag(""));
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"one"}), tm.tags());
    EXPECT_EQ(false, tm.set_tag("one"));
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"one"}), tm.tags());
    EXPECT_CALL(tmt_slots, tags_changed());
    EXPECT_EQ(true, tm.set_tag("two"));
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"one", "two"}), tm.tags());
    EXPECT_CALL(tmt_slots, tags_changed());
    EXPECT_EQ(true, tm.remove_tag("one"));
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"two"}), tm.tags());
    EXPECT_EQ(false, tm.remove_tag("one"));
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"two"}), tm.tags());
    EXPECT_EQ(false, tm.remove_tag("not there"));
    processEvents();
    EXPECT_EQ(std::vector<std::string>({"two"}), tm.tags());
    EXPECT_CALL(tmt_slots, tags_changed());
    Tiddlerstore::Tiddler t;
    EXPECT_EQ(true, t.set_tag("set data"));
    tm.set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.tags(), tm.tags());
    tm.set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, fields)
{
    EXPECT_CALL(tmt_slots, field_added(testing::StrEq("one")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, tm.set_field("one", "1"));
    processEvents();
    std::unordered_map<std::string, std::string> expected;
    expected["one"] = "1";
    EXPECT_EQ(expected, tm.fields());
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, tm.set_field("", "vanish"));
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, tm.set_field("vanish", ""));
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, field_added(testing::StrEq("two")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, tm.set_field("two", "2"));
    expected["two"] = "2";
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, field_removed(testing::StrEq("two")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Remove, tm.set_field("two", ""));
    expected.erase("two");
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, field_added(testing::StrEq("three")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, tm.set_field("three", "3"));
    expected["three"] = "3";
    processEvents();
    EXPECT_CALL(tmt_slots, field_changed(testing::StrEq("three")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Value, tm.set_field("three", "333"));
    expected["three"] = "333";
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, field_removed(testing::StrEq("one")));
    EXPECT_EQ(true, tm.remove_field("one"));
    expected.erase("one");
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_EQ(false, tm.remove_field("one"));
    processEvents();
    EXPECT_EQ(expected, tm.fields());
    EXPECT_CALL(tmt_slots, fields_reset());
    Tiddlerstore::Tiddler t;
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_field("set data", "set data"));
    tm.set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.fields(), tm.fields());
    tm.set_tiddler_data(t);
    processEvents();
}

TEST_F(Tiddler_Model_Test, lists)
{
    EXPECT_CALL(tmt_slots, list_added(testing::StrEq("one")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, tm.set_list("one", {"1"}));
    processEvents();
    std::unordered_map<std::string, std::vector<std::string>> expected;
    expected["one"] = {"1"};
    EXPECT_EQ(expected, tm.lists());
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, tm.set_list("", {"vanish"}));
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, tm.set_list("vanish", {}));
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, list_changed(testing::StrEq("one")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Value, tm.set_list("one", {"1", "1_1"}));
    expected["one"] = {"1", "1_1"};
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_EQ(expected["one"], tm.list("one"));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::None, tm.set_list("one", {"1", "1_1"}));
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_EQ(expected["one"], tm.list("one"));
    EXPECT_CALL(tmt_slots, list_added(testing::StrEq("two")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, tm.set_list("two", {"2"}));
    expected["two"] = {"2"};
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, list_removed(testing::StrEq("two")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Remove, tm.set_list("two", {""}));
    expected.erase("two");
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, list_added(testing::StrEq("three")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, tm.set_list("three", {"3"}));
    expected["three"] = {"3"};
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, list_removed(testing::StrEq("one")));
    EXPECT_EQ(true, tm.remove_list("one"));
    expected.erase("one");
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_EQ(false, tm.remove_list("one"));
    processEvents();
    EXPECT_EQ(expected, tm.lists());
    EXPECT_CALL(tmt_slots, lists_reset());
    Tiddlerstore::Tiddler t;
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, t.set_list("set data", {"set data"}));
    tm.set_tiddler_data(t);
    processEvents();
    EXPECT_EQ(t.lists(), tm.lists());
    tm.set_tiddler_data(t);
    processEvents();
}

Tiddlerstore_Model_Test::Tiddlerstore_Model_Test()
    : tsm(ts)
{
    connect(&tsm, &Tiddlerstore_Model::added, &tsm_slots, &Tiddlerstore_Model_Test_Slots::added);
    connect(&tsm, &Tiddlerstore_Model::model_created, &tsm_slots, &Tiddlerstore_Model_Test_Slots::model_created);
    connect(&tsm, &Tiddlerstore_Model::begin_remove, &tsm_slots, &Tiddlerstore_Model_Test_Slots::begin_remove);
    connect(&tsm, &Tiddlerstore_Model::removed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::removed);
    connect(&tsm, &Tiddlerstore_Model::title_changed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::title_changed);
    connect(&tsm, &Tiddlerstore_Model::text_changed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::text_changed);
    connect(&tsm, &Tiddlerstore_Model::history_size_changed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::history_size_changed);
    connect(&tsm, &Tiddlerstore_Model::tags_changed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::tags_changed);
    connect(&tsm, &Tiddlerstore_Model::field_changed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::field_changed);
    connect(&tsm, &Tiddlerstore_Model::field_added, &tsm_slots, &Tiddlerstore_Model_Test_Slots::field_added);
    connect(&tsm, &Tiddlerstore_Model::field_removed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::field_removed);
    connect(&tsm, &Tiddlerstore_Model::fields_reset, &tsm_slots, &Tiddlerstore_Model_Test_Slots::fields_reset);
    connect(&tsm, &Tiddlerstore_Model::list_changed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::list_changed);
    connect(&tsm, &Tiddlerstore_Model::list_added, &tsm_slots, &Tiddlerstore_Model_Test_Slots::list_added);
    connect(&tsm, &Tiddlerstore_Model::list_removed, &tsm_slots, &Tiddlerstore_Model_Test_Slots::list_removed);
    connect(&tsm, &Tiddlerstore_Model::lists_reset, &tsm_slots, &Tiddlerstore_Model_Test_Slots::lists_reset);
}

Tiddlerstore_Model_Test::~Tiddlerstore_Model_Test() = default;

TEST_F(Tiddlerstore_Model_Test, add_model_remove)
{
    using testing::_;
    EXPECT_CALL(tsm_slots, added(0));
    EXPECT_CALL(tsm_slots, model_created(_));
    auto& tm1 = tsm.add();
    auto tm1_same = tsm.model_for_index(0);
    auto tm1_still_same = tsm.model_for_tiddler(tm1.tiddler());
    EXPECT_EQ(&tm1, tm1_same);
    EXPECT_EQ(&tm1, tm1_still_same);
    EXPECT_EQ(nullptr, tsm.model_for_index(1));
    EXPECT_CALL(tsm_slots, added(1));
    EXPECT_CALL(tsm_slots, model_created(_));
    auto& tm2 = tsm.add();
    auto tm2_same = tsm.model_for_index(1);
    auto tm2_still_same = tsm.model_for_tiddler(tm2.tiddler());
    EXPECT_EQ(&tm2, tm2_same);
    EXPECT_EQ(&tm2, tm2_still_same);
    EXPECT_EQ(nullptr, tsm.model_for_index(2));
    auto& t1 = *ts.emplace_back(new Tiddlerstore::Tiddler);
    EXPECT_CALL(tsm_slots, model_created(_));
    auto tm3 = tsm.model_for_tiddler(t1);
    Tiddlerstore::Tiddler f1;
    EXPECT_EQ(nullptr, tsm.model_for_tiddler(f1));
    EXPECT_CALL(tsm_slots, begin_remove(&tm1));
    EXPECT_CALL(tsm_slots, removed);
    tm1.request_remove();
    auto tm2_still_there = tsm.model_for_index(0);
    EXPECT_EQ(&tm2, tm2_still_there);
    EXPECT_CALL(tsm_slots, begin_remove(&tm2));
    EXPECT_CALL(tsm_slots, removed);
    tm2.request_remove();
    EXPECT_CALL(tsm_slots, begin_remove(tm3));
    EXPECT_CALL(tsm_slots, removed);
    tm3->request_remove();
    EXPECT_EQ(true, ts.empty());
}

TEST_F(Tiddlerstore_Model_Test, model_signals)
{
    using testing::_;
    EXPECT_CALL(tsm_slots, added(0));
    EXPECT_CALL(tsm_slots, model_created(_));
    auto& tm1 = tsm.add();
    EXPECT_CALL(tsm_slots, added(1));
    EXPECT_CALL(tsm_slots, model_created(_));
    auto& tm2 = tsm.add();
    EXPECT_CALL(tsm_slots, title_changed(&tm1));
    EXPECT_EQ(true, tm1.set_title("tm1 title"));
    processEvents();
    EXPECT_CALL(tsm_slots, text_changed(&tm2));
    EXPECT_EQ(true, tm2.set_text("tm2 text"));
    processEvents();
    EXPECT_CALL(tsm_slots, history_size_changed(&tm1));
    EXPECT_EQ(true, tm1.set_history_size(5));
    processEvents();
    EXPECT_CALL(tsm_slots, tags_changed(&tm2));
    EXPECT_EQ(true, tm2.set_tag("tm2 tag"));
    processEvents();
    EXPECT_CALL(tsm_slots, field_added(&tm1, testing::StrEq("tm1 field")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, tm1.set_field("tm1 field", "1"));
    processEvents();
    EXPECT_CALL(tsm_slots, field_changed(&tm1, testing::StrEq("tm1 field")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Value, tm1.set_field("tm1 field", "2"));
    processEvents();
    EXPECT_CALL(tsm_slots, field_removed(&tm1, testing::StrEq("tm1 field")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Remove, tm1.set_field("tm1 field", ""));
    processEvents();
    Tiddlerstore::Tiddler t;
    Tiddler_Model tmcopy(t);
    tmcopy.set_tiddler_data(tm1);
    tmcopy.set_field("tm1 field", "3");
    EXPECT_CALL(tsm_slots, fields_reset(&tm1));
    tm1.set_tiddler_data(tmcopy);
    processEvents();
    EXPECT_CALL(tsm_slots, list_added(&tm2, testing::StrEq("tm2 list")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Add, tm2.set_list("tm2 list", {"1"}));
    processEvents();
    EXPECT_CALL(tsm_slots, list_changed(&tm2, testing::StrEq("tm2 list")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Value, tm2.set_list("tm2 list", {"2"}));
    processEvents();
    EXPECT_CALL(tsm_slots, list_removed(&tm2, testing::StrEq("tm2 list")));
    EXPECT_EQ(Tiddlerstore::Set_Field_List_Change::Remove, tm2.set_list("tm2 list", {}));
    processEvents();
    tmcopy.set_tiddler_data(tm2);
    tmcopy.set_list("tm2 list", {"3"});
    EXPECT_CALL(tsm_slots, lists_reset(&tm2));
    tm2.set_tiddler_data(tmcopy);
    processEvents();
}

TEST_F(Tiddlerstore_Model_Test, filter)
{
    using testing::_;
    std::string store_json = R"(
                             [
                             {"ti":"aaTidABC","v":1},
                             {"ti":"bbTidBCD","ta":["bbT1"],"v":1},
                             {"ti":"ccTidCDE","v":1}
                             ])";
    ts = nlohmann::json::parse(store_json);
    EXPECT_EQ(3, ts.size());
    Tiddlerstore::Filter_Groups fg1;
    auto check_fg1 = [this, &fg1](Tiddlerstore::Store_Indexes si) {
        EXPECT_EQ(si, tsm.apply_filter(fg1).filtered_idx());
        std::vector<Tiddler_Model*> expected_models;
        for (const auto& i : si) {
            expected_models.push_back(tsm.model_for_index(i));
        }
        EXPECT_EQ(expected_models, tsm.filtered_models(tsm.apply_filter(fg1)));
        EXPECT_EQ(expected_models[0], tsm.first_filtered_model(tsm.apply_filter(fg1)));
    };
    EXPECT_CALL(tsm_slots, model_created(_)).Times(3);
    check_fg1({0, 1, 2});
    auto sg1 = fg1.emplace_back(new Tiddlerstore::Single_Group).get();
    check_fg1({0, 1, 2});
    auto fd1_1 = sg1->emplace_back(new Tiddlerstore::Filter_Data).get();
    check_fg1({0, 1, 2});
    fd1_1->key = "c";
    check_fg1({0, 1, 2});
    fd1_1->case_sensitive = true;
    check_fg1({2});
    check_fg1(tsm.filter().title_contains("c", true).filtered_idx());
    fd1_1->negate = true;
    check_fg1({0, 1});
    auto sg2 = fg1.emplace_back(new Tiddlerstore::Single_Group).get();
    check_fg1({0, 1});
    auto fd2_1 = sg2->emplace_back(new Tiddlerstore::Filter_Data).get();
    check_fg1({0, 1});
    fd2_1->key = "CC";
    check_fg1({0, 1, 2});
    auto fd1_2 = sg1->emplace_back(new Tiddlerstore::Filter_Data).get();
    check_fg1({0, 1, 2});
    fd1_2->filter_type = Tiddlerstore::Filter_Type::Tag;
    check_fg1({0, 1, 2});
    fd1_2->key = "bbT1";
    check_fg1({1, 2});
    fd1_2->negate = true;
    check_fg1({0, 2});
}
