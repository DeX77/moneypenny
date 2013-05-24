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
#ifndef GUIKNOWNBANKS_H
#define GUIKNOWNBANKS_H

#include <ui_guiknownbanksbase.h>

#include <QList>

#include "hobit.h"



/**
*A dialog which lists up all known banks using the banklist of ktoblzcheck
*/
class GuiKnownBanks : public QWidget, public Ui::GuiKnownBanksBase
{
  Q_OBJECT

public:

  /**
  *constructor of GuiKnownBanks
  *
  *@param parent a pointer on the main widgets widget stack
  *@param name widgets title
  *@param hobit the hobit object, needed for hbci functions
  */
  GuiKnownBanks(QWidget *parent = 0);

  /**
  *constructor of GuiKnownBanks
  */
  ~GuiKnownBanks();

protected slots:
 /** Emits signal back()
 */
 void on_buttonBack_clicked();
signals:
  void back();
private:
  QList <banking::Bank> _Banks;

};

#endif
