#include "mainwindowuisetup.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "titlebar.h"

MainWindowUISetup::MainWindowUISetup() {}

 void MainWindowUISetup::apply_ui(MainWindow* w, Ui::MainWindow* ui, TitleBar* title_bar) {
     w->setWindowFlags(w->windowFlags() | Qt::FramelessWindowHint);

    //Layout:
    ui->cb_search_column->setEnabled(false);
    auto central = ui->centralwidget;
    auto main_layout = new QVBoxLayout(central);
    main_layout->setContentsMargins(0,0,0,0);

    main_layout->addWidget(title_bar);
    main_layout->addWidget(ui->tabWidget, 1);
    central->setLayout(main_layout);

    ui->tabWidget->setTabsClosable(true);
    QTabBar* bar = ui->tabWidget->tabBar();
    QWidget* close_btn = bar->tabButton(0,QTabBar::RightSide);
    if (close_btn) {
        close_btn->hide();
        bar->setTabButton(0, QTabBar::RightSide, nullptr);
    }
    ui->tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->lbl_search_results->setWordWrap(true);

    //TreeWidget:
    ui->tw_opened_files->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tw_opened_files->setExpandsOnDoubleClick(false);

    //Initialize Labels:
    ui->lbl_parsed_files->setText("0");
    ui->lbl_parsed_lines->setText("0");
    ui->lbl_parsing_status->setText("");
    ui->lbl_search_results->setText("");
}

 void MainWindowUISetup::connect_slots(MainWindow* w, Ui::MainWindow* ui, TitleBar* title_bar, std::unique_ptr<FileManager>& file_manager) {
     QObject::connect(title_bar, &TitleBar::open_file_requested, file_manager.get(), &FileManager::open_file);
     QObject::connect(title_bar, &TitleBar::open_folder_requested, file_manager.get(), &FileManager::open_folder);
     QObject::connect(title_bar, &TitleBar::open_archive_requested, file_manager.get(), &FileManager::open_archive);
     QObject::connect(title_bar, &TitleBar::clear_space_requested, w, &MainWindow::clear_space);
     QObject::connect(title_bar, &TitleBar::export_global_search_requested, w, &MainWindow::export_global);
     QObject::connect(title_bar, &TitleBar::export_specific_search_requested, w, &MainWindow::export_specific);
 }
