#ifndef SRC_QT_UTILITIES_COMBOBOX_LABEL_ELIDE_LEFT_PROXY_STYLE
#define SRC_QT_UTILITIES_COMBOBOX_LABEL_ELIDE_LEFT_PROXY_STYLE

#include <QProxyStyle>

class Combobox_Label_Elide_Left_Proxy_Style : public QProxyStyle
{
public:
    virtual void drawControl(QStyle::ControlElement element, const QStyleOption* combobox_option, QPainter* painter, const QWidget* widget = nullptr) const override;
};

#endif // SRC_QT_UTILITIES_COMBOBOX_LABEL_ELIDE_LEFT_PROXY_STYLE
