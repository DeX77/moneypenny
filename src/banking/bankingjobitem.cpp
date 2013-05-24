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
 
#include "bankingjobitem.h"
#include "kernel/helpers/bankingjob.h"
#include "../global/globals.h"
#include "valueitem.h"
#include "dateitem.h"

#include <QStringList>
#include <QDateTime>
#include <QTableWidget>
#include <QLabel>
#include <QApplication>

#include <string>

using namespace banking;
#include <QTextCodec>

//Encoding functions

/**The default encoding settings for Hbci 2.X to FinTS 3.0
  *FinTS 4.0 needs Utf8!
  *This is completely independent of the system wide enconding settings in _ToEncoding and _FromEncoding
  */
BankingEncoding _BankingFromEncoding = Encoding_Iso8859;
BankingEncoding _BankingToEncoding = Encoding_Iso8859;
QTextCodec * codec = QTextCodec::codecForName("ISO 8859-15");

/** Convert from 8bit string using global encoding setting _BankingFromEncoding
 */
QString banking::from8Bit(const string & s){
 if(_BankingFromEncoding == Encoding_Utf8)
    return QString::fromUtf8(s.c_str()); 
 
 return codec->toUnicode(s.c_str()); //default
}

/** Convert to 8bit string using global encoding setting _BankingToEncoding
 */
string banking::to8Bit(const QString & s){
  if(_BankingToEncoding == Encoding_Utf8)
    return s.toUtf8().data();

  return codec->fromUnicode(s).data(); //default
}


//class BankingJob

/**Constructor
*/ 
 BankingJobItem::BankingJobItem(const BankingJob & bj ){
   _Type = bj.type();
  _StartDate = QDate(bj.startDate().yearNumber(), bj.startDate().monthNumber(), bj.startDate().dayNumber());
  _EndDate= QDate(bj.endDate().yearNumber(), bj.endDate().monthNumber(), bj.endDate().dayNumber());

  _OurBankCode = from8Bit(bj. ourBankCode());
  _OurBic = from8Bit(bj. ourBic());
  _BankType = bj.ourBankType();
  _OurAccountId = from8Bit(bj.ourAccountId());
  _OurIban = from8Bit(bj.ourIban());
  _Id = bj.id();
  _Message = from8Bit(bj.message());
  _TwiStatus = NULL;
  _TwiMessage = NULL;
  setStatus(bj.status());
 }

/**Destructor
*/
BankingJobItem::~BankingJobItem(){
 //do not delete table items, they are owned by table
}

/** Create a BankingJob object that can be used with the HBCI kernel. 
 * This function does the typecasting for all the Qt-dependent attributes.
*/
BankingJob * BankingJobItem::createJob(){
 BankingJob * job = new BankingJob(_Type,
                     string(qPrintable(_OurAccountId)),
                     string(qPrintable(_OurBankCode))
                      );
 job->setId(_Id);
 return job;
}


/** Encodes the Transfer data as an html- table row string. Includes the most important data only.
*/
QString BankingJobItem::htmlTableRow(const QString & bgColor)const{
  QString result;
  
   result += "<tr bgcolor =\"" + bgColor + "\">";

   
   result += "<td>";
    result += QString(BankingJob::typeToString(_Type).c_str());
   result += "</td>";

   result += "<td>";
    result += _OurBankCode;
   result += "</td>";

   result += "<td>";
    result += _OurAccountId;
   result += "</td>";
   
   result += "<td>";
    result += _StartDate.toString( Qt::LocalDate);
   result += "</td>";
   
   result += "<td>";
    result += _EndDate.toString( Qt::LocalDate);
   result += "</td>";
 result += "</tr>\n";
 
 return result;
}




/** Adds the item as last row to table.
   */
void BankingJobItem::addToTable(QTableWidget * table, int row){
  //the twi (=tablewidgetitem) prefix of the locals prevents name clashes
 
  QTableWidgetItem *twiType; 
  QTableWidgetItem *twiStartDate; 
  QTableWidgetItem *twiBankCode;
  QTableWidgetItem *twiAccountId;
  QTableWidgetItem *twiEndDate; 

  if(table->rowCount() < row +1)
   table->setRowCount(row +1);
  int column = 0;

  _TwiStatus = new QTableWidgetItem(statusString());
  _TwiStatus->setBackground(statusColor());
  table->setItem( row, column, _TwiStatus);
  column++;

  _TwiMessage = new  QTableWidgetItem( _Message );
  _TwiMessage->setBackground(statusColor());
  table->setItem( row, column, _TwiMessage);
  column ++;

  switch (_Type) {
    case  BankingJob::FirstKeyRequest: twiType = new QTableWidgetItem( QObject::tr("Get bank keys"));break;
    case  BankingJob::GetBPD: twiType = new QTableWidgetItem( QObject::tr("Get bank parameters"));break;
    case  BankingJob::GetNewSystemId: twiType = new QTableWidgetItem( QObject::tr("Get new system id"));break;
    case  BankingJob::Resynchronize: twiType = new QTableWidgetItem( QObject::tr("Resynchronize active key"));break;
    case  BankingJob::FirstKeySend : twiType = new QTableWidgetItem( QObject::tr("Send public keys for the first time"));break;
    case  BankingJob::KeyChange : twiType = new QTableWidgetItem( QObject::tr("Replace public keys"));break;
    case  BankingJob::GetBalance : twiType = new QTableWidgetItem( QObject::tr("Get balance"));break;
    case  BankingJob::GetTransactions     : twiType = new QTableWidgetItem( QObject::tr("Get transactions"));break;
    default: twiType = new QTableWidgetItem( QObject::tr("Error: Unknown type"));break;
  }
  table->setItem( row, column, twiType);
  column++;

  twiBankCode = new  QTableWidgetItem( _OurBankCode );
  table->setItem( row, column, twiBankCode);
  column ++;

  twiAccountId = new  QTableWidgetItem( _OurAccountId);
  table->setItem( row, column, twiAccountId);
  column ++;

  if(_Type == BankingJob::GetTransactions)
    twiStartDate = new  DateItem( _StartDate );
  else twiStartDate = new  DateItem();
    table->setItem( row, column, twiStartDate);
  column ++;


  if(_Type == BankingJob::GetBalance || _Type == BankingJob::GetTransactions)
    twiEndDate = new  DateItem( _EndDate );
  else twiEndDate = new  DateItem();
  table->setItem( row, column, twiEndDate);
  column ++;

  //adjust row heights for whole table later on
 }

/** Returns a translated string describing the status of the job.
 */
QString BankingJobItem::statusString(){
   QString sStatus;
   switch(_Status){
     case BankingJob::JobNew: sStatus = QObject::tr("New");break;
     case BankingJob::JobQueued: sStatus = QObject::tr("Queued");break;
     case BankingJob::JobSent: sStatus = QObject::tr("Sent");break;
     case BankingJob::JobExecuted: sStatus = QObject::tr("Executed");break;
     case BankingJob::JobFailed: sStatus = QObject::tr("Failed");break;
     case BankingJob::JobConfirmed: sStatus = QObject::tr("Confirmed");break;
     default: sStatus = QObject::tr("Unknown");break;
   }
  return sStatus;
}

/** Returns a color palette indicating new, in kernel, ok or fail.
 */
QColor  BankingJobItem::statusColor(){
   switch(_Status){
     case BankingJob::JobNew: return Qt::white;
     case BankingJob::JobQueued: return Qt::yellow;
     case BankingJob::JobSent: return Qt::yellow;
     case BankingJob::JobExecuted: return Qt::green;
     case BankingJob::JobFailed: return Qt::red;
     case BankingJob::JobConfirmed: return Qt::green;
     case BankingJob::JobWarning: return Qt::magenta;
     default: return Qt::red;
   }
}

/** Sets the new status and updates _TwiStatus
*/
void BankingJobItem::setStatus(BankingJob::JobStatus s){
  _Status = s;
//   cerr << "BankingJobItem::setStatus id: "<< _Id  << " type: " << BankingJob::typeToString(_Type) << " status: " << qPrintable(statusString()) << endl;
   if (_TwiStatus){
    _TwiStatus->setText(statusString());
    _TwiStatus->setBackground(statusColor());
    qApp->flush();
    qApp->processEvents();
   }
}

/** Sets the new message and updates _TwiMessage
*/
void BankingJobItem::setMessage(const QString & msg){
   _Message = msg;
   if (_TwiMessage){
    _TwiMessage->setText(msg);
    _TwiMessage->setBackground(statusColor());
    qApp->flush();
    qApp->processEvents();
   }
}
