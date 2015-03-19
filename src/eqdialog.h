#ifndef EQDIALOG_H
#define EQDIALOG_H

#include <QDialog>
#include "ui_eqdialog.h"

class QAction;
class SoundCore;

class EQDialog : public QDialog
{
    Q_OBJECT
public:
    EQDialog(SoundCore *soundCore, QWidget *parent = 0);
    ~EQDialog();

    void writeSettings();

public slots:
    void onOffUiSlot(bool);
    void clearEQUiSlot();
    void EQChangedSlot();
    void onOffEQSlot(bool);
    void clearEQSlot();
    void setEQValues(int* ,int);

signals:
    void EQStateSignal(bool);
    void EQChangedSignal(double*, double);

protected:
    void changeEvent(QEvent *e);

private:
    void readSettings();

    Ui::EQDialog ui;
    QAction *actionLoadEQ;
    QAction *actionSavePreloadEQ;
    QAction *actionSaveAutoPreloadEQ;
    QAction *actionImportEQ;
    QAction *actionClearEQ;
    QMenu *presetsMenu;
    bool EQState;
    SoundCore *m_core;
};

#endif // EQDIALOG_H
