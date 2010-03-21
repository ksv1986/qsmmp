#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>

class QDragMoveEvent;
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

class MyTableView : public QTableView
{
    Q_OBJECT
public:
    MyTableView(QWidget *parent);
    virtual ~MyTableView();
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void selectAll();
    void setup();

private slots:
    void toggleColumn(bool toggled);

private:
    int m_anchor_row;
    int m_pressed_row;
    bool m_select_on_release;

    QList<QAction*> m_columnActions; //! Maintains the mapping action<->column

};

#endif // MYTABLEVIEW_H
