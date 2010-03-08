#include <QWheelEvent>

#include "volumetoolbutton.h"

VolumeToolButton::VolumeToolButton(QWidget *parent) :
    QToolButton(parent)
{
}

void VolumeToolButton::wheelEvent(QWheelEvent *event)
{
    emit volumeChanged(event->delta());
    event->accept();
}
