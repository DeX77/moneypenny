/***************************************************************************
                          guigettransactions.cpp  -  description
                             -------------------
    begin                : Sa Nov 6 2004
    copyright            : (C) 2004 by Marcel Mulch
    email                : marcel@mulch-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "guigettransactions.h"
#include <QComboBox>
#include <QDateTimeEdit>
#include <QString>
#include <QRadioButton>
#include <QDebug>

#include <openhbci2.h>
#include <openhbci2/date.h>
using namespace HBCI;

/**
*this is the constructor for transactions to transaction list
*
*@param parent the parent dialog of this widget
*@param hobit
*/
GuiGetTransactions::GuiGetTransactions( QWidget *parent)
:QWidget(parent) {
   setupUi(this);
   setObjectName("GuiGetTransactions"); 
  qDebug("Constructor GuiGetTransActions");
}

GuiGetTransactions::~GuiGetTransactions(){
qDebug("Destructor GuiGetTransactions finished");
}

/** 
* Look which transactions the user chose for Get and get them!
*/

void GuiGetTransactions::on_buttonGet_clicked(){
    QDate fromDate = dateEditFromDate->date();
    QDate toDate = dateEditToDate->date();

    if(radioButtonAllAccounts->isChecked())    
       HoBIT::hobit()->createJobGetBalance("*"); // get all balances
    else HoBIT::hobit()->createJobGetBalance(HoBIT::hobit()->currentAccountId());  
    if(! (checkBoxBalancesOnly->isChecked())){
       qDebug()<< "Get transactions from: " << fromDate.toString("dd.MMM.yyyy") << " to: " << toDate.toString("dd.MMM.yyyy");
       if (radioButtonAllAccounts->isChecked()) 
           HoBIT::hobit()->createJobGetTransactions(fromDate, toDate, "*");
       else 
           HoBIT::hobit()->createJobGetTransactions(fromDate, toDate, HoBIT::hobit()->database()->currentAccountId().c_str());
    }
    emit jobsCreated(); 
    emit raiseWidget(JOBS_WIDGET);
    emit sendNow(NULL, tr("Requesting transaction data from the bank."), true);
 }

 /** Called on show. Sets current account.
 */
 void GuiGetTransactions::showEvent ( QShowEvent * ){
   QDate currentDate; currentDate = currentDate.currentDate();
   QDate lastYear = currentDate.addYears(-1);
   QDate lastTransactionDate = HoBIT::hobit()->lastDateOfTransactions();
   if(lastTransactionDate < lastYear)
      lastTransactionDate = lastYear;
   dateEditFromDate->setDate(lastTransactionDate);
   dateEditToDate->setDate(currentDate);
   QString accountId = HoBIT::hobit()->currentAccountId();
   if(accountId.isEmpty())
     accountId = tr("unknown");
   radioButtonAccount->setText(tr("For Account:") + " " + accountId);
   if(lastTransactionDate > currentDate){ //lastTransactionDate may be in the future!! fix it here.
     lastTransactionDate = currentDate; 
     radioButtonUpdate-> setEnabled(false);
     radioButtonDated->setChecked(true); 
   } 
   else{
     radioButtonUpdate->setEnabled(true);
     radioButtonUpdate->setChecked(true);
   }
 }

/** Called on Cancel clicked. Sends signal cancelled.
*/
void GuiGetTransactions::on_buttonCancel_clicked(){
    emit canceled();
}

/**Enables /disables Dateedits
*/
void GuiGetTransactions::on_radioButtonDated_toggled(bool checked){
   dateEditFromDate->setEnabled(checked);
   dateEditToDate->setEnabled(checked);
}

