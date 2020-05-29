#include "ar_history_search.h"
#include "qt_utilities/treeview_hide_expand.h"
#include "qt_utilities/combobox_label_elide_left_proxy_style.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QToolButton>
#include <QTreeView>

AR_History_Search::AR_History_Search(const QStringList& start_archives, int history_size, QWidget* parent)
    : QWidget(parent)
    , ar_ui(history_size, start_archives)
{
    auto main_layout(new QVBoxLayout(this));
    auto dir_layout = new QHBoxLayout;
    ar_ui.combobox()->setStyle(new Combobox_Label_Elide_Left_Proxy_Style);
    search_text_edit.setClearButtonEnabled(true);
    dir_tree.setDragEnabled(true);
    dir_tree.setExpandsOnDoubleClick(false);
    dir_tree.setSelectionMode(QAbstractItemView::ExtendedSelection);
    dir_tree.setHeaderHidden(true);
    dir_tree.setModel(&arhelper.get_archive_directory_model());
    filter_pattern.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    dir_layout->addWidget(ar_ui.combobox());
    dir_layout->addWidget(ar_ui.browse_button("...", tr("Please Select Archive"), tr("Archives (*.zip *.7z)")));
    main_layout->addLayout(dir_layout);
    main_layout->addWidget(&search_text_edit);
    main_layout->addWidget(&dir_tree, main_stretch_factor);
    main_layout->addStretch();

    connect(&search_text_edit, &QLineEdit::textChanged, this, [this] {
        if (search_text_edit.text().isEmpty()) {
            filter_pattern.setPattern(QString());
        } else {
            filter_pattern.setPattern(QRegularExpression::wildcardToRegularExpression("*" + search_text_edit.text() + "*"));
        }
        Qt_Utilities::treeview_hide_expand(&dir_tree, dir_tree.model()->index(0, 0), filter_pattern, true, true);
    });
    connect(dir_tree.model(), &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex& index) {
        Qt_Utilities::treeview_hide_expand(&dir_tree, index, filter_pattern, true, true);
    });
    connect(&dir_tree, &QTreeView::doubleClicked, this, [this](const QModelIndex& index) {
        arhelper.open_path(index, archive);
    });
    connect(&ar_ui, &FS_History_UI::current_element_changed, this, [this](const QString& archive_path) {
        archive = archive_path;
        arhelper.ls(archive_path);
        search_text_edit.setFocus();
    });
}
