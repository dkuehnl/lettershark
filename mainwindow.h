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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void clear_space();
    void export_global();
    void export_specific();

private:
    Ui::MainWindow *ui;
    TitleBar *m_title_bar;
    std::unique_ptr<FileManager> m_file_manager;
    SearchController* m_search_controller;
    std::unique_ptr<ContextMenuHandler> m_context_handler;
    int m_last_row = -1;

    unsigned int m_parsed_files = 0;
    unsigned int m_parsed_lines = 0;
    bool m_global_search = true;

    void add_new_tab(QString filename, bool is_folder);
    void parse_folder(QString folder);
    void unload(QString filename);
    void delete_file(FileTabInfo file);
    void open_search_window();
    void open_next_result();
    void open_prev_result();
    void reset_shortcut_search();

private slots:
    void on_btn_search_clicked();
    void on_btn_reset_clicked();
    void on_tw_opened_files_itemDoubleClicked(QTreeWidgetItem* item);
    void on_tabWidget_tabCloseRequested(int index);
    void on_chkb_global_search_checkStateChanged(Qt::CheckState state);

    void add_tree_item_file(const FileTabInfo& info);
    void add_tree_item_folder(const QVector<FileTabInfo>& infos, QString root_folder);

    void show_context_menu(const QPoint& pos);


protected:
    void showEvent(QShowEvent* event) override;

};
#endif // MAINWINDOW_H
