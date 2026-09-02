#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QToolButton>
#include <QMenuBar>

/**
 * @brief Custom title bar for the frameless main window: hosts the File/Export
 *        menu bar and the minimize/maximize/close buttons, and translates
 *        menu actions into signals for MainWindow to act on.
 *
 * Window dragging and the double-click-to-maximize behavior are handled
 * separately by WindowDragger, which is installed on the main window itself.
 */
class TitleBar : public QWidget{
    Q_OBJECT
public:
    /**
     * @brief Builds the menu bar and window-control buttons and lays them out in a horizontal bar.
     * @param parent The window this title bar controls (minimize/maximize/close act on parent).
     */
    explicit TitleBar(QWidget *parent = nullptr);

private:
    QToolButton *m_min_btn;
    QToolButton *m_max_btn;
    QToolButton *m_close_btn;
    QMenuBar *m_menu_bar = nullptr;

    /// @brief Builds the File menu (open file/folder/archive, clear space) and Export menu (specific/global search export).
    QMenuBar* generate_menu_bar();

    /// @brief Creates the minimize/maximize/close buttons and wires them to parent's window state (showMinimized/showNormal/showMaximized/close).
    void generate_title_buttons(QWidget *parent);

signals:
    /// @brief Emitted from the File menu's "Open File" action.
    void open_file_requested();
    /// @brief Emitted from the File menu's "Open Folder" action.
    void open_folder_requested();
    /// @brief Emitted from the File menu's "Open Archive" action.
    void open_archive_requested();
    /// @brief Emitted from the File menu's "Clear Space" action.
    void clear_space_requested();
    /// @brief Emitted from the Export menu's "Export global search" action.
    void export_global_search_requested();
    /// @brief Emitted from the Export menu's "Export specific search" action.
    void export_specific_search_requested();
};

#endif // TITLEBAR_H
