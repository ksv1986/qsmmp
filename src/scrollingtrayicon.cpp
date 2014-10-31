#include "scrollingtrayicon.h"
#include <QWheelEvent>

ScrollingTrayIcon::ScrollingTrayIcon(QObject *parent) :
    QSystemTrayIcon(parent)
{
}

bool ScrollingTrayIcon::event(QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        emit wheelScrolled(static_cast<QWheelEvent*>(event)->delta());
    }
    return QSystemTrayIcon::event(event);
}
