#ifndef VOLUMETOOLBUTTON_H
#define VOLUMETOOLBUTTON_H

#include <QToolButton>

class VolumeToolButton : public QToolButton
{
Q_OBJECT
public:
    explicit VolumeToolButton(QWidget *parent = 0);

signals:
    void volumeChanged(int delta);
public slots:
    virtual void wheelEvent(QWheelEvent *);
};

#endif // VOLUMETOOLBUTTON_H
