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
#include "cryptdata.h"
#include "qapplication.h"

/** Call constructor with zero, one or all three parameters. Data without length results in zero data.
  * The Crypt object is owned by CryptData. It is deleted automatically.
  */
CryptData::CryptData(Crypt * crypt, unsigned char * data, unsigned int length)
{
 qDebug("Constructor CryptData");
 _Crypt = crypt;
 if(_Crypt)
   _Crypt->setParent(this);
 setData(data, length);
}


/** deletes the Crypt object and the data buffer.
*/
CryptData::~CryptData()
{
 if(_Crypt)
   delete _Crypt;
 qDebug("Destructor CryptData finished."); 
}

/** Copy length bytes beginning at data into the internal buffer
*/
void CryptData::setData(unsigned char * data, unsigned int length){  
   _Data.resize(length);
   _Data.duplicate(data, length);
}


/** Copy the data to internal _Data buffer
*/
void CryptData::setData(const UCharArray & data){
  _Data= data;
}


/**Read property of unsigned int _Length
*/
unsigned int CryptData::getLength()const{
  return _Data.strLength();
}


/** Read property of unsigned char * _Data
*/
unsigned char * CryptData::getData(){
  return _Data.data();
}


/** Attach a new crypt engine for encryption and decryption
*/
void CryptData::setCrypt(Crypt * crypt){
 if(_Crypt)
   delete _Crypt; 
 _Crypt = crypt;
 if(_Crypt)  
  _Crypt->setParent(this);
}


/** The crypt engine for encryption and decryption
*/
Crypt * CryptData::getCrypt(){
  return _Crypt;
}


/** encrypt data using the attached Crypt engine
*/
bool CryptData::encrypt(){
  return _Crypt->encrypt();
}


/** decrypt data using the attached Crypt engine
*/
bool CryptData::decrypt(){
  return _Crypt->decrypt();
}

/** Returns a pointer to the last valid byte of data.
*/
unsigned char * CryptData::getDataEnd(){
  return _Data.data() + _Data.strLength()-1;
}
