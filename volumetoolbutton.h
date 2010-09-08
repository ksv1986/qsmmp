#ifndef VOLUMETOOLBUTTON_H
#define VOLUMETOOLBUTTON_H

#include <QToolButton>

class VolumeToolButton : public QToolButton
{
Q_OBJECT
public:
    explicit VolumeToolButton(QWidget *parent = 0, int min = 0, int max = 100);

signals:
    void volumeChanged(int delta);
public slots:
    virtual void wheelEvent(QWheelEvent *);

protected slots:
    virtual void paintEvent(QPaintEvent *);
    virtual QSize sizeHint() const;

private:
    int volume;
    int min;
    int max;
};

#endif // VOLUMETOOLBUTTON_H
