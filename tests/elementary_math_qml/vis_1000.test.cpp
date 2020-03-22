#include "vis_1000.test.h"

#include "qt_gtest_helper/qt_gtest_helper.h"

Vis_1000_Test::Vis_1000_Test()
    : v1(v.engine(), v.contentItem())
{
    Q_INIT_RESOURCE(vis_1000);
    processEvents();
}

TEST_F(Vis_1000_Test, ctordtor)
{
}
