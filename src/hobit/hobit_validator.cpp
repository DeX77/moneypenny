/***************************************************************************
                          hobit_validator.cpp  -  description
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

#include "hobit_validator.h"
#include "aqmoney2/bankmanager.h"
#include "aqmoney2/accountjobs.h"
#include <iostream>
#include <stdlib.h>

#include <qapplication.h>


/* Internationalization */
#ifdef ENABLE_NLS
# include <libintl.h>
# include <locale.h>
# define I18N(m) gettext(m)
#else
# define I18N(m) m
#endif




HoBIT_Validator::HoBIT_Validator(AqMoneyAPI* api){
  _api = api;
}
HoBIT_Validator::~HoBIT_Validator(){
 qDebug("Destructor HoBIT_Validator");
}


bool HoBIT_Validator::checkAccount( unsigned int country,
                                    const string &bankCode,
                                    const string &accountid) {
  Pointer<BankManager> mgr;
  BANKMANAGER_CHECKRESULT rv;
  mgr=_api->bankManager();
  rv=mgr.ref().checkAccount(country, bankCode, accountid);
  switch(rv) {
  case BankManagerCheckResultFailed:
      cerr << "HoBITValidator: Account  " << country << " : "  << bankCode.c_str()
             <<   " : " << accountid.c_str() << "seems to be invalid\n";
              return false;
  case BankManagerCheckResultBankUnknown:
        cerr << "HoBITValidator: Unknown Bank:      " << country << " : "  << bankCode.c_str()  
               << " : " << accountid.c_str();
            return false;
    default:
           return true;
  }
}


/**
*  Checks if a transaction is valid
*/
Error HoBIT_Validator:: checkTransaction(GWEN_DB_NODE *xa,
                                         GWEN_DB_NODE *params) {
  GWEN_DB_NODE *n;
  int tk;

  // check textkey
  tk=GWEN_DB_GetIntValue(xa, "textkey", 0, -1);
  if (tk==-1)
    tk=atoi(GWEN_DB_GetCharValue(xa, "textkey", 0, "-1"));
  if (tk==-1) {
    GWEN_DB_SetCharValue(xa, GWEN_DB_FLAGS_DEFAULT,
			 "textkey", "51");
    tk=51;
  }

  // check textkey
  n=GWEN_DB_GetGroup(params, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "textKey");
  if (n) {
    int i;

    for (i=0;;i++) {
      int ck;

      ck=GWEN_DB_GetIntValue(n, "key", i, -1);
      if (ck==-1) {
	if (i) {
          AccountJob::storeResult(xa, 9999,
                                  "textkey not supported by bank");
	  fprintf(stderr,
		  I18N("Textkey %d not supported by your bank\n"),
		  tk);
	  return Error("ModuleTransfer::_checkTransaction",
		       ERROR_LEVEL_NORMAL,
		       HBCI_ERROR_CODE_INVALID,
		       ERROR_ADVISE_DONTKNOW,
		       "textkey not supported for this account",
		       String::num2string(tk));
	}
	break;
      }
      if (ck==tk)
	break;
    } // for
  } // if textKey given
  return Error();
}




Error HoBIT_Validator ::  checkDeleteJob(GWEN_DB_NODE *xa,
                                                             GWEN_DB_NODE *params) {
  GWEN_DB_NODE *n;
  int tk;
  const char *p;

  // check textkey
  tk=GWEN_DB_GetIntValue(xa, "textkey", 0, -1);
  if (tk==-1)
    tk=atoi(GWEN_DB_GetCharValue(xa, "textkey", 0, "-1"));
  if (tk==-1) {
    GWEN_DB_SetCharValue(xa, GWEN_DB_FLAGS_DEFAULT,
			 "textkey", "51");
    tk=51;
  }
  p=GWEN_DB_GetCharValue(xa, "jobid", 0, "");
  if (!*p) {
    return Error("ModuleDeleteDatedTransfer::_checkTransaction",
                 ERROR_LEVEL_NORMAL,
                 HBCI_ERROR_CODE_INVALID,
                 ERROR_ADVISE_DONTKNOW,
                 "jobid missing");
  }

  // check textkey
  n=GWEN_DB_GetGroup(params, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "textKey");
  if (n) {
    int i;

    for (i=0;;i++) {
      int ck;

      ck=GWEN_DB_GetIntValue(n, "key", i, -1);
      if (ck==-1) {
	if (i) {
          AccountJob::storeResult(xa, 9999,
                                  "textkey not supported by bank");
          fprintf(stderr,
                  I18N("Textkey %d not supported by your bank\n"),
                  tk);
          return Error("ModuleTransfer::_checkTransaction",
		       ERROR_LEVEL_NORMAL,
		       HBCI_ERROR_CODE_INVALID,
		       ERROR_ADVISE_DONTKNOW,
		       "textkey not supported for this account",
		       String::num2string(tk));
	}
	break;
      }
      if (ck==tk)
	break;
    } // for
  } // if textKey given
  return Error();
}



