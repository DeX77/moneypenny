/***************************************************************************
 *   Copyright (C) 2004 by Walter Roth                                     *
 *   walter@amd2600-roth                                                   *
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
#include "dateitem.h"

#include <QString>

QString DateItem::_DateFormat = "dddd\ndd.MM.yyyy";
QDate _DefaultDate = QDate::fromString("1.1.1900", "d.M.yyyy");

/** Default constructor 
  */
DateItem::DateItem(){
}

/**Constructor 
 */
DateItem::DateItem(const QDate &date, const QString & format)
  :QTableWidgetItem(){
 _Date = date;
 if(_Date == _DefaultDate) //default date 1.1.1900
    setText("");
 else{
   if(format.isEmpty())
      setText(_Date.toString(_DateFormat));
   else setText(_Date.toString(format));
 }
}
 
/** Destructor
*/ 
DateItem::~DateItem(){
}

/**The operator used for sorting. Compares date values.
 */
bool DateItem::operator <(const QTableWidgetItem & other)const{
  return _Date < ((const DateItem &)other)._Date;
}
/** Read property of double _Date
 */
const QDate & DateItem::date()const{
  return _Date;
}
/** Set the format for all new date objects
 */
void DateItem::setDateFormat(const QString & newDateFormat){
  _DateFormat = newDateFormat;
}
