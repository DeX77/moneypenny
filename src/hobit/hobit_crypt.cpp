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
#include "hobit_crypt.h"

#include <qstring.h>
#include <qdatetime.h>
#include <qprogressdialog.h>
#include <iostream>
#include <fstream>
#include <qdir.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/err.h>
#include <openssl/ssl.h>


//#define DEBUG_CRYPT 

#define ENCRYPT		1
#define DECRYPT         0

using namespace std;

HoBIT_Crypt::HoBIT_Crypt()
{
 	_CryptDir = ""; //must be set later
        _ChecksumDigestType = EVP_ripemd160();
        _KeyDigestType = EVP_ripemd160();
	_MsgDigestContext = EVP_MD_CTX_create();

	_CipherType = EVP_bf_cbc();
        //EVP_add_cipher(_CipherType);
	//_ModuleId = EVP_CIPHER_nid(_CipherType);
	_IniVectorLength = EVP_CIPHER_iv_length(_CipherType);
	_BlockSize = EVP_CIPHER_block_size(_CipherType);

        //create a new initialisation vector for this session
	_EncryptIniVector.resize(_IniVectorLength);
	srand((unsigned)time(NULL));
        for(uint ivpos = 0; ivpos < _EncryptIniVector.strLength(); ivpos++)
    	   _EncryptIniVector[ivpos] = (unsigned) rand();
	_DecryptIniVector.resize(_IniVectorLength);
        
	SSL_load_error_strings();	
#ifdef DEBUG_CRYPT
        cout <<"EVP_MAX_MD_SIZE : " << EVP_MAX_MD_SIZE << endl;
	cout << "Ini vector length: " << _IniVectorLength << endl;
        QString sIni, sNum;
        for (int pos = 0; pos < _IniVectorLength; pos++)
         sIni += sNum.setNum(_EncryptIniVector[pos], 16); 
	cout << "New Ini vector: " << sIni.toLatin1().data() << endl;
	cout << "Blocksize: " << _BlockSize << endl;
	//cout << "Password: " << _Password << endl;
	cout << "OLDIV: Not yet valid" << endl;
	cout << "NEWIV: " << qPrintable(_EncryptIniVector.toHexString(0)) << endl;
#endif
	
}


HoBIT_Crypt::~HoBIT_Crypt()
{
	//EVP_CIPHER_CTX_cleanup(&cipherContext);
	EVP_MD_CTX_destroy(_MsgDigestContext);
        qDebug("Destructor HoBIT_Crypt finished");
}


int HoBIT_Crypt::encryptFile (FILE * infile, FILE * cryptfile)
{
  int result;
    
  result= docrypt(infile, cryptfile, ENCRYPT, _EncryptIniVector);
  return result;
}

int HoBIT_Crypt::decryptFile(FILE * infile, FILE * outfile)
{
   int result;
  result= docrypt(infile, outfile, DECRYPT, _DecryptIniVector);
  return result;
}

int HoBIT_Crypt::docrypt(FILE * infile, FILE * outfile, int mode, UCharArray & initVector)
{
   EVP_CIPHER_CTX cipherContext;
//allow enough space for input and output
   unsigned char inbuf[1024];
   unsigned char outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
   char errbuf [256];
   int errorCode = 0;
  // string testName = _ConfigDataDir + "/" + MONEYPENNY_DATADIR_NAME +"/testfile.tar.gz";
   int inlen, outlen;   
   int totalin = 0;
   int totalout = 0;
   // Don't set _Password or IV because we will modify the parameters 
    EVP_CIPHER_CTX_init(&cipherContext);
    EVP_CipherInit_ex(&cipherContext, _CipherType, NULL, NULL, NULL, mode);
    EVP_CIPHER_CTX_set_key_length(&cipherContext, _Key.strLength());
    //EVP_CIPHER_CTX_set_padding(&cipherContext, true); // this is already the default
   // We finished modifying parameters so now we can set key and initialization vector 
    EVP_CipherInit_ex(&cipherContext, _CipherType, NULL, _Key.data(), initVector.data(), mode);
    errorCode = ERR_get_error();
    if(errorCode)
     cerr << "EVP_CipherInit_ex error: " << ERR_error_string(errorCode, errbuf) << endl;
    int nLine = 1; 
     for(;;){
          inlen = fread(inbuf, 1, sizeof(inbuf), infile);
          totalin+= inlen;
          if(inlen <= 0) break;
          if(EVP_CipherUpdate(&cipherContext, outbuf, &outlen, inbuf, inlen)){
            totalout += fwrite(outbuf, 1, outlen, outfile);
            nLine ++;
          }    
          else{
              cerr << "EVP_CipherUpdate error: " << ERR_error_string(ERR_get_error(), errbuf) << endl;
              return -1; //error
          }    
        #ifdef DEBUG_CRYPT
         cerr << "inlen: " << inlen << "   outlen: " << outlen << endl;
         cerr << "Total inlen: " << totalin << " Total outlen: "<< totalout <<"\n";
       #endif
       }//End for
        if(EVP_CipherFinal_ex(&cipherContext, outbuf, &outlen))                     
          totalout += fwrite(outbuf, 1, outlen, outfile); 
        else {
          cerr << "EVP_CipherFinal_ex error: " << ERR_error_string(ERR_get_error(), errbuf) << endl;
          return -1; //error 
        }
      cerr << "Crypt: Total inlen: " << totalin << " Total outlen: "<< totalout <<"\n";
      
      EVP_CIPHER_CTX_cleanup(&cipherContext);
      return 0;
}

int HoBIT_Crypt::do_cipher(FILE * infile, FILE * outfile, int operation,  UCharArray & initVector)
{
        unsigned char buf[1024];
	unsigned char ebuf[1024 + 8];
	int ebuflen; /* rc; */
     //   unsigned char iv[EVP_MAX_IV_LENGTH], key[EVP_MAX_KEY_LENGTH];
	/* unsigned int ekeylen, net_ekeylen;  */
	EVP_CIPHER_CTX ectx;
        
	//memcpy(iv, _EncryptIniVector.data(), sizeof(iv));

	EVP_CIPHER_CTX_init(&ectx);
        EVP_CipherInit_ex(&ectx, _CipherType, NULL, _Key.data(), initVector.data(), operation);
	//EVP_CipherInit_ex(&ectx, _CipherType, NULL, key, iv, operation);
	while(1)
	{
		int readlen = fread(buf, 1, sizeof(buf), infile);
	
		if (readlen <= 0)
		{
			if (!readlen)
			   break;
			else
			{
				cerr << "HoBIT_Crypt::do_cipher : read error" << endl;
				return -1;
			}
		}

		EVP_CipherUpdate(&ectx, ebuf, &ebuflen, buf, readlen);

		fwrite(ebuf, 1, ebuflen, outfile);
	}

        if(!EVP_CipherFinal_ex(&ectx, ebuf, &ebuflen)){
	 printf("Error in EVP_CipherFinal"); 
         return -1;
        }
	EVP_CIPHER_CTX_cleanup(&ectx);

	fwrite(ebuf, 1, ebuflen, outfile); 
        return 0;
}



/*!
    \fn HoBIT_Crypt::loadInitVector
 */
int HoBIT_Crypt::loadDecryptInitVector(QString fileName)
{	
	ifstream ivin(fileName.toLatin1().data(),ios_base::binary);
	if(!ivin){
		qDebug("Failed to open iv file for data decryption: %s", fileName.toLatin1().data());
		return -1;
	}
	char c;
	for(uint niv=0; niv < _DecryptIniVector.strLength(); niv++){
		ivin.get(c);
		_DecryptIniVector[niv] = (unsigned char) c;
	}
	ivin.close();
#ifdef DEBUG_CRYPT
	cout << "Old Ini vector loaded: " << qPrintable(_DecryptIniVector.toHexString(0)) << endl;
#endif
	
	return 0;
}


/*!
    \fn HoBIT_Crypt::saveInitVector()
 */
int HoBIT_Crypt::saveEncryptInitVector(QString fileName)
{     
 //store new init-vector before using it
     
 #ifdef DEBUG_CRYPT 
     QFile ivFile (fileName);
     if(ivFile.exists()){
             qDebug("Deleting old iv file: %s", fileName.toLatin1().data());
     } else {
             qDebug("Iv file does not exist! Creating new: %s", fileName.toLatin1().data());
     }
 #endif
     ofstream ivout(fileName.toLatin1().data(),ios_base::binary);
     if(!ivout){
             qDebug("Failed to write iv file for data decryption. Could not write: %s", fileName.toLatin1().data());
             return -1;
     }
     for(int niv=0; niv<_IniVectorLength; niv++)
             ivout.put((const char) _EncryptIniVector[niv]);
     ivout.close();

     return 0;
}


/*!
    \fn HoBIT_Crypt::savePasswordMD(const string &filepath)
 */
int HoBIT_Crypt::savePasswordMD(const QString &filepath)
{
   QString fp;
   if(filepath.isEmpty()){
           fp = _CryptDir + "/" + MONEYPENNY_PASSWORD_FILE;
   } else {
           fp = filepath;
   }
   cerr << "Password path:" << fp.toStdString() << endl;
   ofstream ofmd(fp.toLatin1().data(),ios_base::binary);
   if(!ofmd){
           cerr << "Failed to open file " << filepath.toStdString() << endl;
           return -1;
   }
   for(unsigned int i=0; i< _Checksum.strLength(); i++)
           ofmd.put(_Checksum[i]);
   ofmd.close();
   
   return 0;
}


/*!
    \fn HoBIT_Crypt::checkPasswordMD(const string &filepath = 0)
 */
int HoBIT_Crypt::checkPasswordMD(const QString &filepath)
{
   QString fileName;
   if(filepath.isEmpty())
        fileName = _CryptDir + "/" + MONEYPENNY_PASSWORD_FILE;
   else 
        fileName = filepath;
   
   ifstream stream(fileName.toLatin1().data(),ios_base::binary);
   if(!stream){
     cerr << "Failed to open password file " << fileName.toStdString() << endl;
     return -1;
   }
   UCharArray oldchecksum;
   oldchecksum.resize(EVP_MD_size(_ChecksumDigestType));
   char c;
   uint pos = 0;
   while(stream.get(c)&& (pos <= oldchecksum.strLength())){
         oldchecksum[pos] = (unsigned) c;
         pos++;
   }
   stream.close();
   if(oldchecksum == _Checksum) //password ok
      return 0;
   else 
   {
        #ifdef DEBUG_CRYPT
        qDebug("HoBIT_Crypt::checkPasswordMD Input: %s", qPrintable(_Password.toString()));
        qDebug("Filepath: %s", qPrintable(filepath));
        qDebug("Filecontent: %s", qPrintable(oldchecksum.toHexString(0)));
        qDebug("Checksum   : %s", qPrintable(_Checksum.toHexString(0)));
       #endif
        return -2;   //wrong password
   }
}


/*!
    \fn HoBIT_Crypt::setPassword(const string &password)
 */
bool HoBIT_Crypt::setPassword(const QString &password)
{
  UCharArray passwordArray; // keep it on the stack
  passwordArray.loadFromString(password);
  //derive key
  _Key.resize(EVP_MD_size(_KeyDigestType)); //make it big enough
  unsigned int keyLength;
  EVP_DigestInit(_MsgDigestContext, _KeyDigestType);
  EVP_DigestUpdate(_MsgDigestContext, passwordArray.data(), passwordArray.strLength());
  EVP_DigestFinal(_MsgDigestContext, _Key.data(),&keyLength);
  _Key.resize(keyLength); //resize to correct size, just for safety

  //Overwrite the password copy, just for paranoia
  for(uint i = 0; i < passwordArray.strLength(); i++)
    passwordArray[i] = (unsigned)rand();
  
  //derive Checksum
  _Checksum.resize(EVP_MD_size(_ChecksumDigestType));
  unsigned int checksumLength;
  EVP_DigestInit(_MsgDigestContext, _ChecksumDigestType);
  EVP_DigestUpdate(_MsgDigestContext, _Key.data(), _Key.strLength());
  EVP_DigestFinal(_MsgDigestContext, _Checksum.data(), &checksumLength);
  
  return checksumLength == _Checksum.strLength(); //These must be identical, if everything was ok
}

/** Write property of string _CryptDir. 
*/
void HoBIT_Crypt::setCryptDir( const QString& _newVal){
	_CryptDir = _newVal;
}


/** Read property of string _CryptDir. 
*/
const QString& HoBIT_Crypt::getCryptDir()const{
	return _CryptDir;
}

/** Decrypts and extracts an encrypted tar archive file. ivFilename must be the complete path!
*/
int HoBIT_Crypt::decryptArchive(const QString & inFileName, const QString & outDirName, const QString & ivFileName){

    FILE * infile, * outfile;
    int result = loadDecryptInitVector(ivFileName);
    if(result != 0)
      return result;
    //extract decrypted data read from the pipe on stdin and write them to datadir
    QString tarcmd = "tar --extract --gzip --file=- --directory=" + outDirName;// tar archive contains paths relative to _ConfigDataDir; 
    cerr << "decryptArchive: decrypting " << inFileName.toStdString() << endl;
    cerr << "decryptArchive: tar extract command: " << tarcmd.toStdString() << endl;
    
    infile = fopen(inFileName.toLatin1().data(), "ro");   
    if(infile)
     outfile = popen(tarcmd.toLatin1().data(), "w"); //open the pipe for writing decrypted data to tar
    else{
     cerr << "decryptArchive: Open infile failed: " << inFileName.toStdString() << endl;
     return -1;
    }
    if(!outfile){
     cerr << "decryptArchive: Open pipe for output failed";
     fclose(infile);
     return -2;
    } 
    
    result = decryptFile(infile, outfile);
    if(result < 0){
            cerr << "decryptArchive: Failed to decrypt file: "<< inFileName.toStdString() << endl;
            fclose(infile);
            pclose(outfile);
            return -3;
    }
    fclose(infile);
    pclose(outfile); //close the pipe	
    return result;
}

/** Compresses and encrypts file(s) to outFilenName. inFilenames are relative to inDir. 
  * outFileName and ivFileName must be the complete paths.
*/
int HoBIT_Crypt::encryptArchive(const QString & inFileNames, 
                                const QString & inDirName, 
                                const QString & outFileName, 
                                const QString & ivFileName){

     int result = saveEncryptInitVector(ivFileName);
     if(result != 0)
        return result; //do not overwrite old outfile with an undecryptable one

     FILE * tarFile, * cryptFile;     
     //paths relative to outDir
     QString tarcmd = "tar --create --directory=" + inDirName + " --gzip --file=- "  // file =- means: use stdout for output
     + inFileNames;
     cerr <<"tar command for " << outFileName.toLatin1().data() << ": "<< tarcmd.toLatin1().data() << "\n";

    //create the pipe for tar's output and start tar
     tarFile = popen(tarcmd.toLatin1().data(),"r");
     if(!tarFile){
             cerr<<"encryptArchive: Failed to execute tar command" << endl;
             return -1;
     }

     cryptFile = fopen(outFileName.toLatin1().data(), "wb");
     if(!cryptFile){
       cerr<<"encryptArchive: Failed to open cryptFile for writing: " << outFileName.toStdString() << endl;
       pclose(tarFile);
       return -2;
     }

     result = encryptFile(tarFile,cryptFile);//read from the pipe and write to cryptFile 
     if(result < 0){ 
       cerr<<"encryptArchive: Error when encrypting datadir" << endl; 
       result = -2; //need to close files, so do not return here
     }       
     pclose(tarFile); 
     fclose(cryptFile);
     return result;
}
