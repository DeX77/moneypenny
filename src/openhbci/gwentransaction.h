/***************************************************************************
                          transaction.h  -  description
                             -------------------
    begin                : Sa Nov 6 2004
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

#ifndef GWENTRANSACTION_H
#define GWENTRANSACTION_H

#include "kernel/helpers/transaction.h"

#include <string>
#include <list>
#include <iostream>
using namespace std;

#include <openhbci2/date.h>
#include <openhbci2/value.h>
#include <gwenhywfar/db.h>
using namespace HBCI;




/**Simple class for storing transaction data as reported by the bank server
  *@author Marcel Mulch, Walter Roth
  */
class GwenTransaction: public banking::Transaction {

public:
	GwenTransaction(GWEN_DB_NODE *db);
	virtual ~GwenTransaction();
};
#endif
