#ifndef SRC_QTABLEVIEW_DEMO_QTABLEVIEW_DEMO
#define SRC_QTABLEVIEW_DEMO_QTABLEVIEW_DEMO

#include <QWidget>
#include <QToolBox>

#include "qt_utilities/tableview_adjust_row_visibility.h"

class QVBoxLayout;
class QTableView;
class QSpinBox;

class QTableView_Demo
        : public QWidget
{
    Q_OBJECT

public:
    QTableView_Demo(QWidget* parent = nullptr);
    virtual ~QTableView_Demo() override = default;

private:
    void horizontal_scroll();
    void column_size_max_percent();
    void hide_lines();
    QToolBox tb;
    QTableView* column_size_max_percent_tv {nullptr};
    QSpinBox* column_size_max_percent_sb {nullptr};
    Qt_Utilities::Row_Visibility_Checks rv_checks = Qt_Utilities::Row_Visiblility_Check::None;
};

#endif // SRC_QTABLEVIEW_DEMO_QTABLEVIEW_DEMO
