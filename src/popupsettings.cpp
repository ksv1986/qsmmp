/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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

#include <QSettings>
#include <QMenu>
#include <qmmp/qmmp.h>
#include "popupwidget.h"
#include "popupsettings.h"

PopupSettings::PopupSettings(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("PlayList");
    ui.transparencySlider->setValue(100 - settings.value("popup_opacity", 1.0).toDouble()*100);
    ui.coverSizeSlider->setValue(settings.value ("popup_cover_size", 48).toInt());
    ui.textEdit->setPlainText(settings.value ("popup_template", DEFAULT_TEMPLATE).toString());
    ui.delaySpinBox->setValue(settings.value("popup_delay", 2500).toInt());
    ui.coverCheckBox->setChecked(settings.value("popup_show_cover",true).toBool());
    settings.endGroup();
    createMenu();
}


PopupSettings::~PopupSettings()
{}

void PopupSettings::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("PlayList");
    settings.setValue("popup_opacity", 1.0 -  (double)ui.transparencySlider->value()/100);
    settings.setValue("popup_cover_size", ui.coverSizeSlider->value());
    settings.setValue("popup_template", ui.textEdit->toPlainText());
    settings.setValue("popup_delay",  ui.delaySpinBox->value());
    settings.setValue("popup_show_cover", ui.coverCheckBox->isChecked());
    settings.endGroup();
    QDialog::accept();
}

void PopupSettings::createMenu()
{
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Artist"))->setData("%p");
    menu->addAction(tr("Album"))->setData("%a");
    menu->addAction(tr("Title"))->setData("%t");
    menu->addAction(tr("Track number"))->setData("%n");
    menu->addAction(tr("Two-digit track number"))->setData("%NN");
    menu->addAction(tr("Genre"))->setData("%g");
    menu->addAction(tr("Comment"))->setData("%c");
    menu->addAction(tr("Composer"))->setData("%C");
    menu->addAction(tr("Duration"))->setData("%l");
    menu->addAction(tr("Disc number"))->setData("%D");
    menu->addAction(tr("File name"))->setData("%f");
    menu->addAction(tr("File path"))->setData("%F");
    menu->addAction(tr("Year"))->setData("%y");
    menu->addAction(tr("Condition"))->setData("%if(%p&%t,%p - %t,%f)");
    ui.insertButton->setMenu(menu);
    connect(menu, SIGNAL(triggered (QAction *)), SLOT(insertExpression(QAction *)));
}

void PopupSettings::insertExpression(QAction *a)
{
    ui.textEdit->insertPlainText(a->data().toString());
}

void PopupSettings::on_resetButton_clicked()
{
    ui.textEdit->setPlainText(DEFAULT_TEMPLATE);
}
