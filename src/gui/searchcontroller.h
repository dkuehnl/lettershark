#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>

#include "filemanager.h"

/**
 * @brief Drives text search and match highlighting across the open log
 *        tabs, either globally (all tabs) or within a single selected file,
 *        and exports the highlighted results to the Downloads folder.
 *
 * Highlighting is stored directly as background/foreground colors on the
 * affected QTableWidgetItems; per-file match counts are tracked in
 * m_results, keyed by tab/file name.
 */
class SearchController
{
public:
    /// @brief Per-file search bookkeeping: the file's table widget and its accumulated global/specific match counts.
    struct SearchResult {
        QTableWidget* table;
        QString filename;
        unsigned int glob_matches;
        unsigned int spec_matches;
    };

    /**
     * @brief Constructs the controller.
     * @param tabs The tab widget hosting all open log tables. Not owned.
     * @param file_manager Used to resolve a display filename to its tab page for a specific-file search. Not owned.
     */
    SearchController(QTabWidget* tabs, FileManager* file_manager);

    /**
     * @brief Runs a search, either across all open tabs or within one file selected via combo.
     * @param global_search If true, search every open tab; otherwise search only the file selected in combo.
     * @param search_text Text to match (Qt::MatchContains semantics).
     * @param combo Combo box holding the specific-file selection; only read when global_search is false.
     * @return A human-readable result summary, or an "Error: ..." message if global_search is false and no valid file is selected.
     */
    QString search(bool global_search, const QString& search_text, QComboBox* combo);

    /// @brief Highlights all matches of search_text in table with color, without touching match-count bookkeeping (used by the Ctrl+F-style shortcut search).
    void shortcut_search(QTableWidget* table, const QString& search_text, const QColor& color);

    /// @brief Clears all match highlighting (including user-triggered shortcut highlights) across every open tab and resets tracked results.
    void reset_search_controller();

    /// @brief Clears tracked per-file results without touching any highlighting already applied to the tables.
    void clear();

    /// @brief Writes all tracked global-search matches across all files to "global_search_export.txt" in the user's Downloads folder.
    /// @return true on success, false if the export file could not be opened for writing.
    bool export_global_results();

    /// @brief Writes tracked matches for a single file to "<file>.txt" in the user's Downloads folder.
    /// @param file Display filename identifying the tracked SearchResult to export.
    /// @return true on success, false if the export file could not be opened for writing.
    bool export_specific_results(const QString file);

private:
    /// @brief Searches every open tab's table for search_text, applying green highlighting and recording glob_matches per file.
    QString global_search(const QString& search_text);

    /// @brief Searches a single tab page's table for search_text, applying orange highlighting and accumulating spec_matches for filename.
    QString specific_search(QWidget* page, const QString& search_text, const QString& filename);

    /// @brief Clears highlighting from table; when user_triggered is false, only clears the orange specific-search highlight color (green global-search highlights are left in place).
    void reset_background(QTableWidget* table, bool user_triggered);

    /// @brief Applies color as background (and a fixed dark foreground) to every cell in the rows of items.
    void set_background(QTableWidget* table, QList<QTableWidgetItem*> items, const QBrush& color);

    QTabWidget* m_tabs;
    FileManager* m_file_manager;
    QMap<QString, SearchResult> m_results;
    unsigned int m_total_matches = 0;
    bool m_global_search;
};

#endif // SEARCHCONTROLLER_H
