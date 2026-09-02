#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlistwidget.h>
#include <qtablewidget.h>
#include <qtreewidget.h>

#include "filemanager.h"
#include "searchcontroller.h"
#include "contextmenuhandler.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class TitleBar;

/**
 * @brief Application main window: hosts the opened-files tree, the per-file
 *        tab/table views, and the search bar; wires together FileManager,
 *        SearchController, ContextMenuHandler, TabFactory and the custom
 *        frameless-window chrome (TitleBar, WindowDragger).
 *
 * @note m_search_controller is a raw, non-QObject pointer that is never
 *       deleted (the destructor only deletes ui); it currently leaks for
 *       the lifetime of the process (freed only on application exit).
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// @brief Constructs the window and wires up FileManager, SearchController, ContextMenuHandler, TitleBar and WindowDragger.
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /// @brief Confirms with the user, then unloads all files, clears all search results, and removes every tab except the first.
    void clear_space();

    /// @brief Exports all accumulated global-search matches via SearchController and shows a status message.
    void export_global();

    /// @brief Prompts the user to pick a specific open file, then exports that file's accumulated search matches.
    void export_specific();

private:
    Ui::MainWindow *ui;
    TitleBar *m_title_bar;
    std::unique_ptr<FileManager> m_file_manager;
    // TODO: never deleted (SearchController is not a QObject, and ~MainWindow() only
    //       deletes ui) -- leaks for the process lifetime. Delete in ~MainWindow(), or
    //       switch to std::unique_ptr like m_file_manager.
    SearchController* m_search_controller;
    std::unique_ptr<ContextMenuHandler> m_context_handler;
    int m_last_row = -1;

    unsigned int m_parsed_files = 0;
    unsigned int m_parsed_lines = 0;
    bool m_global_search = true;

    /// @brief Opens (or re-attaches an already-open) tab for filename, parsing it first if needed; warns if available RAM looks insufficient.
    void add_new_tab(QString filename, bool is_folder);

    /// @brief Opens tabs for every file tracked under a folder/archive grouping (see FileManager::find_items_to_root()).
    void parse_folder(QString folder);

    /// @brief Removes filename from the tree/tabs/FileManager; if filename is a folder grouping, unloads all files under it instead.
    void unload(QString filename);

    /// @brief Removes a single tracked file's tree item, tab, and FileManager entry.
    void delete_file(FileTabInfo file);

    /// @brief Shows the CustomDialog for entering a shortcut-search phrase/color and applies it to the current tab (bound to Ctrl+F).
    void open_search_window();

    /// @brief Selects the next highlighted row after m_last_row in the current tab's table (bound to Find-Next).
    void open_next_result();

    /// @brief Selects the previous highlighted row before m_last_row in the current tab's table (bound to Find-Previous).
    void open_prev_result();

    /// @brief Clears shortcut-search highlighting in the current tab (bound to Ctrl+R).
    void reset_shortcut_search();

private slots:
    /// @brief Runs a search (global or specific, per m_global_search) for the entered phrase and shows the result summary.
    void on_btn_search_clicked();

    /// @brief Clears the current search results and the search phrase field.
    void on_btn_reset_clicked();

    /// @brief Opens a tab for a double-clicked leaf tree item, or toggles expansion for a group item.
    void on_tw_opened_files_itemDoubleClicked(QTreeWidgetItem* item);

    /// @brief Removes the closed tab from the UI and its FileManager entry, unless it is the first (fixed) tab.
    void on_tabWidget_tabCloseRequested(int index);

    /// @brief Toggles between global and specific search mode based on the checkbox state, enabling/disabling the file-selection combo accordingly.
    void on_chkb_global_search_checkStateChanged(Qt::CheckState state);

    /// @brief Adds a leaf tree item for a single newly opened file (connected to FileManager::file_added).
    void add_tree_item_file(const FileTabInfo& info);

    /// @brief Adds/merges a grouped subtree for a scanned folder/archive's discovered files (connected to FileManager::folder_scanned).
    void add_tree_item_folder(const QVector<FileTabInfo>& infos, QString root_folder);

    /// @brief Shows a per-column visibility toggle menu for the table at pos (right-click on a table header).
    void show_context_menu(const QPoint& pos);


protected:
    /// @brief Repositions the logo label once the window is first shown (depends on the final window height).
    void showEvent(QShowEvent* event) override;

};
#endif // MAINWINDOW_H
