#include "tiddler_inspector_qt.h"
#include "tiddlerstore_qt/tiddler_model_qt.h"
#include "tiddlerstore_qt/tiddler_pure_view_qt.h"
#include "tiddlerstore_qt/tiddler_pure_edit_qt.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QStackedWidget>

Tiddler_Inspector::Tiddler_Inspector(const QString& tiddlerstore_json, QWidget* parent)
    : QWidget(parent)
    , pure_view(new Tiddler_Pure_View)
    , pure_edit(new Tiddler_Pure_Edit)
{
    auto main_layout(new QVBoxLayout(this));

    auto bastelmodel(new Tiddler_Model(this));
    pure_view->set_tiddler_model(bastelmodel);

    auto tiddler_view_edit_stack(new QStackedWidget(this));
    auto view_index = tiddler_view_edit_stack->addWidget(pure_view);
    auto edit_index = tiddler_view_edit_stack->addWidget(pure_edit);
    connect(pure_view, &Tiddler_Pure_View::trigger_edit, this, [this, tiddler_view_edit_stack, edit_index] {
        is_edit_mode_active = true;
        pure_edit->set_tiddler_model(pure_view->tiddler_model());
        tiddler_view_edit_stack->setCurrentIndex(edit_index);
    });
    connect(pure_edit, &Tiddler_Pure_Edit::accept_edit, this, [this, tiddler_view_edit_stack, view_index] {
        is_edit_mode_active = false;
        tiddler_view_edit_stack->setCurrentIndex(view_index);
    });
    connect(pure_edit, &Tiddler_Pure_Edit::discard_edit, this, [this, tiddler_view_edit_stack, view_index] {
        is_edit_mode_active = false;
        tiddler_view_edit_stack->setCurrentIndex(view_index);
    });

    auto tiddler_scroll = new QScrollArea(this);
    tiddler_scroll->setWidgetResizable(true);
    tiddler_scroll->setWidget(tiddler_view_edit_stack);
    main_layout->addWidget(tiddler_scroll);

    if (!tiddlerstore_json.isEmpty()) {
        //
    }
}

QString Tiddler_Inspector::get_store()
{
    return {};
}
