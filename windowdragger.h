#ifndef WINDOWDRAGGER_H
#define WINDOWDRAGGER_H

#include <QObject>
#include <QPoint>

class WindowDragger : public QObject {
    Q_OBJECT;

public:
    explicit WindowDragger(QWidget* target_window);

protected:
    bool eventFilter(QObject* watched, QEvent* ev) override;

private:
    QWidget* m_window;
    QPoint m_drag_pos;
};

#endif // WINDOWDRAGGER_H
