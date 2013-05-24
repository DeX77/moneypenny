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
#include "keyrsa.h"

#include <qapplication.h>

// keydata
#define MEDIUM_RDH_TAG_KEY_ISPUBLIC      (unsigned char) 0x01
#define MEDIUM_RDH_TAG_KEY_ISCRYPT       (unsigned char) 0x02
#define MEDIUM_RDH_TAG_KEY_OWNER         (unsigned char) 0x03
#define MEDIUM_RDH_TAG_KEY_VERSION       (unsigned char) 0x04
#define MEDIUM_RDH_TAG_KEY_NUMBER        (unsigned char) 0x05
#define MEDIUM_RDH_TAG_KEY_MODULUS       (unsigned char) 0x06
#define MEDIUM_RDH_TAG_KEY_EXP_OLD       (unsigned char) 0x07
#define MEDIUM_RDH_TAG_KEY_N             (unsigned char) 0x08
#define MEDIUM_RDH_TAG_KEY_P             (unsigned char) 0x09
#define MEDIUM_RDH_TAG_KEY_Q             (unsigned char) 0x0a
#define MEDIUM_RDH_TAG_KEY_DMP1          (unsigned char) 0x0b
#define MEDIUM_RDH_TAG_KEY_DMQ1          (unsigned char) 0x0c
#define MEDIUM_RDH_TAG_KEY_IQMP          (unsigned char) 0x0d
#define MEDIUM_RDH_TAG_KEY_D             (unsigned char) 0x0e
// new in version 1.3
#define MEDIUM_RDH_TAG_KEY_EXP           (unsigned char) 0x0f

KeyRsa::KeyRsa()
{
 _Valid = false;
 _Public = false;
 _CanCrypt = false;
 _Version = -1;
 _Number = -1;
 qDebug("Constructor KeyRsa");
}


KeyRsa::~KeyRsa()
{
 qDebug("Destructor KeyRsa");
}

/** Returns true, if key is valid.
*/
bool KeyRsa::isValid(){
 return _Valid;
}

/** Reads key data from OpenHBCI tlv.
*/
bool KeyRsa::readFromHoBIT_TLV(uchar * keyStart){
 uchar * dataEnd = HoBIT_TLV::lastByte(keyStart);
 HoBIT_TLV tlv;
 uchar * tlvStart = HoBIT_TLV::uValue(keyStart);
 
 while(tlvStart < dataEnd){
   tlv = HoBIT_TLV(tlvStart); 
   //qDebug("%s", tlv.debugString().toLatin1().data());
   uchar tag = tlv.tag();
   switch(tag){
    case MEDIUM_RDH_TAG_KEY_ISPUBLIC:{ //0x01
            _Public = tlv.sValue().toUpper() == "YES";
            break; 
    }        
    case MEDIUM_RDH_TAG_KEY_ISCRYPT:{  //0x02
             _CanCrypt = tlv.sValue().toUpper()== "YES";
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_OWNER:{ //0x03
            _Owner = tlv.sValue();
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_VERSION:{ //0x04
            _Version = tlv.sValue().toInt();
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_NUMBER:{ //0x05
            _Number = tlv.sValue().toInt();
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_MODULUS:{ //0x06
            _Modulus = UCharArray(tlv.uValue(), tlv.length());
            qDebug("_Modulus: %s", _Modulus.toHexString(0).toLatin1().data());
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_EXP_OLD:{
            //Do nothing
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_N:{
            _N = UCharArray(tlv.uValue(), tlv.length());
            qDebug("_N: %s", _N.toHexString(0).toLatin1().data());
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_P:{
            _P = UCharArray(tlv.uValue(), tlv.length());
            qDebug("_P: %s", _P.toHexString(0).toLatin1().data());
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_Q:{
            _Q = UCharArray(tlv.uValue(), tlv.length());
            qDebug("_Q: %s", _Q.toHexString(0).toLatin1().data());
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_DMP1:{
            _DMP1 = UCharArray(tlv.uValue(), tlv.length());
            qDebug("_DMP1: %s", _DMP1.toHexString(0).toLatin1().data());
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_DMQ1:{
            _DMQ1 = UCharArray(tlv.uValue(), tlv.length());
            qDebug("_DMQ1: %s", _DMQ1.toHexString(0).toLatin1().data());
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_IQMP:{
            _IQMP = UCharArray(tlv.uValue(), tlv.length());
            qDebug("_IQMP: %s", _IQMP.toHexString(0).toLatin1().data());
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_D:{
            _D = UCharArray(tlv.uValue(), tlv.length());
            qDebug("_D: %s", _D.toHexString(0).toLatin1().data());
            break;
    } 
    case MEDIUM_RDH_TAG_KEY_EXP:{
             _Exponent= UCharArray(tlv.uValue(), tlv.length());
             qDebug("_Exponent: %s", _Exponent.toHexString(0).toLatin1().data());
            break;
    } 

    default: qDebug("Unknown TLV tag in RSA key: %i",tag);
   }//end switch
   tlvStart = HoBIT_TLV::nextTLV(tlvStart);
 }//end while
 _Valid = true;
 return true;
}

/**Returns a single TLV with all the key data in it.
*/
HoBIT_TLV KeyRsa::toTLV(unsigned char tag){
 UCharArray ar;
 HoBIT_TLV tlv;
 
 if(_Public)
    ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_ISPUBLIC, "YES").toUCharArray();
 else 
    ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_ISPUBLIC, "NO").toUCharArray();
 
 if(_CanCrypt)
    ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_ISCRYPT, "YES").toUCharArray();
 else 
    ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_ISCRYPT, "NO").toUCharArray();
 
 ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_OWNER, _Owner).toUCharArray();   
 ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_NUMBER, QString::number(_Number)).toUCharArray();   
 ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_VERSION, QString::number(_Version)).toUCharArray();   
 ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_EXP, _Exponent).toUCharArray();  
 if(_Modulus.count() > 0){
    tlv = HoBIT_TLV(MEDIUM_RDH_TAG_KEY_MODULUS, _Modulus);
    ar += tlv.toUCharArray();
   }
 else qDebug("HoBIT_TLV KeyRsa::toTLV: Error: No Modulus, invalid key!" ); 
 if(_N.count() > 0)
   ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_N, _N).toUCharArray();  
 if(_P.count() > 0)
   ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_P, _P).toUCharArray();  
 if(_Q.count() > 0)
   ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_Q, _Q).toUCharArray();  
 if(_D.count() > 0)
   ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_D, _D).toUCharArray();  
 if(_DMP1.count() > 0)
   ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_DMP1, _DMP1).toUCharArray();  
 if(_DMQ1.count() > 0)
   ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_DMQ1, _DMQ1).toUCharArray();  
 if(_IQMP.count() > 0)
   ar += HoBIT_TLV(MEDIUM_RDH_TAG_KEY_IQMP, _IQMP).toUCharArray();  
 return HoBIT_TLV(tag, ar); 
}
