#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "settings.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    ui->useStandardIconsCheckBox->setChecked(Settings::instance().useStandardIcons());
    ui->hiddenCheckBox->setChecked(Settings::instance().startHidden());
    ui->hideOnCloseCheckBox->setChecked(Settings::instance().hideOnClose());
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsWidget::applySettings()
{
    Settings::instance().setUseStandardIcons(ui->useStandardIconsCheckBox->isChecked());
    Settings::instance().setStartHidden(ui->hiddenCheckBox->isChecked());
    Settings::instance().setHideOnClose(ui->hideOnCloseCheckBox->isChecked());
}
