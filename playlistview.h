#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTreeView>

class QDragMoveEvent;
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

class PlaylistView : public QTreeView
{
    Q_OBJECT
public:
    PlaylistView(QWidget *parent);
    virtual ~PlaylistView();
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void startDrag(Qt::DropActions supportedActions);
    QList<int> selectedRows();

    void selectAll();
    void setup();

private slots:
    void toggleColumn(bool toggled);
    void sectionClicked(int section);

private:
    int m_anchor_row;
    int m_pressed_row;
    bool m_select_on_release;
    QPoint startPos;

    QList<QAction*> m_columnActions; //! Maintains the mapping action<->column
};

#endif // MYTABLEVIEW_H
