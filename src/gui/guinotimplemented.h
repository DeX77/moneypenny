/***************************************************************************
 *   Copyright (C) 2004 by Walter Roth                                     *
 *   walter@amd2600-roth                                                   *
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
#ifndef GUINOTIMPLEMENTED_H
#define GUINOTIMPLEMENTED_H

#include <QWidget>
#include <ui_guinotimplementedbase.h>

/**
A screen with a "Not yet implemented" message

	@author walter <walter@HP2100-Roth>
*/
class GuiNotImplemented : public QWidget, public Ui::GuiNotImplementedBase
{
Q_OBJECT
public:
    GuiNotImplemented(QWidget *parent = 0);

    ~GuiNotImplemented();
public slots:
  void on_pushButtonBack_clicked();

signals:
  void back();

};

#endif
