#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>

#include "filemanager.h"

class SearchController
{
public:
    struct SearchResult {
        QTableWidget* table;
        QString filename;
        unsigned int glob_matches;
        unsigned int spec_matches;
    };

    SearchController(QTabWidget* tabs, FileManager* file_manager);
    QString search(bool global_search, const QString& search_text, QComboBox* combo);
    void reset_search_controller();
    void clear();
    bool export_global_results();
    bool export_specific_results(const QString file);

private:
    QString global_search(const QString& search_text);
    QString specific_search(QWidget* page, const QString& search_text, const QString& filename);
    void reset_background(QTableWidget* table, bool user_triggered);
    void set_background(QTableWidget* table, QList<QTableWidgetItem*> items, const QBrush& color);

    QTabWidget* m_tabs;
    FileManager* m_file_manager;
    QMap<QString, SearchResult> m_results;
    unsigned int m_total_matches = 0;
    bool m_global_search;
};

#endif // SEARCHCONTROLLER_H
