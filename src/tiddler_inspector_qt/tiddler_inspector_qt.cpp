#include "tiddler_inspector_qt.h"
#include "tiddlerstore_qt/tiddler_model_qt.h"
#include "tiddlerstore_qt/tiddler_pure_view_qt.h"
#include "tiddlerstore_qt/tiddler_pure_edit_qt.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QStackedLayout>

Tiddler_Inspector::Tiddler_Inspector(const QString& tiddlerstore_json, QWidget* parent)
    : QWidget(parent)
{
    auto main_layout(new QVBoxLayout(this));
    auto text_tiddler_ui = new Text_Tiddler_ui(this);
    auto tiddler_scroll = new QScrollArea(this);
    auto bastelmodel = new Tiddler_Model(this);
    tiddler_scroll->setWidgetResizable(true);
    tiddler_scroll->setWidget(text_tiddler_ui);
    main_layout->addWidget(tiddler_scroll);
    text_tiddler_ui->set_tiddler_model(bastelmodel);
    if (!tiddlerstore_json.isEmpty()) {
        //
    }
}

QString Tiddler_Inspector::get_store()
{
    return {};
}

Text_Tiddler_ui::Text_Tiddler_ui(QWidget* parent)
    : QWidget(parent)
    , pure_view(new Tiddler_Pure_View)
    , pure_edit(new Tiddler_Pure_Edit)
{
    auto main_layout(new QStackedLayout(this));
    auto view_index = main_layout->addWidget(pure_view);
    auto edit_index = main_layout->addWidget(pure_edit);
    connect(pure_view, &Tiddler_Pure_View::trigger_edit, this, [this, main_layout, edit_index] {
        is_edit_mode_active = true;
        pure_edit->set_tiddler_model(pure_view->tiddler_model());
        main_layout->setCurrentIndex(edit_index);
    });
    connect(pure_edit, &Tiddler_Pure_Edit::accept_edit, this, [this, main_layout, view_index] {
        is_edit_mode_active = false;
        main_layout->setCurrentIndex(view_index);
    });
    connect(pure_edit, &Tiddler_Pure_Edit::discard_edit, this, [this, main_layout, view_index] {
        is_edit_mode_active = false;
        main_layout->setCurrentIndex(view_index);
    });
}

void Text_Tiddler_ui::set_tiddler_model(Tiddler_Model* tiddler_model)
{
    pure_view->set_tiddler_model(tiddler_model);
}
