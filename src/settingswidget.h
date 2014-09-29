#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
    class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

    void applySettings();

protected:
    void changeEvent(QEvent *e);

private slots:
    void chooseNoCoverImage();
    void resetNoCoverImage();
private:
    Ui::SettingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
