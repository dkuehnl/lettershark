#ifndef CONTEXTMENUHANDLER_H
#define CONTEXTMENUHANDLER_H

#include <QObject>
#include <QPoint>
#include <QTreeWidget>

/**
 * @brief Adds a right-click context menu ("Parse File" / "Parse Folder" /
 *        "Unload") to the opened-files tree and translates the chosen
 *        action into a signal for MainWindow to act on.
 */
class ContextMenuHandler : public QObject {
    Q_OBJECT;

public:
    /**
     * @brief Installs a custom context menu policy on tree and connects to its customContextMenuRequested signal.
     * @param tree The tree widget (opened-files list) to attach the context menu to. Not owned.
     */
    explicit ContextMenuHandler(QTreeWidget* tree);

signals:
    /// @brief Emitted when "Parse File" is chosen for a single-file tree item.
    void parse_file_requested(const QString& filename, bool is_folder);

    /// @brief Emitted when "Parse Folder" is chosen for a folder/group tree item.
    void parse_folder_requested(const QString& folder, bool is_folder);

    /// @brief Emitted when "Unload" is chosen for a tree item.
    void unload_requested(const QString& name);

private slots:
    /// @brief Builds and executes the context menu for the tree item at pos, emitting the matching signal for the chosen action.
    void show_menu(const QPoint& pos);

private:
    QTreeWidget* m_tree;
};

#endif // CONTEXTMENUHANDLER_H
