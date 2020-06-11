#include "qtableview_demo.h"
#include "qt_utilities/tableview_resizecolumnstocontents_maxpercent.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QCheckBox>
#include <QTextBrowser>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>

QTableView_Demo::QTableView_Demo(QWidget* parent)
    : QWidget(parent)
{
    auto l = new QVBoxLayout(this);

    horizontal_scroll();
    column_size_max_percent();
    hide_lines();

    l->addWidget(&tb);

    connect(&tb, &QToolBox::currentChanged, this, [this](int index) {
        if (index == 1) {
            Qt_Utilities::tableview_resizecolumnstocontents_maxpercent(column_size_max_percent_tv, column_size_max_percent_sb->value());
        }
    });
}

// private

void QTableView_Demo::horizontal_scroll()
{
    auto w = new QWidget(this);
    auto l = new QVBoxLayout(w);
    auto text = new QTextBrowser(this);
    auto cb = new QCheckBox(tr("use scroll by pixel"), this);
    auto sim = new QStandardItemModel(this);
    auto tv = new QTableView(this);

    QString text_before_docanchor(tr("The"));
    QString docanchor_start("<a href=\"https://doc.qt.io/qt-5/qabstractitemview.html#horizontalScrollMode-prop\">");
    QString docanchor_text(tr("horizontal scroll mode"));
    QString docanchor_end("</a>");
    QString docanchor(docanchor_start + docanchor_text + docanchor_end);
    QString text_after_docanchor(tr("can move the table by item or by pixel. "
                                    "Clicking the long cell with scrolling by item makes the table jump to "
                                    "the end of the cell, which might not be desired. Scrolling by pixel "
                                    "jumps to the start of the cell."));
    text->setOpenExternalLinks(true);
    text->setText(text_before_docanchor + " " + docanchor + " " + text_after_docanchor);
    text->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
    sim->setItem(0, 0, new QStandardItem(tr("short")));
    sim->setItem(0, 1, new QStandardItem(tr("incredibly long, causing a horizontal scrollbar after the column is resized to fit the content and if you click on this item it will make a jump to the right, with your hands on your hips you bring your knees in tight")));
    tv->setModel(sim);
    tv->resizeColumnsToContents();
    tv->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
    tv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);

    l->addWidget(text);
    l->addWidget(cb);
    l->addWidget(tv);

    connect(cb, &QCheckBox::clicked, this, [tv](bool checked) {
        tv->setHorizontalScrollMode(checked ? QAbstractItemView::ScrollPerPixel : QAbstractItemView::ScrollPerItem);
    });

    tb.addItem(w, tr("Horizontal scrolling for long cell"));
}

void QTableView_Demo::column_size_max_percent()
{
    static constexpr int hundred_percent = 100;
    static constexpr int start_percent = 33;
    auto w = new QWidget(this);
    auto l = new QVBoxLayout(w);
    auto text = new QTextBrowser(this);
    auto sim = new QStandardItemModel(this);
    auto sb_layout = new QHBoxLayout;
    auto sb_label_pre = new QLabel(tr("maximum width for each column in % of table width"), this);
    auto sb_label_post = new QLabel(tr("%"), this);
    column_size_max_percent_sb = new QSpinBox(this);
    column_size_max_percent_tv = new QTableView(this);

    QString description(tr("The available space of the table is distributed among the columns. "
                           "If a column's content is wider and the given maximum percent allow "
                           "to use more space, the column will get it. If space is left, the columns "
                           "staring from the last one will get as much as needed until no space is "
                           "left. "
                           "The sizes are set once, so there is no adaption if "
                           "the table gets resized. Nothing is fixed, the user can still adjust "
                           "all columns. You can edit the cells by double clicking."));
    text->setText(description);
    text->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
    sim->setItem(0, 0, new QStandardItem("a b c d e f g end"));
    sim->setItem(0, 1, new QStandardItem("a b c d e f g h i j k l m end"));
    sim->setItem(0, 2, new QStandardItem("a b c d e f g h i j k l m n o p q r s t end"));
    sim->setItem(0, 3, new QStandardItem("a b c d e f g h i j k l m n o p q r s t u v w x y z end"));
    sim->setItem(0, 4, new QStandardItem("a b c d e f g h i j k l m n o p q r s t u v w x y z 1 2 3 4 5 6 7 8 9 0 end"));
    column_size_max_percent_sb->setRange(1, hundred_percent);
    column_size_max_percent_sb->setValue(start_percent);
    column_size_max_percent_tv->setModel(sim);
    column_size_max_percent_tv->resizeColumnsToContents();
    column_size_max_percent_tv->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    column_size_max_percent_tv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);

    sb_layout->addWidget(sb_label_pre);
    sb_layout->addWidget(column_size_max_percent_sb);
    sb_layout->addWidget(sb_label_post);
    sb_layout->addStretch();
    l->addWidget(text);
    l->addLayout(sb_layout);
    l->addWidget(column_size_max_percent_tv);

    connect(column_size_max_percent_sb, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int value) {
        Qt_Utilities::tableview_resizecolumnstocontents_maxpercent(column_size_max_percent_tv, value);
    });

    tb.addItem(w, tr("Set column sizes up to a maximum"));
}

void QTableView_Demo::hide_lines()
{
    static constexpr int number_of_rows = 5;
    static constexpr int number_of_columns = 6;
    static constexpr int last_column_index = 5;
    auto w = new QWidget(this);
    auto l = new QVBoxLayout(w);
    auto text = new QTextBrowser(this);
    auto sim = new QStandardItemModel(this);
    auto table_layout = new QHBoxLayout;
    auto tv = new QTableView(this);
    auto gb = new QGroupBox(tr("visible columns"), this);
    auto gb_layout = new QVBoxLayout(gb);
    auto config_layout = new QHBoxLayout;
    auto start_label = new QLabel(tr("ignore front columns, including: "));
    auto start_sb = new QSpinBox(this);
    auto ignore_icon_cb = new QCheckBox(tr("ignore icon"), this);
    auto ignore_bg_cb = new QCheckBox(tr("ignore background"), this);
    auto ignore_disabled_cb = new QCheckBox(tr("ignore disabled cells"), this);

    QString description(tr("If you hide columns in sparsely populated tables, "
                           "only irrelevant data or empty rows might be left. "
                           "Icons without text can keep the row in view. "
                           "You can edit the cells by double clicking. After "
                           "modifying a cell, toggle the checkbox you are interested "
                           "in."));
    text->setText(description);
    text->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);

    for (int column = 0; column < number_of_columns; column += 1) {
        for (int row = 0; row < number_of_rows; row += 1) {
            sim->setItem(row, column, new QStandardItem);
        }
        if (column > 0) {
            auto cb = new QCheckBox(QString::number(column + 1), this);
            cb->setChecked(true);
            connect(cb, &QCheckBox::clicked, this, [this, tv, column, start_sb](bool checked) {
                tv->setColumnHidden(column, !checked);
                Qt_Utilities::tableview_adjust_row_visibility(tv, rv_checks, start_sb->value());
            });
            gb_layout->addWidget(cb);
        }
    }
    sim->item(0, 0)->setText("Animals with \"A\"");
    sim->item(1, 0)->setText("Animals with \"B\", but what, if none is there?");
    sim->item(2, 0)->setText("\"C\" animals, if empty, who needs this explanation?");
    sim->item(3, 0)->setText("Animals with \"D\"");
    sim->item(4, 0)->setText("Animals with \"E\"");
    sim->item(0, 1)->setText("Ant");
    sim->item(0, 2)->setBackground(QBrush(QColor("mistyrose")));
    sim->item(1, 2)->setText("Bear");
    sim->item(1, last_column_index)->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));
    sim->item(1, last_column_index)->setBackground(QBrush(QColor("palegreen")));
    sim->item(2, 3)->setText("Cat");
    sim->item(2, 3)->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));
    sim->item(2, 3)->setBackground(QBrush(QColor("yellow")));
    sim->item(3, 4)->setText("Dog");
    sim->item(3, last_column_index)->setText("Disabled Dog");
    sim->item(3, last_column_index)->setEnabled(false);
    sim->item(3, 4)->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));
    sim->item(4, 2)->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));
    sim->item(4, last_column_index)->setText("Elephant");
    tv->verticalHeader()->hide();
    tv->setModel(sim);
    tv->resizeColumnsToContents();
    tv->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
    tv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
    start_sb->setRange(0, tv->model()->columnCount());
    start_sb->setValue(1);
    ignore_icon_cb->setChecked(true);
    ignore_bg_cb->setChecked(true);
    ignore_disabled_cb->setChecked(true);
    rv_checks.setFlag(Qt_Utilities::Row_Visiblility_Check::Disabled_Allows_Hiding);

    connect(start_sb, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this, tv](int value) {
        Qt_Utilities::tableview_adjust_row_visibility(tv, rv_checks, value);
    });
    connect(ignore_icon_cb, &QCheckBox::clicked, this, [this, tv, start_sb] (bool checked) {
        rv_checks.setFlag(Qt_Utilities::Row_Visiblility_Check::Decoration_Keeps_In_View, !checked);
        Qt_Utilities::tableview_adjust_row_visibility(tv, rv_checks, start_sb->value());
    });
    connect(ignore_bg_cb, &QCheckBox::clicked, this, [this, tv, start_sb] (bool checked) {
        rv_checks.setFlag(Qt_Utilities::Row_Visiblility_Check::Background_Keeps_in_View, !checked);
        Qt_Utilities::tableview_adjust_row_visibility(tv, rv_checks, start_sb->value());
    });
    connect(ignore_disabled_cb, &QCheckBox::clicked, this, [this, tv, start_sb] (bool checked) {
        rv_checks.setFlag(Qt_Utilities::Row_Visiblility_Check::Disabled_Allows_Hiding, checked); // this "checked" must not be inverted
        Qt_Utilities::tableview_adjust_row_visibility(tv, rv_checks, start_sb->value());
    });

    gb_layout->addStretch();
    config_layout->addWidget(start_label);
    config_layout->addWidget(start_sb);
    config_layout->addWidget(ignore_icon_cb);
    config_layout->addWidget(ignore_bg_cb);
    config_layout->addWidget(ignore_disabled_cb);
    config_layout->addStretch();
    table_layout->addWidget(tv);
    table_layout->addWidget(gb);
    l->addWidget(text);
    l->addLayout(config_layout);
    l->addLayout(table_layout);

    tb.addItem(w, tr("Hide lines in sparse tables"));
}
