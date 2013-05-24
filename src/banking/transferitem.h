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
#ifndef TRANSFERITEM_H
#define TRANSFERITEM_H

#include <QString>
#include <QStringList>
#include <QDate>

#include "kernel/helpers/transfer.h"
#include "bankingjobitem.h"

class QTableWidget;

/**Qt based visualization class for a Transfer. addToTable creates a row
  *of QTableWidgetItems in a QTableWidget. Getter return QStrings.
  *Use createTransfer to build a Transfer that can be sent to the HBCI kernel.
  */
 
namespace banking{

class TransferItem;
/**Convenience container class built on QList
*/
class TransferItemList: public QList<TransferItem>{};

class TransferItem :public BankingJobItem
{
public:
  TransferItem(const Transfer & t);
  virtual ~TransferItem();
  /** Create a Transfer object that can be used with the HBCI kernel.
  */
  BankingJob * createJob(); 
  /** Encodes the Transaction data as an html- table row string.
  */
  QString  htmlTableRow(const QString & bgColor)const;
  /** Read property of QString _PurposeLine. First line is line 1
  */
  const QString & purposeLine(int line)const;
  /** Read property of QString Purpose.
  */
  const QStringList & purpose()const;
  /** Read property for customer reference. Includes translation
   */
  QString  custRef()const;
  /** Adds the transactionitem as row to table.
   */
  virtual void addToTable(QTableWidget * table, int row);
  /** Adds the transferitem as row to a table of templates (no dates)
   */
  virtual void addToTemplateTable(QTableWidget * table, int row);

  //simple getters
  const QString & otherCountry()const{return _OtherCountry;};
  const QString & otherAccountId()const{return _OtherAccountId;};
  const QString & otherBankCode()const{return _OtherBankCode;};
  const QString & otherName1()const{return _OtherName1;};
  const QString & otherName2()const{return _OtherName2;};
  const QDate & firstExecutionDate()const{return _FirstExecutionDate;};
  const QDate & lastExecutionDate()const{return _LastExecutionDate;};
  const QString  stringValue() const {return _Value.readableString().c_str();};
  const banking::Value & value() const {return _Value;};
  const QString & interval()const{return _Interval;};

 protected:
  Transfer::JobType _Type;
  QDate _FirstExecutionDate;
  QDate _LastExecutionDate;
  QString _Interval;

  banking::Value _Value;
  QString _Currency;
  QString _CustRef;
  QString _OtherName1;
  QString _OtherName2;
  QString _OtherBankCode;
  QString _OtherBic;
  QString _OtherAccountId;
  QString _OtherIban;
 
  QStringList _PurposeLines;
  QString _OtherCountry;
  int _Id;
};

}//namespace banking
#endif /*TRANSFERITEM_H*/
