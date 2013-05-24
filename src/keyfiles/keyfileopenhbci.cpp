/***************************************************************************
 *   Copyright (C) 2005 by Walter Roth                                     *
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
#include "keyfileopenhbci.h"
#include "crypttripledes.h"
#include "kernel/global/HBCI_Cern_Message_defines"

#include <qobject.h>

//define the tags, if not already available
//defines copied from openhbci plugin mediumkeyfile.cpp
#ifndef MEDIUMKEYFILE_TAG_VERSION_MAJOR

  #define MEDIUMKEYFILE_TAG_CRYPT              (unsigned char)0xc2
  #define MEDIUMKEYFILE_VERSION_MAJOR          1
  #define MEDIUMKEYFILE_VERSION_MINOR          3
  
  #define MEDIUMKEYFILE_TAG_VERSION_MAJOR      (unsigned char)0x02
  #define MEDIUMKEYFILE_TAG_VERSION_MINOR      (unsigned char)0x03
  #define MEDIUMKEYFILE_TAG_SEQ                (unsigned char)0x04
  
  #define MEDIUMKEYFILE_TAG_USER_PUBSIGNKEY    (unsigned char)0xc5
  #define MEDIUMKEYFILE_TAG_USER_PRIVSIGNKEY   (unsigned char)0xc6
  #define MEDIUMKEYFILE_TAG_USER_PUBCRYPTKEY   (unsigned char)0xc7
  #define MEDIUMKEYFILE_TAG_USER_PRIVCRYPTKEY  (unsigned char)0xc8
  #define MEDIUMKEYFILE_TAG_USER_ID            (unsigned char)0x09
  
  #define MEDIUMKEYFILE_TAG_INST_PUBSIGNKEY    (unsigned char)0xca
  #define MEDIUMKEYFILE_TAG_INST_PUBCRYPTKEY   (unsigned char)0xcb
  #define MEDIUMKEYFILE_TAG_INST_COUNTRY       (unsigned char)0x0c
  #define MEDIUMKEYFILE_TAG_INST_CODE          (unsigned char)0x0d
  #define MEDIUMKEYFILE_TAG_INST_SYSTEMID      (unsigned char)0x0e
  
  // temporary keys (new in version 1.1)
  #define MEDIUMKEYFILE_TAG_TEMP_PUBSIGNKEY    (unsigned char)0xcf
  #define MEDIUMKEYFILE_TAG_TEMP_PRIVSIGNKEY   (unsigned char)0xd0
  #define MEDIUMKEYFILE_TAG_TEMP_PUBCRYPTKEY   (unsigned char)0xd1
  #define MEDIUMKEYFILE_TAG_TEMP_PRIVCRYPTKEY  (unsigned char)0xd2

  // new in version 1.4
  #define MEDIUMKEYFILE_TAG_SERVER_ADDR        (unsigned char)0xd3
  #define MEDIUMKEYFILE_TAG_SERVER_PORT        (unsigned char)0xd4
  #define MEDIUMKEYFILE_TAG_REMOTE_SEQ         (unsigned char)0xd5

  //Security modes RDH1, RDH2, RDH10 coded as 1, 2, 10
  #define MEDIUMKEYFILE_TAG_RDH_SECURITY_MODE     (unsigned char) 0x10

#endif

#define DEBUG_KEYFILEOPENHBCI true

/**Constructor sets filename.
*/
KeyFileOpenHbci::KeyFileOpenHbci(const QString & fileName, const QString & password)
 : KeyFile(fileName, password)
{
 _Seq = 0;
 _RdhSecurityMode = PASSPORT_SECURITY_RDH10_MODE;
 qDebug("Constructor KeyFileOpenHbci");
}

/** destructor
 */
KeyFileOpenHbci::~KeyFileOpenHbci()
{
  qDebug("Destructor KeyFileOpenHbci finished");
}

/** Read and parse the key file. Returns true, if ok. Sends error messages to stderr.
*/
bool KeyFileOpenHbci::parseFile(){
 HoBIT_TLV tlv;
 uchar * tlvStart;
 int bytesRead = 0;
  
 //Read all TLV's of the file
 //The first TLV is the contents of the whole file
 bytesRead = _TLV_File.readNextTLV(this);
  _ErrorText =  QObject::tr("Keyfile format error.");
 if(bytesRead == 0){//could not read
  qDebug("KeyFileOpenHbci::parseFile: Error when reading _TLV_CryptedData. First read attempt."); 
  return false;
 } 
 if(! _TLV_File.isA(MEDIUMKEYFILE_TAG_CRYPT)){//first byte is wrong
  qDebug("KeyFileOpenHbci::parseFile: Wrong TLV Tag at filepositon 0"); 
  return false;
 } 
 if((_TLV_File.length() % 8) != 0){//length must be dividable by 8, otherwise file is damaged.
  qDebug("KeyFileOpenHbci::parseFile: Wrong TLV length at filepositon 1"); 
  return false;
 } 
 //Next parse the file contents. It is now in _TLV_File.value()
 //First try to decrypt the data
 _ErrorText =  QObject::tr("Keyfile decryption error. Most probably the password is wrong.");
 setupCrypt();
 _CryptData->setData(_TLV_File.uValue(), _TLV_File.length());
 if (!_CryptData->decrypt())
    return false;  //decryption error, detected by _CryptData
 tlvStart = _CryptData->getData();
 _TLV_ContextData = HoBIT_TLV(tlvStart);
 qDebug("%s", _TLV_ContextData.debugString().toLatin1().data());
 //check for correct header tag
 _ErrorText =  QObject::tr("Keyfile parse error. Most probably unsupported file format.");
 if(_TLV_ContextData.tag() != MEDIUMKEYFILE_TAG_VERSION_MAJOR)
  return false; //decryption error, wron tag
// debugWriteToBinaryFile("~/decrypted_data.bin", _CryptData->getData(), _CryptData->getLength()); //for debugging
 uchar * dataEnd = _CryptData->getDataEnd();
 
 //Parse the decrypted data section of the file
 tlvStart = HoBIT_TLV::nextTLV(tlvStart);
 while(tlvStart < dataEnd){
   tlv = HoBIT_TLV(tlvStart); 
   if(DEBUG_KEYFILEOPENHBCI)
     qDebug("KeyFileOpenHbci::parseFile: %s", tlv.debugString().toLatin1().data());
   uchar tag = tlv.tag();
   switch(tag){
   
       //key type data
       case MEDIUMKEYFILE_VERSION_MINOR: break; //0x03, ignore
       case MEDIUMKEYFILE_TAG_SEQ:{
               QString s = tlv.sValue();
               _Seq = s.toInt();
               break;
       } 
       
       //User data
       case MEDIUMKEYFILE_TAG_USER_ID: { //0x09   
           _UserId = tlv.sValue();
           break; 
        }       
       case MEDIUMKEYFILE_TAG_USER_PUBSIGNKEY: { //0xC5   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_USER_PUBSIGNKEY");
           _UserPublicSignKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
       case MEDIUMKEYFILE_TAG_USER_PRIVSIGNKEY: { //0xC6   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_USER_PRIVSIGNKEY");
           _UserPrivateSignKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
       case MEDIUMKEYFILE_TAG_USER_PUBCRYPTKEY: { //0xC7   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_USER_PUBCRYPTKEY");
           _UserPublicCryptKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
       case MEDIUMKEYFILE_TAG_USER_PRIVCRYPTKEY: { //0xC8   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_USER_PRIVCRYPTKEY");
           _UserPrivateCryptKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
       
        //temporary keys
        case MEDIUMKEYFILE_TAG_TEMP_PUBSIGNKEY: { //0xCf   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_TEMP_PUBSIGNKEY");
           _TempPublicSignKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
       case MEDIUMKEYFILE_TAG_TEMP_PRIVSIGNKEY: { //0xd0   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_TEMP_PRIVSIGNKEY");
           _TempPrivateSignKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
       case MEDIUMKEYFILE_TAG_TEMP_PUBCRYPTKEY: { //0xd1   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_TEMP_PUBCRYPTKEY");
           _TempPublicCryptKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
       case MEDIUMKEYFILE_TAG_TEMP_PRIVCRYPTKEY: { //0xd2   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_TEMP_PRIVCRYPTKEY");
           _TempPrivateCryptKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
      
      
      //bank data
       case MEDIUMKEYFILE_TAG_INST_COUNTRY: { //0x0c  
           _InstCountry = tlv.sValue();
           break; 
        }   
       case MEDIUMKEYFILE_TAG_INST_CODE: { //0x0d  
           _InstCode = tlv.sValue();
           break; 
        }   
       case MEDIUMKEYFILE_TAG_INST_SYSTEMID: { //0x0e  
           _InstSystemId = tlv.sValue();
           break; 
        }   
       case MEDIUMKEYFILE_TAG_RDH_SECURITY_MODE: { //0x10
           _RdhSecurityMode = tlv.sValue();
           break; 
        }   
      case MEDIUMKEYFILE_TAG_INST_PUBSIGNKEY: { //0xCa   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_INST_PUBSIGNKEY");
           _InstPublicSignKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
      case MEDIUMKEYFILE_TAG_INST_PUBCRYPTKEY: { //0xCa   
            qDebug("\nParsing key: MEDIUMKEYFILE_TAG_INST_PUBCRYPTKEY");
           _InstPublicCryptKey.readFromHoBIT_TLV(tlvStart);
           break; 
        }   
       case MEDIUMKEYFILE_TAG_SERVER_ADDR: { //0xd3   
           _ServerAddress = tlv.sValue();
           break; 
        }       
       case MEDIUMKEYFILE_TAG_SERVER_PORT: { //0xd4   
           _ServerPort = tlv.sValue();
           break; 
        }       
       case MEDIUMKEYFILE_TAG_REMOTE_SEQ: { //0xd5   
           _RemoteSeq = tlv.sValue();
           break; 
        }       
       default: qDebug("KeyFileOpenHbci::parseFile: Error, unknown TLV tag: %i",tag);
   }//end switch
   
   tlvStart = HoBIT_TLV::nextTLV(tlvStart);
 }//end while
 
 return true;
}

/** Decrypts the file, encrypts it with the new password and saves it.
* Returns true on success.
*/
bool KeyFileOpenHbci::changePassword(const QString & newPassword){
 if(!_Ok)
   return false;
 else{
   _Password = newPassword;
   return writeFile(fileName());
 }  
}

/** Encode the key file. Returns a null array, if not ok. Sends error messages to stderr.
*/
UCharArray KeyFileOpenHbci::encodeFile(){
  UCharArray finalResult; 
  HoBIT_TLV  TLV_File;
  HoBIT_TLV  TLV_ContextData, tlv;
  UCharArray result; 
  uint length; //for debugging
  
  //header
  result += HoBIT_TLV(MEDIUMKEYFILE_TAG_VERSION_MAJOR, QString::number(MEDIUMKEYFILE_VERSION_MAJOR)).toUCharArray(); 
  length = result.strLength(); //for debugging
  result += HoBIT_TLV(MEDIUMKEYFILE_TAG_VERSION_MINOR, QString::number(MEDIUMKEYFILE_VERSION_MINOR)).toUCharArray();   
  length = result.strLength(); //for debugging
  result += HoBIT_TLV(MEDIUMKEYFILE_TAG_SEQ, QString::number(_Seq)).toUCharArray();   
  length = result.strLength(); //for debugging
  
  //**********user data**********
  //sign keys
  result += _UserPublicSignKey.toTLV(MEDIUMKEYFILE_TAG_USER_PUBSIGNKEY).toUCharArray();  
  length = result.strLength(); //for debugging
  if(_UserPrivateSignKey.isValid()) 
    result += _UserPrivateSignKey.toTLV(MEDIUMKEYFILE_TAG_USER_PRIVSIGNKEY).toUCharArray();   
  length = result.strLength(); //for debugging
  //crypt keys  
  result += _UserPublicCryptKey.toTLV(MEDIUMKEYFILE_TAG_USER_PUBCRYPTKEY).toUCharArray();  
  length = result.strLength(); //for debugging
  if(_UserPrivateCryptKey.isValid()) 
    result += _UserPrivateCryptKey.toTLV(MEDIUMKEYFILE_TAG_USER_PRIVCRYPTKEY).toUCharArray();   
  length = result.strLength(); //for debugging
  
  //temporary keys
  //sign keys
  if(_TempPublicSignKey.isValid()) 
    result += _TempPublicSignKey.toTLV(MEDIUMKEYFILE_TAG_TEMP_PUBSIGNKEY).toUCharArray();  
  length = result.strLength(); //for debugging
  if(_TempPrivateSignKey.isValid()) 
    result += _TempPrivateSignKey.toTLV(MEDIUMKEYFILE_TAG_TEMP_PRIVSIGNKEY).toUCharArray();     
  length = result.strLength(); //for debugging
  //crypt keys  
  if(_TempPublicCryptKey.isValid()) 
    result += _TempPublicCryptKey.toTLV(MEDIUMKEYFILE_TAG_TEMP_PUBCRYPTKEY).toUCharArray();  
  length = result.strLength(); //for debugging
  if(_TempPrivateCryptKey.isValid()) 
    result += _TempPrivateCryptKey.toTLV(MEDIUMKEYFILE_TAG_TEMP_PRIVCRYPTKEY).toUCharArray();   
  length = result.strLength(); //for debugging
  
  //user Id
  if(!_UserId.isEmpty())
    result += HoBIT_TLV(MEDIUMKEYFILE_TAG_USER_ID, _UserId).toUCharArray();
  length = result.strLength(); //for debugging
    
  //*************bank data *************  
  //bank keys     
  if(_InstPublicSignKey.isValid()) 
    result += _InstPublicSignKey.toTLV(MEDIUMKEYFILE_TAG_INST_PUBSIGNKEY).toUCharArray();  
  length = result.strLength(); //for debugging
  if(_InstPublicCryptKey.isValid()) 
    result += _InstPublicCryptKey.toTLV(MEDIUMKEYFILE_TAG_INST_PUBCRYPTKEY).toUCharArray();    
  length = result.strLength(); //for debugging
  //bank Id's   
  result += HoBIT_TLV(MEDIUMKEYFILE_TAG_INST_COUNTRY, _InstCountry).toUCharArray();
  length = result.strLength(); //for debugging
  if(!_InstCode.isEmpty())
     result += HoBIT_TLV(MEDIUMKEYFILE_TAG_INST_CODE, _InstCode).toUCharArray();
  length = result.strLength(); //for debugging
  if(!_InstSystemId.isEmpty())
     result += HoBIT_TLV(MEDIUMKEYFILE_TAG_INST_SYSTEMID, _InstSystemId).toUCharArray();
  result += HoBIT_TLV(MEDIUMKEYFILE_TAG_RDH_SECURITY_MODE, _RdhSecurityMode).toUCharArray();
  length = result.strLength(); //for debugging
  //************encrypt data**********   
  setupCrypt();   
  _CryptData->setData(result.data(), result.strLength());
  if(!_CryptData->encrypt()){
    qDebug("KeyFileOpenHbci::encodeFile: Encryption error");
    result.resize(0); //prevent nonsense to enter a file
  }
  //build final file-TLV
  _TLV_File = HoBIT_TLV(MEDIUMKEYFILE_TAG_CRYPT, _CryptData->getLength(), _CryptData->getData());
  
  return _TLV_File.toUCharArray();
}

/** Prepares _CryptData object for de/encryption.
*/
void KeyFileOpenHbci::setupCrypt(){
 CryptTripleDes * cryptTripleDes = new CryptTripleDes();
 cryptTripleDes->setDesCipherType(DES_EDE2_CBC);
 cryptTripleDes->setPassword(_Password);
 _CryptData->setCrypt(cryptTripleDes);
}

/** Read Property of QString _UserId
*/
const QString & KeyFileOpenHbci::getUserId(){
  return _UserId;
}

/** Read property of QString _InstCode
*/
const QString & KeyFileOpenHbci::getBankCode(){
  return _InstCode;
}

/** Read property of QString _RdhSecurityMode (1..10)
*/
const QString & KeyFileOpenHbci::getRdhSecurityMode()
{
  return _RdhSecurityMode;
}

/**Clear the bank public keys. Must be called prior to loading a new bank key into OpenHBCI
*/
void KeyFileOpenHbci::invalidateInstKeys(){
_InstPublicCryptKey= KeyRsa();
_InstPublicSignKey= KeyRsa();
}
