#include "tableview_adjust_row_visibility.h"

#include <QTableView>

namespace Qt_Utilities

{

void tableview_adjust_row_visibility(QTableView* view, Row_Visibility_Checks checks, int startcolumn)
{
    auto display_allows_hiding = [view, checks](int row, int column) {
        auto display = view->model()->data(view->model()->index(row, column), Qt::DisplayRole);
        bool dcan_hide = !display.isValid() || display.toString().isEmpty();
        if (!dcan_hide && checks.testFlag(Row_Visiblility_Check::Disabled_Allows_Hiding)) {
            dcan_hide = !view->model()->flags(view->model()->index(row, column)).testFlag(Qt::ItemIsEnabled);
        }
        return dcan_hide;
    };

    auto decoration_allows_hiding = [view, checks](int row, int column) {
        if (checks.testFlag(Row_Visiblility_Check::Decoration_Keeps_In_View)) {
            auto decoration = view->model()->data(view->model()->index(row, column), Qt::DecorationRole);
            return !decoration.isValid();
        }
        return true;
    };

    auto background_allows_hiding = [view, checks](int row, int column) {
        if (checks.testFlag(Row_Visiblility_Check::Background_Keeps_in_View)) {
            auto background = view->model()->data(view->model()->index(row, column), Qt::BackgroundRole);
            return !background.isValid();
        }
        return true;
    };

    for (int row = 0; row < view->model()->rowCount(); row += 1) {
        bool can_hide = true;
        for (int column = startcolumn; column < view->model()->columnCount() && can_hide; column += 1) {
            if (!view->isColumnHidden(column)) {
                can_hide = display_allows_hiding(row, column) &&
                        decoration_allows_hiding(row, column) &&
                        background_allows_hiding(row, column);
            }
        }
        if (view->isRowHidden(row) != can_hide) {
            view->setRowHidden(row, can_hide);
        }
    }
}

}
