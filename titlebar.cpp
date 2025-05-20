#include "titlebar.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <qstyle.h>

namespace {
    static const QString title_bar_style = R"(
        /* 1) Basis: TitleBar-Hintergrund */
        TitleBar {
            background-color: #2d2d30;
        }

        /* 2) Grundstil für alle ToolButtons */
        QToolButton {
            background: transparent;
            border: none;
            /* Mehr Platz rund ums Icon */
            padding: 6px;
            /* Einheitliche Größe */
            min-width: 32px;
            /* Icon-Größe festlegen */
            qproperty-iconSize: 24px 24px;
        }

        /* 3) Standard-Iconfarbe */
        QToolButton#minimize_button {
            qproperty-icon: url(":/icons/src/images/min.svg");
        }

        QToolButton#maximize_button {
            qproperty-icon: url(":/icons/src/images/max-2.svg");
        }

        QToolButton#close_button {
            qproperty-icon: url(":/icons/src/images/close-2.svg");
            min-width: 38px;
        }

        /* 4) Hover für alle Buttons */
        QToolButton:hover {
            background-color: rgba(255,255,255,0.1); /* leichter, weißer Overlay */
            opacity: 1.0;                             /* volle Sichtbarkeit */
        }

        /* 5) Pressed State */
        QToolButton:pressed {
            background-color: rgba(255,255,255,0.2);
        }

        /* 6) Spezielles Hover für Minimize & Maximize */
        QToolButton#minimize_button:hover,
        QToolButton#maximize_button:hover {
            background-color: rgba(100,100,255,0.2); /* sanfter Blauton */
        }

        /* 7) Close-Button: Warnfarbe */
        QToolButton#close_button {
            /* Grundsätzlich Icon-Farbe leicht rötlich */
            color: #e81123;
        }
        QToolButton#close_button:hover {
            background-color: rgba(232, 17, 35, 0.3);
        }
        QToolButton#close_button:pressed {
            background-color: rgba(232, 17, 35, 0.5);
        }

        /* 8) Disabled State (optional) */
        QToolButton:disabled {
            opacity: 0.4;
        }
    )";

    static const QString menu_bar_style = R"(
        QMenuBar {
            background: transparent;
            color: #ddd;

        }
        QMenuBar::item:selected { background: #3e3e42; }

        QMenu {
            background-color: #2d2d30;
            border: 1px solid #3e3e42;
            padding: 4px;
            margin: 2px;
        }
        QMenu::item {
            color: #ddd;
            background-color: transparent;
            padding: 6px 20px;
            margin: 2px 0;
        }
        QMenu::item:selected {
            background-color: #3e3e42;
            color: #fff;
        }
        QMenu::item:pressed {
            background-color: #505057;
        }
        QMenu::separator {
            height: 1px;
            background: #3e3e42;
            margin: 4px 0;
        }
    )";
};

TitleBar::TitleBar(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(title_bar_style);

    QMenuBar* menu_bar = TitleBar::generate_menu_bar();
    TitleBar::generate_title_buttons(parent);

    auto h = new QHBoxLayout(this);
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(6);
    h->insertWidget(0, menu_bar);
    h->addStretch();
    h->addWidget(m_min_btn);
    h->addWidget(m_max_btn);
    h->addWidget(m_close_btn);
    setLayout(h);

    setFixedHeight(30);
}

QMenuBar* TitleBar::generate_menu_bar() {
    QMenuBar* menu_bar = new QMenuBar(this);
    QMenu* file_menu = menu_bar->addMenu(tr("File"));
    file_menu->addAction(tr("Open File"), this, &TitleBar::open_file_requested);
    file_menu->addAction(tr("Open Folder"), this, &TitleBar::open_folder_requested);
    file_menu->addAction(tr("Open Archive"), this, &TitleBar::open_archive_requested);
    file_menu->addAction(tr("Clear Space"), this, &TitleBar::clear_space_requested);


    QMenu* export_menu = menu_bar->addMenu(tr("Export"));
    export_menu->addAction(tr("Export specific search"), this, &TitleBar::export_specific_search_requested);
    export_menu->addAction(tr("Export global search"), this, &TitleBar::export_global_search_requested);

    menu_bar->setNativeMenuBar(false);
    menu_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    int hHint = menu_bar->sizeHint().height();
    menu_bar->setMinimumHeight(hHint);
    menu_bar->setStyleSheet(menu_bar_style);

    return menu_bar;
}

void TitleBar::generate_title_buttons(QWidget *parent) {
    m_min_btn = new QToolButton(this);
    m_min_btn->setObjectName("minimize_button");
    m_min_btn->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
    connect(m_min_btn, &QToolButton::clicked, parent, &QWidget::showMinimized);

    m_max_btn = new QToolButton(this);
    m_max_btn->setObjectName("maximize_button");
    m_max_btn->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    connect(m_max_btn, &QToolButton::clicked, [this, parent]() {
        QWidget* w = qobject_cast<QWidget*>(parent);
        w->isMaximized() ? w->showNormal() : w->showMaximized();
    });

    m_close_btn = new QToolButton(this);
    m_close_btn->setObjectName("close_button");
    m_close_btn->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    connect(m_close_btn, &QToolButton::clicked, parent, &QWidget::close);
}
