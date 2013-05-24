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
#include "guiaddaccount.h"
#include <qlineedit.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <string.h>

/**
 * this is the addaccountbade constructor
 *
 *@param parent the parent dialog of this widget
 *@param stack
 *@param name widgets title
 *@param hobit
 */

GuiAddAccount::GuiAddAccount( QWidget* parent)
    :QWidget(parent), Ui::GuiAddAccountBase()
{
  setupUi(this);
  setObjectName("GuiAddAccountBase");
  qDebug("Constructor GuiAddAccount");
}

/** destructor*/
GuiAddAccount::~GuiAddAccount(){
qDebug("Destructor GuiAddAccount");
}


/**
*After the user typed in the information of the account to be added and pressed the button this function
*checks if at least one accountid is there and then read in the necessary informations for adding the account
*/
void GuiAddAccount::on_buttonOk_clicked()
{
  if (lineEditAccountId->text().isEmpty())
  {
    QMessageBox::critical(this, tr("Input Error"),
    ("You have to specify one account at least."));
    return;
  }

  QString accountId = lineEditAccountId->text();
  QString accountSubId = lineEditAccountSubId->text();
  QString accountName = lineEditAccountName->text();

  if( HoBIT::hobit()->addAccount(accountId, accountSubId, accountName) == HobitErrorSuccess){;
   QMessageBox::information(this, tr("Information"), 
                                  tr("The new account has been added to the account list."));
   HoBIT::hobit()->deleteAccount(MONEYPENNY_DUMMY_ACCOUNT_ID); //delete dummy if it exists
   emit accountAdded();
  }
  else QMessageBox::critical(this,tr("Error"), tr("Your account could not be created. See log file for details."));
}

/** Creates a dummy account, if account list is empty.
*/
void GuiAddAccount::on_buttonBack_clicked()
{
 if(HoBIT::hobit()->user() && HoBIT::hobit()->user()->accounts().empty())
   HoBIT::hobit()->addAccount(MONEYPENNY_DUMMY_ACCOUNT_ID,"", tr("No account available"));
 emit back();
}


/**
*validation for informations on account to be added.
*/
void GuiAddAccount::validateInput()
{

  bool ok;
   QString accountId = lineEditAccountId->text();
   QString accountSubId = lineEditAccountSubId->text();

    // if validation is true current input get saved to var. var is used for check next time the text get changed
   ok = HoBIT::hobit()-> validateAccountIdString(accountId);
   if(ok) 
     _lastInputAccountId = accountId;
   else lineEditAccountId->setText(_lastInputAccountId);
   
   if(ok)
     ok = HoBIT::hobit()-> validateAccountIdString(accountSubId);
   if(ok) 
     _lastInputAccountSubId = accountSubId;
   else lineEditAccountSubId->setText(_lastInputAccountSubId);
  
   if (!ok)   
     QMessageBox::warning( this,
                         tr("Error"),
                         tr("Input Error\n Only digits are allowed here. \nMaximum is 15 digits."),
                         tr("OK"), 
                         QString::null, 0, 1 ) ;
}

/**Set bankdata 
*/
void GuiAddAccount::showEvent(QShowEvent * )
{
  QString bankName = tr("Error: No bank");
  if(HoBIT::hobit()->bank())
    bankName = HoBIT::hobit()->bank()->bankName().c_str();
  labelBank->setText(bankName);
}

