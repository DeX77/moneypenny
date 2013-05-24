/***************************************************************************
                          accountitem.h  -  description
                             -------------------
    begin                : Fr, march 24th 2006, 2007
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
#ifndef ACCOUNTITEM_H
#define ACCOUNTITEM_H

#include <QString>
#include <QStringList>
#include <QDate>

#include "kernel/helpers/value.h"


class QTableWidget;

namespace banking{ 
class Account;

/**Visualization class for a Account. addToTable creates a row
  *of QTableWidgetItems in a QTableWidget. Getter return QStrings.
  * _BankName must be set separately, because it is not available in Account
  */

class AccountItem {
public:
 // AccountItem();
  AccountItem(const Account & t);
  //AccountItem(QTable * table, int row, Pointer<Account> dt);
  virtual ~AccountItem();
  /** Encodes the Account data as an html- table row QString.
  */
  QString  htmlTableRow(const QString & bgColor)const;
 
  /** Adds the accountitem as row to table.
   */
  void addToTable(QTableWidget * table, int row);
  /**Sets the (user defined) name of the bank.
  */
  void setBankName(const QString & bn){_BankName = bn;};

  //simple getters
  const QString & bankName()const{return _BankName;};


 private:
  QString _BankCode; //BLZ
  QString _BankName; //(user defined) bankname, not set by constructor
  QString _AccountId; //kontonummer
  QString _Currency;// default:Eur waehrung
  QString _AccountName; //bezeichnung
  QString _AccountLimit;
  QString _AccountEnabledJobs; //erlaubte_GVs	
  banking::Value _Balance;
  QDate _BalanceDate;
  QDate _StoreDate;
};

} //namespace
#endif /*ACCOUNTITEM_H_*/
