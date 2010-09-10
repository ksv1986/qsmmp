#ifndef EQDIALOG_H
#define EQDIALOG_H
#include "ui_eqdialog.h"
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QDebug>

class SoundCore;

class EQDialog : public QDialog
{
    Q_OBJECT
public:
    EQDialog(SoundCore *soundCore, QWidget *parent = 0);
    ~EQDialog();
    Ui::EQDialog ui;
    QAction *actionLoadEQ;
    QAction *actionSavePreloadEQ;
    QAction *actionSaveAutoPreloadEQ;
    QAction *actionImportEQ;
    QAction *actionClearEQ;
    void writeSettings();


public slots:
    void onOffUiSlot(bool);
    void clearEQUiSlot();
    void EQChangedSlot();
    void onOffEQSlot(bool);
    void clearEQSlot();
    void setEQValues(double* ,double);

signals:
    void EQStateSignal(bool);
    void EQChangedSignal(double*, double);

protected:
    void changeEvent(QEvent *e);

private:
    QMenu *presetsMenu;
    void readSettings();
    bool EQState;
    SoundCore *m_core;
};

#endif // EQDIALOG_H
