/***************************************************************************
 *   Copyright (C) 2004 by Walter Roth                                     *
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
#include "dtasection.h"

#include <QTextCodec>

/** The global codec for string conversion
  */
QTextCodec * DTACodec = QTextCodec::codecForName("IBM 850");


DTASection::DTASection(const QByteArray & s)
 : QByteArray(s)
{
 goToFirstField();
}


DTASection::~DTASection()
{
}

/** Use this operator to set contents from a QString of length 128.
  * Throws ExDTAError on wrong length.
  */
const DTASection & DTASection::operator = (const QByteArray & s)
{
  if(s.length() != 128)
   throw ExDTAError ("DTASection::operator = : Wrong string length!");
  *this = DTASection(s);
  return *this; 
}


 /** Add a field of length length.
   * Align left and padd with blanks for alpha, align right and padd with zeros for numeric fields.
   */ 
 void DTASection::addField(const QString & text, int length, PaddingMode mode)
{
  append(paddField(text, length, mode));
}

/** Add an uppercase field of length text.length().
  */ 
void DTASection::addField(const QString & text)
{
  append(toDTA8Bit(text.toUpper()).c_str());
}

/** Add a numeric field of length digits.
  */ 
void DTASection::addField(int number, int digits)
{
  addField(QString::number(number), digits, PaddingNumeric);
}


/** Create an uppercase QString of length length.
  * Align left and padd with blanks for alpha, align right and padd with zeros for numeric fields.
  */ 
QString DTASection::paddField(const QString & text, int length, PaddingMode mode)
{
  QString result;

  if(text.length() > length)
    throw ExDTAError
       (qPrintable("DTASection::paddField: Error, text is longer than specified length: " + QString::number(length)));

  switch(mode)
  {
    case PaddingAlpha:
    {
      result = text.toUpper();
      for(int pos = text.length(); pos < length; pos ++) // pad right
        result += " ";
    } break;

    case PaddingNumeric:
    {
      int padLength = length - text.length();
      for(int pos = 0; pos < padLength; pos ++) //pad left
        result += "0";
      result += text;
    } break;

    default: throw ExDTAError(qPrintable("DTASection::paddField: Unknown Paddingmode" + QString::number(mode)));
  }
  return result;
}

/** Returns a left aligned string of length 128 padded with blanks if required.
  */
QByteArray DTASection::getSectionString()
{
   return toDTA8Bit(paddField(*this, 128, PaddingAlpha)).c_str();
}

/**For debugging. Outputs data added so far and present data length to stderr.
 */
void DTASection::debugOutput(const QString & prefix)
{
   #ifdef DEBUG_DTA
   QString debugString = fromDTA8Bit(*this); 
   debugString.replace(" ", ".");
   qDebug(qPrintable(prefix + "DTASection  Length: %i Data: " + debugString), length()); 
   #endif
}

/** Adds a string with n blanks.
*/
void DTASection::addBlankString(int length)
{
  addField(DTASection::blankString(length));
}

/** Returns a string with n blanks.
*/
QString DTASection::blankString(int length)
{
  QString result;
   for (int i = 0; i < length; i++)
    result.append(' ');
  return result;
}

/**
  * Set nextFieldPos to 0.
  */
void DTASection::goToFirstField()
{
  nextFieldPos = 0;
}

/**
  * Returns length of Satz.
  * Length is always the first field with a length of 4 of a DTA-Satz.
  * Sets nextField to 4.
  * Throws ExDTAError on non-numeric data and non-multiple of 128.
  */
QString DTASection::satzLength(int *length)
{
  goToFirstField();
  QString result = nextNumericField(4);
  bool ok = false;
  int l = result.toInt(&ok);
  if(length != NULL)
   *length = l;
  if( !ok || (l < 1))
    throw(ExDTAError(
     ("DTASection::satzLength: DTA Format Error, no valid length in first 4 bytes.\n" + *this).data()));
  return result;
}

/**
  * Read the next field with fieldLength. Set nextField to start of next field.
  * Throws ExDTAError on non-numeric data.
  */
QString DTASection::nextNumericField( int fieldLength)throw(ExDTAError)
{
  int start = nextFieldPos;
  QString result = nextField(fieldLength);
  bool isNumeric = true;
  for(int pos = 0; pos < result.length(); pos ++)
  { 
     if(!result[pos].isDigit())
       isNumeric = false;
  }

  if(!isNumeric)
  {
      QString msg = QString(" DTA Format Error, non numeric data in numeric field. \n")
                   + "Startposition: " + QString::number(start) + "\n"
                   + "Length: " + QString::number(fieldLength) + "\n"
                   + "Field contents: " + result + "\n"
                   + "Satz contents: " + *this +"\n";
      throw(ExDTAError(qPrintable("DTASection::nextNumericField: " + msg)));
  }
  return result;
}


/**
  * Read the next field with fieldLength. Set nextField to start of next field.
  * Throws ExDTAError on non-zero data.
  */
QString DTASection::nextZeroField( int fieldLength)throw(ExDTAError)
{
  int start = nextFieldPos;
  QString result = nextNumericField(fieldLength);
  bool nonZero = false;
  //Check all chars for "0"
  for(int pos = 0; pos < result.length(); pos ++)
  { 
     if(result[pos] != QChar('0'))
       nonZero = true;
  }
  if(nonZero)
  {
      QString msg = QString(" DTA Format Error, non zero data in zero field. \n")
                   + "Startposition: " + QString::number(start) + "\n"
                   + "Length: " + QString::number(fieldLength) + "\n"
                   + "Field contents: " + result + "\n"
                   + "Satz contents: " + *this +"\n";
      throw(ExDTAError(qPrintable("DTASection::nextNumericField: " + msg)));
  }
  return result;
}


/**
  * Read the next field with fieldLength. Set nextField to start of next field.
  * Throws ExDTAError on non-blank data.
  */
QString DTASection::nextBlankField( int fieldLength)throw(ExDTAError)
{
  int start = nextFieldPos;
  QString result = nextField(fieldLength);
  bool nonBlank = false;
  //Check all chars for "0"
  for(int pos = 0; pos < result.length(); pos ++)
  { 
     if(result[pos] != QChar(' '))
       nonBlank = true;
  }
  if(nonBlank)
  {
      QString msg = QString(" DTA Format Error, non blank data in blank field. \n")
                   + "Startposition: " + QString::number(start) + "\n"
                   + "Length: " + QString::number(fieldLength) + "\n"
                   + "Field contents: " + result + "\n"
                   + "Satz contents: " + *this +"\n";
      throw(ExDTAError(qPrintable("DTASection::nextBlankField: " + msg)));
  }
  return result;
}

/**
  * Read the next field with fieldLength from s. Set nextField to start of next field in s.
  * Throws ExDTAError on too short string.
  */
QString DTASection::nextField( int fieldLength)throw(ExDTAError)
{
  int start = nextFieldPos;
  nextFieldPos += fieldLength;
  if(length() < start + fieldLength)
  {
      QString msg = QString(" DTA Format Error, satz is too short. \n")
                   + "Startposition: " + QString::number(start) + "\n"
                   + "Length: " + QString::number(fieldLength) + "\n"
                   + "Satzlength : " + QString::number(length()) + "\n"
                   + "Satz contents: " + *this +"\n";
      throw(ExDTAError(qPrintable("DTASection::nextNumericField: " + msg)));
  }
  return fromDTA8Bit(mid(start, fieldLength));
}

/**Set the global codec for DTA files
  */
void DTASection::setDTAencoding(const QString & codecName)
{
  DTACodec = QTextCodec::codecForName(qPrintable(codecName));
}
/** Convert from 8 bit to unicode using DTACodec
  */
QString DTASection::fromDTA8Bit(const QByteArray & s)
{
   return DTACodec->toUnicode(s);
}
/** Convert from 8 bit to unicode using DTACodec
  */
QString DTASection::fromDTA8Bit(const std::string & s)
{
   return DTACodec->toUnicode(s.c_str());
}


/** Convert from unicode to 8 bit using DTACodec
  */
std::string DTASection::toDTA8Bit(const QString & s)
{
   return std::string(DTACodec->fromUnicode(s).data());
}

