/***************************************************************************
                          hobit_creator.cpp  -  description
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

 #ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Internationalization */
#ifdef ENABLE_NLS
# include <libintl.h>
# include <locale.h>
# define I18N(m) gettext(m)
#else
# define I18N(m) m
#endif


 
#include "hobit_creator.h"
#include "aqmoney2/aqmapi.h"
#include "aqmoney2/accountjobs.h"
#include <gwenhywfar/db.h>
#include <gwenhywfar/debug.h>



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


/**
* This method genereates a new XML-Transaction
* Look into the HBCI-Specification to understand what is done here
*/
GWEN_DB_NODE* HoBIT_Creator::  makeTransaction(Pointer<Account> acc,
                                               int otherCountry,
                                               const string &otherBankCode,
                                               const string &otherAccount,
                                               const list<string> &otherName,
                                               const list<string> &purpose,
                                               const Value &val,
                                               int textKey,
                                               const Date &date,
                                               const string &id) {
  GWEN_DB_NODE *xa;
  list<string>::const_iterator sit;

  xa=GWEN_DB_Group_new("transaction");
  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "ouraccount/country",
                       acc.ref().country());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "ouraccount/bankCode",
                         acc.ref().bankCode().c_str());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                        "ouraccount/accountId",
                         acc.ref().accountId().c_str());

    GWEN_DB_SetIntValue(xa,
                        GWEN_DB_FLAGS_DEFAULT,
                        "otheraccount/country",
                        otherCountry);

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                       "otheraccount/bankCode",
                        otherBankCode.c_str());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                        "otheraccount/accountId",
                         otherAccount.c_str());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "value/value",
                          val.toString(true).c_str());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "value/currency",
                           val.getCurrency().c_str());

    GWEN_DB_SetIntValue(xa,
                        GWEN_DB_FLAGS_DEFAULT,
                        "textkey",
                         textKey);

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "name",
                          acc.ref().userName().c_str());

  // store othername
      if (otherName.size()>2 || otherName.empty()) {
      GWEN_DB_Group_free(xa);
          throw Error("JobSingleTransfer::JobSingleTransfer",
                          ERROR_LEVEL_NORMAL,
                          HBCI_ERROR_CODE_INVALID,
                          ERROR_ADVISE_DONTKNOW,
                          "bad number of otherNames");
  }

    for (sit=otherName.begin(); sit!=otherName.end(); sit++) {
        GWEN_DB_SetCharValue(xa,
                             GWEN_DB_FLAGS_DEFAULT,
                             "othername",
                             (*sit).c_str());
  }

  // store purpose
  for (sit=purpose.begin(); sit!=purpose.end(); sit++) {
    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                          "purpose",
                           (*sit).c_str());
  }

    if (date.isValid()) {
        GWEN_DB_SetCharValue(xa,
                             GWEN_DB_FLAGS_DEFAULT,
                            "date",
                              date.toString().c_str());
  }

    if (!id.empty()) {
            GWEN_DB_SetCharValue(xa,
                                 GWEN_DB_FLAGS_DEFAULT,
                                 "jobId",
                                  id.c_str());
}

  return xa;
}



/**
* This method genereates a new XML-Transaction
* Look into the HBCI-Specification to understand what is done here
*/
GWEN_DB_NODE*  HoBIT_Creator::  makeTransaction(Pointer<Account> acc,
                                                int otherCountry,
                                                const string &otherBankCode,
                                                const string &otherAccount,
                                                const list<string> &otherName,
                                                const list<string> &purpose,
                                                const Value &val,
                                                int textKey,
                                                int textKeyExt,
                                                const Date &date,
                                                const string &id) {
  GWEN_DB_NODE *xa;
  list<string>::const_iterator sit;

  xa=GWEN_DB_Group_new("transaction");
  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "ouraccount/country",
                       acc.ref().country());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "ouraccount/bankCode",
                         acc.ref().bankCode().c_str());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                        "ouraccount/accountId",
                         acc.ref().accountId().c_str());

    GWEN_DB_SetIntValue(xa,
                        GWEN_DB_FLAGS_DEFAULT,
                        "otheraccount/country",
                        otherCountry);

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                       "otheraccount/bankCode",
                        otherBankCode.c_str());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                        "otheraccount/accountId",
                         otherAccount.c_str());

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "value/value",
                          val.toString(true).c_str());

    GWEN_DB_SetCharValue(xa,
                        GWEN_DB_FLAGS_DEFAULT,
                        "value/currency",
                          val.getCurrency().c_str());

    GWEN_DB_SetIntValue(xa,
                        GWEN_DB_FLAGS_DEFAULT,
                        "textkey",
                         textKey);

    GWEN_DB_SetIntValue(xa,
                        GWEN_DB_FLAGS_DEFAULT,
                        "textkeyext",
                         textKeyExt);

    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "name",
                          acc.ref().userName().c_str());

  // store othername
      if (otherName.size()>2 || otherName.empty()) {
      GWEN_DB_Group_free(xa);
          throw Error("JobSingleTransfer::JobSingleTransfer",
                          ERROR_LEVEL_NORMAL,
                          HBCI_ERROR_CODE_INVALID,
                          ERROR_ADVISE_DONTKNOW,
                          "bad number of otherNames");
  }

    for (sit=otherName.begin(); sit!=otherName.end(); sit++) {
        GWEN_DB_SetCharValue(xa,
                             GWEN_DB_FLAGS_DEFAULT,
                             "othername",
                             (*sit).c_str());
  }

  // store purpose
  for (sit=purpose.begin(); sit!=purpose.end(); sit++) {
    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                          "purpose",
                           (*sit).c_str());
  }

    if (date.isValid()) {
        GWEN_DB_SetCharValue(xa,
                             GWEN_DB_FLAGS_DEFAULT,
                            "date",
                              date.toString().c_str());
  }

    if (!id.empty()) {
            GWEN_DB_SetCharValue(xa,
                                 GWEN_DB_FLAGS_DEFAULT,
                                 "jobId",
                                  id.c_str());
}

  return xa;
}


GWEN_DB_NODE * HoBIT_Creator::makeStandingOrder(Pointer<Account> acc,
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
                                                int day){
  GWEN_DB_NODE *xa;
  list<string>::const_iterator sit;

  xa=GWEN_DB_Group_new("standingOrder");

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                        "ouraccount/country",
                        acc.ref().country());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "ouraccount/bankCode",
                       acc.ref().bankCode().c_str());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "ouraccount/accountId",
                       acc.ref().accountId().c_str());

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "otheraccount/country",
                      otherCountry);

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "otheraccount/bankCode",
                        otherBankCode.c_str());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "otheraccount/accountId",
                        otherAccount.c_str());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "value/value",
                        val.toString(true).c_str());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "value/currency",
                       val.getCurrency().c_str());

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "textkey",
                       textKey);

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "unit",
                       (weekly?"W":"M"));

  GWEN_DB_SetIntValue(xa,
                     GWEN_DB_FLAGS_DEFAULT,
                      "turnus",
                       turnus);

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "day",
                      day);

  // store othername
  if (otherName.size()>2 || otherName.empty()) {
                GWEN_DB_Group_free(xa);
                 throw Error("JobSingleTransfer::JobSingleTransfer",
                                  ERROR_LEVEL_NORMAL,
                                  HBCI_ERROR_CODE_INVALID,
                                  ERROR_ADVISE_DONTKNOW,
                                  "bad number of otherNames");
  }

  for (sit= otherName.begin(); sit!= otherName.end(); sit++) {
          GWEN_DB_SetCharValue(xa,
                               GWEN_DB_FLAGS_DEFAULT,
                               "othername",
                                 (*sit).c_str());
  }

  // store purpose
  for (sit= purpose.begin(); sit!=purpose.end(); sit++) {
      GWEN_DB_SetCharValue(xa,
                          GWEN_DB_FLAGS_DEFAULT,
                          "purpose",
                           (*sit).c_str());
  }

  if (date.isValid()) {
    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "date",
                          date.toString().c_str());
  }

  if (firstdate.isValid()) {
            GWEN_DB_SetCharValue(xa,
            GWEN_DB_FLAGS_DEFAULT,
             "firstDate",
             firstdate.toString().c_str());
  }

  if (lastdate.isValid()) {
              GWEN_DB_SetCharValue(xa,
              GWEN_DB_FLAGS_DEFAULT,
               "lastDate",
              lastdate.toString().c_str());
  }

  return xa;
}



GWEN_DB_NODE * HoBIT_Creator::makeStandingOrder(Pointer<Account> acc,
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
                                                int day){
  GWEN_DB_NODE *xa;
  list<string>::const_iterator sit;

  xa=GWEN_DB_Group_new("standingOrder");

  GWEN_DB_SetIntValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                        "ouraccount/country",
                        acc.ref().country());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "ouraccount/bankCode",
                       acc.ref().bankCode().c_str());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "ouraccount/accountId",
                       acc.ref().accountId().c_str());

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "otheraccount/country",
                      otherCountry);

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "otheraccount/bankCode",
                        otherBankCode.c_str());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "otheraccount/accountId",
                        otherAccount.c_str());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "value/value",
                        val.toString(true).c_str());

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "value/currency",
                       val.getCurrency().c_str());

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "textkey",
                       textKey);

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "textkeyext",
                       textKeyExt);

  GWEN_DB_SetCharValue(xa,
                       GWEN_DB_FLAGS_DEFAULT,
                       "unit",
                       (weekly?"W":"M"));

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                       "turnus",
                        turnus);

  GWEN_DB_SetIntValue(xa,
                      GWEN_DB_FLAGS_DEFAULT,
                      "day",
                       day);

  // store othername
  if (otherName.size()>2 || otherName.empty()) {
                GWEN_DB_Group_free(xa);
                 throw Error("JobSingleTransfer::JobSingleTransfer",
                                  ERROR_LEVEL_NORMAL,
                                  HBCI_ERROR_CODE_INVALID,
                                  ERROR_ADVISE_DONTKNOW,
                                  "bad number of otherNames");
  }

  for (sit= otherName.begin(); sit!= otherName.end(); sit++) {
          GWEN_DB_SetCharValue(xa,
                               GWEN_DB_FLAGS_DEFAULT,
                               "othername",
                                 (*sit).c_str());
  }

  // store purpose
  for (sit= purpose.begin(); sit!=purpose.end(); sit++) {
      GWEN_DB_SetCharValue(xa,
                           GWEN_DB_FLAGS_DEFAULT,
                           "purpose",
                            (*sit).c_str());
  }

  if (date.isValid()) {
    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "date",
                          date.toString().c_str());
  }

  if (firstdate.isValid()) {
            GWEN_DB_SetCharValue(xa,
            GWEN_DB_FLAGS_DEFAULT,
             "firstDate",
             firstdate.toString().c_str());
  }

  if (lastdate.isValid()) {
              GWEN_DB_SetCharValue(xa,
              GWEN_DB_FLAGS_DEFAULT,
               "lastDate",
              lastdate.toString().c_str());
  }

   if (!jobId.empty())
    GWEN_DB_SetCharValue(xa,
                         GWEN_DB_FLAGS_DEFAULT,
                         "jobid",
                          jobId.c_str());

  return xa;
}


