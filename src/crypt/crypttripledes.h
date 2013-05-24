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
#ifndef CRYPTTRIPLEDES_H
#define CRYPTTRIPLEDES_H

#include <qstring.h>

#include <openssl/des.h>
#include <openssl/evp.h>

#include "crypt.h"

/**
Encryption and decryption using the openssl implementation of DES. It is NOT recommended to use the simple ECB algorithm. EDE3_CBC is presently (2005) considered to be secure.
To be used as the Crypt engine of a CryptData object.
@author Walter Roth
*/
typedef union{
  unsigned char Array [24];
  DES_cblock Blocks[3];
}KeyTripleDes;
/** The type of algorithm used. see man:des for details.
*/
typedef enum {
 DES_ECB,
 DES_EDE2_CBC,
 DES_EDE3_CBC
}DES_CIPHER_TYPE;

class CryptTripleDes: public Crypt{
public:
    CryptTripleDes();

    virtual ~CryptTripleDes();
  /** set the keystring from password.
  */
  void setPassword(const QString password);  
  /** encrypt data */
  virtual bool encrypt();

  /** decrypt data */
  virtual bool decrypt();

  /** return the 16-byte key-string */
  const KeyTripleDes & getKey() const;


  /** set the 16-byte key-string */
  void setKey(const KeyTripleDes & key);
  /** Read property of DES_CIPHER_TYPE _DesCipherType. 
  */
  const DES_CIPHER_TYPE& getDesCipherType()const;
  /** Write property of DES_CIPHER_TYPE _DesCipherType. Sets Keylength too.
  */
  void setDesCipherType( const DES_CIPHER_TYPE& _newVal);

  /** Read property of int _Keylength. 
  */
  const int& getKeyLength()const;

  
private:
 /** Make the data length dividable by 8 and copy parent data to buffer. 
 */
 int paddAndCopyData();
 /** Remove padded characters and return data to parent. Checks for correct padding.
 */
 int unpaddAndReturnData(bool * ok);
 /** Create the key schedules from _Key
 */
 bool createKeySchedules();
 /** The 16 byte (128bit) key
 */
 KeyTripleDes _Key;
 /** The length of the key used May be 8, 16 or 24 bytes
 */
 int _KeyLength;
 /** The key schedules. see man:des for details
 */  
  DES_key_schedule _Ks[3];
 /** The key blocks. see man:des for details
 */ 
  DES_cblock * _Block[3];
 /** The initialization vector. see man:des for details
 */
 DES_cblock _Ivec; 
 /** The password from which the key was generated.
 */
 QString _Password;  
 /** DesCipherType controls the algorithm used for de/encryption
 */
 DES_CIPHER_TYPE _DesCipherType;
};

#endif
