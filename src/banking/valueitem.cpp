/***************************************************************************
 *   Copyright (C) 2007 by Walter Roth                                     *
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

#include "valueitem.h"

#include <QString>

/**Constructor 
 */
ValueItem::ValueItem(const banking::Value & value)
  :QTableWidgetItem(value.readableString().c_str()){
 _Value = value;
 if(_Value.isDebit()) //negative value
   setForeground(Qt::red);
}
 
/** Destructor
*/ 
ValueItem::~ValueItem(){
}

/**The operator used for sorting. Compares numeric values.
 */
bool ValueItem::operator <(const QTableWidgetItem & other)const{
 return _Value < ((const ValueItem &)other)._Value;
}

/** Read property of double _Value
 */
double ValueItem::numericValue()const{
  return _Value.doubleValue();
}
