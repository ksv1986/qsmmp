#include "eqdialog.h"
#include <qmmp/soundcore.h>

EQDialog::EQDialog(SoundCore *soundCore, QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);

	m_core = soundCore;
	actionLoadEQ = new QAction(this);
	actionLoadEQ->setText(tr("Load/Delete"));
	actionLoadEQ->setDisabled(true);	//temporary inactive
	actionSavePreloadEQ = new QAction(this);
	actionSavePreloadEQ->setText(tr("Save preload"));
	actionSavePreloadEQ->setDisabled(true);	// temporary inactive
	actionSaveAutoPreloadEQ = new QAction(this);
	actionSaveAutoPreloadEQ->setText(tr("Save Auto Preload"));
	actionSaveAutoPreloadEQ->setDisabled(true);	// temporary inactive
	actionImportEQ = new QAction(this);
	actionImportEQ->setText(tr("Import"));
	actionImportEQ->setDisabled(true);	// temporary inactive
	actionClearEQ = new QAction(this);
	actionClearEQ->setText(tr("Clear"));

	presetsMenu = new QMenu;
	presetsMenu->addAction(actionLoadEQ);
	presetsMenu->addSeparator();
	presetsMenu->addAction(actionSavePreloadEQ);
	presetsMenu->addAction(actionSaveAutoPreloadEQ);
	presetsMenu->addAction(actionImportEQ);
	presetsMenu->addSeparator();
	presetsMenu->addAction(actionClearEQ);

	ui.onOffToolButton->setCheckable(true);
	ui.autoToolButton->setCheckable(true);
	ui.autoToolButton->setDisabled(true);	// temporary inactive
	ui.presetsPushButton->setMenu(presetsMenu);

	ui.verticalPremapSlider->setRange(-20.0, 20.0);
	ui.verticalBandSlider1->setRange(-20.0, 20.0);
	ui.verticalBandSlider2->setRange(-20.0, 20.0);
	ui.verticalBandSlider3->setRange(-20.0, 20.0);
	ui.verticalBandSlider4->setRange(-20.0, 20.0);
	ui.verticalBandSlider5->setRange(-20.0, 20.0);
	ui.verticalBandSlider6->setRange(-20.0, 20.0);
	ui.verticalBandSlider7->setRange(-20.0, 20.0);
	ui.verticalBandSlider8->setRange(-20.0, 20.0);
	ui.verticalBandSlider9->setRange(-20.0, 20.0);
	ui.verticalBandSlider10->setRange(-20.0, 20.0);

	connect(ui.onOffToolButton, SIGNAL(clicked(bool)), this, SIGNAL(EQStateSignal(bool)));
	connect(this, SIGNAL(EQStateSignal(bool)), this, SLOT(onOffUiSlot(bool)));
	connect(actionClearEQ, SIGNAL(triggered()), this, SLOT(clearEQUiSlot()));
	connect(ui.verticalPremapSlider, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider1, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider2, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider3, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider4, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider5, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider6, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider7, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider8, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider9, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));
	connect(ui.verticalBandSlider10, SIGNAL(valueChanged(int)), this, SLOT(EQChangedSlot()));

	connect(this, SIGNAL(EQStateSignal(bool)), this, SLOT(onOffEQSlot(bool)));
	connect(this->actionClearEQ, SIGNAL(triggered()), this, SLOT(clearEQSlot()));

	readSettings();
}

EQDialog::~EQDialog()
{

}

void EQDialog::clearEQUiSlot()
{
	ui.verticalPremapSlider->setValue(0);
	ui.verticalBandSlider1->setValue(0);
	ui.verticalBandSlider2->setValue(0);
	ui.verticalBandSlider3->setValue(0);
	ui.verticalBandSlider4->setValue(0);
	ui.verticalBandSlider5->setValue(0);
	ui.verticalBandSlider6->setValue(0);
	ui.verticalBandSlider7->setValue(0);
	ui.verticalBandSlider8->setValue(0);
	ui.verticalBandSlider9->setValue(0);
	ui.verticalBandSlider10->setValue(0);

}

void EQDialog::EQChangedSlot()
{
	double bands[10] = {	ui.verticalBandSlider1->value(),
				ui.verticalBandSlider2->value(),
				ui.verticalBandSlider3->value(),
				ui.verticalBandSlider4->value(),
				ui.verticalBandSlider5->value(),
				ui.verticalBandSlider6->value(),
				ui.verticalBandSlider7->value(),
				ui.verticalBandSlider8->value(),
				ui.verticalBandSlider9->value(),
				ui.verticalBandSlider10->value(),
				};
	double premap = ui.verticalPremapSlider->value();
	setEQValues(bands, premap);
}

void EQDialog::readSettings()
{
	QSettings settings("Qmmp-fooUI", "qmmpui");
	settings.beginGroup("/EQgeneral");
	EQState = settings.value("/state", 0).toBool();
	emit EQStateSignal(EQState);
	ui.onOffToolButton->setChecked(EQState);
	settings.endGroup();
	settings.beginGroup("/bands");
	bool ok;
	ui.verticalBandSlider1->setValue(settings.value("/band1").toInt(&ok));
	ui.verticalBandSlider2->setValue(settings.value("/band2").toInt(&ok));
	ui.verticalBandSlider3->setValue(settings.value("/band3").toInt(&ok));
	ui.verticalBandSlider4->setValue(settings.value("/band4").toInt(&ok));
	ui.verticalBandSlider5->setValue(settings.value("/band5").toInt(&ok));
	ui.verticalBandSlider6->setValue(settings.value("/band6").toInt(&ok));
	ui.verticalBandSlider7->setValue(settings.value("/band7").toInt(&ok));
	ui.verticalBandSlider8->setValue(settings.value("/band8").toInt(&ok));
	ui.verticalBandSlider9->setValue(settings.value("/band9").toInt(&ok));
	ui.verticalBandSlider10->setValue(settings.value("/band10").toInt(&ok));
	ui.verticalPremapSlider->setValue(settings.value("/premap").toInt(&ok));
	settings.endGroup();
	settings.deleteLater();
}

void EQDialog::writeSettings()
{
	QSettings settings("Qmmp-fooUI", "qmmpui");
	settings.beginGroup("/EQgeneral");
	settings.setValue("/state", EQState);
	settings.endGroup();
	settings.beginGroup("/bands");
	settings.setValue("/band1", ui.verticalBandSlider1->value());
	settings.setValue("/band2", ui.verticalBandSlider2->value());
	settings.setValue("/band3", ui.verticalBandSlider3->value());
	settings.setValue("/band4", ui.verticalBandSlider4->value());
	settings.setValue("/band5", ui.verticalBandSlider5->value());
	settings.setValue("/band6", ui.verticalBandSlider6->value());
	settings.setValue("/band7", ui.verticalBandSlider7->value());
	settings.setValue("/band8", ui.verticalBandSlider8->value());
	settings.setValue("/band9", ui.verticalBandSlider9->value());
	settings.setValue("/band10", ui.verticalBandSlider10->value());
	settings.setValue("/premap", ui.verticalPremapSlider->value());
	settings.endGroup();
	settings.deleteLater();
}

void EQDialog::onOffUiSlot(bool x)
{
	EQState = x;
	if(x)
	{
		ui.onOffToolButton->setText(tr("Off"));
		ui.verticalPremapSlider->setEnabled(true);
		ui.verticalBandSlider1->setEnabled(true);
		ui.verticalBandSlider2->setEnabled(true);
		ui.verticalBandSlider3->setEnabled(true);
		ui.verticalBandSlider4->setEnabled(true);
		ui.verticalBandSlider5->setEnabled(true);
		ui.verticalBandSlider6->setEnabled(true);
		ui.verticalBandSlider7->setEnabled(true);
		ui.verticalBandSlider8->setEnabled(true);
		ui.verticalBandSlider9->setEnabled(true);
		ui.verticalBandSlider10->setEnabled(true);
	}
	else
	{
		ui.onOffToolButton->setText(tr("On"));
		ui.verticalPremapSlider->setEnabled(false);
		ui.verticalBandSlider1->setEnabled(false);
		ui.verticalBandSlider2->setEnabled(false);
		ui.verticalBandSlider3->setEnabled(false);
		ui.verticalBandSlider4->setEnabled(false);
		ui.verticalBandSlider5->setEnabled(false);
		ui.verticalBandSlider6->setEnabled(false);
		ui.verticalBandSlider7->setEnabled(false);
		ui.verticalBandSlider8->setEnabled(false);
		ui.verticalBandSlider9->setEnabled(false);
		ui.verticalBandSlider10->setEnabled(false);
	}
}

void EQDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type())
	{
		case QEvent::LanguageChange:
			ui.retranslateUi(this);
			break;
		default:
			break;
	}
}

void EQDialog::setEQValues(double bands[10], double premap)
{
//	m_core->setEQ(bands, premap);
}

void EQDialog::clearEQSlot()
{
	double bands[10] = {0,0,0,0,0,0,0,0,0,0,}, premap=0;
//	m_core->setEQ(bands, premap);
}

void EQDialog::onOffEQSlot(bool x)
{
//	m_core->setEQEnabled(x);
}
