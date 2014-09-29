#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "settings.h"
#include "qmmpui/filedialog.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    ui->noCoverWidget->setCover(Settings::instance().noCoverPixmap());

    ui->noCoverWidget->addAction(ui->action_NoCover);
    ui->useStandardIconsCheckBox->setChecked(Settings::instance().useStandardIcons());
    ui->hiddenCheckBox->setChecked(Settings::instance().startHidden());
    ui->hideOnCloseCheckBox->setChecked(Settings::instance().hideOnClose());
    connect(ui->noCoverWidget, SIGNAL(clicked()), SLOT(chooseNoCoverImage()));
    connect(ui->action_NoCover, SIGNAL(triggered()), SLOT(resetNoCoverImage()));
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

void SettingsWidget::chooseNoCoverImage()
{
    QString path = FileDialog::getOpenFileName(this, tr("Choose no cover image"),
                                               QDir::homePath(),
                                               tr("Images") + " (*.png *.jpg *.gif)");
    if (!path.isEmpty()) {
        Settings::instance().setNoCoverImage(path);
        ui->noCoverWidget->setCover(QPixmap(path));
    }
}

void SettingsWidget::resetNoCoverImage()
{
    Settings::instance().setNoCoverImage(QString());
    ui->noCoverWidget->setCover(QPixmap());
}

void SettingsWidget::applySettings()
{
    Settings::instance().setUseStandardIcons(ui->useStandardIconsCheckBox->isChecked());
    Settings::instance().setStartHidden(ui->hiddenCheckBox->isChecked());
    Settings::instance().setHideOnClose(ui->hideOnCloseCheckBox->isChecked());
}
