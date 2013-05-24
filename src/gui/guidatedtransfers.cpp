/***************************************************************************
                          guidatedtransfers.cpp  -  description
                             -------------------
    begin                : Do Nov 11 2004
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

#include <QDate>
#include <QComboBox>
#include <QTableWidget>

#include "guidatedtransfers.h"
#include "datedtransfer.h"
#include "hobit.h"
#include "transferitem.h"
  /**
  *this is the constructor for the dialog
  *
  *@param parent the parent dialog of this widget
  *@param HoBIT::hobit() the banking engine
  */
GuiDatedTransfers::GuiDatedTransfers(QWidget *parent)
: QWidget(parent) {
   setupUi(this);
   setObjectName("GuiDatedTransfers");
   QDate currentDate= QDate::currentDate();
   dateEdit->setDate(QDate(currentDate.year() +1, currentDate.month(), currentDate.day()));
   textLabelAccount->setText(HoBIT::hobit()->currentAccountId());
}

GuiDatedTransfers::~GuiDatedTransfers(){
   qDebug("Destructor GuiDatedTransfers finished");
}



/** 
* updates the listview depending on the selected account 
*/
void GuiDatedTransfers::slotChangeAccount(){
    banking::TransferList * datedTransfers = (banking::TransferList *)HoBIT::hobit()->listTransactions();//HoBIT::hobit()->listDatedTransfers();
    buildTable( datedTransfers ); 
    delete datedTransfers;
}


/** 
* create list for dated transfers
*/
void GuiDatedTransfers::buildTable(banking::TransferList * datedTransfers){

  datedTransferList.clear();
  tableWidget->setRowCount (0); //delete old transfers
  banking::TransferList::iterator it;
  int rows = 0;

  for(it = datedTransfers->begin(); it != datedTransfers->end(); it++)
     rows++;
  tableWidget->setRowCount(rows); //resize to new number of transfers

  int row = 0;
  for (it= datedTransfers->begin(); it!= datedTransfers->end(); it++){
    banking::TransferItem item = banking::TransferItem(**it);
    item.addToTable(tableWidget, row);
    datedTransferList.append(item);
    row++;
  }

  tableWidget->setColumnHidden(1, true); // hide message column
  tableWidget->setColumnHidden(tableWidget->columnCount()-1, true); // hide id column
  tableWidget->resizeColumnsToContents();
  tableWidget->resizeRowsToContents();
}


/** 
* creates job of dated transfers and send 
*/
void GuiDatedTransfers::on_buttonGetList_clicked(){
      HoBIT::hobit()->createJobGetDatedTransfers(QDate::currentDate(), 
                                        dateEdit->date(), 
                                        HoBIT::hobit()->currentAccountId());
     bool ok = false;
     emit  sendJobsNow(&ok, tr("Asking bank for list of pending dated transfers."), true);
}


 /** 
  *executed when user performs a double click on a list item.
  *shows details of the item
  */
void GuiDatedTransfers::on_tableWidget_cellDoubleClicked ( int row, int )
{
  banking::TransferItem dt = datedTransferList.at(row);
   emit showInformation(dt);
}


/** 
* raises widget of dated single transfer formular 
*/
void GuiDatedTransfers::on_buttonNewDatedTransfer_clicked(){
  emit newDatedTransferRequired();
}


/** 
* deletes an existing dated transfer job
*/
void GuiDatedTransfers::slotDeleteDatedTransfer(){

 int currentRow = tableWidget->currentRow();
 bool ok = true;
 if(currentRow>=0)
 {
  QString text = tr("You are about to delete the selected dated transfer.");
  bool  keep =  QMessageBox::question(  this,
                                        tr("Delete Dated Transfer"),
                                        text,
                                        tr("&Yes"), tr("&No"), QString::null, 0, 1 );

  if (!keep){
     QTableWidgetItem * idItem = tableWidget->item(currentRow, tableWidget->columnCount() -1);
     int selectedId = idItem->text().toInt();
     banking::TransferList * datedTransfers = HoBIT::hobit()->listDatedTransfers();
     banking::Transfer * transfer = NULL;
     banking::TransferList::iterator it;
   
     for (it= datedTransfers->begin(); it!= datedTransfers->end(); it++)
     {
       if((*it)->id() == selectedId)
         transfer = *it;
     }
     if(transfer != NULL)
     {
        HoBIT::hobit()->createJobDeleteDatedTransfer(transfer);
        HoBIT::hobit()->createJobGetDatedTransfers(QDate::currentDate(), dateEdit->date(), HoBIT::hobit()->currentAccountId());
        emit sendJobsNow(&ok, tr("Deleting dated transfer and updating list."), false);
     }
     else ok = false;
     delete datedTransfers;
  }
  if(!ok)
   QMessageBox::critical(this, tr("Error"), tr("An error ocurred. Transfer was probably not deleted."));
 }
 else QMessageBox::critical(this, tr("Problem"), tr("You did not select an item"));
}

/** 
* set accountlabel to current account
*/
void GuiDatedTransfers::refresh()
{  
  if(HoBIT::hobit()->currentAccountId() != textLabelAccount->text())
  {
    textLabelAccount->setText(HoBIT::hobit()->currentAccountId());
    slotChangeAccount();
  }
}
