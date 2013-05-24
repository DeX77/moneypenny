/***************************************************************************
                          datedtransfer.cpp  -  description
                             -------------------
    begin                : Do Nov 11 2004
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

#include "datedtransfer.h"

#include <iostream>
using namespace std;

/**

*/
DatedTransfer::DatedTransfer(GWEN_DB_NODE* db){

  if (db){
  GWEN_DB_NODE* value;
  value = GWEN_DB_GetGroup( db,
                            GWEN_DB_FLAGS_DEFAULT |
                            GWEN_PATH_FLAGS_NAMEMUSTEXIST,
                            "value");

  GWEN_DB_NODE* ouracc;
  ouracc = GWEN_DB_GetGroup(db,
                            GWEN_DB_FLAGS_DEFAULT |
                            GWEN_PATH_FLAGS_NAMEMUSTEXIST,
                            "ouraccount");

  GWEN_DB_NODE* otheracc;
  otheracc = GWEN_DB_GetGroup(db,
                              GWEN_DB_FLAGS_DEFAULT |
                              GWEN_PATH_FLAGS_NAMEMUSTEXIST,
                             "otheraccount");
  
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

  if (ouracc){  
   _country = GWEN_DB_GetIntValue(ouracc, "country", 0, MONEYPENNY_DEFAULT_COUNTRY);
   _bankCode = GWEN_DB_GetCharValue(ouracc, "bankcode", 0, "");
   _accountId = GWEN_DB_GetCharValue(ouracc, "accountid", 0, "");
//   _name = GWEN_DB_GetCharValue(ouracc, "name", 0, "");
  }
   
  if (otheracc){
   _otherCountry = GWEN_DB_GetIntValue(otheracc, "country", 0, MONEYPENNY_DEFAULT_COUNTRY);
   _otherBankId = GWEN_DB_GetCharValue(otheracc, "bankcode", 0, "");
   _otherAccountId = GWEN_DB_GetCharValue(otheracc, "accountid", 0, "");
   }

   _date = Date(GWEN_DB_GetCharValue(db, "date", 0, ""));

   const char *p;

   for (int i = 0; i < 2; i++)
   {
    p = GWEN_DB_GetCharValue(db, "othername", i, "");
    if (!p)
       break;
    _othernames.push_back(p);
    cerr << p;
   }

   cerr << "  .. ";

  for (int i = 0; i < 2; i++)
   {
    p = GWEN_DB_GetCharValue(db, "purpose", i, "");
    if (!p)
       break;
    _purposes.push_back(p);
    cerr << p;
   }

   _value = Value(val,cur);
   _textKey = GWEN_DB_GetIntValue(db, "textkey", 0, 52);
   _textKeyExt = GWEN_DB_GetIntValue(db, "textkeyext", 0, 0);
   _purpose = GWEN_DB_GetCharValue(db, "purpose", 0, "");
   _otherName = GWEN_DB_GetCharValue(db, "othername", 0, "");
   _jobId = GWEN_DB_GetCharValue(db, "jobid", 0, "");

   cerr << "\n Transaction from: "
           <<_country << "  "
           << _bankCode << "  "
           << _accountId << "  "
           << "   to: "<< _otherCountry
           << _otherBankId << "  "
           << _otherAccountId;
}
else cerr << "Error StandingOrder::StandingOrder() ... No DB_POINTER";

}

DatedTransfer::~DatedTransfer(){
}

/** Read property of int _country. 
*/
const int& DatedTransfer::country(){
	return _country;
}
/** Write property of int _country. 
*/
void DatedTransfer::setCountry( const int& _newVal){
	_country = _newVal;
}
/** Read property of string _bankCode. 
*/
const string& DatedTransfer::bankCode(){
	return _bankCode;
}
/** Write property of string _bankCode. 
*/
void DatedTransfer::setBankCode( const string& _newVal){
	_bankCode = _newVal;
}
/** Read property of string _accountId. 
*/
const string& DatedTransfer::accountId(){
	return _accountId;
}
/** Write property of string _accountId. 
*/
void DatedTransfer::setAccountId( const string& _newVal){
	_accountId = _newVal;
}
/** Read property of int _otherCountry. 
*/
const int& DatedTransfer::otherCountry(){
	return _otherCountry;
}
/** Write property of int _otherCountry.
 */
void DatedTransfer::setOtherCountry( const int& _newVal){
	_otherCountry = _newVal;
}
/** Read property of string _otherBankId. 
*/
const string& DatedTransfer::otherBankId(){
	return _otherBankId;
}

/** Write property of string _otherBankId. 
*/
void DatedTransfer::setOtherBankId( const string& _newVal){
	_otherBankId = _newVal;
}

/** Read property of string _otherAccountId. 
*/
const string& DatedTransfer::otherAccountId(){
	return _otherAccountId;
}

/** Write property of string _otherAccountId.
 */
void DatedTransfer::setOtherAccountId( const string& _newVal){
	_otherAccountId = _newVal;
}

/** Read property of Date _date.
 */
const Date& DatedTransfer::date(){
	return _date;
}

/** Write property of Date _date. 
*/
void DatedTransfer::setDate( const Date& _newVal){
	_date = _newVal;
}
/** Read property of string _otherName.
 */
const string& DatedTransfer::otherName(){
	return _otherName;
}
/** Write property of string _otherName. 
*/
void DatedTransfer::setOtherName( const string& _newVal){
	_otherName = _newVal;
}
/** Read property of Value _value. 
*/
const Value& DatedTransfer::value(){
	return _value;
}
/** Write property of Value _value. 
*/
void DatedTransfer::setValue( const Value& _newVal){
	_value = _newVal;
}
/** Read property of int _textKey.
 */
const int& DatedTransfer::textKey(){
	return _textKey;
}
/** Write property of int _textKey. 
*/
void DatedTransfer::setTextKey( const int& _newVal){
	_textKey = _newVal;
}
/** Read property of int _texktKeyExt.
 */
const int& DatedTransfer::textKeyExt(){
	return _textKeyExt;
}
/** Write property of int _texktKeyExt. 
*/
void DatedTransfer::setTextKeyExt( const int& _newVal){
	_textKeyExt = _newVal;
}
/** Read property of string _purpose. 
*/
const string& DatedTransfer::purpose(){
	return _purpose;
}
/** Write property of string _purpose. 
*/
void DatedTransfer::setPurpose( const string& _newVal){
	_purpose = _newVal;
}
/** Read property of string _jobId. 
*/
const string& DatedTransfer::jobId(){
	return _jobId;
}
/** Write property of string _jobId. 
*/
void DatedTransfer::setJobId( const string& _newVal){
	_jobId = _newVal;
}

/** Read property of list<string> _purposes.
 */
const list<string>& DatedTransfer::purposes(){
	return _purposes;
}
/** Write property of list<string> _purposes.
 */
void DatedTransfer::setPurposes( const list<string>& _newVal){
	_purposes = _newVal;
}
/** Read property of list<string> _othernames.
 */
const list<string>& DatedTransfer::othernames(){
	return _othernames;
}
/** Write property of list<string> _othernames. 
*/
void DatedTransfer::setOthernames( const list<string>& _newVal){
	_othernames = _newVal;
}
