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
#ifndef KEYFILEOPENHBCI_H
#define KEYFILEOPENHBCI_H

#include "keyfile.h"
#include "keyrsa.h"

/**
The keyfile format used by OpenHbci 2. 

@author Walter Roth
*/
class KeyFileOpenHbci : public KeyFile
{
public:
    /**Constructor sets filename.
    */
    KeyFileOpenHbci(const QString & fileName, const QString & password);

    /** destructor
    */
    virtual ~KeyFileOpenHbci();
    
    /** Decrypts the file, encrypts it with the new password and saves it.
    * Returns true on success.
    */
    bool changePassword(const QString & newPassword);
    /** Read Property of QString _UserId
    */
    const QString & getUserId();

    /** Read property of QString _InstCode (BLZ)
    */
    const QString & getBankCode();
    /** Read property of QString _RdhSecurityMode (1..10)
    */
    const QString & getRdhSecurityMode();
    /**Clear the bank public keys. Must be called prior to loading a new bank key into OpenHBCI
    */
    void invalidateInstKeys();
protected:
    /** Read and parse the key file. Returns true, if ok. Sends error messages to stderr.
    */
    virtual bool parseFile();

    /** Encode the key file. Returns a null array, if not ok. Sends error messages to stderr.
    */
    virtual UCharArray encodeFile();
    
    /** Prepares _CryptData object for de/encryption.
    */
    virtual void setupCrypt();
    
private:  
    /** The whole file is a TLV
    */  
    HoBIT_TLV _TLV_File;
    /** the crypted data part of the file
    */  
    HoBIT_TLV _TLV_CryptedData;
    /** the crypted data part of the file
    */  
    HoBIT_TLV _TLV_ContextData;
    /** Seq ??
    */
    int _Seq;
    
    //User related data
    
    /** The user-id normally a long decimal number.
    */
    QString _UserId;
   /** The user's public sign key
    */
    KeyRsa _UserPublicSignKey;
    /** The user's private sign key
    */
    KeyRsa _UserPrivateSignKey;
    /** The user's public crypt key
    */
    KeyRsa _UserPublicCryptKey;
    /** The user's private crypt key
    */
    KeyRsa _UserPrivateCryptKey;
    
   //Temporary keys. These are used for a key change. This key set has not yet been activated by the bank.
   /** The user's temporary public sign key
    */
    KeyRsa _TempPublicSignKey;
    /** The user's temporary private sign key
    */
    KeyRsa _TempPrivateSignKey;
    /** The user's temporary public crypt key
    */
    KeyRsa _TempPublicCryptKey;
    /** The user's temporary private crypt key
    */
    KeyRsa _TempPrivateCryptKey;
    
    
    //Bank related data
    
    /**The bank country code. A decimal number (Germany = 280)
    */
    QString _InstCountry;
    /**The bank id (BLZ)
    */
    QString _InstCode;
    /**The bank's server system id (BLZ)
    */
    QString _InstSystemId;
   /** The bank's public sign key
    */
    KeyRsa _InstPublicSignKey;
   /** The bank's public crypt key
    */
    KeyRsa _InstPublicCryptKey;
   /**The bank's server
     */
    QString _ServerAddress;
   /** The server port.
    */
    QString _ServerPort;
    /**The remote sequence (whatever that is?)
     */
    QString _RemoteSeq;
    /**The RDH security mode version
    */
    QString _RdhSecurityMode;
};

#endif
