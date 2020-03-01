#include "qt_gtest_helper.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <QApplication>

#include <chrono>
#include <thread>


void processEvents()
{
    while (qApp->hasPendingEvents()) {
        qApp->processEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

int main(int argc, char **argv)
{
    QApplication app {argc, argv};
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
