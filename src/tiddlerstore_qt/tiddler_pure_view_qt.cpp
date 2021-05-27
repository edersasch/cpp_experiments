#include "tiddler_model_qt.h"
#include "tiddler_pure_view_qt.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QTextBrowser>
#include <QStyle>

Tiddler_Pure_View::Tiddler_Pure_View(QWidget* parent)
    : QWidget(parent)
    , title_label(new QLabel)
    , text_browser(new QTextBrowser)
    , edit_button(new QToolButton)
{
    auto main_layout = new QVBoxLayout(this);

    auto title_layout(new QHBoxLayout);
    title_layout->addWidget(title_label);
    edit_button->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    title_layout->addWidget(edit_button);
    connect(edit_button, &QToolButton::clicked, this, &Tiddler_Pure_View::trigger_edit);
    main_layout->addLayout(title_layout);

    main_layout->addWidget(text_browser);
}

Tiddler_Model* Tiddler_Pure_View::tiddler_model()
{
    return tm;
}

void Tiddler_Pure_View::set_tiddler_model(Tiddler_Model* model)
{
    if (tm != model) {
        if (tm) {
            disconnect(tm);
        }
        tm = model;
        title_label->setText(tm ? tm->title().c_str() : QString());
        text_browser->setText(tm ? tm->text().c_str() : QString());
        if (tm) {
            connect(tm, &Tiddler_Model::title_changed, this, [this] {
                title_label->setText(tm->title().c_str());
            });
            connect(tm, &Tiddler_Model::text_changed, this, [this] {
                text_browser->setText(tm->text().c_str());
            });
            connect(tm, &Tiddler_Model::destroyed, this, [this]{
                set_tiddler_model(nullptr);
            });
        }
        present_edit_button();
    }
}

// protected

void Tiddler_Pure_View::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    present_edit_button();
}

// private

void Tiddler_Pure_View::present_edit_button()
{
    tm ? edit_button->show() : edit_button->hide();
}
