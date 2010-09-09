#include <QWheelEvent>
#include <QPainter>
#include <QSettings>

#include <qmmp/qmmp.h>
#include <qmmp/qmmpsettings.h>

#include "volumetoolbutton.h"

VolumeToolButton::VolumeToolButton(QWidget *parent, int min, int max) :
    QToolButton(parent), min(min), max(max)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    volume = settings.value("Volume/left", 80).toInt();
    volume = (volume > max) ? max : volume;
    volume = (volume < min) ? min : volume;
}

void VolumeToolButton::wheelEvent(QWheelEvent *event)
{
    int d = event->delta()/12;
    volume += d;
    if(volume > max)
	volume = max;
    else if(volume < min)
	volume = min;

    emit volumeChanged(d);
    event->accept();
    repaint();
}

void VolumeToolButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
    int x = sizeHint().width();
    int y = sizeHint().height();
    static const QPointF defaultPoly[3] = {
        QPointF(2, y - 2),
        QPointF(x - 2, y - 2),
        QPointF(x - 2, 2),
    };
    painter.setPen(Qt::lightGray);
    painter.setBrush(Qt::lightGray);
    painter.drawConvexPolygon(defaultPoly, 3);
    const QPointF currentPoly[3] = {
        QPointF(2, y - 2),
        QPointF(2 + (x - 4)*volume/max, y - 2),
        QPointF(2 + (x - 4)*volume/max, y - 2 - (y - 4)*volume/max),
    };
    painter.setPen(Qt::darkGray);
    painter.setBrush(Qt::darkGray);
    painter.drawConvexPolygon(currentPoly, 3);
}

QSize VolumeToolButton::sizeHint() const
{
    return QSize(32, 18);
}
