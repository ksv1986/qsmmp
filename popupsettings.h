/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "ui_popupsettings.h"

class Action;

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class PopupSettings : public QDialog
{
Q_OBJECT
public:
    PopupSettings(QWidget *parent = 0);
    ~PopupSettings();


public slots:
    virtual void accept();

private slots:
    void insertExpression(QAction *);
    void on_resetButton_clicked();

private:
    void createMenu();
    Ui::PopupSettings ui;
    QMap<uint, QPushButton*> m_buttons;
};

#endif
