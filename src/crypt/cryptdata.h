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
#ifndef CRYPTDATA_H
#define CRYPTDATA_H

#include <cstddef>
#include "crypt.h"

/**
A data container for cryptable data. Automatic padding and unpadding is performed by the Crypt engine if required. 
Therefore the length of encrypted data may be different from the length of decrypted data.
Do always call getLength to get the present length of data. 
Call encrypt or decrypt for data encryption and decryption. 
Data are encrypted and decrypted by the Crypt object assigned to the data by setCrypt. 
Passwords and keys etc. have to be set in the Crypt object prior to en/decryption. 
The Crypt object is owned by CryptData. It is deleted automatically.

@author Walter Roth
*/
class CryptData{
public:
    /** Call constructor with zero, one or all three parameters. Data without length results in zero data.
      * The Crypt object is owned by CryptData. It is deleted automatically.
      */
    CryptData(Crypt * crypt, unsigned char * data = NULL, unsigned int length = 0);

    /** deletes the Crypt object and the data buffer.
    */
    virtual ~CryptData();
    
    /** Copy length bytes beginning at data into the internal buffer
    */
    virtual void setData(unsigned char * data, unsigned int length);
    /** Copy the data to internal _Data buffer
    */
    virtual void setData(const UCharArray & data);
    
    /**Read property of unsigned int _Length
    */
    virtual unsigned int getLength()const;
    /** Read property of unsigned char * _Data
    */
    virtual unsigned char * getData();
    /** Returns a pointer to the last valid byte of data.
    */
    virtual unsigned char * getDataEnd();
    
    /** Attach a new crypt engine for encryption and decryption
    */
    void setCrypt(Crypt * crypt);
    /** The crypt engine for encryption and decryption
    */
    Crypt * getCrypt();
    /** encrypt data using the attached Crypt engine
    */
    virtual bool encrypt();
  
    /** decrypt data using the attached Crypt engine
    */
    virtual bool decrypt();
    
 private:
    /** The data buffer
    */
    UCharArray _Data;
    /** The Crypt engine
    */
    Crypt * _Crypt;     
};

#endif
