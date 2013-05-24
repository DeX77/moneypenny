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
 
#include "transactionitem.h"
#include "kernel/helpers/transaction.h"
#include "valueitem.h"
#include "dateitem.h"

#include <QStringList>
#include <QDateTime>
#include <QTableWidget>
#include <QLabel>
#include <QTextDocument>
#include "globals.h"

#include <string>

using namespace banking;

/**Constructor
*/ 
 TransactionItem::TransactionItem(const Transaction & t )
 :TransferItem(t){
  _ValutaDate = QDate(t.valutaDate().yearNumber(), t.valutaDate().monthNumber(), t.valutaDate().dayNumber());
  _BookingDate= QDate(t.bookingDate().yearNumber(), t.bookingDate().monthNumber(), t.bookingDate().dayNumber());
  _StoreDate  = QDate(t.storeDate().yearNumber(), t.storeDate().monthNumber(), t.storeDate().dayNumber());

  _Key = from8Bit(t. key());
  _TextKey = from8Bit(t. textkey());
  _Text = from8Bit(t. text());
  _Primanota = from8Bit(t. primanota());
 }

/**Destructor
*/
TransactionItem::~TransactionItem(){
 //do not delete table items, they are owned by table
}


/** Returns purpose as a single line string.
 */
QString TransactionItem::purposeString(){
  QStringList::const_iterator line;
  QString purposeText;
  for(line = _PurposeLines.begin(); line!= _PurposeLines.end(); line++) {//chain the lines together
   if(!(*line).isEmpty())
     purposeText += (*line);
  } 
  return purposeText;
}


/** Returns a CSV formatted line of data.
 */
QString TransactionItem::toCSV(QChar separator){
  QString result;
   result += _BookingDate.toString( Qt::LocalDate) + separator;
   result += _OtherName1 + separator;
   result += _OtherName2 + separator;
   result += QString(_Value.readableString().c_str()) + separator;
   result += purposeString() + separator;
   result += _OtherBankCode + separator;
   result += _OtherAccountId + separator;
   result += _Text + separator;
   result += _BookingDate.toString( Qt::LocalDate) + separator;
   result += _ValutaDate.toString( Qt::LocalDate) + separator;
   result += _StoreDate.toString( Qt::LocalDate) + separator;
   result += _Primanota + separator;
   result += _CustRef + separator;
   result += _Id;
   return result;
}

/** Encodes the Transaction data as an html- table row string. Includes the most important data only.
*/
QString TransactionItem::htmlTableRow(const QString & bgColor)const{
  QString result;
  
  QStringList::const_iterator line;
  QString purposeText;
  for(line = _PurposeLines.begin(); line!= _PurposeLines.end(); line++) {//chain the lines together
   if(!(*line).isEmpty())
     purposeText += (*line) + "<br>";
  } 
  purposeText.truncate(purposeText.length() -4); //delete last <br>
  
   result += "<tr bgcolor =\"" + bgColor + "\">";
   
   result += "<td>";
    result += _BookingDate.toString( Qt::LocalDate);
   result += "</td>";
   
   result += "<td>";
    result += _OtherName1 + _OtherName2;
   result += "</td>";
   
   result += "<td>";
    result += purposeText;
   result += "</td>";
   
   result += "<td>";
    result += _Text;
   result += "</td>";
   
   result += "<td>";
    result += _OtherBankCode;
   result += "</td>";
   
   result += "<td>";
    result += _OtherAccountId;
   result += "</td>";  
  
   result += "<td align =\"right\">";
    result += QString(_Value.readableString().c_str());
   result += "</td>";
   
 result += "</tr>\n";
  
 Qt::escape(result); //escape / & and <>
 return result;
}

/** Read property of QString _Text. 
*/
const QString &  TransactionItem::text()const{
    return _Text;
}

/** Read property of QString _ValutaDate. 
*/
const QDate & TransactionItem::valutaDate()const{
    return _ValutaDate;
}

/** Read property of QString _BookingDate. 
*/
const QDate & TransactionItem::bookingDate()const{
    return _BookingDate;
}

/** Read property of QString _StoreDate. 
*/
const QDate & TransactionItem::storeDate()const{
    return _StoreDate;
}

/** Read property of QString _Primanota. 
*/
const QString &  TransactionItem::primanota()const{
    return _Primanota;
}


/** Adds the transactionitem as last row to table.
   */
void TransactionItem::addToTable(QTableWidget * table, int row){
  //the twi (=tablewidgetitem) prefix of the locals prevents name clashes
  QTableWidgetItem *twiDate; //this is the valuta date
  QTableWidgetItem *twiOtherName;
    // QTableWidgetItem *custRef;
  QTableWidgetItem *twiBankCode;
  QTableWidgetItem *twiAccountId;
    // QTableWidgetItem *textKey;
    // QTableWidgetItem *key;
  QTableWidgetItem *twiPurpose;
  QTableWidgetItem *twiText;
  QTableWidgetItem *twiValue;  
  QTableWidgetItem *twiBookingDate; 
  QTableWidgetItem *twiStoreDate;
  QTableWidgetItem *twiValutaDate; //copy of valuta date
  QTableWidgetItem *twiPrimanota;
  QTableWidgetItem *twiCustRef;
  QTableWidgetItem *twiId;

  int column = 0;
  if(table->rowCount() < row +1)
   table->setRowCount(row +1);

  twiDate = new  DateItem( _ValutaDate );
  table->setItem( row, column, twiDate);
  column ++;
  #ifdef MESSE
  twiOtherName = new  QTableWidgetItem( _OtherName.left(6) + "... Datenschutz");
  #else
  twiOtherName = new  QTableWidgetItem( _OtherName1 + _OtherName2 );
  #endif
  table->setItem( row, column, twiOtherName);
  column ++;

  twiValue = new ValueItem(_Value);
  table->setItem( row, column, twiValue);
  column ++;

  QString purposeText = _PurposeLines.join("\n");
  purposeText= purposeText.trimmed(); //remove trailing empty lines
  
  twiPurpose = new  QTableWidgetItem( purposeText );
  table->setItem( row, column, twiPurpose);
  column ++;

  twiBankCode = new  QTableWidgetItem( _OtherBankCode );
  table->setItem( row, column, twiBankCode);
  column ++;

  twiAccountId = new  QTableWidgetItem( _OtherAccountId);
  table->setItem( row, column, twiAccountId);
  column ++;

  twiText = new  QTableWidgetItem( _Text );
  table->setItem( row, column, twiText);
  column ++;

  twiBookingDate = new  DateItem( _BookingDate );
  table->setItem( row, column, twiBookingDate);
  column ++;

  twiValutaDate = new  DateItem( _ValutaDate );
  table->setItem( row, column, twiValutaDate);
  column ++;

  //store date is the date at which the data have been stored by OpenHbci
  twiStoreDate = new  DateItem( _StoreDate ); 
  table->setItem( row, column, twiStoreDate);
  column ++;

  twiPrimanota = new  QTableWidgetItem( _Primanota );
  table->setItem( row, column, twiPrimanota);
  column ++;

  twiCustRef = new  QTableWidgetItem( _CustRef );
  table->setItem( row, column, twiCustRef);
  column ++;

  twiId = new  QTableWidgetItem(QString::number( _Id) );
  table->setItem( row, column, twiId);
  column ++;
  //adjust row heights for whole table later on
 }

