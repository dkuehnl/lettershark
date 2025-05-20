#ifndef MAINWINDOWUISETUP_H
#define MAINWINDOWUISETUP_H

#include "mainwindow.h"


class MainWindowUISetup
{
public:
    MainWindowUISetup();
    static void apply_ui(MainWindow* w, Ui::MainWindow* ui, TitleBar* title_bar);
    static void connect_slots(MainWindow* w, Ui::MainWindow* ui, TitleBar* title_bar, std::unique_ptr<FileManager>& file_manager);

};

#endif // MAINWINDOWUISETUP_H
