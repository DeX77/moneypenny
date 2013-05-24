/***************************************************************************
 *   Copyright (C) 2004, 2006 by Walter Roth                               *
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


#include "guiwelcome.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>

/**
*constructor
*/
GuiWelcome::GuiWelcome(QWidget* parent)
    :QWidget(parent), Ui::GuiWelcome()
{
  setupUi(this);
  setObjectName("GuiWelcome");
  labelMessage->hide();
  buttonOk->hide();
  qDebug("Constructor GuiWelcome");
}

/**
*destructor
*/
GuiWelcome::~GuiWelcome()
{
  qDebug("Destructor GuiWelcome finished");
}

/**
* show the message on labelText
*/
void GuiWelcome::displayMessage(const QString & message)
{
   labelMessage->setText(message);
   labelMessage->show();
   buttonOk->show();
}

/**
* hide labelText
*/
void GuiWelcome::clearMessage()
{
   labelMessage->setText("");
   labelMessage->hide();
}

  /**Fires signal back
   */
  void GuiWelcome::slotBack(){
    labelMessage->hide();
    buttonOk->hide();
    emit back();
  }	 
