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

#ifndef GUIWELCOME_H
#define GUIWELCOME_H

#include "ui_guiwelcomebase.h"

#include <QWidget>

/**
*this is the welcome dialog class
*/
class GuiWelcome : public QWidget, public Ui::GuiWelcome
{
  Q_OBJECT

public:
  
  /**
  *constructor
  */
  GuiWelcome(QWidget* parent = 0);
  
  /**
  *destructor
  */
  ~GuiWelcome();
  
  /**
  * Show the message on labelText
  */
  void displayMessage(const QString & message);
  /**
  * hide labelText
  */
  void clearMessage();
public slots:
  /**Fires signal back
   */
  void slotBack();

signals:
  void back();

};

#endif

