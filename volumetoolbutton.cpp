#include <QWheelEvent>
#include <QPainter>

#include <qmmp/qmmp.h>
#include <qmmp/qmmpsettings.h>

#include "volumetoolbutton.h"

VolumeToolButton::VolumeToolButton(int volume, QWidget *parent, int min, int max) :
        QToolButton(parent), volume(volume), min(min), max(max), lastVolume(0)
{
    int newVolume = qMin(volume, max);
    newVolume = qMax(volume, min);
    setVolume(newVolume, newVolume);

    if (volume == min)
        lastVolume = max;
}

void VolumeToolButton::wheelEvent(QWheelEvent *event)
{
    int d = event->delta()/12;
    int newVolume = volume + d;
    if (newVolume > max)
        newVolume = max;
    else if (newVolume < min)
        newVolume = min;

    setVolume(newVolume, newVolume);
    event->accept();
}

void VolumeToolButton::setVolume(int left, int)
{
    if (volume == left)
        return;

    volume = left;

    if (volume != min)
        lastVolume = min;

    emit volumeChanged(volume, volume);
    repaint();
}

void VolumeToolButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        int currentVolume = volume;
        setVolume(lastVolume, lastVolume);
        lastVolume = currentVolume;
        event->accept();
    }
    else if (event->button() == Qt::LeftButton)
    {
        int x = event->x();
        int width = sizeHint().width();
        int newVolume = (int)(100.0 * x / width);
        setVolume(newVolume, newVolume);
        event->accept();
    }
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
