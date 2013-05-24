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
#include "hobit_tlv.h"
#include "hobit_string.h"

#include <assert.h>
#include <cstddef>
#include <string.h>

#include "uchararray.h"
#include "globals.h"

 // this may be used with a template typ to take care of RDH2 2Byte tags.
 uint HoBIT_TLV::_LengthOffset = sizeof(uchar);
 uint HoBIT_TLV::_DataOffset = sizeof(uchar) + sizeof(unsigned short int);
 
 /** Default constructor, creates a NULL TLV
 */ 
 HoBIT_TLV::HoBIT_TLV(){
    _Tag = (uchar)0;
 }
 /** Constructor, may be called with a NULL pointer
 */ 
 HoBIT_TLV::HoBIT_TLV(uchar * data){
    _Tag = (uchar)0;
    parse(data);
 }

/** Constructor. Constructs a TLV from Tag and Value string.
*/ 
HoBIT_TLV::HoBIT_TLV(uchar tag, const QString & sValue){
 _Tag = tag;
 _Value.loadFromString(sValue);
 if(sValue.length() >= 0xFFFF){
   _Value.resize( 0xFFFE); 
   qDebug("HoBIT_TLV::HoBIT_TLV: Error! String sValue is too long! Cilpped.");
 }    
}

/** Constructor. Constructs a TLV from Tag, Length and Value .
*/ 
HoBIT_TLV::HoBIT_TLV(uchar tag, uint length, uchar * uValue){
 _Tag = tag;
 if(length < 0xFFFF)
   _Value = UCharArray(uValue, length);
 else {
   _Value = UCharArray(uValue, 0xFFFE);
   qDebug("HoBIT_TLV::HoBIT_TLV: Error! Parameter length is too big! Clipped.");
 }    
}

/** Constructor. Constructs a TLV from Tag and binary Value data.
*/ 
HoBIT_TLV::HoBIT_TLV(uchar tag, const UCharArray & uValue){
 _Tag = tag;
 _Value = uValue;
 if(uValue.count() >= 0xFFFF){
   _Value.resize(0xFFFE);
   qDebug("HoBIT_TLV::HoBIT_TLV: Error! UCharArray uValue is too long! Clipped.");
 }  
}
 
 
 /** Destructor
 */
 HoBIT_TLV::~HoBIT_TLV(){
}
 
  /** Parses a char * to a TLV record
  */
  void HoBIT_TLV::parse(uchar * data){
    assert(sizeof(short int) == 2); // This will not work if size of short int is != 2
    if(data){ //parse TLV record
       _Tag =  data[0];
       unsigned short int len = data[_LengthOffset] + data[_LengthOffset +1] * 256;
       _Value.resize(len);
       for (uint i = 0; i < len; i++) //copy value
         _Value[i] = data[i+ _DataOffset];
    }
    else{ //NULL pointer passed, create a NULL-TLV
       _Tag = 0;
       _Value.resize(0);
    }
  }
  
/** The copy operator. 
 */
 HoBIT_TLV & HoBIT_TLV::operator =(const HoBIT_TLV & toCopy){
   if(this != & toCopy){
    _Tag = toCopy._Tag;
    _Value = toCopy._Value;
    /*
    _Value.resize(toCopy._Length);
    for (int i = 0; i < _Length; i++) //copy value
      _Value[i] = toCopy._Value[i];*/
   }
   return * this;
 }

  /** Convenience copy operator. Parses data and makes a deep copy.
  */
  HoBIT_TLV & HoBIT_TLV::operator =(uchar * data){
    parse(data);
    return *this;
  }
     
  /**Returns a Pointer to the last char of the TLV + 1. 
  * To be used for parsing a gapless block of TLV's. In this case, the first
  * byte of the next TLV is the address of the last byte of thisTLV  + 1.
  */
  uchar * HoBIT_TLV::nextTLV(uchar * thisTLV){
   HoBIT_TLV tlv = HoBIT_TLV (thisTLV);
   return thisTLV + _DataOffset + tlv.length();
  }
  
  /** returns tag
  */
 uchar HoBIT_TLV::tag()const{
    return _Tag;
  }
  
  /** returns length
  */
  uint HoBIT_TLV::length()const{
    return _Value.strLength();
  }
  
  /** Returns a char * to an ASCIIZ string constructed from value. 
  */
  char * HoBIT_TLV::cValue(){  
    return _Value.cString();
  }
  
  /** returns value
  */
  uchar * HoBIT_TLV::uValue(uchar * thisTLV){
    return thisTLV + _DataOffset;
  }
  
  /** returns value
  */
  uchar * HoBIT_TLV::uValue()const{
    return (uchar *) _Value.data();
  }
  /** Returns true, if tag equals tagToCompare
  */
  bool HoBIT_TLV::isA(uchar tagToCompare)const{
    return _Tag == tagToCompare;
  }
  
  /** Create a readable string, format value as string
  */
  QString HoBIT_TLV::debugString(){  
   QString sNumber;
   sNumber.setNum((ushort)_Tag, 16); 
   return QString(QString("HoBIT_TLV::debugString: Tag: ") 
                  + sNumber 
                  +"  Length: " + QString::number(length()) 
                  +"  Value: " + sValue());
  }
  
  /** Create a readable string, format value as String and as Hex numbers
  */
  QString HoBIT_TLV::debugHex(){
    QString s0, result;
    s0.setNum(_Tag, 16);
    result = "HoBIT_TLV::debugString: Tag: " 
             + s0  
             +"  Length: " + QString::number(length()) 
             +"  sValue: " + sValue()
             +"  uValue: " + binToString(result.length());
    return result;               
 }

 /**Read the next TLV record form file.Returns number of bytes read.
 */
 int HoBIT_TLV::readNextTLV(QFile * file){
  uint bytesRead;
  unsigned short int len;
  if(file->atEnd())
    return 0;
  bytesRead = file->read((char *)&_Tag, sizeof(_Tag)); //read Tag
  if(bytesRead != sizeof(_Tag))
    return 0;
  bytesRead = file->read((char *)&len, sizeof(len)); //read Length
  if(bytesRead != sizeof(len))
    return 0;

  /* need conversion here if platform is not Little Endian   (AG) */
  if (!PLATFORM_IS_LITTLE_ENDIAN) 
    len = ( ((len & 0xFF00) >> 8) | ((len & 0xFF) << 8) );	
	
  _Value.resize(len); 
  bytesRead = file->read((char*)_Value.data(), len);  //read Value
  if(bytesRead != len)
    return 0;
  return length() + _DataOffset;
 }
   
/** Returns a QString constructed from _Value.
*/
QString HoBIT_TLV::sValue(){
 return QString(cValue());
}

/** Returns a pointer to the last byte of the TLV.
*/
uchar * HoBIT_TLV::lastByte(uchar * thisTLV){
 return nextTLV(thisTLV) -1;
}

/** For debugging. Creates a formatted hex dump
*/
QString HoBIT_TLV::binToString(uint leftBorder) {
    QString result;
    QString sHex;
    QString sLeft = "\n";
    for (uint i = 0; i < leftBorder; i++)
      sLeft += " ";
    
    uint pos;
    result+="   ";
    for (pos=0; pos < length(); pos++) {
        if ((pos % 32) == 0)
            result += sLeft;
        else if ((pos & 1) == 0)
            result+=" ";
        sHex.setNum(uValue()[pos], 16);   
        result += sHex;
    } // for;
    return result;
}

/**Returns an UCharArray ready to be added to a chain of TLV-records
*/
UCharArray HoBIT_TLV::toUCharArray(){
  UCharArray ar;
  ar.resize(_DataOffset + length());
//  uchar * d = ar.data(); //for debugging
//  char * cd = (char *)ar.data(); //for debugging
  //copy tag to byte 0
  ar[0] = tag();
  //copy the length to bytes 1 and 2
  unsigned short int len = length();
 
  // convert to Little Endian if necessary... (AG)
  if (!PLATFORM_IS_LITTLE_ENDIAN) 
    len = ( ((len & 0xFF00) >> 8) | ((len & 0xFF) << 8) ); //swap bytes
	      
  uchar *plen = (uchar *)&len;
  ar[_LengthOffset] = plen[0];
  ar[_LengthOffset + 1] = plen[1];
  
  //copy value to bytes 3 and following
  len = length();
  
  for (int i = 0; i < len; i++) //copy value
    ar[i + _DataOffset] = _Value[i];
  QString s = ar.toHexString(0);//for debugging  
  return ar;  
}

