#include "fs_history_ui.h"

#include <QToolButton>
#include <QComboBox>
#include <QFileDialog>
#include <QTextOption>
#include <QPainter>
#include <QMenu>

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

FS_History_UI::FS_History_UI(FS_History::Operation_Mode mode, const QString& fallback, int history_size, const QStringList& initial_elements, QObject* parent)
    : QObject(parent)
    , history(mode, fallback, history_size, initial_elements)
{
    model.setStringList(history.get_elements());
    connect(&history, &FS_History::elements_changed, this, [this](const QStringList& list) {
        model.setStringList(list);
        if (chooser) {
            chooser->setCurrentIndex(0);
        }
        if (fs_menu) {
            menu();
        }
        emit current_element_changed(list.isEmpty() ? QString() : list.first());
    });
}

FS_History_UI::FS_History_UI(const QStringList& initial_elements, QObject* parent)
    : FS_History_UI(FS_History::Operation_Mode::OP_FILE, "", FS_History::default_history_size, initial_elements, parent)
{
}

FS_History_UI::FS_History_UI(int history_size, const QStringList& initial_elements, QObject* parent)
    : FS_History_UI(FS_History::Operation_Mode::OP_FILE, "", history_size, initial_elements, parent)
{
}

FS_History_UI::FS_History_UI(const QString& fallback_dir, int history_size, const QStringList& initial_elements, QObject* parent)
    : FS_History_UI(FS_History::Operation_Mode::OP_DIR, fallback_dir, history_size, initial_elements, parent)
{
}

QComboBox* FS_History_UI::combobox()
{
    if (!chooser) {
        chooser = new QComboBox;
        chooser->setModel(&model);
        connect(chooser, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), &history, &FS_History::set_current_element);
    }
    return chooser;
}

QToolButton* FS_History_UI::browse_button(const QString& button_text, const QString& dialog_caption)
{
    if (!file_dialog_button) {
        file_dialog_button = new QToolButton;
        file_dialog_button->setText(button_text);
        connect(file_dialog_button, &QToolButton::clicked, this, [this, dialog_caption] {
            switch (history.operation_mode()) {
            case FS_History::Operation_Mode::OP_FILE:
                history.set_current_element(QFileDialog::getOpenFileName(nullptr, dialog_caption, chooser->currentText()));
                break;
            case FS_History::Operation_Mode::OP_DIR:
                history.set_current_element(QFileDialog::getExistingDirectory(nullptr, dialog_caption, chooser->currentText()));
                break;
            }
        });
    }
    return file_dialog_button;
}

QMenu* FS_History_UI::menu(bool use_hotkey, const QString& name)
{
    if (!fs_menu) {
        fs_menu = new QMenu(name);
    }
    fs_menu->clear();
    auto elms = history.get_elements();
    bool prepend_hotkey = elms.size() <= FS_History::default_history_size;
    int hotkey = 1;
    for (auto element : elms) {
        auto a = fs_menu->addAction(element);
        connect(a, &QAction::triggered, this, [this, element] {
            history.set_current_element(element);
        });
        if (prepend_hotkey && use_hotkey) {
            element.prepend("&" + QString::number(hotkey) + " ");
            a->setText(element);
            hotkey += 1;
        }
    }
    return fs_menu;
}
