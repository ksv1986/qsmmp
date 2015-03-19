#include <QWheelEvent>
#include "scrollingtrayicon.h"

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
