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
#ifndef GUILOGIN_H
#define GUILOGIN_H

#include "ui_guiloginbase.h"
#include "hobit.h"

#include <QWidget>
//#include "guimain.h"
//#include <qwidgetstack.h>

class GuiMain;


class GuiLogin: public QWidget, public Ui::GuiLogin
{

  Q_OBJECT


public:

  /**
  *constructor of GuiLogin
  *
  *@param parent a pointer on the main widgets widget stack
  *@param name widgets title
  *@param hobit the hobit object, needed for hbci functions
  */
  GuiLogin(QWidget* parent);


   /** destructor*/
   ~GuiLogin();

  /**
  *function for refreshing
  */
  virtual void refresh();
signals:
  void setCurrentAccount(const QString & bankCode, const QString & account);
  void raiseWidget(int widget); 
  void close();

public slots:

  /**
  *slot for the login
  */
  void on_buttonLogin_clicked();
 
 /** Select the text on the login button according to valid/invalid accounts
 */
  void on_tableWidget_currentCellChanged( int currentRow, int currentColumn, int previousRow, int previousColumn );
};

#endif
