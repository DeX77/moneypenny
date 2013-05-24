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
#include "crypttripledes.h"
#include "cryptdata.h"

#include <openssl/md5.h>

CryptTripleDes::CryptTripleDes():
Crypt()
{
 _KeyLength = sizeof(_Key); // 24 bytes 
 for (int i = 0; i < _KeyLength; i++)
    _Key.Array[i] = 0;
 for (int i=0; i<8; i++) // start with a zero initialization vector
   _Ivec[i] = 0;	+
 EVP_add_cipher(EVP_des_ecb());
 EVP_add_cipher(EVP_des_ede3_cbc());
}


CryptTripleDes::~CryptTripleDes()
{
  qDebug("Destructor CryptTripleDes finished");
}

/** set the keystring from password.
*/
void CryptTripleDes::setPassword(const QString password){
 _Password = password;
 switch (_KeyLength){
 case 16: MD5((unsigned char *)_Password.toLatin1().data(), 
          _Password.length(), 
          (unsigned char *)&_Key); break; //compute md5 digest of password and put it into _Key
  default: break;
 }
}

 /** Create the key schedules from _Key
 */
 bool CryptTripleDes::createKeySchedules(){
  int nBlocks = _KeyLength /8;
  int result = true;
  //create key schedules from KeyData
  for(int nBlock = 0; nBlock < nBlocks; nBlock ++){
    DES_set_key_unchecked(&_Key.Blocks[nBlock], &_Ks[nBlock]);
   //uncomment the following code, if you need parity checking. 
  /*  result = DES_set_key_checked(&_Key.Blocks[nBlock], &_Ks[nBlock]);
    if(result < -1){ //-1 is parity error, this is very probable, since parity has not been set
      qDebug("Error when creating DES key schedules. DES Error code: %i", result);
      return false;
    }*/
  }//end for
  return result;
 }

/** encrypt data */
bool CryptTripleDes::encrypt(){
  if (!createKeySchedules())
   return false;
  paddAndCopyData();
  UCharArray outputBuffer;
  outputBuffer.resize(_Buffer.strLength());  
  switch(_DesCipherType){
   case DES_ECB        : for (uint nBlock = 0; nBlock < _Buffer.strLength() / 8; nBlock ++)
                       DES_ecb_encrypt((DES_cblock *)_Buffer.data(), (DES_cblock *)outputBuffer.data(), &_Ks[0], Crypt::ENCRYPT);
                     break;
   case DES_EDE2_CBC   : DES_ede2_cbc_encrypt(_Buffer.data(), outputBuffer.data(), _Buffer.strLength(), 
                                          &_Ks[0], &_Ks[1], &_Ivec, Crypt::ENCRYPT);
                     break;
   case DES_EDE3_CBC   : DES_ede3_cbc_encrypt(_Buffer.data(), outputBuffer.data(), _Buffer.strLength(), 
                                          &_Ks[0], &_Ks[1], &_Ks[2], &_Ivec, Crypt::ENCRYPT);
                     break;
   default         : DES_ede3_cbc_encrypt(_Buffer.data(), outputBuffer.data(), _Buffer.strLength(), 
                                          &_Ks[0], &_Ks[1], &_Ks[2], &_Ivec, Crypt::ENCRYPT);
                     break;
  };
  _Parent->setData(outputBuffer);
  return true;
}

/** decrypt data */
bool CryptTripleDes::decrypt(){
  bool result;
  if (!createKeySchedules())
   return false;
  _Buffer.resize(_Parent->getLength());  
  
  switch(_DesCipherType){
   case DES_ECB        : for (uint nBlock = 0; nBlock < _Buffer.strLength() / 8; nBlock ++)
                       DES_ecb_encrypt((DES_cblock *)_Parent->getData(), (DES_cblock *)_Buffer.data(), &_Ks[0], Crypt::DECRYPT);
                       break;
   case DES_EDE2_CBC   : DES_ede2_cbc_encrypt(_Parent->getData(), _Buffer.data(), _Buffer.strLength(), 
                                          &_Ks[0], &_Ks[1], &_Ivec, Crypt::DECRYPT);
                       break;
   case DES_EDE3_CBC   : DES_ede3_cbc_encrypt(_Parent->getData(), _Buffer.data(), _Buffer.strLength(), 
                                          &_Ks[0], &_Ks[1], &_Ks[2], &_Ivec, Crypt::DECRYPT);
                       break;
   default         : DES_ede3_cbc_encrypt(_Parent->getData(), _Buffer.data(), _Buffer.strLength(), 
                                          &_Ks[0], &_Ks[1], &_Ks[2], &_Ivec, Crypt::DECRYPT);
                     break;
  };  
  unpaddAndReturnData(&result);
  return result;
}

/** return the 16-byte key-string */
const KeyTripleDes & CryptTripleDes::getKey() const{
 return _Key;
}


/** set the 16-byte key-string */
void CryptTripleDes::setKey(const KeyTripleDes & key) {
  for(uint i = 0; i < sizeof(_Key); i++)
    _Key.Array[i] = key.Array[i];
}

 /** Make the data length dividable by 8 (as specified in RFC 1423) and copy parent data to buffer. 
 */
 int CryptTripleDes::paddAndCopyData(){
  uint dataLength = _Parent->getLength();
  uchar paddByte = 8 -  dataLength % 8;// This makes shure, that the last byte in buffer contains the padd length.
  uint bufferLength = (dataLength / 8 + 1) * 8; //include the padd bytes
  _Buffer.resize(bufferLength);
  uchar * parentData = _Parent->getData();
  for(uint i = 0; i < dataLength; i++)
   _Buffer[i] = parentData[i];
  for(uint i = dataLength; i < bufferLength; i++)
   _Buffer[i] = paddByte;
  return _Buffer.strLength(); 
 }
 
 /** Remove padded characters and return data to parent.
 */
 int CryptTripleDes::unpaddAndReturnData(bool * ok){ 
  uint paddLength =(unsigned int) _Buffer[_Buffer.strLength() -1];
  bool paddingOk = (paddLength <= 8) && (paddLength >= 1); // paddLength must be 1 to 8
  if(paddingOk){//the last bytes must be identical
   for(uint nByte = 1; nByte <= paddLength; nByte++) // start from 1, last byte is [_BufferLength -1]
     if((_Buffer[_Buffer.strLength() - nByte]) != (_Buffer[_Buffer.strLength()-1]))
      paddingOk = false;
  }
  *ok = paddingOk;
  if(!paddingOk) //do not touch parent data, return 0.
    return 0;  
  uint realDataLength = _Buffer.strLength() - paddLength; //last byte contains number of padded bytes 
  _Buffer.resize(realDataLength);
  _Parent->setData(_Buffer);
  return realDataLength;
 }

/** Read property of DES_CIPHER_TYPE _DesCipherType. 
*/
const DES_CIPHER_TYPE& CryptTripleDes::getDesCipherType()const{
	return _DesCipherType;
}

/** Write property of DES_CIPHER_TYPE _DesCipherType. 
*/
void CryptTripleDes::setDesCipherType( const DES_CIPHER_TYPE& _newVal){
  _DesCipherType = _newVal;
  switch (_DesCipherType){
   case DES_ECB      : _KeyLength = 8;  break;
   case DES_EDE2_CBC : _KeyLength = 16; break;
   case DES_EDE3_CBC : _KeyLength = 24; break;
   default :  _KeyLength =24;break;
  }//end switch      
}

/** Read property of int _Keylength. 
*/
const int& CryptTripleDes::getKeyLength()const{
	return _KeyLength;
}



