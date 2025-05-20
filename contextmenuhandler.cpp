#include "contextmenuhandler.h"

#include <QMenu>
#include <QAction>

namespace {
    static const QString menu_style = R"(
        QMenu {
            background-color: #2D2D30;
            border: 1px solid #3C3C3C;
            /*padding: 4px;*/
        }
        QMenu::item {
            color: #D4D4D4;
            background-color: transparent;
            /*padding: 5px 24px 5px 24px;  Abstand rundherum */
        }
        QMenu::item:selected {            /* Hover oder Tastatur-Fokus */
            background-color: #094771;    /* dunkles Blau */
            color: #FFFFFF;
        }
        QMenu::icon {                     /* Icon-Abstand, falls du welche hast */
            padding-left: 6px;
        }
        QMenu::separator {
            height: 1px;
            background: #3C3C3C;
            /*margin: 4px 0;*/
        }
    )";
}

ContextMenuHandler::ContextMenuHandler(QTreeWidget* tree)
    : QObject(tree), m_tree(tree)
{
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree, &QTreeWidget::customContextMenuRequested, this, &ContextMenuHandler::show_menu);
}

void ContextMenuHandler::show_menu(const QPoint& pos) {
    auto* item = m_tree->itemAt(pos);
    if (!item) return;

    QMenu menu;
    menu.setStyleSheet(menu_style);
    QAction* file = menu.addAction("Parse File");
    QAction* folder = menu.addAction("Parse Folder");
    QAction* unload = menu.addAction("Unload");
    QAction* chosen = menu.exec(m_tree->viewport()->mapToGlobal(pos));

    QString name = item->text(0);
    if (chosen == file) {
        emit parse_file_requested(name, false);
    } else if (chosen == folder) {
        emit parse_folder_requested(name, true);
    } else if (chosen == unload) {
        emit unload_requested(name);
    }
}
