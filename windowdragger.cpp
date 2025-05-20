#include "windowdragger.h"

#include <QMouseEvent>
#include <qwidget.h>

WindowDragger::WindowDragger(QWidget* target_window)
    : QObject(target_window), m_window(target_window) {
    target_window->installEventFilter(this);
}

bool WindowDragger::eventFilter(QObject* watched, QEvent* ev) {
    if (watched != m_window) return false;

    switch (ev->type()) {
        case QEvent::MouseButtonPress: {
            auto* me = static_cast<QMouseEvent*>(ev);
            if (me->button() == Qt::LeftButton) {

                if (m_window->isMaximized()) {
                    m_window->showNormal();
                    QPoint global = me->globalPosition().toPoint();
                    m_drag_pos = QPoint(global.x() - m_window->frameGeometry().x(), me->position().y());
                } else {
                    QPoint global = me->globalPosition().toPoint();
                    m_drag_pos = global - m_window->frameGeometry().topLeft();
                }
                QPoint global = me->globalPosition().toPoint();
                m_drag_pos = global - m_window->frameGeometry().topLeft();
                ev->accept();
                return true;
            }
            break;
        }

        case QEvent::MouseMove: {
            auto* me = static_cast<QMouseEvent*>(ev);
            if (me->buttons() & Qt::LeftButton && !m_window->isMaximized()) {
                QPoint global = me->globalPosition().toPoint();
                m_window->move(global - m_drag_pos);
                ev->accept();
                return true;
            }
            break;
        }

        case QEvent::MouseButtonRelease: {
            auto* me = static_cast<QMouseEvent*>(ev);
            if (me->button() == Qt::LeftButton) {
                QPoint global = me->globalPosition().toPoint();
                if (global.y() <= 0) {
                    m_window->showMaximized();
                }
                ev->accept();
                return true;
            }
            break;
        }

        default:
            break;
    }
    return false;
}
