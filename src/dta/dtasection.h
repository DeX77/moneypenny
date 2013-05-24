/***************************************************************************
 *   Copyright (C) 2011 by Walter Roth                                     *
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
#ifndef DTASECTION_H
#define DTASECTION_H

#include <QString>

#include <string.h>
#include <iostream>

#include "exdtaerror.h"

#define DEBUG_DTA 1


/**
A 128 byte long 8-bit string as used in DTA files. Encoding to and from unicode is done 
by toDTA8bit and from DTA8Bit functions using a global codec. Default codec is IBM 850.
Use setDTAEncoding to set a different codec.
Use addField to add data. The getSectionString function returns a left aligned string of 128 chars length padded with blanks to the right, if required.
Use operator = (QString) to set contents. Use satzLength() and nextXXXField() functions to parse a DTA string. 

	@author Walter Roth 
*/
class DTASection : public QByteArray
{
public:
    typedef enum
    {
      PaddingAlpha,
      PaddingNumeric
    }PaddingMode;

    DTASection(const QByteArray & s = "");

    ~DTASection();

     /**Set the global codec for DTA files. Default is IBM 850
       */
     static void setDTAencoding(const QString & codecName);
     /** Convert from 8 bit to unicode using DTACodec
       */
     static QString fromDTA8Bit(const QByteArray & s);
     /** Convert from 8 bit to unicode using DTACodec
       */
     static QString fromDTA8Bit(const std::string & s);
     /** Convert from unicode to 8 bit using DTACodec
       */
     static std::string toDTA8Bit(const QString & s);

   //Set functions

    /** Use this operator to set contents from a QString of length 128.
      * Throws ExDTAError on wrong length.
      */
    const DTASection & operator = (const QByteArray & s);
    /** Add an uppercase field of length length.
      * Align left and padd with blanks for alpha, align right and padd with zeros for numeric fields.
      */ 
    void addField(const QString & text, int length, PaddingMode mode);
    /** Add an uppercase field of length text.length().
      */ 
    void addField(const QString & text);
    /** Add a numeric field of length digits.
      */ 
    void addField(int number, int digits);
    /** Create an uppercase QString of length length.
      * Align left and padd with blanks for alpha, align right and padd with zeros for numeric fields.
      */ 
    QString paddField(const QString & text, int length, PaddingMode mode);
    /** Adds a string with n blanks.
    */
    void addBlankString(int length);
    /** Returns a string with n blanks.
    */
    static QString blankString(int length);

    /** Returns a left aligned string of length 128 padded with blanks if required.
      */
    QByteArray getSectionString();
   /**For debugging. Outputs data added so far and present data length to stderr.
    */
   void debugOutput(const QString & prefix);


   //Parse functions

   /**
     * Returns length of Satz.
     * Length is always the first field with a length of 4 of a DTA-Satz.
     * If length * is != NULL, length is written to length *. 
     * If a number conversion errro occurs, -1 is written to length* and an
     * ExDTAError is thrown.
     * Sets nextField to 4.
     * Throws ExDTAError on non-numeric data.
     */
   QString satzLength(int * length = NULL);
   /**
     * Read the next field with fieldLength. Set nextField to start of next field.
     * Throws ExDTAError on non-numeric data.
     */
   QString nextNumericField( int fieldLength)throw(ExDTAError);
   /**
     * Read the next field with fieldLength. Set nextField to start of next field.
     * Throws ExDTAError on non-zero data.
     */
   QString nextZeroField( int fieldLength)throw(ExDTAError);
   /**
     * Read the next field with fieldLength from s. Set nextField to start of next field.
     * Throws ExDTAError on non-blank data.
     */
   QString nextBlankField( int fieldLength)throw(ExDTAError);

   /**
     * Set nextFieldPos to 0.
     */
   void goToFirstField();

   /**
     * Read the next field with fieldLength from s. Set nextField to start of next field.
     * Throws ExDTAError on non-numeric data.
     */
   QString nextField( int fieldLength)throw(ExDTAError);

 private:
   /** 
     * Used for getNextField(). Points to the next field in string.
     */
   int nextFieldPos;
};

#endif
