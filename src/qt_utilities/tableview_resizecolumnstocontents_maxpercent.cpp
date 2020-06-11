#include "tableview_resizecolumnstocontents_maxpercent.h"

#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>

#include <algorithm>

namespace Qt_Utilities

{

void tableview_resizecolumnstocontents_maxpercent(QTableView* view, int maxpercent)
{
    static constexpr int hundred_percent = 100;
    static constexpr int startcolumn = 0;
    const int endcolumn = view->model()->columnCount();

    if (!view->isVisible()) {
        return;
    }
    int number_of_visible_columns = 0;
    for (int i = startcolumn; i < endcolumn; i += 1) {
        if (!view->isColumnHidden(i)) {
            number_of_visible_columns += 1;
        }
    }
    if (number_of_visible_columns == 0) {
        return;
    }
    maxpercent = std::clamp(maxpercent, 1, hundred_percent);
    view->resizeColumnsToContents();
    int column_area_width_rest = view->viewport()->width();
    int max_column_width = std::max(column_area_width_rest / number_of_visible_columns,
                                    (column_area_width_rest * maxpercent) / hundred_percent);
    for (int i = startcolumn; i < endcolumn; i += 1) {
        if (!view->isColumnHidden(i)) {
            if (view->columnWidth(i) > max_column_width) {
                view->setColumnWidth(i, max_column_width);
            }
            column_area_width_rest -= view->columnWidth(i);
        }
    }
    if (column_area_width_rest > 0) {
        for (int i = endcolumn - 1; i >= 0; i -= 1) {
            if (!view->isColumnHidden(i)) {
                auto w = view->columnWidth(i);
                if (w == max_column_width) {
                    view->resizeColumnToContents(i);
                    int diff = view->columnWidth(i) - w;
                    if (diff > column_area_width_rest) {
                        view->setColumnWidth(i, w + column_area_width_rest);
                        break;
                    }
                    column_area_width_rest -= diff;
                }
            }
        }
    }
}

}
