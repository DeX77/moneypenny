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
#ifndef KEYFILE_H
#define KEYFILE_H

#include <qfile.h>

#include "cryptdata.h"
#include "hobit_tlv.h"

/**
The base class for the various keyfile types. Inherited by KeyFileOpenHbci, KeyFileRDH1 and KeyFileRDH2.

@author Walter Roth
*/
class KeyFile : public QFile
{
public:
    /**Constructor sets filename.
    */
    KeyFile(const QString & fileName, const QString & password);

    /** destructor
    */
    virtual ~KeyFile();

    /** Returns true, if file has been parsed correctly.
    */
    virtual bool isOk();
    
    /**Read the file. If ok, set Ok Flag to true, else to false. 
    * If this function returns false, use errorText() to inform the user.
    * You should not need to overwrite this function. Overwrite parseFile
    */   
    virtual bool readFile();
    /**Write the file. Returns true on ok.
    * If this function returns false, use errorText() to inform the user.
    * You should not need to overwrite this function. Overwrite encodeFile
    */   
    virtual bool writeFile(const QString & fileName);

    /**Returns error text. Use this function to get a translated user-readable message, if readFile failed.
    */
    virtual QString errorText();
protected:
    /**Write binary data to file. Useful for debugging of decrpted parts of the keyfile.
    */
    virtual void debugWriteToBinaryFile(const QString & fileName, unsigned char * data, unsigned int length);
    /** Read and parse the key file. Returns true, if ok. Sends error messages to stderr.
    */
    virtual bool parseFile();
    /** Encode the key file. Returns a null array, if not ok. Sends error messages to stderr.
    */
    virtual UCharArray encodeFile();
       
    /** The CryptData object used for decrypting the private keys.
    */
    CryptData * _CryptData;
    
    /** Flag for correctly parsed file.
    */
    bool _Ok;
    /** The password for keyfile decryption
    */   
    QString _Password; 
    /**The text for the error message. This text is always set prior to an action that might fail.
    *
    */
    QString _ErrorText;   
};

#endif
