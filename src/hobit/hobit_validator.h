/***************************************************************************
                          hobit_validator.h  -  description
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


#ifndef HOBIT_VALIDATOR_H
#define HOBIT_VALIDATOR_H

#include "aqmoney2/aqmapi.h"
 



/**This is a class which includes static methods to validate Accounts and Transactions
  *@author Marcel Mulch
  */

class HoBIT_Validator{

public: 
HoBIT_Validator(AqMoneyAPI *api);
virtual ~HoBIT_Validator();


AqMoneyAPI *_api;


 /** checks if an account exists
 */
bool checkAccount( unsigned int country,
                               const string &bankCode,
                               const string &accountid);

 /**
*  Checks if a transaction is valid
*/
 Error checkTransaction(GWEN_DB_NODE *xa,
                                    GWEN_DB_NODE *params) ;


 /**
*  Checks if a transaction for a delete job is valid
*/
 Error checkDeleteJob (GWEN_DB_NODE *xa,
                                   GWEN_DB_NODE *params) ;



 	
};

#endif




