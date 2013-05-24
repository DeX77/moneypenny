/***************************************************************************
 *   Copyright (C) 200ACCOUNT_DESCRIPTION_ROW by Walter Roth                                     *
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
#include "guiaccountinformation.h"
#include <QTableWidget>
#include <QHeaderView>

#include <aqmoney2/account.h>
#include <openhbci2/pointer.h>
#include <openhbci2/bank.h>


#define BANK_DESCRIPTION_ROW 1
#define ACCOUNT_DESCRIPTION_ROW 5
#define NAME_ROW 7

/**
* this is the constructor of class for operations for account/and accountinformations referring to guimain
* @param parent the parent dialog of this window
* @param hobit
* @param name widgets title
*/
GuiAccountInformation::GuiAccountInformation(QWidget * parent)
    :QWidget(parent)
{
   _FillingTable = true;
   setupUi(this);
  setObjectName("GuiAccountInformation");
  qDebug("Constructor GuiAccountInformation");
}


/**
 * destructor
*/
GuiAccountInformation::~GuiAccountInformation()
{
  qDebug("Destructor GuiAccountInformation");
}



/*!
    \fn GuiAccountInformation::refresh()
 */

/**
* get/refresh information about current account
*/
void GuiAccountInformation::refresh()
{
    banking::Bank * bank = HoBIT::hobit()->database()->currentBank();
    banking::Account * account= HoBIT::hobit()->database()->currentAccount();
    _AccountId = "";

  if ((account!= NULL) && (bank != NULL))
  {
    _AccountId = account->accountId().c_str();

    QString accountName= account->accountName().c_str();
    labelAccount->setText(accountName);
    if (accountName.isEmpty()) 
       accountName = tr("no description selected");

    QString bankId = bank->bankCode().c_str();
    QString bankBIC = bank->bankBic().c_str();
    QString bankName = HoBIT::hobit()-> getBankNameForId(bankId); //get the official name from the list
    QString userDefinedBankName = QString::fromUtf8(bank->bankName().c_str());
    // get Balances
    QString noted;// = QString(account.ref().notedBalance().value().toReadableString().c_str());
    banking::Value balance = account->balance();
    QString booked = QString(balance.readableString().c_str());
    QString creditLine = account->accountLimit().readableString().c_str();
    QString owner = account->ownerName().c_str();
    QString custId = QString( bank->currentUserId().c_str());
    QString securityMode;
    banking::User * user = HoBIT::hobit()->user();
    if(user)
      securityMode = user->passportTypeString().c_str();

    _FillingTable = true;
    int row = 0;
    tableWidget->setItem(row, 0,new QTableWidgetItem( bankName ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( userDefinedBankName ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( bankId ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( bankBIC ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( securityMode ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( accountName ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( _AccountId ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( owner ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( custId ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( booked ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( noted ));
    row++;
    tableWidget->setItem(row, 0,new QTableWidgetItem( creditLine ));
    _FillingTable = false;

    int tableWidth= tableWidget->verticalHeader()->width()+10;
    for (int column = 0; column < tableWidget->columnCount(); column ++){
      tableWidget->resizeColumnToContents(column);
      tableWidth += tableWidget->columnWidth(column);
    }
    int tableHeight= tableWidget->horizontalHeader()->height() +10;
    for (int row = 0; row < tableWidget->rowCount(); row ++){
      tableHeight += tableWidget->rowHeight(row);
    }
    QSize size = QSize(tableWidth, tableHeight);
    if(qApp->desktop()->height() > 700) //DO NOT adjust height on small screens (EEE-PC)
      tableWidget->setMinimumSize(size);
  }
}

/**
* delete account referring to guimain
*/
void GuiAccountInformation::on_buttonDeleteAccount_clicked()
{
  if(QMessageBox::question(this,tr("Delete Account"), tr("Delete account", "first part of: Delete account account-number?")
    + " " +  _AccountId + tr("?") + "<br>" + tr("All data for this account will be deleted."), 
    tr("Yes"), tr("No"), QString::null, 1)== 0)
      emit deleteAccount(_AccountId);
}

/**
* add account referring to guimain
*/
void GuiAccountInformation::on_buttonAddAccount_clicked()
{
  emit addAccount();
}

/** Used to edit account description
 */
void GuiAccountInformation::on_tableWidget_cellChanged(int row, int column){
  if(_FillingTable)
   return;

  if(row == ACCOUNT_DESCRIPTION_ROW && column == 0){
    if(QMessageBox::question(this, tr("Confirmation"), 
     tr("Change account description to: ") + tableWidget->item(ACCOUNT_DESCRIPTION_ROW, 0)->text(), 
       QMessageBox::Yes | QMessageBox::No 
       ) == QMessageBox::Yes)
    HoBIT::hobit()->setCurrentAccountName(tableWidget->item(ACCOUNT_DESCRIPTION_ROW, 0)->text());
    emit dataModified();
  }
  if(row == NAME_ROW && column == 0){
    if(QMessageBox::question(this, tr("Confirmation"), 
     tr("Change owner name to: ") + tableWidget->item(NAME_ROW, 0)->text(), 
       QMessageBox::Yes | QMessageBox::No 
       ) == QMessageBox::Yes)
    HoBIT::hobit()->currentAccount()->setOwnerName(qPrintable(tableWidget->item(NAME_ROW, 0)->text()));
    emit dataModified();
  }
  if(row == BANK_DESCRIPTION_ROW && column == 0){
    if(QMessageBox::question(this, tr("Confirmation"), 
     tr("Change bank description to: ") + tableWidget->item(BANK_DESCRIPTION_ROW, 0)->text(), 
       QMessageBox::Yes | QMessageBox::No 
       ) == QMessageBox::Yes)
    HoBIT::hobit()->setUserDefinedBankName(tableWidget->item(BANK_DESCRIPTION_ROW, 0)->text());
    emit dataModified();
  }
}

/**
* change account referring to guimain
*/
void GuiAccountInformation::on_buttonEditAccountDescription_clicked()
{
   tableWidget->editItem(tableWidget->item(ACCOUNT_DESCRIPTION_ROW, 0));
}

/**
* switch account referring to guimain
*/
void GuiAccountInformation::on_buttonChangeAccount_clicked()
{
  emit changeAccount();
}

/** Raise transaction list
*/
void GuiAccountInformation::on_buttonShowTransactions_clicked(){
  emit showTransactions();
}
