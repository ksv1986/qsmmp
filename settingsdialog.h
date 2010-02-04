#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

protected:
    void changeEvent(QEvent *e);
    void accept();

private:
    Ui::SettingsDialog *m_ui;
};

#endif // SETTINGSDIALOG_H
