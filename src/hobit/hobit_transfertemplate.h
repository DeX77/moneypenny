/***************************************************************************
 *   Copyright (C) 2004 by Marcel Mulch 2005 Walter Roth                   *
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
#ifndef HOBIT_TRANSFERTEMPLATE_H
#define HOBIT_TRANSFERTEMPLATE_H

#include <QStringList>
#include <QString>

#include <openhbci2.h>
using namespace HBCI;

/**
@author Marcel Mulch, Walter Roth
This class holds the data for a transfer template. It uses gwen to store and read the data.
The default currency is EUR. Create a new template, if you need to modify the data.
*/
class HoBIT_TransferTemplate{
public:
    /** base constructor creating 
      * a new transfer template
      */
    HoBIT_TransferTemplate(const QString& ourAccountId="",
                           const QString& otherName="", 
                           const QString& otherAccountId="",
                           const QString& otherBankCode="",
                           double value = 0.0,
                           QStringList purpose= QStringList(),
                           const QString& currency = "EUR");

   
   /** Constructor which creates a new transfer template
    * from a gwen node - needed for reading the config 
    * file 
    */
   HoBIT_TransferTemplate(GWEN_DB_NODE *db);

     /** base destructor 
     */
    ~HoBIT_TransferTemplate();
    
    /** writes a single database entry. 
      * stores a single transfertemplate 
      */
    void toConfig(GWEN_DB_NODE *db);
    
    /** reads a single transfer template entry 
      * from the database
      */
    void fromConfig(GWEN_DB_NODE *db);
    
    /** Returns true , if the minimum information is set.
    */
    bool isValid();
    /** Compares two templates. Ignores differences due to empty purpose lines.
     */
    bool operator ==(const HoBIT_TransferTemplate & t2);    

    // simple getter methods
    const QString& ourAccountId()const;
    const QString& otherName()const;
    const QString& otherAccountId()const;
    const QString& otherBankCode()const;
    double value()const;
    const QString& currency()const;
    const QStringList& purpose()const; 
    const list < string> & purposeStrings()const;

private:
    QString _OurAccountId;
    QString _OtherName;
    QString _OtherAccountId;
    QString _OtherBankCode;
    double _Value;
    QStringList _Purpose;
    list <string> _PurposeStrings;
    QString _Currency;
};



#endif
