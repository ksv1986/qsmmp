#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QWheelEvent>

#include <qmmp/qmmp.h>
#include <qmmp/qmmpsettings.h>

#include "volumetoolbutton.h"

VolumeToolButton::VolumeToolButton(int volume, QWidget *parent, int min, int max) :
        QToolButton(parent), volume(volume), min(min), max(max), lastVolume(0)
{
    int newVolume = limitVolume(volume);
    setVolume(newVolume, newVolume);

    if (volume == min)
        lastVolume = max;

    if(QApplication::palette().color(QPalette::Window).lightness() > 127)
    {
        backgroundColor = QApplication::palette().color(QPalette::Mid);
        sliderColor = backgroundColor.darker();
    }
    else
    {
        backgroundColor = QApplication::palette().color(QPalette::Light);
        sliderColor = backgroundColor.lighter();
    }
}

int VolumeToolButton::limitVolume(int volume) const
{
    return qMax(qMin(volume, max), min);
}

void VolumeToolButton::wheelEvent(QWheelEvent *event)
{
    int d = event->delta()/12;
    int newVolume = limitVolume(volume + d);
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
        int newVolume = (int)(100.0 * (x - 2) / (width - 4));
        setVolume(newVolume, newVolume);
        event->accept();
    }
}

void VolumeToolButton::paintEvent(QPaintEvent *)
{
    int x = sizeHint().width();
    int y = sizeHint().height();
    static const QPoint background[3] = {
        QPoint(2, y - 2),
        QPoint(x - 2, y - 2),
        QPoint(x - 2, 2),
    };

    QImage img(sizeHint(), QImage::Format_ARGB32_Premultiplied);
    img.fill(QColor(0, 0, 0, 0).rgba());
    QPainter painter(&img);

    painter.setPen(backgroundColor);
    painter.setBrush(backgroundColor);
    painter.drawConvexPolygon(background, 3);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

    QRect slider(QPoint(2, 2), QSize((x - 3)*volume/max, y - 3));
    painter.fillRect(slider, sliderColor);
    painter.end();
    QPainter buttonPainter(this);
    buttonPainter.drawImage(0, 0, img);
}

QSize VolumeToolButton::sizeHint() const
{
    return QSize(32, 18);
}
