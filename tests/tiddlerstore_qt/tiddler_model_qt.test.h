#ifndef TESTS_TIDDLERSTORE_QT_TIDDLER_MODEL_QT_TEST
#define TESTS_TIDDLERSTORE_QT_TIDDLER_MODEL_QT_TEST

#include "tiddlerstore_qt/tiddler_model_qt.h"
#include "tiddlerstore/tiddlerstore_types.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>

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
    MOCK_METHOD(void, field_changed, (const char*));
    MOCK_METHOD(void, field_added, (const char*));
    MOCK_METHOD(void, field_removed, (const char*));
    MOCK_METHOD(void, fields_reset, ());
    MOCK_METHOD(void, list_changed, (const char*));
    MOCK_METHOD(void, list_added, (const char*));
    MOCK_METHOD(void, list_removed, (const char*));
    MOCK_METHOD(void, lists_reset, ());
};

class Tiddler_Model_Test
        : public QObject
        , public ::testing::Test
{
    Q_OBJECT

public:
    Tiddler_Model_Test();
    virtual ~Tiddler_Model_Test() override;

protected:
    ::testing::StrictMock<Tiddler_Model_Test_Slots> tmt_slots;
    std::unique_ptr<Tiddlerstore::Tiddler> tiddler_for_model;
    Tiddler_Model tm {*tiddler_for_model};
};

class Tiddlerstore_Model_Test_Slots
        : public QObject
{
    Q_OBJECT

public:
    Tiddlerstore_Model_Test_Slots() = default;
    virtual ~Tiddlerstore_Model_Test_Slots() = default;

    MOCK_METHOD(void, added, (int));
    MOCK_METHOD(void, model_created, (Tiddler_Model*));
    MOCK_METHOD(void, removed, ());
};

class Tiddlerstore_Model_Test
        : public QObject
        , public ::testing::Test
{
    Q_OBJECT

public:
    Tiddlerstore_Model_Test();
    virtual ~Tiddlerstore_Model_Test() override;

protected:
    ::testing::StrictMock<Tiddlerstore_Model_Test_Slots> tsm_slots;
    Tiddlerstore::Store ts;
    Tiddlerstore_Model tsm;
};

#endif // TESTS_TIDDLERSTORE_QT_TIDDLER_MODEL_QT_TEST
