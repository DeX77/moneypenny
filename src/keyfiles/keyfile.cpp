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
#include "keyfile.h"

#include <QString>

/**Constructor loads data into QByteArray _Data
*/
KeyFile::KeyFile(const QString & fileName, const QString & password)
 : QFile(fileName)
{
 qDebug("Constructor KeyFile");
 _CryptData = new CryptData(NULL);
 _Ok= false;
 _Password = password;
}


KeyFile::~KeyFile()
{
  if(_CryptData)
    delete _CryptData;
  qDebug("Destructor Keyfile finished");
}

/** Read and parse the key file. Returns true, if ok. Sends error messages to stderr.
*/
bool KeyFile::parseFile(){
 qDebug("KeyFile::parseFile: Error. This function must be overwritten.");
 return false; // to be overwritten
}

/** Returns true, if file has been parsed correctly.
*/
bool KeyFile::isOk(){
 return _Ok;
}

/**Read the file. If ok, set Ok Flag to true, else to false. 
* You should not need to overwrite this function. Overwrite parseFile.
*/
bool KeyFile::readFile(){  
  _Ok = false;
  _ErrorText =  QObject::tr("Can not read keyfile.");
  if(!exists())
   return false;
  open(QIODevice::ReadOnly); 
  _Ok = parseFile();
  close();
  return _Ok;  
}

/**Returns error text
*/
QString KeyFile::errorText(){
  return _ErrorText;
}

/**Write binary data to file. Useful for debugging of decrpted parts of the keyfile.
*/
void KeyFile::debugWriteToBinaryFile(const QString & fileName, unsigned char * data, unsigned int length){
  QFile outFile(fileName);
  if(outFile.open(QIODevice::WriteOnly)){
    outFile.write((char *)data, length);
    outFile.close();
  }
}

/** Write the file. Returns true on ok.
  * If this function returns false, use errorText() to inform the user.
  * You should not need to overwrite this function. Overwrite encodeFile
  */   
bool KeyFile::writeFile(const QString & fileName){
  int fileLength;
  QFile newFile(fileName);
  UCharArray ar = encodeFile();
  if(ar.strLength() > 0){
    if(newFile.open(QIODevice::WriteOnly)){
      fileLength = newFile.write((char *)ar.data(), ar.strLength());
      newFile.close();
      if(fileLength > 0)
        return true;
      else { //nothing was written
        _ErrorText = QObject::tr("Writing keyfile failed. Is Device write protected? Filename: ") + newFile.fileName();
        return false;
      }   
    }
    else{//no write access
     _ErrorText = QObject::tr("Writing keyfile failed. Filename: ") + newFile.fileName();
     return false;
    } 
  }
  else{  //encoding failed
     _ErrorText = QObject::tr("Encoding keyfile failed. Filename: ") + newFile.fileName();
    return false;
  }   
}

/** Encode the key file. Returns a null array, if not ok. Sends error messages to stderr.
*/
UCharArray KeyFile::encodeFile(){
  return UCharArray(); // return an empty array, to be overwritten!
}

