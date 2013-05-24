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
#ifndef DATEITEM_H
#define DATEITEM_H

#include <QTableWidgetItem>
#include <QDate>

/** Holds a date value. 
  * Class for overloading the < operator used for sorting. This is neccessary for
  * sorting order based on date instead of alphabetical order.
  * Use setDateFormat to modify the format of the date strings.
  */

class DateItem: public QTableWidgetItem{
  public:
    /** Default constructor
      */
    DateItem();
    /**Constructor 
      */
    DateItem(const QDate &date, const QString & format = "");
    /** Destructor
    */ 
    ~DateItem();
   /**The operator used for sorting. Compares date values.
    */
   bool operator <(const QTableWidgetItem & other)const;
   /** Read property of double _Date
    */
   const QDate & date()const;
   /** Set the format for all new date objects
    */
   static void setDateFormat(const QString & newDateFormat);
  protected:
    QDate _Date;
   static QString _DateFormat;
};

#endif
