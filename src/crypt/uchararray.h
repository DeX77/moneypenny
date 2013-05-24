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
#ifndef UCHARARRAY_H
#define UCHARARRAY_H

#include <QVector>
#include <qstring.h>

/**
An array of unsigned chars for use in cryptograhic classes. A trailing zero is added
when constructed from strings. 
The count() function returns the data length including any trailing zero. 
The strLength() function returns the length without the trailing ASCIIZ zero.
Use data(), strLength() if a pointer to the data and the data length are required by low level C functions. (e.g. OpenSSL)
The private _IsAsciiz field keeps track of an added zero.

@author Walter Roth
*/
class UCharArray : public QVector<unsigned char>
{
public:
    /** Creates a NULL array
    */
    UCharArray();
    /** Creates an array from data. Performs a deep copy.
    */
    UCharArray(unsigned char * sourceData, int length);
    /** Creates an array from data. Performs a shallow copy.
    */
    UCharArray(const UCharArray & toCopy);

    ~UCharArray();
    /** Returns a QString constructed from data.
    */
    QString toString();
    /** Returns a pointer to an ASCIIZ String. Adds trailing zero, if not already present.
     */
    char * cString(); 
    /** Returns length of string-data without any added ASCIIZ zero.
    */
    uint strLength()const;
    /** Loads all characters of the string into data.
    */
    void loadFromString(const QString s);
    /** Returns a dump of hex formatted numbers.
    */
    QString toHexString(uint leftBorder);
    /** Works like adding a string. Concatenation operator.
    */
    UCharArray & operator +=(const UCharArray &toAdd);
    /** Copy operator. calls inherited operator =, which makes a shallow copy
    */
    UCharArray & operator =(const UCharArray &toCopy);
    /** Compare operator. 
    */
    bool operator ==(const UCharArray &toCopy);
    /** Fast copy function, uses memcpy
     */
     void duplicate(uchar * start, int length);
private:
     /** True, if a trailing zero has been added to make an ASCIIZ string
     */
     bool _IsAsciiz;

};

#endif
