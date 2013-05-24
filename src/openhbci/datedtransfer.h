/***************************************************************************
                          datedtransfer.h  -  description
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

#ifndef DATEDTRANSFER_H
#define DATEDTRANSFER_H


#include <gwenhywfar/db.h>
#include <openhbci2/date.h>
#include <openhbci2/value.h>
using namespace HBCI;

#include <string>
#include <list>
using namespace std;

#include "globals.h"

/**stores Information about a single dated transfer
  *@author Marcel Mulch
  */

class DatedTransfer {

private:
  int _country;
  string _bankCode;
  string _accountId;
 // string _name;
  int _otherCountry;
  string _otherBankId;
  string _otherAccountId;
  Date _date;
  string _otherName;
  Value _value;
  int _textKey;
  int _textKeyExt;
  string _purpose;
  string _jobId;

  list<string> _purposes;
  list<string> _othernames;


public:
  DatedTransfer(GWEN_DB_NODE* db);
  virtual ~DatedTransfer();
  /** Write property of int _country. */
  virtual void setCountry( const int& _newVal);
  /** Read property of int _country. */
  virtual const int& country();
  /** Write property of string _bankCode. */
  virtual void setBankCode( const string& _newVal);
  /** Read property of string _bankCode. */
  virtual const string& bankCode();
  /** Write property of string _accountId. */
  virtual void setAccountId( const string& _newVal);
  /** Read property of string _accountId. */
  virtual const string& accountId();
  /** Write property of int _otherCountry. */
  virtual void setOtherCountry( const int& _newVal);
  /** Read property of int _otherCountry. */
  virtual const int& otherCountry();
  /** Write property of string _otherBankId. */
  virtual void setOtherBankId( const string& _newVal);
  /** Read property of string _otherBankId. */
  virtual const string& otherBankId();
  /** Write property of string _otherAccountId. */
  virtual void setOtherAccountId( const string& _newVal);
  /** Read property of string _otherAccountId. */
  virtual const string& otherAccountId();
  /** Write property of Date _date. */
  virtual void setDate( const Date& _newVal);
  /** Read property of Date _date. */
  virtual const Date& date();
  /** Write property of string _otherName. */
  virtual void setOtherName( const string& _newVal);
  /** Read property of string _otherName. */
  virtual const string& otherName();
  /** Write property of Value _value. */
  virtual void setValue( const Value& _newVal);
  /** Read property of Value _value. */
  virtual const Value& value();
  /** Write property of int _textKey. */
  virtual void setTextKey( const int& _newVal);
  /** Read property of int _textKey. */
  virtual const int& textKey();
  /** Write property of int _texktKeyExt. */
  virtual void setTextKeyExt( const int& _newVal);
  /** Read property of int _texktKeyExt. */
  virtual const int& textKeyExt();
  /** Write property of string _purpose. */
  virtual void setPurpose( const string& _newVal);
  /** Read property of string _purpose. */
  virtual const string& purpose();
  /** Write property of string _jobId. */
  virtual void setJobId( const string& _newVal);
  /** Read property of string _jobId. */
  virtual const string& jobId();
  /** Write property of list<string> _purposes. */
  virtual void setPurposes( const list<string>& _newVal);
  /** Read property of list<string> _purposes. */
  virtual const list<string>& purposes();
  /** Write property of list<string> _othernames. */
  virtual void setOthernames( const list<string>& _newVal);
  /** Read property of list<string> _othernames. */
  virtual const list<string>& othernames();
};

#endif
