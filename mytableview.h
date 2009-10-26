#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>

class QDragMoveEvent;
class QDragEnterEvent;
class QDropEvent;

class MyTableView : public QTableView
{
    Q_OBJECT
public:
    MyTableView(QWidget *parent);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
};

#endif // MYTABLEVIEW_H
