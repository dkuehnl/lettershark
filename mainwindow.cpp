#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <qheaderview.h>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>

#include "parser.h"
#include "parser_exceptions.h"
#include "titlebar.h"
#include "mainwindowuisetup.h"
#include "tabfactory.h"
#include "windowdragger.h"
#include "ramwatcher.h"
#include "customdialog.h"

namespace {
constexpr size_t RAM_AVAIL_FILE = 500 * 1024 * 1024;
constexpr size_t RAM_AVAIL_FOLDER = 1024 * 1024 * 1024;
constexpr std::uint64_t RAM_FOR_GLOB_SEARCH = 1024ull * 1024 * 1024 * 4;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    new WindowDragger(this);

    m_title_bar = new TitleBar(this);
    m_file_manager = std::make_unique<FileManager>(this);
    m_search_controller = new SearchController(ui->tabWidget, m_file_manager.get());
    m_context_handler = std::make_unique<ContextMenuHandler>(ui->tw_opened_files);

    MainWindowUISetup::apply_ui(this, ui, m_title_bar);
    MainWindowUISetup::connect_slots(this, ui, m_title_bar, m_file_manager);

    connect(m_file_manager.get(), &FileManager::file_added, this, &MainWindow::add_tree_item_file);
    connect(m_file_manager.get(), &FileManager::folder_scanned, this, &MainWindow::add_tree_item_folder);
    connect(m_context_handler.get(), &ContextMenuHandler::parse_file_requested, this, &MainWindow::add_new_tab);
    connect(m_context_handler.get(), &ContextMenuHandler::parse_folder_requested, this, &MainWindow::parse_folder);
    connect(m_context_handler.get(), &ContextMenuHandler::unload_requested, this, &MainWindow::unload);

    QShortcut *find_shortcut = new QShortcut(QKeySequence::Find, this);
    QShortcut *next_shortcut = new QShortcut(QKeySequence::FindNext, this);
    QShortcut *prev_shortcut = new QShortcut(QKeySequence::FindPrevious, this);
    QShortcut *reset_shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this);
    connect(find_shortcut, &QShortcut::activated, this, &MainWindow::open_search_window);
    connect(next_shortcut, &QShortcut::activated, this, &MainWindow::open_next_result);
    connect(prev_shortcut, &QShortcut::activated, this, &MainWindow::open_prev_result);
    connect(reset_shortcut, &QShortcut::activated, this, &MainWindow::reset_shortcut_search);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, [this](int index){
                Q_UNUSED(index);
                m_last_row = -1;
            });
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);

    ui->lbl_logo->move(1, this->height() - ui->lbl_logo->height() - 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::add_tree_item_file(const FileTabInfo& info) {
    QTreeWidgetItem* root = new QTreeWidgetItem(ui->tw_opened_files);
    root->setText(0, info.filename);
    ui->tw_opened_files->addTopLevelItem(root);
}

void MainWindow::add_tree_item_folder(const QVector<FileTabInfo>& infos, QString root_folder) {

    auto root_items = ui->tw_opened_files->findItems(root_folder, Qt::MatchExactly);
    QTreeWidgetItem* folder_root = nullptr;

    if (root_items.isEmpty()) {
        folder_root = new QTreeWidgetItem(ui->tw_opened_files);
        folder_root->setText(0, root_folder);
        ui->tw_opened_files->addTopLevelItem(folder_root);
    } else {
        folder_root = root_items.first();
    }

    for (const auto& info : infos) {
        auto items = ui->tw_opened_files->findItems(info.root_folder, Qt::MatchRecursive);

        if (items.empty()) {
            QTreeWidgetItem* root = new QTreeWidgetItem(folder_root);
            root->setText(0, info.root_folder);

            QTreeWidgetItem* child = new QTreeWidgetItem(root);
            child->setText(0, info.filename);

            ui->tw_opened_files->addTopLevelItem(root);
        } else {
            QTreeWidgetItem* first_item = items.first();
            QTreeWidgetItem* child = new QTreeWidgetItem(first_item);
            child->setText(0, info.filename);
            first_item->addChild(child);
        }
    }
}

void MainWindow::clear_space() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
                this,
                "Confirm Reset",
                "Warning! If you clear the space all files will be unloaded and all Search-Results will be deleted.",
        QMessageBox::Yes | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
        return;
    }

    m_global_search = true;
    m_parsed_files = 0;
    m_parsed_lines = 0;
    ui->lbl_parsed_files->setText("0");
    ui->lbl_parsed_lines->setText("0");
    ui->lbl_parsing_status->setText("");
    ui->lbl_search_results->setText("");

    ui->chkb_global_search->setCheckState(Qt::Unchecked);
    ui->cb_search_column->clear();
    ui->le_search_phrase->clear();
    ui->tw_opened_files->clear();

    m_search_controller->clear();
    m_file_manager->clear();

    size_t tab_count = ui->tabWidget->count();
    for (size_t i = tab_count-1; i > 0; i--) {
        QWidget* page = ui->tabWidget->widget(i);
        ui->tabWidget->removeTab(i);
        if (page) page->deleteLater();
    }
}

void MainWindow::unload(QString filename) {
    auto file_entry = m_file_manager->get_entry(filename);
    if (file_entry) {
        FileTabInfo file = file_entry->get();
        MainWindow::delete_file(file);
    } else {
        const QList<QString> filelist = m_file_manager->find_items_to_root(filename);
        if (filelist.size() > 0) {
            for (const auto& item : filelist) {
                auto file_entry = m_file_manager->get_entry(item);
                if (!file_entry) {
                    return;
                }
                FileTabInfo file = file_entry->get();
                MainWindow::delete_file(file);
            }
        }
        const auto root_tree = ui->tw_opened_files->findItems(filename, Qt::MatchRecursive);
        for (QTreeWidgetItem* root_item : root_tree) {
            delete root_item;
        }
    }


}

void MainWindow::delete_file(FileTabInfo file) {
    //TreeWidget entfernen
    const auto tree_items = ui->tw_opened_files->findItems(file.filename, Qt::MatchRecursive);
    for (QTreeWidgetItem* item : tree_items) {
        delete item;
    }
    //SearchResults entfernen

    //Tab und Table entfernen
    size_t tab_index = ui->tabWidget->indexOf(file.page);
    if (tab_index < 0) {
        return;
    }
    QWidget* page = ui->tabWidget->widget(tab_index);
    ui->tabWidget->removeTab(tab_index);
    if (page) page->deleteLater();

    //Filetab bereinigen
    if (m_file_manager->delete_entry(file.filename) == 0) {
        return;
    }
}

void MainWindow::on_tw_opened_files_itemDoubleClicked(QTreeWidgetItem* item) {
    QString file = item->text(0);
    if (item->childCount() == 0) {
        MainWindow::add_new_tab(file, false);
    } else {
        bool is_expanded = item->isExpanded();
        item->setExpanded(!is_expanded);
    }

}

void MainWindow::on_tabWidget_tabCloseRequested(int index) {
    if (index == 0) return;
    auto file_entry = m_file_manager->get_entry(ui->tabWidget->tabText(index));
    if (file_entry) {
        FileTabInfo& entry = file_entry->get();
        TabFactory::remove_tab(ui->tabWidget, index, ui->cb_search_column, entry.filename);
    }
}

void MainWindow::on_btn_search_clicked() {
    ui->lbl_search_results->setText("");
    if (m_global_search && ui->tabWidget->count() >= 5) {
        if (!RamWatcher::enoug_memory_available(RAM_FOR_GLOB_SEARCH)) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::warning(
                this,
                "Attention!",
                "Global-Search needs much RAM and you're propaly running out of RAM (<4GB). Are you sure you want to proceed?",
                QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

            if (reply == QMessageBox::Cancel) {
                return;
            }
        }
    }
    QString result = m_search_controller->search(m_global_search, ui->le_search_phrase->text(), ui->cb_search_column);
    ui->lbl_search_results->setText(result);
}

void MainWindow::on_btn_reset_clicked() {
    m_search_controller->reset_search_controller();
    ui->lbl_search_results->setText("");
    ui->le_search_phrase->clear();
}


void MainWindow::on_chkb_global_search_checkStateChanged(Qt::CheckState state) {
    if (state == 0) {
        ui->cb_search_column->setEnabled(false);
        m_global_search = true;
    } else if (state >= 1) {
        ui->cb_search_column->setEnabled(true);
        m_global_search = false;
    }
}

void MainWindow::add_new_tab(QString filename, bool is_folder) {
    size_t ram = is_folder ? RAM_AVAIL_FOLDER : RAM_AVAIL_FILE;

    if (!RamWatcher::enoug_memory_available(ram)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(
            this,
            "Attention!",
            "You're probably running out of RAM if you continue the parsing-process. Are you sure you want to proceed?",
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

        if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    auto file_entry = m_file_manager->get_entry(filename);
    if (file_entry) {
        FileTabInfo& entry = file_entry->get();

        if (entry.page != nullptr) {
            TabFactory::re_add_existing_tag(entry.page, ui->cb_search_column, ui->tabWidget, entry.filename);
            return;
        }

        try {
            Parser parser(entry.filepath);
            m_parsed_files++;
            m_parsed_lines += parser.get_parsed_lines();
            ui->lbl_parsed_lines->setText(QString::number(m_parsed_lines));
            ui->lbl_parsed_files->setText(QString::number(m_parsed_files));

            auto page = TabFactory::create_tab(filename, ui->cb_search_column, ui->tabWidget);
            entry.page = page.container;

            ui->lbl_parsing_status->setText("Parsing...");
            ui->lbl_parsing_status->setText("Building Table...");
            std::vector<TableRow> data = parser.get_parsed_data();
            TabFactory::fill_table(page.table, data);

            QTableWidget* table = page.table;
            table->setContextMenuPolicy(Qt::CustomContextMenu);
            QObject::connect(table, &QTableWidget::customContextMenuRequested, this, &MainWindow::show_context_menu);

            ui->lbl_parsing_status->setText("Finished");
        }
        catch (const unspupported_format_exception& ex) {
            QMessageBox::warning(this, "Wrong format", "Unspported type. Only .log, .txt or .gz-Files are supported");
            return;
        }
        catch (const unable_to_read_file& ex) {
            QMessageBox::warning(this, "Unable to read", "Something went wrong while opening the file");
            return;
        }
        catch (const std::exception& ex) {
            QMessageBox::warning(this, "Something went wrong", QString("There is some issue while parsing the file:\n%1").arg(QString::fromStdString(ex.what())));
        }
    }
}

void MainWindow::show_context_menu(const QPoint& pos) {
    QTableWidget* table = qobject_cast<QTableWidget*>(sender());
    if (!table) return;

    QMenu menu(table);
    int cols = table->columnCount();

    for (int c = 0; c < cols; c++) {
        QString header = table->horizontalHeaderItem(c)->text();
        QAction* act = menu.addAction(header);
        act->setCheckable(true);
        act->setChecked(!table->isColumnHidden(c));

        QObject::connect(act, &QAction::toggled, [table, c](bool checked) {table->setColumnHidden(c, !checked);});
    }

    menu.addSeparator();
    QAction* show_all = menu.addAction("Show all");
    QObject::connect(show_all, &QAction::triggered, [table]() {
        for (int c = 0; c < table->columnCount(); c++) table->setColumnHidden(c, false);
    });

    QPoint global_pos = table->viewport()->mapToGlobal(pos);
    menu.exec(global_pos);
}

void MainWindow::parse_folder(QString folder) {
    const auto items = ui->tw_opened_files->findItems(folder, Qt::MatchRecursive);
    for (QTreeWidgetItem* root : items) {
        for (size_t i = 0; i < root->childCount(); i++) {
            QTreeWidgetItem* child = root->child(i);
            MainWindow::add_new_tab(child->text(0), true);
        }
    }
}

void MainWindow::export_global() {
    if (m_search_controller->export_global_results()) {
        ui->lbl_search_results->setText("Global export finished. You can find the global_search_export.txt in your Downloads-Folder");
    } else {
        ui->lbl_search_results->setText("Something went wrong with the export... :-(");
    }

}

void MainWindow::export_specific() {
    QStringList options;
    for (size_t i = 0; i < ui->cb_search_column->count(); i++) {
        QString tmp = ui->cb_search_column->itemText(i);
        options.push_back(tmp);
    }

    CustomDialog dialog(options, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString selected = dialog.selected_option();
        if (m_search_controller->export_specific_results(selected)) {
            ui->lbl_search_results->setText("File-Match export finished. You can find it in your Downloads-Folder");
        } else {
            ui->lbl_search_results->setText("Something went wrong with the export... :-(");
        }
    };
}

void MainWindow::open_search_window() {
    int index = ui->tabWidget->currentIndex();
    QWidget* tab = ui->tabWidget->widget(index);
    QTableWidget* table = tab->findChild<QTableWidget*>();
    if (!table) return;

    QString search_phrase;
    CustomDialog dialog("Enter Searchphrase: ", this);
    if (dialog.exec() == QDialog::Accepted) {
        search_phrase = dialog.selected_option();
    }

    if (search_phrase.isEmpty()) return;
    m_search_controller->shortcut_search(table, search_phrase);

}

void MainWindow::open_next_result() {
    int index = ui->tabWidget->currentIndex();

    QWidget* tab = ui->tabWidget->widget(index);
    QTableWidget* table = tab->findChild<QTableWidget*>();
    if (!table) return;

    table->setFocus();
    int row_count = table->rowCount();
    for (int r = m_last_row + 1; r < row_count; r++) {
        QTableWidgetItem* item = table->item(r,0);
        if (item && (item->background() != Qt::NoBrush)) {
            table->setCurrentItem(item);
            table->scrollToItem(item, QAbstractItemView::PositionAtCenter);
            m_last_row = r;
            break;
        }
    }
}
void MainWindow::open_prev_result() {
    int index = ui->tabWidget->currentIndex();

    QWidget* tab = ui->tabWidget->widget(index);
    QTableWidget* table = tab->findChild<QTableWidget*>();
    if (!table) return;

    table->setFocus();
    for (int r = m_last_row - 1; r >= 0; r--) {
        QTableWidgetItem* item = table->item(r,0);
        if (item && (item->background() != Qt::NoBrush)) {
            table->setCurrentItem(item);
            table->scrollToItem(item, QAbstractItemView::PositionAtCenter);
            m_last_row = r;
            break;
        }
    }
}

void MainWindow::reset_shortcut_search() {
    int index = ui->tabWidget->currentIndex();
    QWidget* tab = ui->tabWidget->widget(index);
    QTableWidget* table = tab->findChild<QTableWidget*>();
    if (!table) return;

    m_search_controller->reset_search_controller();
}
