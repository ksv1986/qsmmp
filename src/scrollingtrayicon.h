#ifndef SCROLLINGTRAYICON_H
#define SCROLLINGTRAYICON_H

#include <QSystemTrayIcon>

class ScrollingTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit ScrollingTrayIcon(QObject *parent = 0);

signals:
    void wheelScrolled(int delta);

protected:
    bool event(QEvent *event);
};

#endif // SCROLLINGTRAYICON_H
