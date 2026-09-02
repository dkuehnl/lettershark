#ifndef WINDOWDRAGGER_H
#define WINDOWDRAGGER_H

#include <QObject>
#include <QPoint>

/**
 * @brief Makes a frameless window draggable and snap-to-maximize by
 *        installing an event filter on it, since a frameless QWidget has no
 *        native title bar to drag from.
 *
 * Behavior: left-press starts a drag (un-maximizing first if the window was
 * maximized, repositioning it under the cursor); left-move while dragging
 * moves the window; releasing at the top of the screen (y <= 0) maximizes
 * it, mirroring Windows' Aero Snap.
 */
class WindowDragger : public QObject {
    Q_OBJECT;

public:
    /**
     * @brief Installs this as an event filter on target_window.
     * @param target_window The frameless window to make draggable. Not owned; also used as this object's QObject parent.
     */
    explicit WindowDragger(QWidget* target_window);

protected:
    /// @brief Handles MouseButtonPress/MouseMove/MouseButtonRelease on the watched window to implement drag-to-move and snap-to-maximize.
    bool eventFilter(QObject* watched, QEvent* ev) override;

private:
    QWidget* m_window;
    QPoint m_drag_pos;
};

#endif // WINDOWDRAGGER_H
