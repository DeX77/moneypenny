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
#ifndef GUIACCOUNTINFORMATION_H
#define GUIACCOUNTINFORMATION_H

#include "ui_guiaccountinformationbase.h"
#include "hobit.h"

class GuiMain;
/**
* this is the class for operations for account/and accountinformations referring to guimain
*/
class GuiAccountInformation: public QWidget, public Ui::GuiAccountInformation
{
  Q_OBJECT
public:

  /**
  * this is the constructor of class for operations for account/and accountinformations referring to guimain
  * @param parent the parent dialog of this window
  * @param hobit
  * @param name widgets title
  */
  GuiAccountInformation(QWidget * parent);
  /**
  * get/refresh information about current account
  */

  /**
   * destructor
  */
  ~GuiAccountInformation();

  void refresh();

signals:
  void deleteAccount(QString accountId);
  void addAccount();
  void changeAccount();
  void showTransactions();
  void dataModified();

protected slots:
  /**
  * delete account referring to guimain
  */
  virtual void on_buttonDeleteAccount_clicked();
  /**
  * add account referring to guimain
  */
  virtual void on_buttonAddAccount_clicked();
  /**
  * change account referring to guimain
  */
  virtual void on_buttonEditAccountDescription_clicked();
  /**
  * switch account referring to guimain
  */
  virtual void on_buttonChangeAccount_clicked();
  /** Raise transaction list
  */
  virtual void on_buttonShowTransactions_clicked();
  /** Used to edit account description
   */
  virtual void on_tableWidget_cellChanged(int row, int column);
private:
   /** The id of the current account
  */
  QString _AccountId;
  /**Flag for program caused cell changes
   */
  bool _FillingTable;

};

#endif
