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
#include "guiloginpassword.h"

#include "string.h"

#include <QApplication>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QCursor>
#include <QMessageBox>
#include <QLayout>

#include "hobit_crypt.h"

/**
*constructor of GuiLoginPassword
*
*@param app pointer on QApplication
*@param homedir name of home-directory
*@param crypt a pointer on the HoBIT_Crypt Interface
*/
GuiLoginPassword::GuiLoginPassword(QWidget * parent, HoBIT_Crypt * crypt)
 :QDialog(parent){
   setupUi(this);
   setObjectName("GuiLoginPassword");

   _crypt = crypt;
   _CreateNewPassword = true;
   lineEditPassword->setText("");
   lineEditPassword2->setText("");
   //adjustSize();
}


/**
*destructor of GuiLoginPassword
*/
GuiLoginPassword::~GuiLoginPassword()
{
  qDebug("Destructor GuiLoginPassword finished");
}
 /**
   *Show or hide password letters.
   */
void GuiLoginPassword::on_checkBoxShowPassword_toggled(bool checked)
{
 if(checked)
 {
   lineEditPassword->setEchoMode(QLineEdit::Normal);
   lineEditPassword2->setEchoMode(QLineEdit::Normal);
 }
else
 {
   lineEditPassword->setEchoMode(QLineEdit::Password);
   lineEditPassword2->setEchoMode(QLineEdit::Password);
 }

}

/**
* checks password
*/
void GuiLoginPassword::on_buttonLogin_clicked(){
 int passwordState =0;
 
  QString pw = lineEditPassword->text();
  
  if (_CreateNewPassword) {// no cryptfiles exists, write a new one  
      if(lineEditPassword->text() !=lineEditPassword2->text()){
        QMessageBox::critical(this, tr("Password Error"), tr("The passwords you input are not identical."));
        return;
      }
     _crypt->setPassword(pw);     
      passwordState = _crypt->savePasswordMD();
      if (passwordState != 0) 
        QMessageBox::critical(this, tr("Encryption Error"), tr("Can not save hash value."));     
  }
  else{ //check password
     _crypt->setPassword(pw);
     passwordState =  _crypt->checkPasswordMD("");
     if (passwordState == -2) //invalid password
      {
       QMessageBox::information(this, tr("Password Error"), tr("The password you input is invalid."));
      }
  }//end check password

  if (passwordState == 0) 
   accept();
}

/**
*slot for closing
*/
void GuiLoginPassword::on_buttonExit_clicked(){
  setCursor(Qt::WaitCursor);
  reject();
}

/**
*slot for the internet setup
*/
void GuiLoginPassword::slotInternetSetup(){
 QMessageBox::information(this, tr("Sorry"),tr("Not implemented yet"));	
}

/**
*shows a short help message in a mesagebox.
*/
void GuiLoginPassword::on_buttonHelp_clicked(){
   QString msg =
   tr("If there is a single edit field, enter your password in this field and press 'Return'. \
       If there are two edits, enter an identical new password into both edits. \
      <br>More help is available from the 'Help' button on top of the main window of MoneyPenny. \
       It will pop up a short time after your password has been checked.");
   QMessageBox::information(this, "MoneyPenny Help", msg);
}
  
/** Checks for an existing password file
*/
bool GuiLoginPassword::checkForPasswordFile(){
   lineEditPassword->setText("");
   lineEditPassword2->setText("");

   QFile pwFile(_crypt->getCryptDir() + "/" + MONEYPENNY_PASSWORD_FILE);

   if (pwFile.exists()) {
     _CreateNewPassword = false;
     lineEditPassword2->hide();
     labelPassword2->hide();
     labelFileDetected->setText(tr("Your password verification file has been found."));
   }
   else  {
    _CreateNewPassword = true;
     lineEditPassword2->show();
     labelPassword2->show();
   } 
   return _CreateNewPassword; 
}

/** Prepares dialog for a password change
*/
void GuiLoginPassword::switchToEditPasswordMode(){
     lineEditPassword->setText("");
     lineEditPassword2->setText("");
     lineEditPassword2->show();
     labelPassword2->show();
     labelFileDetected->setText(tr("Please input your new password into both edit fields."));
     _CreateNewPassword = true;
}


/**centers and resizes the dialog
 */
void GuiLoginPassword::showEvent(QShowEvent * ){
     int newHeight;
     if(_CreateNewPassword)
       newHeight = 550;
     else newHeight = 400;
     int newY = (qApp->desktop()->height() - newHeight) /2;
     int newX = (qApp->desktop()->width() - width()) /2;

     setGeometry(newX,newY, width(), newHeight);
}
