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
#ifndef CRYPT_H
#define CRYPT_H

#include <cstddef>

#include "uchararray.h"

class CryptData;
/**
Abstract base class for various crypt algorithm classes.
Crypt objects should be owned by CryptData objects. 
Call the parent CryptData object's encrypt and decrypt functions for data encryption and decryption.
Allows CryptData to call the virtual encrypt and decrypt functions of derived classes (Crypt_TripleDes, Crypt_Rsa, Crypt_Blowfish).

@author Walter Roth
*/
class Crypt{
public:
    Crypt();

    virtual ~Crypt();

  /** Encrypt data. Returns true on success
  . * To be overwritten by subclasses. 
    */
  virtual bool encrypt()=0;

  /** Decrypt data. Returns true on success
    * To be overwritten by subclasses
    */
  virtual bool decrypt()=0;
  
  /**Set the CryptData object which owns the Crypt.
  */
  virtual void setParent(CryptData * parent);
  
protected:
  /**The parent CryptData object. Use its data buffer for reading input and writing output.
  */
  CryptData * _Parent;
  /** the buffer for the data to be en- or decrypted
  */
  UCharArray _Buffer;  
  /** De/encrypt constants
  */
  static const int ENCRYPT = 1;
  static const int DECRYPT = 0;
};

#endif
