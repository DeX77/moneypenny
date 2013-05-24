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
#ifndef HOBIT_TLV_H
#define HOBIT_TLV_H

#include <QString>
#include <QFile>

#include "uchararray.h"

/**
Implements the Tag - Length - Value format of the FinTS file spec. 
Datatype is uchar = unsigned char = byte
Bytes 0 and 1 = Tag
Bytes 2 and 3 = Length
following bytes = Value
Value-Data are copied. The copy is automatically deleted by the destructor.
cValue() returns a pointer to a zero terminated ASCIIZ string.
Partially inspired from hbcistring by Martin Preuss.

@author Walter Roth
*/
class HoBIT_TLV{
public:
     /** Constructor, creates a NULL TLV. 
     */ 
     HoBIT_TLV();
     /** Constructor, may be called with a NULL pointer. Constructs a TLV from a TLV record.
     */ 
     HoBIT_TLV(uchar * data);
     
     /** Constructor. Constructs a TLV from Tag and Value string.
     */ 
     HoBIT_TLV(uchar tag, const QString & sValue);
     
     /** Constructor. Constructs a TLV from Tag and binary Value data.
     */ 
     HoBIT_TLV(uchar tag, const UCharArray & uValue);
     
     /** Constructor. Constructs a TLV from Tag, Length and Value.
     */ 
     HoBIT_TLV(uchar tag, uint length, uchar * uValue);
 
     ~HoBIT_TLV();
 
     /** The copy operator. Makes a deep copy.
     */
     HoBIT_TLV & operator =(const HoBIT_TLV & toCopy);
     /** Convenience copy operator. Parses data and makes a deep copy.
     */
     HoBIT_TLV & operator =(uchar * data);    
     /**Returns a Pointer to the last char of the TLV + 1. 
     * To be used for parsing a gapless block of TLV's. In this case, the first
     * byte of the next TLV is the address of the last byte of thisTLV  + 1.
     */
     static uchar * nextTLV(uchar * thisTLV);
     /** Returns a pointer to the last byte of the TLV.
     */
     static uchar * lastByte(uchar * thisTLV);
     /** returns tag as an int for easy comparison
     */
     uchar tag()const;
     /** returns length
     */
     uint length()const;
     /** Returns a char * to an ASCIIZ string constructed from value.
     */
     char * cValue();
     /** Returns a char * to an ASCIIZ string constructed from value in thisTLV.
     */
     static uchar * uValue(uchar * thisTLV);
     /** returns value
     */
     uchar * uValue()const;
     /** Returns a QString constructed from _Value. This will not work for encrypted data, which may contain several \0!
     */
     QString sValue();
     /** Returns true, if tag equals tagToCompare
     */
     bool isA(uchar tagToCompare)const;
     /** Create a readable string, format value as string
     */
     QString debugString();
     /** Create a readable string, format Value as Hex numbers
     */
     QString debugHex();
     /**Read the next TLV record from file.Returns number of bytes read.
     */
     int readNextTLV(QFile * file);  
     /**Returns value as a string of hex numbers
     */
     QString binToString(uint leftBorder);
     
     /**Returns an UCharArray ready to be added to a chain of TLV-records
     */
     UCharArray toUCharArray();
       
private:
     /** Parses a char * to a TLV record
     */
     void parse(uchar * data);
    
    /** The Tag, Length, Value fields
    */
    uchar _Tag; 
    /**A private copy of the value. Will be deleted.
    * _Value is of length _Length +1 and terminated by a \0, so that it may be used as a ASCIIZ string.
    * However, the trailing zero must not be copied to blocks of TLV's.
    */
    UCharArray _Value;
    
    static uint _DataOffset;
    static uint _LengthOffset;
};

#endif
