#ifndef TESTS_ELEMENTARY_MATH_QML_VIS_1000_TEST
#define TESTS_ELEMENTARY_MATH_QML_VIS_1000_TEST

#include "elementary_math_qml/vis_1000.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <QQuickView>
#include <QQuickItem>
#include <QObject>

#include <string>
#include <vector>
#include <memory>

//void processEvents();

/*class Move_List_Qml_Test_Slots
        : public QObject
{
    Q_OBJECT
public:
    virtual ~Move_List_Qml_Test_Slots() = default;
    MOCK_METHOD1(request_set_current_move_and_branch_start_id, void(const int));
    MOCK_METHOD0(request_move_list_forward, void());
    MOCK_METHOD0(request_move_list_back, void());
    MOCK_METHOD1(request_delete_branch, void(const int));
};*/

class Vis_1000_Test
        : public QObject
        , public ::testing::Test
{
    Q_OBJECT
public:
    Vis_1000_Test();
protected:
    QQuickView v;
    Vis_1000 v1;
};

#endif // TESTS_ELEMENTARY_MATH_QML_VIS_1000_TEST
