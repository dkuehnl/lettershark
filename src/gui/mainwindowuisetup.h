#ifndef MAINWINDOWUISETUP_H
#define MAINWINDOWUISETUP_H

#include "mainwindow.h"

/**
 * @brief Post-construction UI wiring for MainWindow, factored out of the
 *        constructor: layout assembly/tweaks not expressible in the .ui
 *        file, and signal/slot connections between the custom title bar
 *        and the window's own components.
 *
 * Stateless; both members are static and operate purely on the objects
 * passed in.
 */
class MainWindowUISetup
{
public:
    MainWindowUISetup();

    /**
     * @brief Applies frameless-window flags and finishes assembling the layout (title bar, tab widget) and initial widget state.
     * @param w The window being set up.
     * @param ui The window's generated Ui::MainWindow instance.
     * @param title_bar The custom title bar widget to insert above the tab widget.
     */
    static void apply_ui(MainWindow* w, Ui::MainWindow* ui, TitleBar* title_bar);

    /**
     * @brief Connects the title bar's action signals (open file/folder/archive, clear space, export) to FileManager and MainWindow.
     * @param w The window whose slots (clear_space/export_global/export_specific) are connected.
     * @param ui The window's generated Ui::MainWindow instance.
     * @param title_bar The custom title bar whose signals are connected.
     * @param file_manager The window's FileManager instance, connected to the open-file/folder/archive signals.
     */
    static void connect_slots(MainWindow* w, Ui::MainWindow* ui, TitleBar* title_bar, std::unique_ptr<FileManager>& file_manager);
};

#endif // MAINWINDOWUISETUP_H
