#include "searchcontroller.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextSTream>

SearchController::SearchController(QTabWidget* tabs, FileManager* file_manager)
    : m_tabs(tabs), m_file_manager(file_manager) {}

QString SearchController::search(bool global_search, const QString& search_text, QComboBox* combo) {
    m_global_search = global_search;
    if (m_global_search) {
        return SearchController::global_search(search_text);
    } else {
        if (combo->currentIndex() < 0) {
            return "Error: Please select a specific file to search in.";
        }
        auto file_entry = m_file_manager->get_entry(combo->currentText());
        if (file_entry) {
            FileTabInfo& entry = file_entry->get();

            return SearchController::specific_search(entry.page, search_text, entry.filename);
        } else {
            return "Error: no page found";
        }
    }
}

QString SearchController::global_search(const QString& search_text) {
    SearchController::reset_search_controller();
    QString result;

    int tab_count = m_tabs->count();
    for (int tab = 0; tab < tab_count; tab++) {
        QWidget* tab_page = m_tabs->widget(tab);
        QTableWidget* table = tab_page->findChild<QTableWidget*>();
        if (!table) {
            continue;
        }
        auto items = table->findItems(search_text, Qt::MatchContains);
        m_total_matches += items.size();
        unsigned int file_matches = items.size();

        int index = m_tabs->indexOf(tab_page);
        QString tab_name = m_tabs->tabText(index);
        QString result_text;

        result_text += "    File: " + tab_name + "\n";
        if (file_matches <= 0) {
            result_text += "        no Matches\n";
        } else {
            result_text += "        Matches: " + QString::number(file_matches) + "\n";
            QTableWidgetItem* first = items.first();
            int first_row = first->row();
            result_text += "        First Match in: Line " + QString::number(first_row+1) + "\n\n";
        }

        SearchController::reset_background(table, false);
        SearchController::set_background(table, items, QColor("#4eda71"));

        result += result_text;
        SearchResult results = {table, tab_name, file_matches};
        m_results.insert(tab_name, results);
    }

    return result;

}

QString SearchController::specific_search(QWidget* page, const QString& search_text, const QString& filename) {
    SearchResult& entry = m_results[filename];

    QString result;

    QTableWidget* table = page->findChild<QTableWidget*>();
    if (!table) {
        result = "Something went wrong, no Table found\n";
        return result;
    }
    auto items = table->findItems(search_text, Qt::MatchContains);
    entry.spec_matches += items.size();

    SearchController::reset_background(table, false);
    SearchController::set_background(table, items, QColor("#e3a563"));
    result = "Searched in File:\t\t" + filename + "\n\tMatches:\t" + QString::number(entry.spec_matches);

    return result;
}

void SearchController::set_background(QTableWidget* table, QList<QTableWidgetItem*> items, const QBrush& color) {
    for (auto* cell : items) {
        int row = cell->row();
        int col_count = table->columnCount();
        for (int col = 0; col < col_count; col++) {
            QTableWidgetItem* item = table->item(row,col);
            item->setBackground(color);
            item->setForeground(QBrush(QColor("#1E1E1E")));
        }
    }
}

void SearchController::reset_background(QTableWidget* table, bool user_triggered) {
    for (int row = 0; row < table->rowCount(); row++) {
        for (int col = 0; col < table->columnCount(); col++) {
            QTableWidgetItem* item = table->item(row,col);
            if (item) {
                if (user_triggered) {
                    item->setBackground(QBrush());
                    item->setForeground(QBrush(QColor("#d4d4d4")));
                } else {
                    if (item->background() == QColor("#e3a563")) {
                        item->setBackground(QBrush());
                        item->setForeground(QBrush(QColor("#d4d4d4")));
                    }
                }
            }
        }
    }
}

void SearchController::reset_search_controller() {
    int tab_count = m_tabs->count();
    for (int tab = 0; tab < tab_count; tab++) {
        QWidget* tab_page = m_tabs->widget(tab);
        QTableWidget* table = tab_page->findChild<QTableWidget*>();
        if (!table) {
            continue;
        }
        SearchController::reset_background(table, true);
    }

    m_results.clear();
}

void SearchController::clear() {
    m_results.clear();
}

bool SearchController::export_global_results() {
    QString download_folder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString export_path = QDir(download_folder).filePath("global_search_export.txt");

    QFile file(export_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    for (const auto& result : m_results) {
        out << "File: " << result.filename << "\t\t" << "Matches: " << result.glob_matches+result.spec_matches << "\n";
        for (int row = 0; row < result.table->rowCount(); row++) {
            QTableWidgetItem* item = result.table->item(row, 0);
            if (item && item->background().color() != QColor("#d4d4d4")) {
                for (int col = 0; col < result.table->columnCount(); col++) {
                    out << result.table->item(row,col)->text() << "\t";
                }
                out << "\n";
            }
        }
        out << "\n\n";
    }

    return true;
}

bool SearchController::export_specific_results(const QString input_file) {
    QString download_folder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString export_filename = input_file + ".txt";
    QString export_path = QDir(download_folder).filePath(export_filename);

    QFile file(export_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);

    SearchResult& entry = m_results[input_file];
    out << "File: " << entry.filename << "\t\t" << "Matches: " << entry.glob_matches + entry.spec_matches << "\n";
    for (int row = 0; row<entry.table->rowCount(); row++) {
        QTableWidgetItem* item = entry.table->item(row,0);
        if (item && item->background().color() != QColor("#d4d4d4")) {
            for (int col = 0; col < entry.table->columnCount(); col++) {
                out << entry.table->item(row,col)->text() << "\t";
            }
            out << "\n";
        }
    }

    return true;
}

