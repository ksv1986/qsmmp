#include <QDirModel>
#include <QUrl>
#include <QApplication>
#include "mytreeview.h"

MyTreeView::MyTreeView(QWidget *parent = 0)
	: QTreeView(parent)
{
}


void MyTreeView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
	startPos = event->pos();
    QTreeView::mousePressEvent(event);
}

void MyTreeView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
	int distance = (event->pos() - startPos).manhattanLength();
	if (distance >= QApplication::startDragDistance())
	    startDrag();
    }
    QTreeView::mouseMoveEvent(event);
}

void MyTreeView::startDrag()
{
    QModelIndex index = currentIndex();
    if (index.isValid())
    {
	QDirModel *dirModel = qobject_cast<QDirModel*>(model());
	if(dirModel == NULL)
	    return;
	QString path = dirModel->filePath(index);
	QMimeData *mimeData = new QMimeData();
	mimeData->setUrls(QList<QUrl>()<<QUrl(path));
	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	//drag->setPixmap(QPixmap(":/images/person.png"));
	drag->start(Qt::CopyAction);

    }
}
