/***************************************************************************
                          hobit_string.cpp  -  description
                             -------------------
    begin                : Mi Okt 20 2004
    copyright            : (C) 2004 by Marcel Mulch
    email                : marcel@mulch-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "hobit_string.h"
#include <string>
#include <stdio.h>
#include <stdarg.h>

#include <qobject.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qfile.h>


using namespace std;

const string HoBIT_String::format(const char *fmt, ...) {
  va_list ap;
  char *buf = 0;
  size_t size;
  string str="";

  va_start(ap,fmt);
  size = vsnprintf(buf,0,fmt,ap);
  va_end(ap);
  if (size > 0) {
    size++;
    buf=new char[size];
    va_start(ap,fmt);
    vsnprintf(buf,size,fmt,ap);
    va_end(ap);
    str=buf;
    delete[] buf;
  }
  return str;
}



/** dumps a hex string, so it can be
  * printed to the ini letter
  */
const string HoBIT_String::dumpHexString(string s) {
    string result;
    unsigned int pos;

    result+="   ";
    for (pos=0; pos < s.length(); pos++) {
        if ((pos % 40)==0)
            result+="\n   ";
        else if ((pos % 12)==0)
            result+=" ";
        result+=s.at(pos);
    } // for
    result+="\n";
    return result;
}

/** dumps a char array, so it can be
  * printed to the ini letter
  */
const string HoBIT_String::dumpHexString(char * data, int length) {
 string temp(data, length);
 return dumpHexString(temp); 
}
/** dumps a fingerprint (20 byte HEX formatted) hex string
*/
const string HoBIT_String::dumpFingerprint(string indent, string s){
    string result = "\n" + indent;
    for(int pos = 0; pos < 20; pos += 2)
     result += "  " + s.substr(pos, 2);
    result += "\n" + indent;
    for(int pos = 20; pos < 40; pos += 2)
     result += "  " + s.substr(pos, 2);
    return result;
}
/**removes German Umlaute or other special chars illegal for HBCI
 * Lots of trouble with utf8, iso8859-15 and so on. Now uses unicode from linguist.
* ToDO: implement for other languages
*/
const QString  HoBIT_String::replaceSpecialChars(const QString & text, bool replaceUmlaute){
   QString buffer;
   bool wasUpper = isUpper(text);
   for(int pos = 0; pos < text.length(); pos ++){
    QString cPos = text[pos]; //Debugging only
    if(replaceUmlaute)
    {
        if( cPos == QObject::tr("aUmlaut")) 
               buffer += "ae"; 
        else if(cPos == QObject::tr("AUmlaut")) 
               buffer += "Ae"; 
        else if(cPos == QObject::tr("oUmlaut")) 
               buffer += "oe"; 
        else if(cPos == QObject::tr("OUmlaut")) 
               buffer += "Oe"; 
        else if(cPos == QObject::tr("uUmlaut")) 
               buffer += "ue"; 
        else if(cPos == QObject::tr("UUmlaut")) 
               buffer += "Ue"; 
        else if(cPos == QObject::tr("sz Char = Beta")) 
               buffer += "ss"; 
    }
 
    if(cPos == ":")
           buffer += ".";        
    else if(cPos == ";")
           buffer += ",";        
         else buffer+= text[pos]; 
   }//end for
   if(wasUpper)
      buffer= buffer.toUpper();
   return buffer;  
}
/** True, if string contains only uppercase lettters and spaces.
*/
bool HoBIT_String::isUpper(const QString & text){
  QString textUp= text.toUpper();
  return textUp == text;
}

/** Strips leading zeros and returns a char *
*/
QString HoBIT_String::toIdFormat(QString s){
 QString result;
 if(s== "")
    return "";

 int pos = 0;
 while(s[pos] == '0') //strip leading zeros
  pos++; 
 while((s[pos] >= '0') && (s[pos] <= '9') && (pos < s.length())){ //strip currency
  result += s[pos];
  pos ++;
 } 
 return result;
}

/** Copies a QStringList to a STL list<string>
*/
void HoBIT_String::copyQStringList(QStringList * src, list <string> * dest){
  QStringList::iterator it = src->begin();
  while(it != src->end()){
   dest->push_back((*it).toLatin1().data());
   it++;
  } 
}

/** Returns true, if list is empty or contains only empty lines or whitespaces.
*/
bool HoBIT_String::stringListHasEmptyLines(QStringList list){
  bool result = list.isEmpty();
  if(! result){
   result = true;
   QStringList::iterator it = list.begin();
   while(it != list.end() && result){
     QString s = (*it).trimmed();
     if( ! s.isEmpty())
       result = false;
     it++;  
   }//end while
  }//end if
  return result;
}

/** Remove all trailing empty lines from the list 
*/
void HoBIT_String::stripTrailingEmptyLines(QStringList * list){
   while((list->count() > 0) && (list->last().trimmed().isEmpty()))
     list->removeLast();
 }

/** Prints the lines to stdout
*/
void HoBIT_String::debugStringList(QStringList  list){
 QStringList::iterator it = list.begin();
 int line =0;
 while(it != list.end()){
   qDebug("Line %i: %s", line, (*it).toLatin1().data());
   it++; 
   line ++; 
 }//end while
}//end if


/** Removes all the text from the beginning of startBlock to the end of endBlock from file fileName.
*   startBlock and endBlock should be regular expressions.
*/
bool HoBIT_String::removeTextBlock(QString startBlock, QString endBlock, QString fileName){
      QFile confFile (fileName);
      qDebug("HoBIT::removeTextBlock: from File: %s  from Startblock: %s  to EndBlock: %s",
             fileName.toLatin1().data(), startBlock.toLatin1().data(), endBlock.toLatin1().data());
      if(!confFile.open(QIODevice::ReadWrite)) //check for read and write 
       return false;
      QString confData = confFile.readAll();
      confFile.close();
      QFile::remove(fileName);

      //for Debugging
      QRegExp testExp;
      testExp.setPattern(startBlock);
      int pos = testExp.indexIn(confData);
      if(pos < 0)
       qDebug("Startblock not found");
      else qDebug("Startblock found at position: %i", pos);
      testExp.setPattern(endBlock);
      pos = testExp.indexIn(confData);
      if(pos < 0)
       qDebug("Endblock not found");
      else qDebug("Endblock found at position: %i", pos);   

      QRegExp re;
      re.setMinimal(true); //switch off "greedy" behaviour
      re.setPattern(startBlock + ".*" + endBlock); //find the  textblock
      re.indexIn(confData); //catch it
      qDebug("Captured Text: %s", re.capturedTexts()[0].toLatin1().data());
      confData.remove(re.capturedTexts()[0]); //remove it
      confFile.open(QIODevice::ReadWrite);
      confFile.write(confData.toLatin1());
      confFile.close();
      return true;
}


/** Replaces all the text between the end of startBlock to the beginning of endBlock from file fileName
 * with the string newBlock. startBlock and endBlock should be regular expressions.
*/
bool HoBIT_String::replaceTextBlock(QString startBlock, QString endBlock, QString newBlock, QString fileName){
      QFile confFile (fileName);
      if(!confFile.open(QIODevice::ReadWrite)){ //check for read and write 
       qDebug( "HoBIT_String::replaceTextBlock: Error, can not open file for writing: %s", fileName.toLatin1().data());
       return false;
      }

      QTextStream s (&confFile);
      QString confData = confFile.readAll();
      confFile.close();
      QFile::remove(fileName);

      QRegExp re;
      re.setMinimal(true); //switch off "greedy" behaviour
      re.setPattern(startBlock + "(.*)" + endBlock); //find the  textblock
      re.indexIn(confData); //catch it
      QString capturedText = re.capturedTexts()[1];
      qDebug("HoBIT_String::replaceTextBlock: Captured Text: %s", capturedText.toLatin1().data());
      int startOfCapture = confData.indexOf(capturedText);
      QString leftPart = confData.left(startOfCapture);
      QString rightPart = confData.right(confData.length()- startOfCapture - capturedText.length());
      confData = leftPart + newBlock  + rightPart;
      //qDebug("New File: %s", confData.toLatin1().data());
      confFile.open(QIODevice::ReadWrite);
      confFile.write(confData.toLatin1());
      confFile.close();
      return true;
}

/** Converts binary data to ASCII-hex code.
  */
string HoBIT_String::binToHexString(const unsigned char * bin, int length, 
                                    int lineLength)
{
  string result;
  char byte[4];

  for(int i = 0; i < length; i++){
   unsigned char char1 = (bin[i]>>4); //0 to 9 are from '0' to 'F', high nibble
   switch(char1){
     case 0: byte[0] = '0'; break;
     case 1: byte[0] = '1'; break;
     case 2: byte[0] = '2'; break;
     case 3: byte[0] = '3'; break;
     case 4: byte[0] = '4'; break;
     case 5: byte[0] = '5'; break;
     case 6: byte[0] = '6'; break;
     case 7: byte[0] = '7'; break;
     case 8: byte[0] = '8'; break;
     case 9: byte[0] = '9'; break;
     case 10: byte[0] = 'A'; break;
     case 11: byte[0] = 'B'; break;
     case 12: byte[0] = 'C'; break;
     case 13: byte[0] = 'D'; break;
     case 14: byte[0] = 'E'; break;
     case 15: byte[0] = 'F'; break;
   }
   unsigned char char2 = bin[i] & 0x0F; //low nibble
   switch(char2){
     case 0: byte[1] = '0'; break;
     case 1: byte[1] = '1'; break;
     case 2: byte[1] = '2'; break;
     case 3: byte[1] = '3'; break;
     case 4: byte[1] = '4'; break;
     case 5: byte[1] = '5'; break;
     case 6: byte[1] = '6'; break;
     case 7: byte[1] = '7'; break;
     case 8: byte[1] = '8'; break;
     case 9: byte[1] = '9'; break;
     case 10: byte[1] = 'A'; break;
     case 11: byte[1] = 'B'; break;
     case 12: byte[1] = 'C'; break;
     case 13: byte[1] = 'D'; break;
     case 14: byte[1] = 'E'; break;
     case 15: byte[1] = 'F'; break;
   }
   byte[2] = ' '; //space
   byte[3] = '\0'; //terminate
   result += byte;
   if(((i +1) % lineLength) == 0)
    result += "\n";
  }//end for
  return result;
}
