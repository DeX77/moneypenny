/***************************************************************************
                          transaction.cpp  -  description
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

#include "gwentransaction.h"
#include <QString>

GwenTransaction::GwenTransaction(GWEN_DB_NODE *db):
banking::Transaction(banking::BankingJob::BookedTransaction){

   GWEN_DB_NODE* value;
    value = GWEN_DB_GetGroup( db,
                              GWEN_DB_FLAGS_DEFAULT |
                              GWEN_PATH_FLAGS_NAMEMUSTEXIST,
                              "value");

    GWEN_DB_NODE* ouracc;
    ouracc = GWEN_DB_GetGroup(db,
                              GWEN_DB_FLAGS_DEFAULT |
                              GWEN_PATH_FLAGS_NAMEMUSTEXIST,
                              "ourAccount");
    GWEN_DB_NODE* otheracc;
    otheracc = GWEN_DB_GetGroup(db,
                                GWEN_DB_FLAGS_DEFAULT |
                                GWEN_PATH_FLAGS_NAMEMUSTEXIST,
                                "otherAccount");

  const char* val;
  const char* cur;

  if (value){
  val = GWEN_DB_GetCharValue(value, "value", 0, "");
  cur = GWEN_DB_GetCharValue(value, "currency", 0, "");
  }
  else{ 
  val = "0.0";
  cur = "EUR";
  }
  QString sVal = val;
  double dVal = sVal.toDouble();
  setValue(dVal);
  setCurrency(cur);

  if (ouracc){  
     setOurBankCode(GWEN_DB_GetCharValue(ouracc, "BankCode", 0, ""));
     setOurAccountId(GWEN_DB_GetCharValue(ouracc, "AccountId", 0, ""));
     }
   
  if (otheracc){
     setOtherBankCode(GWEN_DB_GetCharValue(db, "otherAccount/BankCode", 0, ""));
     setOtherAccountId(GWEN_DB_GetCharValue(db, "otherAccount/AccountId", 0, ""));
   }

     Date d = Date(GWEN_DB_GetCharValue(db, "ValutaDate", 0, ""));
     setValutaDate(DateTime(d.year(), d.month(), d.day()));
     d = Date(GWEN_DB_GetCharValue(db, "date", 0, ""));
     setBookingDate(DateTime(d.year(), d.month(), d.day()));
     d = Date(GWEN_DB_GetCharValue(db, "storedate", 0, ""));
     setStoredate(DateTime(d.year(), d.month(), d.day()));

     setKey(GWEN_DB_GetCharValue(db, "key", 0, ""));
     setCustref(GWEN_DB_GetCharValue(db, "custref", 0, ""));
     setTextkey(GWEN_DB_GetCharValue(db, "textkey", 0, ""));
     setText(GWEN_DB_GetCharValue(db, "text", 0, ""));
     setPrimanota(GWEN_DB_GetCharValue(db, "primanota", 0, ""));
     setOtherName1(GWEN_DB_GetCharValue(db, "othername", 0, ""));

     const char *p;
     for (int i = 0; i < 14; i++)  
        {
         p = GWEN_DB_GetCharValue(db, "purpose", i, "");
         if (!p)
            break;
         _PurposeLines.push_back(p);  //This includes empty lines. Remove empty lines later, if you don't need them.
        }
}


GwenTransaction::~GwenTransaction(){
}

