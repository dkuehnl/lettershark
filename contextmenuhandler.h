#ifndef CONTEXTMENUHANDLER_H
#define CONTEXTMENUHANDLER_H

#include <QObject>
#include <QPoint>
#include <QTreeWidget>

class ContextMenuHandler : public QObject {
    Q_OBJECT;

public:
    explicit ContextMenuHandler(QTreeWidget* tree);

signals:
    void parse_file_requested(const QString& filename, bool is_folder);
    void parse_folder_requested(const QString& folder, bool is_folder);
    void unload_requested(const QString& name);

private slots:
    void show_menu(const QPoint& pos);

private:
    QTreeWidget* m_tree;

};

#endif // CONTEXTMENUHANDLER_H
