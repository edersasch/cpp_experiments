#include "tiddler_inspector_qt.h"
#include "tiddlerstore_qt/tiddler_pure_view_qt.h"
#include "tiddlerstore_qt/tiddler_pure_edit_qt.h"
#include "tiddlerstore_qt/tiddlerstore_handler_qt.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QStackedWidget>
#include <QSplitter>
#include <QSortFilterProxyModel>

Tiddler_Inspector::Tiddler_Inspector(const QStringList& tiddlerstore_list, QWidget* parent)
    : QWidget(parent)
    , store_handler(new Tiddlerstore_Handler(tiddlerstore_list))
    , pure_view(new Tiddler_Pure_View)
    , pure_edit(new Tiddler_Pure_Edit)
{
    auto tiddler_view_edit_stack(new QStackedWidget(this));
    auto view_index = tiddler_view_edit_stack->addWidget(pure_view);
    auto edit_index = tiddler_view_edit_stack->addWidget(pure_edit);
    connect(pure_view, &Tiddler_Pure_View::trigger_edit, this, [this, tiddler_view_edit_stack, edit_index] {
        pure_edit->set_tiddler_model(pure_view->tiddler_model());
        tiddler_view_edit_stack->setCurrentIndex(edit_index);
    });
    connect(pure_edit, &Tiddler_Pure_Edit::accept_edit, tiddler_view_edit_stack, [tiddler_view_edit_stack, view_index] {
        tiddler_view_edit_stack->setCurrentIndex(view_index);
    });
    connect(pure_edit, &Tiddler_Pure_Edit::discard_edit, tiddler_view_edit_stack, [tiddler_view_edit_stack, view_index] {
        tiddler_view_edit_stack->setCurrentIndex(view_index);
    });
    connect(store_handler, &Tiddlerstore_Handler::open_tiddler_model, this, [this, tiddler_view_edit_stack, view_index](Tiddler_Model* tm) {
        pure_view->set_tiddler_model(tm);
        if (tm) {
            connect(tm, &Tiddler_Model::destroyed, tiddler_view_edit_stack, [tiddler_view_edit_stack, view_index]{
                tiddler_view_edit_stack->setCurrentIndex(view_index);
            });
        }
    });

    auto tiddler_scroll(new QScrollArea);
    tiddler_scroll->setWidgetResizable(true);
    tiddler_scroll->setWidget(tiddler_view_edit_stack);

    auto main_splitter(new QSplitter);
    main_splitter->addWidget(store_handler);
    main_splitter->addWidget(tiddler_scroll);
    main_splitter->setStretchFactor(1, 2);

    auto main_layout(new QVBoxLayout(this));
    main_layout->addWidget(main_splitter);
}

QStringList Tiddler_Inspector::get_tiddlerstore_list()
{
    return store_handler->get_tiddlerstore_list();
}
