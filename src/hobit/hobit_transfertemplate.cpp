/***************************************************************************
 *   Copyright (C) 2004 by Marcel Mulch                                    *
 *   marcel@mulch-online.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "hobit_transfertemplate.h"
#include "hobit_string.h"

HoBIT_TransferTemplate::HoBIT_TransferTemplate(const QString& ourAccountId,
                           const QString& otherName, 
                           const QString& otherAccountId,
                           const QString& otherBankCode,
                           double value,
                           QStringList purpose,
                           const QString& currency)
{
	_OurAccountId = ourAccountId;
	_OtherName = otherName;
	_OtherAccountId = otherAccountId;
	_OtherBankCode = otherBankCode;
	_Value = value;
        int lines = 0;
	QStringList::iterator it = purpose.begin();
        //purpose may contain illegal chars and empty lines, remove them
       while(it != purpose.end() && lines < 14){ //not more than 14 lines
         (*it)= (*it).trimmed();
         if(! (*it).isEmpty()){
           _Purpose.push_back(HoBIT_String::replaceSpecialChars((*it)));
           lines ++;
         }
         it++; 
        }
        _Currency = currency;	
}


HoBIT_TransferTemplate::HoBIT_TransferTemplate(GWEN_DB_NODE *db) {
	fromConfig(db);
}


HoBIT_TransferTemplate::~HoBIT_TransferTemplate()
{
}


void HoBIT_TransferTemplate::fromConfig(GWEN_DB_NODE *db){
  _OurAccountId=GWEN_DB_GetCharValue(db, "ownAccountId", 0, "");
  _OtherName=GWEN_DB_GetCharValue(db, "otherName", 0, "");
  _OtherAccountId=GWEN_DB_GetCharValue(db, "otherAccountId", 0, "");
  _OtherBankCode=GWEN_DB_GetCharValue(db, "otherBankId", 0, "");
  _Value= QString(GWEN_DB_GetCharValue(db, "value/value", 0, "0,0")).toDouble();
  _Currency=  GWEN_DB_GetCharValue(db, "value/currency", 0, "EUR");
  _Purpose.clear();
  _Purpose.push_back(GWEN_DB_GetCharValue(db, "purpose/line1", 0, ""));  
  _Purpose.push_back(GWEN_DB_GetCharValue(db, "purpose/line2", 0, ""));
  _PurposeStrings.clear();
  for(int line = 0; line < _Purpose.count(); line ++)
    _PurposeStrings.push_back(qPrintable(_Purpose[line]));


}



void HoBIT_TransferTemplate::toConfig(GWEN_DB_NODE *db){
   QString sValue;
   
   GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                       "ownAccountId",
                       _OurAccountId.toLatin1().data());
   GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                        "otherName",
                        _OtherName.toLatin1().data());
   GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                       "otherAccountId",
                       _OtherAccountId.toLatin1().data());
   GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                        "otherBankId",
                        _OtherBankCode.toLatin1().data());
   sValue.setNum(_Value,'f',2);                     
   GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                        "value/value",
                        sValue.toLatin1().data());
   GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                        "value/currency",
                        _Currency.toLatin1().data());
   
   GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                        "purpose/line1",
                        _Purpose[0].toLatin1().data());
   GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                        "purpose/line2",
                        _Purpose[1].toLatin1().data());
}

/** Returns true , if the minimum information is set.
*/
bool HoBIT_TransferTemplate::isValid(){
   return   (!_OtherName.isEmpty()) 
         && (!_OtherBankCode.isEmpty()) 
         && (!_OtherAccountId.isEmpty()) 
         && (!_OurAccountId.isEmpty());
}


/** Compares two templates. Ignores differences due to empty purpose lines.
 */
bool HoBIT_TransferTemplate::operator ==(const HoBIT_TransferTemplate & t2){
    bool purposeEqual = 
        ( HoBIT_String::stringListHasEmptyLines(_Purpose) && HoBIT_String::stringListHasEmptyLines( t2._Purpose) );

    if(!purposeEqual){ //compare pupose lines. ignore empty ones
      QStringList::Iterator it1 = _Purpose.begin();
      QStringList list2 = t2._Purpose;
      QStringList::Iterator it2 = list2.begin();
      QString s1 = "";
      QString s2 = "";
      purposeEqual=true;
      while (  it1 != _Purpose.end() && purposeEqual) { //run through own list and compare lines to lines from t2
         s1 = (*it1);
         if(it2== list2.end())
            s2 = ""; //use an empty line, if list from t2 has less lines
         else s2 = (*it2);
         if(s1.isEmpty() && s2.isEmpty()){
           it1++;
           it2++;
         }
         else {
           purposeEqual= s1==s2;
           it1++;
           it2++;
         }
       }
    }

    HoBIT_String::debugStringList(_Purpose); HoBIT_String::debugStringList(t2._Purpose);

    bool accountEqual= 
          HoBIT_String::toIdFormat(_OurAccountId) == HoBIT_String::toIdFormat(t2._OurAccountId); 
    bool nameEqual= 
          _OtherName == t2._OtherName;
    bool otherAccountEqual =
          HoBIT_String::toIdFormat(_OtherAccountId) == HoBIT_String::toIdFormat(t2._OtherAccountId);
    bool otherBankEqual= 
          _OtherBankCode == t2._OtherBankCode;
    bool valueEqual= 
          _Value == t2._Value;
    if (purposeEqual && accountEqual && nameEqual && otherAccountEqual && otherBankEqual && valueEqual )
      return true;
    else return false;
}   


/** All the getter methods
*/

const QString& HoBIT_TransferTemplate::ourAccountId()const
{
    return _OurAccountId;
}
const QString& HoBIT_TransferTemplate::otherName()const
{
    return _OtherName;
}
const QString& HoBIT_TransferTemplate::otherAccountId()const
{
    return _OtherAccountId;
}
const QString& HoBIT_TransferTemplate::otherBankCode()const
{
    return _OtherBankCode;
}
double HoBIT_TransferTemplate::value()const
{
    return _Value;
}
const QString& HoBIT_TransferTemplate::currency()const
{
    return _Currency;
}
const QStringList& HoBIT_TransferTemplate::purpose()const 
{
    return _Purpose;
}

const list < string>  & HoBIT_TransferTemplate::purposeStrings()const{
    return _PurposeStrings;
}
