/***************************************************************************
                          transactionitem.h  -  description
                             -------------------
    begin                : Fr, march 24th 2006, 2007
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
#ifndef TRANSACTIONITEM_H
#define TRANSACTIONITEM_H

#include <QString>
#include <QStringList>
#include <QDate>

#include "transferitem.h"

class QTableWidget;

namespace banking{ 
class Transaction;
class TransactionItem;
/**Convenience container class built on QList
*/
class TransactionItemList: public QList<TransactionItem>{};

/**Visualization class for a Transaction. addToTable creates a row
  *of QTableWidgetItems in a QTableWidget. Getter return QStrings.
  */

class TransactionItem: public TransferItem {
public:
 // TransactionItem();
  TransactionItem(const Transaction & t);
  //TransactionItem(QTable * table, int row, Pointer<Transaction> dt);
  virtual ~TransactionItem();
  /** Encodes the Transaction data as an html- table row string.
  */
  QString  htmlTableRow(const QString & bgColor)const;
  /** Read property of QString _Text. 
  */
  const QString & text()const;
  /** Read property of QString _Primanota. 
  */
  const QString & primanota()const;
  /** Read property of QString _CustRef. 
  */
  QString custRef()const;
  /** Read property of QString _ValutaDate. 
  */
  const QDate & valutaDate()const;
  /** Read property of QDate _BookingDate. 
  */
  const QDate & bookingDate()const;
  /** Read property of QDate _StoreDate. 
   * Store date is the date at which the data have been stored by OpenHbci
  */
  const QDate & storeDate()const;
  /** Set the _Id
   */
   virtual void setId(int newId){_Id = newId;};
 
  /** Adds the transactionitem as row to table.
   */
  void addToTable(QTableWidget * table, int row);
  /** Returns a CSV formatted line of data.
   */
  virtual QString toCSV(QChar separator); 
  /** Returns purpose as a single line string.
   */
  virtual QString purposeString(); 
 private:
  QDate _ValutaDate;
  QDate _BookingDate;
  QDate _StoreDate;

  QString _Key;
  QString _TextKey;
  QString _Text;
  QString _Primanota;
};

} //namespace
#endif /*TRANSACTIONITEM_H_*/
