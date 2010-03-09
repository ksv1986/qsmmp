#include <QWheelEvent>
#include <QPainter>

#include "volumetoolbutton.h"

VolumeToolButton::VolumeToolButton(QWidget *parent, int current, int min, int max) :
    QToolButton(parent), volume(current), min(min), max(max)
{
}

void VolumeToolButton::wheelEvent(QWheelEvent *event)
{
    int d = event->delta()/12;
    volume -= d;
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
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRect r = rect();
    r.adjust(2,2 + (r.height() - 4) * volume/max,-2,-2);
    painter.drawRoundRect(r, 10, 10);
}

QSize VolumeToolButton::sizeHint() const
{
    return QSize(18, 18);
}
