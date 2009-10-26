#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>
#include <QMouseEvent>

class MyTreeView : public QTreeView
{
    Q_OBJECT
public:
    MyTreeView(QWidget *parent);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent* event);
private:
    void startDrag();
    QPoint startPos;
};

#endif // MYTREEVIEW_H
