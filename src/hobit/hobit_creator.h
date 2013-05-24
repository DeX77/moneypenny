/***************************************************************************
                          hobit_creator.h  -  description
                             -------------------
    begin                : So Okt 24 2004
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

#ifndef HOBIT_CREATOR_H
#define HOBIT_CREATOR_H


#include <openhbci2.h>
#include <openhbci2/value.h>
#include <openhbci2/date.h>
#include <openhbci2/pointer.h>
using namespace HBCI;
#include <gwenhywfar/db.h>
#include "aqmoney2/account.h"


/**A  class for making XML-HBCI-usecases
  *@author Marcel Mulch
  */


class HoBIT_Creator {

public:
/**
* This method genereates a new XML-Transaction
* Look into the HBCI-Specification to understand what is done here
*/
static GWEN_DB_NODE * makeTransaction(Pointer<Account> acc,
                                                                 int otherCountry,
                                                                 const string &otherBankCode,
                                                                 const string &otherAccount,
                                                                 const list<string> &otherName,
                                                                 const list<string> &purpose,
                                                                 const Value &val,
                                                                  int textKey,
                                                                 const Date &date,
                                                                 const string &id);

/**
* This method genereates a new XML-Transaction
* Look into the HBCI-Specification to understand what is done here
*/
static GWEN_DB_NODE * makeTransaction(Pointer<Account> acc,
                                                                 int otherCountry,
                                                                 const string &otherBankCode,
                                                                 const string &otherAccount,
                                                                 const list<string> &otherName,
                                                                 const list<string> &purpose,
                                                                 const Value &val,
                                                                  int textKey,
                                                                  int textKeyExt,
                                                                 const Date &date,
                                                                 const string &id);



/**
* This method genereates a new XML-Transaction
* Look into the HBCI-Specification to understand what is done here
*/						 
static GWEN_DB_NODE * makeStandingOrder(Pointer<Account> acc,
                                                                       int otherCountry,
                                                                       const string &otherBankCode,
                                                                       const string &otherAccount,
                                                                       const list<string> &otherName,
                                                                       const list<string> &purpose,
                                                                       const Value &val,
                                                                       int textKey,
                                                                       const Date &date,
                                                                       const Date &firstdate,
                                                                       const Date &lastdate,
                                                                       bool weekly,
                                                                       int turnus,
                                                                       int day);


								       /**
* This method genereates a new XML-Transaction
* Look into the HBCI-Specification to understand what is done here
*/
static GWEN_DB_NODE * makeStandingOrder(Pointer<Account> acc,
                                                                       int otherCountry,
                                                                       const string &otherBankCode,
                                                                       const string &otherAccount,
                                                                       const list<string> &otherName,
                                                                       const list<string> &purpose,
                                                                       const Value &val,
                                                                       int textKey,
                                                                       int textKeyExt,
                                                                       const Date &date,
                                                                       const Date &firstdate,
                                                                       const Date &lastdate,
                                                                       const string& jobId,
                                                                       bool weekly,
                                                                       int turnus,
                                                                       int day);                                                               


};






#endif

