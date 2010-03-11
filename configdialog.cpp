/***************************************************************************
 *   Copyright (C) 2007-2009 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QDir>
#include <QSettings>
#include <QFontDialog>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QCheckBox>
#include <QRadioButton>
#include <QMenu>

#include <qmmp/decoder.h>
#include <qmmp/output.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/outputfactory.h>
#include <qmmp/visualfactory.h>
#include <qmmp/effectfactory.h>
#include <qmmp/effect.h>
#include <qmmp/soundcore.h>
#include <qmmpui/generalfactory.h>
#include <qmmpui/general.h>
#include <qmmpui/generalhandler.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmodel.h>


#include "pluginitem.h"
#include "configdialog.h"

ConfigDialog::ConfigDialog ( QWidget *parent )
        : QDialog ( parent )
{
    ui.setupUi ( this );
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    setAttribute(Qt::WA_DeleteOnClose, FALSE);
    ui.preferencesButton->setEnabled(FALSE);
    ui.informationButton->setEnabled(FALSE);
    connect (ui. contentsWidget,
             SIGNAL (currentItemChanged (QListWidgetItem *, QListWidgetItem * )),
             this, SLOT (changePage (QListWidgetItem *, QListWidgetItem* )));
    connect (ui.mainFontButton, SIGNAL (clicked()), SLOT (setMainFont()));
    connect (ui.plFontButton, SIGNAL (clicked()), SLOT (setPlFont()));
    connect (ui.preferencesButton, SIGNAL(clicked()), SLOT (showPluginSettings()));
    connect (ui.informationButton, SIGNAL(clicked()), SLOT (showPluginInfo()));
    connect (this, SIGNAL(accepted()),SLOT(saveSettings()));
    connect (ui.inputPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.outputPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.visualPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.generalPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.effectPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.pluginsTab, SIGNAL(currentChanged(int)), SLOT(updateButtons()));
    connect (ui.fileDialogComboBox, SIGNAL (currentIndexChanged (int)), SLOT(updateDialogButton(int)));
    connect (ui.fdInformationButton, SIGNAL (clicked()), SLOT(showFileDialogInfo()));
    readSettings();
    loadPluginsInfo();
    loadFonts();
    createMenus();
}

ConfigDialog::~ConfigDialog()
{
    while (!m_outputPluginItems.isEmpty())
        delete m_outputPluginItems.takeFirst();
    while (!m_inputPluginItems.isEmpty())
        delete m_inputPluginItems.takeFirst();
    while (!m_visualPluginItems.isEmpty())
        delete m_visualPluginItems.takeFirst();
    while (!m_effectPluginItems.isEmpty())
        delete m_effectPluginItems.takeFirst();
}

void ConfigDialog::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    if (MediaPlayer *player = MediaPlayer::instance())
    {
        ui.formatLineEdit->setText(player->playListModel()->format());
        ui.metadataCheckBox->setChecked(player->playListModel()->useMetadata());
        ui.underscoresCheckBox->setChecked(player->playListModel()->convertUnderscore());
        ui.per20CheckBox->setChecked(player->playListModel()->convertTwenty());
    }
    ui.protocolCheckBox->setChecked(settings.value ("PlayList/show_protocol", FALSE).toBool());
    //proxy settings
    ui.enableProxyCheckBox->setChecked(Qmmp::useProxy());
    ui.authProxyCheckBox->setChecked(Qmmp::useProxyAuth());
    ui.hostLineEdit->setText(Qmmp::proxy().host());
    if (Qmmp::proxy().port(0))
        ui.portLineEdit->setText(QString::number(Qmmp::proxy().port(0)));
    ui.proxyUserLineEdit->setText(Qmmp::proxy().userName());
    ui.proxyPasswLineEdit->setText(Qmmp::proxy().password());

    ui.hostLineEdit->setEnabled(ui.enableProxyCheckBox->isChecked());
    ui.portLineEdit->setEnabled(ui.enableProxyCheckBox->isChecked());
    ui.proxyUserLineEdit->setEnabled(ui.authProxyCheckBox->isChecked());
    ui.proxyPasswLineEdit->setEnabled(ui.authProxyCheckBox->isChecked());

    ui.hiddenCheckBox->setChecked(settings.value("MainWindow/start_hidden", FALSE).toBool());
    ui.hideOnCloseCheckBox->setChecked(settings.value("MainWindow/hide_on_close", FALSE).toBool());
    //volume
    ui.softVolumeCheckBox->setChecked(SoundCore::instance()->softwareVolume());
    connect(ui.softVolumeCheckBox, SIGNAL(clicked(bool)), SoundCore::instance(), SLOT(setSoftwareVolume(bool)));
    //transparency
    ui.mwTransparencySlider->setValue(100 - settings.value("MainWindow/opacity", 1.0).toDouble()*100);
    ui.eqTransparencySlider->setValue(100 - settings.value("Equalizer/opacity", 1.0).toDouble()*100);
    ui.plTransparencySlider->setValue(100 - settings.value("PlayList/opacity", 1.0).toDouble()*100);
}

void ConfigDialog::changePage (QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    ui.stackedWidget->setCurrentIndex (ui.contentsWidget->row (current));
    ui.visibilityGroupBox->setEnabled(GeneralHandler::instance()->visibilityControl());
}

void ConfigDialog::loadPluginsInfo()
{
    /*
        load input plugins information
    */
    QList <DecoderFactory *> *decoders = 0;
    decoders = Decoder::factories();
    QStringList files = Decoder::files();
    ui.inputPluginTable->setColumnCount ( 3 );
    ui.inputPluginTable->verticalHeader()->hide();
    ui.inputPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.inputPluginTable->setRowCount ( decoders->count () );
    for ( int i = 0; i < decoders->count (); ++i )
    {
        InputPluginItem *item = new InputPluginItem(this, decoders->at(i));
        m_inputPluginItems.append(item);
        QCheckBox* checkBox = new QCheckBox ( ui.inputPluginTable );
        checkBox->setFocusPolicy (Qt::NoFocus);
        ui.inputPluginTable->setCellWidget ( i, 0, checkBox );
        checkBox->setChecked(item->isSelected());
        connect(checkBox, SIGNAL(toggled(bool)), item, SLOT(setSelected(bool)));
        ui.inputPluginTable->setItem ( i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.inputPluginTable->setItem ( i,2, new QTableWidgetItem (files.at (i).section('/',-1)));
        ui.inputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setToolTip(files.at (i));
    }
    ui.inputPluginTable->resizeColumnToContents ( 0 );
    ui.inputPluginTable->resizeColumnToContents ( 1 );
    ui.inputPluginTable->resizeRowsToContents ();
    /*
        load output plugins information
    */
    QList <OutputFactory *> *outputs = 0;
    outputs = Output::outputFactories();
    files = Output::outputFiles();
    ui.outputPluginTable->setColumnCount ( 3 );
    ui.outputPluginTable->verticalHeader()->hide();
    ui.outputPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.outputPluginTable->setRowCount ( outputs->count () );

    for ( int i = 0; i < outputs->count (); ++i )
    {
        OutputPluginItem *item = new OutputPluginItem(this,outputs->at(i));
        m_outputPluginItems.append(item);
        QRadioButton* button = new QRadioButton ( ui.outputPluginTable );
        button->setFocusPolicy (Qt::NoFocus);
        ui.outputPluginTable->setCellWidget ( i, 0, button );
        button->setChecked ( item->isSelected() );
        connect(button, SIGNAL(pressed ()), item, SLOT(select()));
        ui.outputPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.outputPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.outputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.outputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.outputPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.outputPluginTable->resizeColumnToContents ( 0 );
    ui.outputPluginTable->resizeColumnToContents ( 1 );
    ui.outputPluginTable->resizeRowsToContents ();
    /*
        load visual plugin information
    */
    QList <VisualFactory *> *visuals = 0;
    visuals = Visual::factories();
    files = Visual::files();
    ui.visualPluginTable->setColumnCount ( 3 );
    ui.visualPluginTable->verticalHeader()->hide();
    ui.visualPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.visualPluginTable->setRowCount ( visuals->count () );

    for ( int i = 0; i < visuals->count (); ++i )
    {
        VisualPluginItem *item = new VisualPluginItem(this,visuals->at(i));
        m_visualPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.visualPluginTable);
        button->setFocusPolicy (Qt::NoFocus);
        ui.visualPluginTable->setCellWidget ( i, 0, button );
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        ui.visualPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.visualPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.visualPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.visualPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.visualPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.visualPluginTable->resizeColumnToContents ( 0 );
    ui.visualPluginTable->resizeColumnToContents ( 1 );
    ui.visualPluginTable->resizeRowsToContents ();

    /*
        load effect plugin information
    */
    QList <EffectFactory *> *effects = 0;
    effects = Effect::effectFactories();
    files = Effect::effectFiles();
    ui.effectPluginTable->setColumnCount ( 3 );
    ui.effectPluginTable->verticalHeader()->hide();
    ui.effectPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.effectPluginTable->setRowCount ( effects->count () );

    for ( int i = 0; i < effects->count (); ++i )
    {
        EffectPluginItem *item = new EffectPluginItem(this,effects->at(i));
        m_effectPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.effectPluginTable);
        button->setFocusPolicy (Qt::NoFocus);
        ui.effectPluginTable->setCellWidget ( i, 0, button );
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        ui.effectPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.effectPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.effectPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.effectPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.effectPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.effectPluginTable->resizeColumnToContents ( 0 );
    ui.effectPluginTable->resizeColumnToContents ( 1 );
    ui.effectPluginTable->resizeRowsToContents ();

    /*
    load general plugin information
    */
    QList <GeneralFactory *> *generals = 0;
    generals = General::generalFactories();
    files = General::generalFiles();
    ui.generalPluginTable->setColumnCount ( 3 );
    ui.generalPluginTable->verticalHeader()->hide();
    ui.generalPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.generalPluginTable->setRowCount ( generals->count () );

    for ( int i = 0; i < generals->count (); ++i )
    {
        GeneralPluginItem *item = new GeneralPluginItem(this,generals->at(i));
        m_generalPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.generalPluginTable);
        button->setFocusPolicy (Qt::NoFocus);
        ui.generalPluginTable->setCellWidget ( i, 0, button );
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        ui.generalPluginTable->setItem (i,1,
                                        new QTableWidgetItem (item->factory()->properties().name));
        ui.generalPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.generalPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.generalPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.generalPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.generalPluginTable->resizeColumnToContents (0);
    ui.generalPluginTable->resizeColumnToContents (1);
    ui.generalPluginTable->resizeRowsToContents ();

    /*
        load file dialog information
    */

    foreach(FileDialogFactory *factory, FileDialog::registeredFactories())
    {
        ui.fileDialogComboBox->addItem(factory->properties().name);
        if (FileDialog::isEnabled(factory))
            ui.fileDialogComboBox->setCurrentIndex(ui.fileDialogComboBox->count()-1);
    }
}


void ConfigDialog::loadFonts()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString fontname = settings.value ( "PlayList/Font").toString();
    QFont font;
    if(!fontname.isEmpty())
        font.fromString(fontname);
    ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.plFontLabel->setFont(font);

    fontname = settings.value ("MainWindow/Font").toString();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    font.fromString(fontname);
    ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.mainFontLabel->setFont(font);
}

void ConfigDialog::setPlFont()
{
    bool ok;
    QFont font = ui.plFontLabel->font();
    font = QFontDialog::getFont ( &ok, font, this );
    if (ok)
    {
        ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        ui.plFontLabel->setFont(font);
        QSettings settings ( Qmmp::configFile(), QSettings::IniFormat );
        settings.setValue ( "PlayList/Font", font.toString() );
    }
}

void ConfigDialog::setMainFont()
{
    bool ok;
    QFont font = ui.mainFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if ( ok )
    {
        ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        ui.mainFontLabel->setFont(font);
        QSettings settings ( Qmmp::configFile(), QSettings::IniFormat );
        settings.setValue ( "MainWindow/Font", font.toString() );
    }
}

void ConfigDialog::showPluginSettings()
{
    switch ((int) ui.pluginsTab -> currentIndex ())
    {
    case 0:
    {
        QList <DecoderFactory *> *decoders = 0;
        decoders = Decoder::factories();
        int row = ui.inputPluginTable->currentRow ();
        if ( !decoders || row<0 )
            return;

        decoders->at ( row )->showSettings ( this );
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if ( m_outputPluginItems.isEmpty() || row < 0 )
            return;
        m_outputPluginItems.at(row)->factory()->showSettings ( this );
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if ( m_visualPluginItems.isEmpty() || row < 0 )
            return;
        Visual::showSettings(m_visualPluginItems.at(row)->factory(), this);
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if ( m_effectPluginItems.isEmpty() || row < 0 )
            return;
        m_effectPluginItems.at(row)->factory()->showSettings ( this );
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if ( m_generalPluginItems.isEmpty() || row < 0 )
	    return;
	GeneralHandler::instance()->showSettings(m_generalPluginItems.at(row)->factory(), this);
        break;
    }
    }
}

void ConfigDialog::showPluginInfo()
{
    switch ( ( int ) ui.pluginsTab -> currentIndex () )
    {
    case 0:
    {
        int row = ui.inputPluginTable->currentRow ();
        if (m_inputPluginItems.isEmpty() || row < 0)
            return;
        m_inputPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if (m_outputPluginItems.isEmpty() || row < 0)
            return;
        m_outputPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if ( m_visualPluginItems.isEmpty() || row < 0 )
            return;
        m_visualPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if ( m_effectPluginItems.isEmpty() || row < 0 )
            return;
        m_effectPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if ( m_generalPluginItems.isEmpty() || row < 0 )
            return;
        m_generalPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    }
}

void ConfigDialog::createMenus()
{
    QMenu *menu = new QMenu(this);

    menu->addAction(tr("Artist"))->setData("%p");
    menu->addAction(tr("Album"))->setData("%a");
    menu->addAction(tr("Title"))->setData("%t");
    menu->addAction(tr("Tracknumber"))->setData("%n");
    menu->addAction(tr("Genre"))->setData("%g");
    menu->addAction(tr("Filename"))->setData("%f");
    menu->addAction(tr("Filepath"))->setData("%F");
    menu->addAction(tr("Date"))->setData("%d");
    menu->addAction(tr("Year"))->setData("%y");
    menu->addAction(tr("Comment"))->setData("%c");
    ui.titleButton->setMenu(menu);
    ui.titleButton->setPopupMode(QToolButton::InstantPopup);
    connect( menu, SIGNAL(triggered ( QAction * )), SLOT(addTitleString( QAction * )));
}

void ConfigDialog::addTitleString( QAction * a)
{
    if (ui.formatLineEdit->cursorPosition () < 1)
        ui.formatLineEdit->insert(a->data().toString());
    else
        ui.formatLineEdit->insert(" - "+a->data().toString());
}

void ConfigDialog::saveSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    if (MediaPlayer *player = MediaPlayer::instance())
    {
        player->playListModel()->setFormat(ui.formatLineEdit->text().trimmed());
        player->playListModel()->setUseMetadata(ui.metadataCheckBox->isChecked());
        player->playListModel()->setConvertUnderscore(ui.underscoresCheckBox->isChecked());
        player->playListModel()->setConvertTwenty(ui.per20CheckBox->isChecked());
    }
    settings.setValue ("PlayList/show_protocol", ui.protocolCheckBox->isChecked());
    FileDialog::setEnabled(FileDialog::registeredFactories().at(ui.fileDialogComboBox->currentIndex()));

    Qmmp::setProxyEnabled(ui.enableProxyCheckBox->isChecked());
    Qmmp::setProxyAuthEnabled(ui.authProxyCheckBox->isChecked());
    QUrl proxyUrl;
    proxyUrl.setHost(ui.hostLineEdit->text());
    proxyUrl.setPort(ui.portLineEdit->text().toUInt());
    proxyUrl.setUserName(ui.proxyUserLineEdit->text());
    proxyUrl.setPassword(ui.proxyPasswLineEdit->text());
    Qmmp::setProxy(proxyUrl);
    settings.setValue ("MainWindow/start_hidden", ui.hiddenCheckBox->isChecked());
    settings.setValue ("MainWindow/hide_on_close", ui.hideOnCloseCheckBox->isChecked());
    settings.setValue ("MainWindow/opacity", 1.0 -  (double)ui.mwTransparencySlider->value()/100);
    settings.setValue ("Equalizer/opacity", 1.0 -  (double)ui.eqTransparencySlider->value()/100);
    settings.setValue ("PlayList/opacity", 1.0 -  (double)ui.plTransparencySlider->value()/100);
}

void ConfigDialog::updateButtons()
{

    bool preferences = FALSE;
    bool information = FALSE;
    switch ( ( int ) ui.pluginsTab -> currentIndex () )
    {
    case 0:
    {
        int row = ui.inputPluginTable->currentRow ();
        if (m_inputPluginItems.isEmpty() || row < 0)
            break;
        DecoderFactory *factory = m_inputPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if (m_outputPluginItems.isEmpty() || row < 0 )
            break;
        OutputFactory *factory = m_outputPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if (m_visualPluginItems.isEmpty() || row < 0 )
            break;
        VisualFactory *factory = m_visualPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if ( m_effectPluginItems.isEmpty() || row < 0 )
            break;
        EffectFactory *factory = m_effectPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if ( m_generalPluginItems.isEmpty() || row < 0 )
            break;
        GeneralFactory *factory = m_generalPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    }
    ui.preferencesButton->setEnabled(preferences);
    ui.informationButton->setEnabled(information);
}

void ConfigDialog::updateDialogButton(int index)
{
    ui.fdInformationButton->setEnabled(FileDialog::registeredFactories()[index]->properties().hasAbout);
}

void ConfigDialog::showFileDialogInfo()
{
    int index = ui.fileDialogComboBox->currentIndex ();
    FileDialog::registeredFactories()[index]->showAbout(this);
}
