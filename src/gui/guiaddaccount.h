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
#ifndef GUIADDACCOUNT_H
#define GUIADDACCOUNT_H

#include "ui_guiaddaccountbase.h"
#include "guimain.h"
#include "hobit.h"

class GuiMain;
/**
* class to add new account by using a widget for read in the accountinformations the user typed in
*/
class GuiAddAccount: public QWidget, public Ui::GuiAddAccountBase
{
  Q_OBJECT
public:
  /**
  * this is the addaccountbade constructor
  *
  *@param parent the parent dialog of this widget
  *@param stack
  *@param name widgets title
  *@param hobit
  */
  GuiAddAccount(QWidget* parent);


  /** destructor */
  ~GuiAddAccount();

  /**Set bankdata 
  */
  void showEvent(QShowEvent * se);
public slots:

  /**
  *return to last widget that was visible
  */
  virtual void on_buttonBack_clicked();

  /**
  *After the user typed in the information of the account to be added and pressed the button this function
  *checks if at least a accountid is there and then read in the necessary informations for add the account
  */
  virtual void on_buttonOk_clicked();

  /**
  *validation for informations of account to be added. referring to the length and if the necessary fields are filled out 
  */
  virtual void validateInput();


signals:
  void accountAdded();
  void back();

private:
  GuiMain* _main;

  // help variables for use by validation
  QString _lastInputAccountId;
  QString _lastInputAccountSubId;

};

#endif
