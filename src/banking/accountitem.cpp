/***************************************************************************
                          transactionitem.cpp  -  description
                             -------------------
    begin                : Fr, march 24th 2006
    copyright            : (C) 2006 by Walter Roth
    email                : moneypenny@in-fh-swf.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include "accountitem.h"
#include "kernel/helpers/account.h"
#include "valueitem.h"
#include "dateitem.h"
#include "bankingjobitem.h"

#include <QDateTime>
#include <QTableWidget>

#include <string>

using namespace banking;


/**Constructor
*/ 
 AccountItem::AccountItem(const banking::Account & a )
 { 
  _BankCode = a.bankCode().c_str();
  _BankName = ""; //set separately
  _AccountId = a.accountId().c_str();
  _AccountName = banking::from8Bit(a.accountName().c_str());
  _Balance = a.balance();
  _BalanceDate = QDate(a.balanceDate().yearNumber(), a.balanceDate().monthNumber(), a.balanceDate().dayNumber());
 }

/**Destructor
*/
AccountItem::~AccountItem(){
 //do not delete table items, they are owned by table
}


/** Encodes the Account data as an html- table row string. Includes the most important data only.
*/
QString AccountItem::htmlTableRow(const QString & bgColor)const{
  QString result;

   result += "<tr bgcolor =\"" + bgColor + "\">";

   result += "<td>";
    result += _BankName;
   result += "</td>";

   result += "<td>";
    result += _BankCode;
   result += "</td>";

   result += "<td>";
    result += _AccountName;
   result += "</td>";

   result += "<td>";
    result += _AccountId;
   result += "</td>";

   result += "<td align =\"right\">";
    result += QString( _Balance.readableString().c_str()); // 2 digits after . "-999.99" 
   result += "</td>";
   
   result += "<td>";
    result += _BalanceDate.toString( Qt::LocalDate);
   result += "</td>";

 result += "</tr>\n";
 
 return result;
}



/** Adds the AccountItem as last row to table.
   */
void AccountItem::addToTable(QTableWidget * table, int row){
  //the twi (=tablewidgetitem) prefix of the locals prevents name clashes
  QTableWidgetItem *twiBalanceDate; //this is the balance date
  ValueItem *twiBalance; 
  QTableWidgetItem *twiBankName;
  QTableWidgetItem *twiBankCode;
  QTableWidgetItem *twiAccountName;
  QTableWidgetItem *twiAccountId;

  if(table->rowCount() < row +1)
   table->setRowCount(row +1);

  int column = 0;
  twiBankName = new  QTableWidgetItem( _BankName );
  table->setItem( row, column, twiBankName);
  column ++;

  twiBankCode = new  QTableWidgetItem( _BankCode );
  table->setItem( row, column, twiBankCode);
  column++;

  twiAccountName = new  QTableWidgetItem( _AccountName );
  table->setItem( row, column, twiAccountName);
  column ++;

  twiAccountId = new  QTableWidgetItem( _AccountId );
  table->setItem( row, column, twiAccountId);
  column ++;

  twiBalance = new  ValueItem( _Balance );
  table->setItem( row, column, twiBalance);
  column ++;

  twiBalanceDate = new  DateItem( _BalanceDate, QObject::tr("dd.MM.yyyy", "The date format for the account list") );
  table->setItem( row, column, twiBalanceDate);
  column ++;

  //adjust row heights for whole table later on
 }

