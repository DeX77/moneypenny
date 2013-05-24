/***************************************************************************
 *   Copyright (C) 2004 by Hendrik Sirges , Walter Roth                    *
 *   hendrik.sirges(at)fh-swf.de                                           *
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
#ifndef HOBIT_CRYPT_H
#define HOBIT_CRYPT_H

#include <openssl/evp.h>
#include <string>

#include "globals.h"
#include "uchararray.h"

using namespace std;

/**
HoBIT_CryptBF implements cryptographic functions used by HoBIT to encrypt and decrypt user data and files. 
A new Init-Vector is created for every session. Decryption is done with the Init-Vector loaded with the loadInitVector call.
Encryption is done with the new (and different) init-vector. Therefore, decryption of a just encrypted file will fail, unless you do not use the correct init-vector.
For security reasons, no errormessages are produced, if decryption fails due to a wrong password or init-vector. 
In ths case the program will simply crash. Therefore make shure, that the password has been verified with the checkPasswordMD function,
before using it.

\note For more information see <a href="www.openssl.org">
@author Hendrik Sirges, Walter Roth
*/
class HoBIT_Crypt{
public:
/*!
\fn HoBIT_Crypt(const string& path, const string& password)
@param path the base datadir (should be set to users homedir)
@param password the passwor used for de/encryption
*/
    HoBIT_Crypt();
	
    ~HoBIT_Crypt();
	
/*!
    \fn HoBIT_Crypt::encryptFile(const string& infilepath, const string& outfilepath)
	@param infilepath the uri of the file to encrypt
	@param outefilepath the uri of the outputfile
	@return 0 if successful
 */
    int encryptFile(FILE * infile, FILE * cryptfile);
	
	
    
    /** Compresses and encrypts file(s) to outFilenName. inFilenames are relative to inDir. 
     * inFileNames are separated by blanks.
     * outFileName and ivFileName must be the complete paths.
     */
    int encryptArchive(const QString & inFileNames, 
                       const QString & inDirName, 
                       const QString & outFileName, 
                       const QString & ivFileName);

     
/*!
    \fn HoBIT_Crypt::decryptFile(const string& infilepath, const string& outfilepath)
	@brief This decrypts an input file und writes it to an output file
	@param infilepath The path of the file to encrypt
	@param outfilepath The path of the output file
 */
    int decryptFile(FILE * infile, FILE * outfile);
	
	
	
    
    /** Decrypts and extracts an encrypted tar archive file using a pipe.
    */
    int decryptArchive(const QString & inFileName, const QString & outDirName, const QString & ivFileName);
	
	/**
	\fn int savePasswordMD(const string &filepath = "")
	@brief This function saves the message digest value of the current password to a file
	@param filepath uri of the path the mesage digest should be saved to. default is /.pwmd in the directory defined 
					by _ConfigDataDir
	@return 0 if successful
	*/
    int savePasswordMD(const QString &filepath = "");
	/**
	\fn int checkPasswordMD(const string &filepath = "")
	@param filepath uri of the path the mesage digest should be saved to. default is /.pwmd in the directory defined 
					by _ConfigDataDir
	@return 0 if the password is valid, -1 if file was not found and -2 if password was wrong
	*/
    int checkPasswordMD(const QString &filepath = "");
	/**
	\fn int setPassword(const string &password)
	@brief sets password to a new value
	@param password the password to set
        @return digest generation ok 
	*/
    bool setPassword(const QString &password);

        /** Read property of string _CryptDir. 
        */
    const QString& getCryptDir()const;
        /** Write property of string _CryptDir. 
        */
    void setCryptDir( const QString& _newVal);

	
private:

	/** Secret _Password for cryptographic functions 
        */
	UCharArray _Password;
	/** Old initialisation vector from the previous session for current decryption 
        */
	UCharArray _DecryptIniVector;
	/** newly generated initialisation vector for current encryption
	 *  the init-vector is generated from /dev/random device
	 */
	UCharArray _EncryptIniVector;
       /** The key to be used with Blowfish
        */
        UCharArray _Key;
        /** The checksum to check the password with. Double hashed password.
        */
        UCharArray _Checksum;
	/** Mesage Digest Type 
        */
	const EVP_MD *_KeyDigestType;
	/** Mesage Digest Type 
        */
	const EVP_MD *_ChecksumDigestType;      
	/** Message Digest Context 
        */
	EVP_MD_CTX *_MsgDigestContext;
	/** Cipher type 
	* @see EVP Man-Pages
	*/
	const EVP_CIPHER *_CipherType;
	/** Encryption Module ID 
        */ 
	//int _ModuleId;
	/** Length of the initialisation vector 
        */
	int _IniVectorLength;
	/** Blocksize of the data fragments the encryption/decryption functions take for en/decryption 
        */
	int _BlockSize;
	
 	/** base datadir where all the crypted config-files reside 
        */
	QString _CryptDir;
        /** the real encryption
         */
         int docrypt(FILE * infile, FILE * outfile, int mode, UCharArray & initVector);
         
         int do_cipher(FILE * infile, FILE * outfile, int operation,  UCharArray & initVector);
    
	/**
	\fn int loadInitVector()
	@brief loads old init vector from iv.dat file (in _ConfigDataDir) and stores it in _OldIniVector
	*/
    int loadDecryptInitVector(QString fileName);
	/**
	\fn int saveInitVector()
	@brief saves _NewIniVector to iv.dat file (in _ConfigDataDir)
	*/
    int saveEncryptInitVector(QString fileName);
};

#endif
