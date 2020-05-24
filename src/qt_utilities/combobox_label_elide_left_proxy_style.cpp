#include "combobox_label_elide_left_proxy_style.h"

#include <QStyleOption>
#include <QTextOption>
#include <QPainter>

void Combobox_Label_Elide_Left_Proxy_Style::drawControl(QStyle::ControlElement element, const QStyleOption* combobox_option, QPainter* painter, const QWidget* widget) const
{
    if (element == QStyle::CE_ComboBoxLabel) {
        if (auto cb = qstyleoption_cast<const QStyleOptionComboBox*>(combobox_option)) {
            // https://stackoverflow.com/questions/41360618/qcombobox-elided-text-on-selected-item
            QRect textRect = subControlRect(QStyle::CC_ComboBox, cb, QStyle::SC_ComboBoxEditField, widget);
            QTextOption textOption;
            textOption.setAlignment(Qt::AlignVCenter);
            QFontMetrics fontMetric(painter->font());
            const QString elidedText = fontMetric.elidedText(cb->currentText, Qt::ElideLeft, textRect.width());
            painter->drawText(textRect, elidedText, textOption);
            return;
        }
    }
    QProxyStyle::drawControl(element, combobox_option, painter, widget);
}
