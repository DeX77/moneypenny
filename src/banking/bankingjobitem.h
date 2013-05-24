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
#ifndef BANKINGJOBITEM_H
#define BANKINGJOBITEM_H

#include <QString>
#include <QStringList>
#include <QDate>

#include "kernel/helpers/transfer.h"

class QTableWidget;
class QTableWidgetItem;
class QColor;

/**Qt based visualization class for a Transfer. addToTable creates a row
  *of QTableWidgetItems in a QTableWidget. Getter return QStrings.
  *Use createTransfer to build a Transfer that can be sent to the HBCI kernel.
  */
 
namespace banking{

/**global string conversion for banking related strings
  */
    typedef enum {
      Encoding_Iso8859,
      Encoding_Utf8
    } BankingEncoding;


  /** Convert from 8bit string using global encoding setting _BankingFromEncoding
   */
  QString from8Bit(const string & s);
  /** Convert to 8bit string using global encoding setting _BankingToEncoding
   */
  string to8Bit(const QString & s);


class BankingJobItem;
/**Convenience container class built on QList
*/
class BankingJobItemList: public QList<BankingJobItem>{};

class BankingJobItem {
public:

  BankingJobItem(const BankingJob & bj);
  virtual ~BankingJobItem();
  /** Create a BankingJob object that can be used with the HBCI kernel.
  *   Return value is used polymorphically. May be Transfer or Transaction.
  */
  virtual BankingJob * createJob(); 
  /**Returns type
   */
  Transfer::JobType type()const{return _Type;};
  /** Encodes the Transaction data as an html- table row string.
  */
  QString  htmlTableRow(const QString & bgColor)const;
  /** Adds the BankingJob as row to table.
   */
  virtual void addToTable(QTableWidget * table, int row);

//simple getters
  int id()const{return _Id;};
  int ourCountry()const{return _OurCountry;};
  const QString & ourAccountId()const{return _OurAccountId;};
  const QString & ourIban()const{return _OurIban;};
  const QString & ourBankCode()const{return _OurBankCode;};
  const QDate & startDate() const{return _StartDate;};
  const QDate & endDate() const{return _EndDate;};

  /**Set id to new value. This is neccessary, because job ids
   * are set when job is queuded.
   */
  void setId(int id){_Id = id;};

  /** Sets the new status and updates _TwiStatus
  */
  void setStatus(BankingJob::JobStatus s);
  /** Sets the new message and updates _TwiMessage
  */
  void setMessage(const QString & msg);
 protected:
  /** Returns a translated string describing the status of the job.
   */
  QString statusString();
  /** Returns a color palette indicating new, in kernel, ok or fail.
   */
  QColor  statusColor();
 
  Transfer::JobType _Type;
  QDate _StartDate;
  QDate _EndDate;
  QString _OurBankCode;
  QString _OurBic;
  BankType _BankType;
  QString _OurAccountId;
  QString _OurIban;
  int _OurCountry;
  int _Id;
  /**The status of the job
   */
  BankingJob::JobStatus _Status;
  QString _Message;

  QTableWidgetItem *_TwiStatus; //required for updating status string
  QTableWidgetItem *_TwiMessage;//required for updating response message string


};


}//namespace banking
#endif /*BANKINGJOBITEM_H*/
