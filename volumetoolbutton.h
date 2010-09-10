#ifndef VOLUMETOOLBUTTON_H
#define VOLUMETOOLBUTTON_H

#include <QToolButton>

class VolumeToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit VolumeToolButton(int currentVolume, QWidget *parent = 0,
                              int min = 0, int max = 100);

signals:
    void volumeChanged(int left, int right);

public slots:
    virtual void wheelEvent(QWheelEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    void setVolume(int left, int right);

protected slots:
    virtual void paintEvent(QPaintEvent *);
    virtual QSize sizeHint() const;

private:
    int volume;
    int min;
    int max;
    int lastVolume;
};

#endif // VOLUMETOOLBUTTON_H
