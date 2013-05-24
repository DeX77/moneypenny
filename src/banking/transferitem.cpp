/***************************************************************************
     transferitem.cpp  - A Qt based visualization class for a transfer  
                             -------------------
    begin                : Fr, Jul 13th 2007
    copyright            : (C) 2007 by Walter Roth
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
 
#include "transferitem.h"
#include "kernel/helpers/transfer.h"
//#include "globals.h"
#include "valueitem.h"
#include "dateitem.h"
#include "hobit_string.h"

#include <QStringList>
#include <QDateTime>
#include <QTableWidget>
#include <QLabel>

#include <string>

using namespace banking;

static QString _EmptyString;

/**Constructor
*/ 
 TransferItem::TransferItem(const Transfer & t )
  :BankingJobItem(t){
  _FirstExecutionDate = QDate(t.firstExecutionDate().yearNumber(), t.firstExecutionDate().monthNumber(), t.firstExecutionDate().dayNumber());
  _LastExecutionDate= QDate(t.lastExecutionDate().yearNumber(), t.lastExecutionDate().monthNumber(), t.lastExecutionDate().dayNumber());

  _Type = t.type();
  _Value = t.value();
  _Currency = from8Bit(t.currency());
  _CustRef = from8Bit(t. custref());
  _OtherName1 = from8Bit(t.otherName1());
  _OtherName2 = from8Bit(t.otherName2());
  _OtherBankCode = from8Bit(t.otherBankCode());
  _OtherBic = from8Bit(t.otherBic());
  _OtherAccountId = from8Bit(t. otherAccountId());
  _OtherIban = from8Bit(t. otherIban());

  list<string>::iterator sit;
  list <string> purposeLines = t.purposeLines();
  for(sit = purposeLines.begin(); sit != purposeLines.end(); sit++){
     QString line ;
     line = from8Bit((*sit));
    _PurposeLines. append(line);
  }
  _OtherCountry = from8Bit(t.otherCountry());
  _Id = t.id();
 }

/**Destructor
*/
TransferItem::~TransferItem(){
 //do not delete table items, they are owned by table
}

/** Create a Transfer object that can be used with the HBCI kernel. 
 * This function does the typecasting for all the Qt-dependent attributes.
*/
BankingJob * TransferItem::createJob(){
  //create a list of strings
  list <string> purposeStrings;
  QStringList::const_iterator cit;
  for(cit = _PurposeLines.constBegin(); cit != _PurposeLines.constEnd(); cit++)
    purposeStrings.push_back(string(qPrintable(*cit)));

 return new Transfer(_Type,
                     _Value,
                     string(qPrintable(_OurAccountId)),
                     string(qPrintable(_OurIban)),
                     string(qPrintable(_OurBankCode)),
                     string(qPrintable(_OurBic)),
                     _BankType,
                     string(qPrintable(_OtherAccountId)),
                     string(qPrintable(_OtherIban)),
                     string(qPrintable(_OtherBankCode)),
                     string(qPrintable(_OtherBic)),
             //optional parameters
                     string(qPrintable(_OtherName1)), 
                     string(qPrintable(_OtherName2)), 
                     purposeStrings,
                     string(qPrintable(_CustRef)),
                     string(qPrintable(_Currency)));

}


/** Encodes the Transfer data as an html- table row string. Includes the most important data only.
*/
QString TransferItem::htmlTableRow(const QString & bgColor)const{
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
    result += _FirstExecutionDate.toString( Qt::LocalDate);
   result += "</td>";
   
   result += "<td>";
    result += _OtherName1;
   result += "</td>";

   result += "<td>";
    result += _OtherName2;
   result += "</td>";
   
   result += "<td>";
    result += purposeText;
   result += "</td>";
   
   result += "<td>";
    result += _OtherBankCode;
   result += "</td>";
   
   result += "<td>";
    result += _OtherAccountId;
   result += "</td>";  
  
   result += "<td align =\"right\">";
    result +=  QString(_Value.readableString().c_str()); // 2 digits after . "-999.99" 
   result += "</td>";
   
 result += "</tr>\n";
 
 return result;
}


/** Read property of QString _CustRef. 
*/
QString TransferItem::custRef()const{
    QString custRef = _CustRef;
    if(custRef == "NONREF")
      custRef= QObject::tr("No customer reference");
    return custRef;
}

/** Read property of QString _PurposeLine. First line is line 1
*/
const QString &  TransferItem::purposeLine(int nLine)const{
  if(nLine < _PurposeLines.count())
    return _PurposeLines[nLine];
  else return _EmptyString;
}


/** Read property of QString Purpose.
*/
const QStringList & TransferItem::purpose()const{
  return _PurposeLines;
}

/** Adds the transactionitem as last row to table.
   */
void TransferItem::addToTable(QTableWidget * table, int row){
  //the twi (=tablewidgetitem) prefix of the locals prevents name clashes
  QTableWidgetItem *twiType; 
  QTableWidgetItem *twiFirstExecutionDate; 
  QTableWidgetItem *twiOtherName;
  QTableWidgetItem *twiBankCode;
  QTableWidgetItem *twiAccountId;
  QTableWidgetItem *twiPurpose;
  QTableWidgetItem *twiValue;  
  QTableWidgetItem *twiLastExecutionDate; 
  QTableWidgetItem *twiCustRef;

  if(table->rowCount() < row +1)
   table->setRowCount(row +1);
  int column = 0;

  _TwiStatus = new QTableWidgetItem(statusString());
  _TwiStatus->setBackground(statusColor());
  table->setItem( row, column, _TwiStatus);
  column++;

  _TwiMessage = new  QTableWidgetItem(_Message);
  _TwiMessage->setBackground(statusColor());
  table->setItem( row, column, _TwiMessage);
  column++;

  switch (_Type) {
    case  Transfer::SingleTransfer: twiType = new QTableWidgetItem( QObject::tr("Single Transfer"));break;
    case  Transfer::DatedTransfer : twiType = new QTableWidgetItem( QObject::tr("Dated Transfer"));break;
    case  Transfer::StandingOrder : twiType = new QTableWidgetItem( QObject::tr("Standing Order"));break;
    case  Transfer::DebitNote     : twiType = new QTableWidgetItem( QObject::tr("Debitnote"));break;
    default: twiType = new QTableWidgetItem( QObject::tr("Error: Unknown Type"));break;
  }
  table->setItem( row, column, twiType);
  column++;

  if(_Type == Transfer::StandingOrder || _Type == Transfer::DatedTransfer)
    twiFirstExecutionDate = new  DateItem( _FirstExecutionDate );
  else twiFirstExecutionDate = new  DateItem(QDate::currentDate());
    table->setItem( row, column, twiFirstExecutionDate);
  column ++;

  twiOtherName = new  QTableWidgetItem( _OtherName1 +_OtherName2 );
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

  if(_Type == Transfer::StandingOrder)
    twiLastExecutionDate = new  DateItem( _LastExecutionDate );
  else twiLastExecutionDate = new  DateItem();
  table->setItem( row, column, twiLastExecutionDate);
  column ++;

  twiCustRef = new  QTableWidgetItem( _CustRef );
  table->setItem( row, column, twiCustRef);
  column ++;

  //adjust row heights for whole table later on
 }

/** Adds the transactionitem as last row to a table of templates
   */
void TransferItem::addToTemplateTable(QTableWidget * table, int row){
  //the twi (=tablewidgetitem) prefix of the locals prevents name clashes
  QTableWidgetItem *twiOtherName;
  QTableWidgetItem *twiBankCode;
  QTableWidgetItem *twiAccountId;
  QTableWidgetItem *twiPurpose;
  QTableWidgetItem *twiValue;

  if(table->rowCount() < row +1)
   table->setRowCount(row +1);
  int column = 0;

  twiOtherName = new  QTableWidgetItem( _OtherName1 + _OtherName2 );
  table->setItem( row, column, twiOtherName);
  column ++;

  twiValue = new ValueItem(0.0);
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
 }
