#include "clear_layout.hpp"

#include <QLayout>
#include <QWidget>

namespace Qt_Utilities
{

void clearLayout(QLayout* layout)
{
    QLayoutItem* child;
    while ((child = layout->takeAt(0) ) != nullptr) {
        auto* childWidget = child->widget();
        if (childWidget != nullptr) {
            childWidget->deleteLater();
        }
        auto* childLAyout = child->layout();
        if (childLAyout != nullptr) {
            Qt_Utilities::clearLayout(childLAyout);
        }
        delete child;
    }
}

}
