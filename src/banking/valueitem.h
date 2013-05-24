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
#ifndef VALUEITEM_H
#define VALUEITEM_H

#include <QTableWidgetItem>
#include "kernel/helpers/value.h"

/** Holds a currency value. Displays right aligned text with 2 decimals.
  * Shows red text for negative values.
  * Class for overloading the < operator used for sorting. This is neccessary for
  * sorting order based on numerical value instead of alphabetical order.
  */

class ValueItem: public QTableWidgetItem{
  public:
    /**Constructor 
      */
    ValueItem(const banking::Value & value);
    /** Destructor
    */ 
    ~ValueItem();
   /**The operator used for sorting. Compares numeric values.
    */
   bool operator <(const QTableWidgetItem & other)const;
   /** Read property of double _Value
    */
   double numericValue()const;
  private:
   banking::Value _Value;
};

#endif
