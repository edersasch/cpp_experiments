#ifndef SRC_QT_UTILITIES_TABLEVIEW_ADJUST_ROW_VISIBILITY
#define SRC_QT_UTILITIES_TABLEVIEW_ADJUST_ROW_VISIBILITY

#include <QFlags>

class QTableView;

namespace Qt_Utilities
{

enum class Row_Visiblility_Check
{
    None = 0, // no additional check: row can hide if cell is empty (disabled is not empty), decoration and backround are ignored
    Decoration_Keeps_In_View = 1, // keep the row in view if decoration is set
    Background_Keeps_in_View = 2, // keep the row in view if background is set
    Disabled_Allows_Hiding = 4 // row can hide if cell is disabled
};

using Row_Visibility_Checks = QFlags<Row_Visiblility_Check>;

void tableview_adjust_row_visibility(QTableView* view, Row_Visibility_Checks checks = Row_Visiblility_Check::None, int startcolumn = 0);

}

#endif // SRC_QT_UTILITIES_TABLEVIEW_ADJUST_ROW_VISIBILITY
