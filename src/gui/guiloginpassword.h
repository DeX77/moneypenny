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
#ifndef GUILOGINPASSWORD_H
#define GUILOGINPASSWORD_H

#include <QDialog>
#include "ui_guiloginpasswordbase.h"

class HoBIT_Crypt;
/**
*class for the GUI-Login-Dialog
*/
class GuiLoginPassword : public QDialog, public Ui::GuiLoginPasswordBase
{
  Q_OBJECT
public:

  /**
  *constructor of GuiLoginPassword
  *
  *@param app pointer on QApplication
  *@param homedir name of home-directory
  *@param crypt a pointer on the HoBIT_Crypt Interface
  */
  GuiLoginPassword(QWidget * parent, HoBIT_Crypt * crypt);

  /**
  *destructor of GuiLoginPassword
  */
  ~GuiLoginPassword();
  /** Checks for an existing password file
  */
  bool checkForPasswordFile();

  /** Prepares dialog for a password change
  */
  void switchToEditPasswordMode();
protected:
  /**centers and resizes the dialog
   */
  void showEvent(QShowEvent * se);
  
public slots:

  /**
  * Checks password
  */
  virtual void on_buttonLogin_clicked();
  /**
  *shows a short help message in a mesagebox.
  */
  virtual void on_buttonHelp_clicked();

  /**
  *slot for closing
  */
  virtual void on_buttonExit_clicked();

  /**
  *slot for the internet setup
  */
  virtual void slotInternetSetup();
 /**
   *Show or hide password letters.
   */
  virtual void on_checkBoxShowPassword_toggled(bool checked);

private:
  
  HoBIT_Crypt* _crypt;
  bool _CreateNewPassword;
};

#endif
