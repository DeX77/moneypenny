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
#include "uchararray.h"

#define DEBUG_UCHARARRAY
/** Creates a NULL array
*/
UCharArray::UCharArray()
 : QVector<unsigned char>()
{
 _IsAsciiz = false;
}

/** Creates an array from data. Performs a deep copy.
*/
UCharArray::UCharArray(unsigned char * sourceData, int length)
: QVector<unsigned char>(){
  resize(length);
  for(int i=0; i < length; i++)
   data()[i]= sourceData[i];  
 _IsAsciiz = false;
}
 
/** Creates an array from data. Performs a shallow copy.
*/
UCharArray::UCharArray(const UCharArray & toCopy)
: QVector<unsigned char>(toCopy){
 _IsAsciiz = toCopy._IsAsciiz;
}

UCharArray::~UCharArray()
{
}

/** Returns a QString constructed from data.
*/
QString UCharArray::toString(){
    return QString(cString());
}

/** Returns a pointer to an ASCIIZ String. Adds trailing zero, if not already present.
 */
char * UCharArray::cString(){
 if(! _IsAsciiz){
   resize(count() +1);
   data()[count()-1] = 0; //add a trailing zero, to make an ASCIIZ string
   _IsAsciiz = true;
 } 
// uchar end = data()[count()-1]; //for debugging
 char * result = (char *) data();
 return result;
}

/** Returns a dump of hex formatted numbers.
*/
QString UCharArray::toHexString(uint leftBorder){
    QString result;
    QString sHex;
    QString sLeft;
    for (uint i = 0; i < leftBorder; i++)
      sLeft += " ";

    result+= sLeft;
    uint last = count();
    if(_IsAsciiz) //do not dump a trailing ASCIIZ zero
      last--;
    for (uint pos=0; pos < last; pos++) {
        if ((pos % 32) == 0)
            result += "\n" + sLeft;
        else if ((pos % 4) == 0)
            result+="  ";
        sHex.setNum(data()[pos], 16);
        if(sHex.length() < 2)
          sHex = "0" + sHex;   
        result += sHex + " ";
    } // for;
    return result;
}

/** Loads all characters of the string into data.
*/
void UCharArray::loadFromString(const QString s){
  resize(s.length() + 1);
  for(int i = 0; i <= s.length(); i++)
     data()[i] = s.toLatin1().data()[i];
  data()[count()-1] = 0; //add trailing zero as last byte   
  _IsAsciiz = true; 
}

/** Works like adding a string. Concatenation operator.
*/
UCharArray & UCharArray::operator += (const UCharArray & toAdd){
 uint catPosition = count();
 if(_IsAsciiz)   //overwrite any trailing ASCIIZ zero
   catPosition --;
 uint addCount = toAdd.count();
 resize(count() + addCount);
 for(uint i = 0; i < addCount; i++)
   data()[catPosition + i] = toAdd[i];
 _IsAsciiz = toAdd._IsAsciiz; //ASCIIZ zero at the end or not?  
 return * this;  
}

/** Copy operator. Calls inherited operator =, which makes a shallow copy
*/
UCharArray & UCharArray::operator =(const UCharArray &toCopy){
  QVector<unsigned char>::operator=(toCopy);
  _IsAsciiz = toCopy._IsAsciiz;
  return *this; 
}

/** Compare operator. 
*/
bool UCharArray::operator ==(const UCharArray &toCompare)
{
  if(size() != toCompare.size())
  {
    #ifdef DEBUG_UCHARARRAY
     qDebug("UCharArray::operator == : Size mismatch: this:% i toCompare% i", size(), toCompare.size());
    #endif
    return false;
  }
  for (int i = 0; i < size(); i++)
  {
    if(at(i) != toCompare.at(i))
    {
    #ifdef DEBUG_UCHARARRAY
     qDebug("UCharArray::operator == : Data mismatch at position: %i this:% i toCompare% i", i, at(i), toCompare.at(i));
     return false;
    #endif
    }
   } //end for
  return true;
}

/** Returns length of string-data without any added ASCIIZ zero.
*/
uint UCharArray::strLength()const{
 if(_IsAsciiz)
    return count() -1;
 else
    return count();   
}

/** Fast copy function, uses memcpy
 */
void UCharArray::duplicate(uchar * start, int length){
  memcpy(data(), start, length);
}
