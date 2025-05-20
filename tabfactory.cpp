#include "tabfactory.h"
#include <qboxlayout.h>
#include <QTableWidget>
#include <qheaderview.h>
#include <qmenu.h>

TabFactory::TabInfo TabFactory::create_tab(const QString& title, QComboBox* combo, QTabWidget* tabs) {
    QWidget* tab_page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab_page);
    layout->setContentsMargins(0,0,0,0);
    tab_page->setLayout(layout);

    QTableWidget* table = new QTableWidget(tab_page);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(table, 1);

    tabs->addTab(tab_page, title);
    combo->addItem(title);

    TabFactory::setup_default_headers(table);
    TabFactory::apply_default_table_style(table);
    return {tab_page, table};
}

void TabFactory::re_add_existing_tag(QWidget* page, QComboBox* combo, QTabWidget* tab, const QString& title) {
    QWidget* existing_page = page;
    tab->insertTab(tab->count()+1, existing_page, title);
    combo->addItem(title);
}

void TabFactory::remove_tab(QTabWidget* tabs, int index, QComboBox* combo, const QString& title) {
    //Remove Tab
    QWidget* page = tabs->widget(index);
    if (!page) return;
    tabs->removeTab(index);

    //Remove Item from ComboBox
    int cb_index = combo->findText(title);
    combo->removeItem(cb_index);
}

void TabFactory::fill_table(QTableWidget* table, std::vector<TableRow> data) {
    table->setRowCount(static_cast<int>(data.size()));
    for (int row = 0; row < static_cast<int>(data.size()); ++row) {
        const auto& row_data = data[row];
        for (int col = 0; col < static_cast<int>(row_data.cols.size()); ++col) {
            auto text = QString::fromStdString(row_data.cols[col]);
            auto item = new QTableWidgetItem(text);
            table->setItem(row,col,item);
        }
    }
    table->resizeColumnsToContents();
}

void TabFactory::apply_default_table_style(QTableWidget* table) {
    QString tableStyle = R"(
        QTableWidget {
            background-color: #1E1E1E;
            color: #D4D4D4;
            gridline-color: #3C3C3C;
            selection-background-color: #094771;
            selection-color: #FFFFFF;
            alternate-background-color: #252526;
        }

        QHeaderView::section {
            background-color: #2D2D30;
            color: #D4D4D4;
            padding: 4px;
            border: 1px solid #3C3C3C;
            font-weight: bold;
        }

        QTableCornerButton::section {
            background-color: #2D2D30;
            border: 1px solid #3C3C3C;
        }

        QTableWidget::item:selected {
            /* Wenn die Zelle aktuell ausgewählt und die Tabelle aktiv ist */
            background-color: #094771;  /* z.B. dunkles Blau wie bei deinem Default */
            color: #FFFFFF;             /* Text in Weiß */
        }

        QTableWidget::item:selected:!active {
            /* Wenn die Zelle ausgewählt, die Tabelle aber gerade nicht im Fokus ist */
            background-color: #094771;
            color: #CCCCCC;             /* etwas dezenterer Text, z.B. Hellgrau */
        }
    )";

    table->setStyleSheet(tableStyle);
}

void TabFactory::setup_default_headers(QTableWidget* table) {
    table->setColumnCount(8);
    table->setHorizontalHeaderLabels(QStringList() << "Timestamp" << "Log-Level" << "Tag" << "PID" << "ID" << "Module" << "Function" << "Message");
    QFont header_font = table->horizontalHeader()->font();
    header_font.setBold(true);
    table->horizontalHeader()->setFont(header_font);
}
