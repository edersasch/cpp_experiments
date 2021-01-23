#ifndef TESTS_TIDDLERSTORE_TIDDLERSTORE_TEST
#define TESTS_TIDDLERSTORE_TIDDLERSTORE_TEST

#include "tiddlerstore/tiddlerstore.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

class Tiddlerstore_Test
        : public ::testing::Test
{
public:
    Tiddlerstore_Test();
    virtual ~Tiddlerstore_Test() = default;
};

#endif // TESTS_TIDDLERSTORE_TIDDLERSTORE_TEST
