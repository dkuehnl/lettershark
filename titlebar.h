#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QToolButton>
#include <QMenuBar>

class TitleBar : public QWidget{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);

private:
    QToolButton *m_min_btn;
    QToolButton *m_max_btn;
    QToolButton *m_close_btn;
    QMenuBar *m_menu_bar = nullptr;

    QMenuBar* generate_menu_bar();
    void generate_title_buttons(QWidget *parent);

signals:
    void open_file_requested();
    void open_folder_requested();
    void open_archive_requested();
    void clear_space_requested();
    void export_global_search_requested();
    void export_specific_search_requested();
};

#endif // TITLEBAR_H
