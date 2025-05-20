#ifndef TABFACTORY_H
#define TABFACTORY_H

#include <qcombobox.h>
#include <qtablewidget.h>

#include "parser.h"

class TabFactory
{

struct TabInfo{
    QWidget* container;
    QTableWidget* table;
};

public:
    static TabInfo create_tab(const QString& title, QComboBox* combo, QTabWidget* tabs);
    static void fill_table(QTableWidget* table, std::vector<TableRow> data);
    static void re_add_existing_tag(QWidget* existing_page, QComboBox* combo, QTabWidget* tab, const QString& title);
    static void remove_tab(QTabWidget* tabs, int index, QComboBox* combo, const QString& title);

private:
    static void apply_default_table_style(QTableWidget* table);
    static void setup_default_headers(QTableWidget* table);
    static void show_context_menu(QTableWidget* table, const QPoint& pos);

};

#endif // TABFACTORY_H
