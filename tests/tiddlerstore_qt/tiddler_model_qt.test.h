#ifndef TESTS_TIDDLERSTORE_QT_TIDDLER_MODEL_QT_TEST
#define TESTS_TIDDLERSTORE_QT_TIDDLER_MODEL_QT_TEST

#include "tiddlerstore_qt/tiddler_model_qt.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

class Tiddler_Model_Test_Slots
        : public QObject
{
    Q_OBJECT

public:
    Tiddler_Model_Test_Slots() = default;
    virtual ~Tiddler_Model_Test_Slots() = default;

    MOCK_METHOD(void, title_changed, ());
    MOCK_METHOD(void, text_changed, ());
    MOCK_METHOD(void, history_size_changed, ());
    MOCK_METHOD(void, tags_changed, ());
    MOCK_METHOD(void, fields_changed, ());
    MOCK_METHOD(void, lists_changed, ());
    MOCK_METHOD(void, single_list_changed, (const char*));
};

class Tiddler_Model_Test
        : public QObject
        , public ::testing::Test
{
    Q_OBJECT

public:
    Tiddler_Model_Test();
    virtual ~Tiddler_Model_Test() = default;

protected:
    ::testing::StrictMock<Tiddler_Model_Test_Slots> tmt_slots;
    Tiddler_Model tm;
};

#endif // TESTS_TIDDLERSTORE_QT_TIDDLER_MODEL_QT_TEST
