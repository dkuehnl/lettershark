#ifndef TABFACTORY_H
#define TABFACTORY_H

#include <qcombobox.h>
#include <qtablewidget.h>

#include "parser.h"

/**
 * @brief Builds and manages the per-file tab pages (table + container
 *        widget) shown in the main tab widget, and applies the dark table
 *        styling/column headers for both supported log formats.
 *
 * Stateless collection of static helpers; holds no data of its own.
 *
 * @note show_context_menu() is declared but not implemented/used anywhere
 *       in the current codebase (MainWindow has its own, unrelated
 *       show_context_menu member) and can likely be removed.
 */
class TabFactory
{
    /// @brief Result of create_tab(): the tab's container widget and the table inside it.
    struct TabInfo{
        QWidget* container;
        QTableWidget* table;
    };

public:
    /**
     * @brief Creates a new tab page containing a styled, empty table, adds it to tabs, and registers title in combo.
     * @param title Tab label and combo-box entry text.
     * @param combo Specific-file search combo box to append title to.
     * @param tabs Tab widget to add the new page to.
     * @param vnmf_mode If true, use the 9-column VNMF header layout; otherwise the 8-column default layout.
     * @return The created container widget and its (still empty) table. TabInfo is a private nested type; capture the result with auto.
     */
    static TabInfo create_tab(const QString& title, QComboBox* combo, QTabWidget* tabs, bool vnmf_mode = false);

    /// @brief Fills table with parsed rows (one QTableWidgetItem per column per row) and resizes columns to fit.
    static void fill_table(QTableWidget* table, std::vector<TableRow> data);

    /// @brief Re-inserts an already-built tab page (e.g. previously removed) at the end of tabs and re-adds title to combo.
    static void re_add_existing_tag(QWidget* existing_page, QComboBox* combo, QTabWidget* tab, const QString& title);

    /// @brief Removes the tab at index from tabs and removes the matching title entry from combo.
    static void remove_tab(QTabWidget* tabs, int index, QComboBox* combo, const QString& title);

private:
    /// @brief Applies the dark Qt style sheet used for all log tables.
    static void apply_default_table_style(QTableWidget* table);

    /// @brief Sets up the 8-column header layout for the default (tagged) log format.
    static void setup_default_headers(QTableWidget* table);

    // TODO: declared but never implemented/called anywhere -- implement or remove.
    /// @brief Not implemented; declared but currently unused.
    static void show_context_menu(QTableWidget* table, const QPoint& pos);

    /// @brief Sets up the 9-column header layout for the VNMF (pipe-delimited) log format.
    static void setup_vnmf_headers(QTableWidget* table);
};

#endif // TABFACTORY_H
