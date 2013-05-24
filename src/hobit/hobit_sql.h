/***************************************************************************
                          hobit_sql  -  description
                             -------------------
    begin                : Fr Sept 9 2005
    copyright            : (C) 2005 by Marcel Mulch & Thomas Richter
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


#ifndef HOBIT_SQL_H
#define HOBIT_SQL_H

#include <QFile>
#include <QTextStream>

#include <openhbci2/pointer.h>
#include "kernel/helpers/transaction.h"
#include "standingorder.h"
#include "datedtransfer.h"


#include <list>
using namespace std;


class HoBIT_SQL:public QObject{

Q_OBJECT

public:

   /** base constructor of the SQL Management class */
   HoBIT_SQL();
   
   /** base destructor */
   virtual ~HoBIT_SQL();

   /** set all lists that are exported to the database */
   virtual void setAll(	banking::TransactionList transactions, 
   			list<Pointer<DatedTransfer> > datedtransfers,
			list<Pointer<StandingOrder> > standingorders);
			
			
   /** set the transaction list to be exported */
   virtual void setTransactions(banking::TransactionList transactions);
   
   
   /** set the dated transfer list to be exported */
   virtual void setDatedTransfers(list<Pointer<DatedTransfer> > datedtransfers);
   
   
   /** set the standing order list to be exported */
   virtual void setStandingOrders(list<Pointer<StandingOrder> > standingorders);

   
   /** method that generates and writes an sql source file  */
   virtual void writeSqlFile(QString path);
   
   /** creates the sql tables */
   virtual void createTables(QTextStream &stream);
   
   /** creates inserts for moneypenny's transactions */
   virtual void insertTransactions(QTextStream &stream);
   
   /** creates inserts for moneypenny's standing order's */
   virtual void insertStandingOrders(QTextStream &stream);

   /** creates inserts for moneypenny's dated transfer's */
   virtual void insertDatedTransfers(QTextStream &stream);
   
private:

banking::TransactionList _Transactions;
list<Pointer<DatedTransfer> > _Datedtransfers;
list<Pointer<StandingOrder> > _Standingorders;

QFile* _file;

};




#endif

