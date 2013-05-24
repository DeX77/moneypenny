/***************************************************************************
                          hobit_string.h  -  description
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

#ifndef HOBIT_STRING_H
#define HOBIT_STRING_H

#include <string>
#include <qstringlist.h> 
using namespace std;


/**This class is a helper class for string handling partially inspired by AqMoney
  *@author Marcel Mulch, Walter Roth
  */

class HoBIT_String {
public:
  /**
   * sprintf like function returning string
   */
  static const string format(const char *fmt, ...);

   /** dumps a hex string, so it can be
   * printed to the ini letter
   */
  static const string dumpHexString(string s);
   /** dumps a char array, so it can be
   * printed to the ini letter
   */
  static const string dumpHexString(char * data, int length);
  /** Converts binary data to ASCII-hex code.
   */
  static string binToHexString(const unsigned char * bin, int length, 
                                    int lineLength);
  /** dumps a fingerprint (20 byte HEX formatted) hex string
  */
  static const string dumpFingerprint(string indent, string s);
  /**removes German Umlaute or other special chars illegal for HBCI
  */
  static const QString replaceSpecialChars(const QString & text, bool replaceUmlaute = true);
  /** True, if string contains onlx uppercase lettters and spaces.
  */
  static bool isUpper(const QString & text);
  /** Strips leading zeros and returns a char *
  */
  static QString toIdFormat(QString s);
  /** Copies a QStringList to a STL list<string>
  */
  static void copyQStringList(QStringList * src, list <string> * dest);
  /** Returns true, if list is empty or contains only empty lines or whitespaces.
  */
  static bool stringListHasEmptyLines(QStringList list);
  /** Remove all trailing empty lines from the list 
  */
  static void stripTrailingEmptyLines(QStringList * list);
  /** Prints the lines to stdout
  */
  static void debugStringList(QStringList  list);
  /** Removes all the text from the beginning of startBlock to the end of endBlock from file fileName.
  *   startBlock and endBlock should be regular expressions.
  */
  static bool removeTextBlock(QString startBlock, QString endBlock, QString fileName);
  /** Replaces all the text between the end of startBlock to the beginning of endBlock from file fileName
   * with the string newBlock. startBlock and endBlock should be regular expressions.
  */
  static bool replaceTextBlock(QString startBlock, QString endBlock, QString newBlock, QString fileName);

};

#endif
