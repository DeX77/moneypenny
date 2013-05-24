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

 #include "globals.h"

 #include <QDir>
 #include <QInputDialog>
 #include <QMessageBox>
 #include <QLocale>
 #include <QApplication>
 #include <QDesktopWidget>
 #include <QColor>

//static variable for _Mp_Dir()
QString  _Mp_Dir;
//static variable for library prefix for libchipcard etc.
QString _Lib_Prefix;

//static palette objects for colored widget backgrounds
QPalette *_Palette_Green =NULL;
QPalette *_Palette_Yellow = NULL; 
QPalette *_Palette_Orange = NULL;
QPalette *_Palette_Red = NULL;
QPalette *_Palette_Red_Text = NULL;
QPalette *_Palette_Black_Text = NULL;

//The values for the separators
//                                   0    1    2    3    4     5
char MONEYPENNY_CSV_SEPARATORS(uint index){
 char separators[] = {';', ',', ':','\t', '\'', '"'};
 return separators[index];
}
//global debug flag
bool _Debug_Database = false; 

/**Set the palette colors
 */
 void initPalettes(){
  _Palette_Green = new QPalette();
  _Palette_Green->setColor(QPalette::Window, Qt::green);	
  _Palette_Green->setColor(QPalette::Base, Qt::green);	
  _Palette_Green->setColor(QPalette::Button, Qt::green);	
  _Palette_Yellow = new QPalette();
  _Palette_Yellow->setColor(QPalette::Window, Qt::yellow);	
  _Palette_Yellow->setColor(QPalette::Base, Qt::yellow);	
  _Palette_Yellow->setColor(QPalette::Button, Qt::yellow);	
  _Palette_Orange = new QPalette();
  _Palette_Orange->setColor(QPalette::Window, QColor(255, 128,0));	
  _Palette_Orange->setColor(QPalette::Base, QColor(255, 128,0));	
  _Palette_Orange->setColor(QPalette::Button, QColor(255, 128,0));	
  _Palette_Red = new QPalette();
  _Palette_Red->setColor(QPalette::Window, Qt::red);	
  _Palette_Red->setColor(QPalette::Base, Qt::red);	
  _Palette_Red->setColor(QPalette::Button, Qt::red);	
  _Palette_Red_Text = new QPalette();
  _Palette_Red_Text->setColor(QPalette::WindowText, Qt::red);	
  _Palette_Black_Text= new QPalette();
  _Palette_Black_Text->setColor(QPalette::WindowText, Qt::black);	
 }	
/**Free the palettes
 */
 void freePalettes(){
  delete _Palette_Green;
  delete _Palette_Yellow;
  delete _Palette_Orange;
  delete _Palette_Red;
  delete _Palette_Red_Text;
  delete _Palette_Black_Text;
 }
 /**Read property for predefined palette green
  */
 const QPalette & PALETTE_GREEN(){
     if(!_Palette_Green){
       qDebug("Error: Palettes are not initialized!");
       initPalettes();
     }
 	return *_Palette_Green;
 }	
 /**Read property for predefined palette yellow
  */
 const QPalette & PALETTE_YELLOW(){
     if(!_Palette_Yellow){
       qDebug("Error: Palettes are not initialized!");
       initPalettes();
     }
 	return *_Palette_Yellow;
 }	
 /**Read property for predefined palette red
  */
 const QPalette & PALETTE_RED(){
     if(!_Palette_Red){
       qDebug("Error: Palettes are not initialized!");
       initPalettes();
     }
 	return *_Palette_Red;
 }	 
 /**Read property for predefined palette orange
  */
 const QPalette & PALETTE_ORANGE(){
     if(!_Palette_Orange){
       qDebug("Error: Palettes are not initialized!");
       initPalettes();
     }
 	return *_Palette_Red;
 }	 
/**Read property for predefined palette red text color
  */
 const QPalette & PALETTE_RED_TEXT(){
     if(!_Palette_Red_Text){
       qDebug("Error: Palettes are not initialized!");
       initPalettes();
     }
 	return *_Palette_Red_Text;
 }	 
 /**Read property for predefined palette black text color
  */
 const QPalette & PALETTE_BLACK_TEXT(){
     if(!_Palette_Black_Text){
       qDebug("Error: Palettes are not initialized!");
       initPalettes();
     }
 	return *_Palette_Black_Text;
 }	  


/** set the main project directory from the program filepath
*/
void setMP_DIR(QString programFilePath){
 if(_Mp_Dir.length() == 0){ // do not allow modifications
    qDebug("Program file path : %s", programFilePath.toLatin1().data());
    qDebug("pwd:"); system("pwd");
    QFileInfo mpBinary(programFilePath);
    QDir dir( mpBinary.absoluteDir()); //absolute directory path of the binary, we are in moneypenny/bin or /usr/bin now
    dir.cd("..");// go up to the main dir. This is ~/moneypenny, when running from the development tree
                 // When running from the install tree it is /usr
    _Mp_Dir = dir.path() +"/";
    if(QFile(_Mp_Dir + "moneypenny.pro").exists())
      qDebug("Running from development directory tree.");
    else
     if(_Mp_Dir.indexOf("usr")>=1 ){ // may be /usr or /usr/local or /UNIONFS/usr on a KNOPPIX CD
       _Mp_Dir = _Mp_Dir + "share/moneypenny/";
     }  
     if(_Mp_Dir.indexOf("opt")>=1){ // may be /opt or /UNIONFS/opt on a Live CD
       _Mp_Dir = _Mp_Dir; //MP_DIR is already correct
     }  
    QString fileName = _Mp_Dir + MONEYPENNY_DATA_DIR + "/" + MONEYPENNY_ALL_BANKDATA_FILE;
    if(!QFile(fileName).exists()){
     QString msg("Error in installation. Can not find file ");
     msg += fileName + QString(" in moneypenny's main directory.");
     qDebug("%s", msg.toLatin1().data());
     qDebug("MoneyPenny (false) main directory _Mp_Dir: %s", _Mp_Dir.toLatin1().data());
     exit (4);
    }
 }
 qDebug("MoneyPenny main directory _Mp_Dir: %s", _Mp_Dir.toLatin1().data());
}

/** Returns top resource dir with trailing /
*/
QString MP_DIR(){
  return _Mp_Dir;
}

/** Check for existing .qm file.
*/
QString tryLocale(QString language, QString baseName){
    QString qmFileName =  MP_DIR() + QString(MONEYPENNY_TRANSLATIONS_DIR) + "/" + baseName + language + ".qm";
    QFile qmFile(qmFileName);
    if(!qmFile.exists()){
       qDebug("Could not find translation file: %s", qmFileName.toLatin1().data());
       return "";
    }
    return qmFileName;
}

/** Returns the name of the translation file for the local language. Returns
  * an empty string, if it does nor exist
  */
QString QM_FILE_NAME(QString baseName){
    QString result;
    #ifdef FORCE_LANGUAGE
      QString language = FORCED_LANGUAGE ;
    #else
      QString language = QLocale::system().name();
    #endif
    qDebug("System language setting (locale): %s", language.toLatin1().data());
    result = tryLocale(language, baseName);
    if(result.isEmpty()){
      language = language.left(language.indexOf(".")); //remove encoding info e.g. .UTF8
      result = tryLocale(language, baseName);
    }
    if(result.isEmpty()){
      language = language.left(language.indexOf("_")); //remove sublanguage e.g. _eur and try again
      result = tryLocale(language, baseName);
    }
    return result;
 }

 QFont calculateScreenFont(){
      //Set default font size according to screen and teststring size
  uint globalFontSize = 18;
  uint screenWidth =  qApp->desktop()->width();
  uint screenHeight = qApp->desktop()->height();
  uint charsPerLine = (140 * screenWidth) / 1024; //use more chars on a bigger screen
  if (charsPerLine > 160) //more makes no sense
    charsPerLine = 160;
  qDebug("Screensize: %i * %i  Chars per line: %i", screenWidth, screenHeight, charsPerLine);
  if ((double)screenWidth / (double)screenHeight > (4.0 / 3.0)) 
    charsPerLine = (charsPerLine * 3 * 16) / 9 /4; //longer lines on a wide-screen
  if (screenWidth >1600)
    screenWidth = 1600; //not more than 1600, dialogs do not grow
  uint stringWidth; 
  QFont globalFont(MONEYPENNY_SCREEN_FONT, globalFontSize);  //The FreeSans font familiy must be available in /usr/X11R6/lib/X11/fonts/truetype
                                                // See knx2moneypenny.sxw for details.
  QString testString(QObject::tr("abcdefghijklmnopqrstuvwxyz    eeeerrmnncy.,!?1234567890", 
                         "String with the alphabet for calculating font size. Take care of character probabilty. Space is a frequent character!"));
  do{  
    globalFontSize--; 
    QFontMetrics metrics(globalFont);
    stringWidth = metrics.width(testString);
    globalFont.setPointSize(globalFontSize); 
    qDebug("Testing font size: %i", globalFontSize);
   }while ((stringWidth * charsPerLine) / testString.length() > screenWidth ); 
   qDebug("Using font : %s   Fontsize: %i",globalFont.rawName().toLatin1().data(),globalFontSize);
  return globalFont;  
 }   

/** Parse command line arguments
  */
void parseCommandLine(){
  setMP_DIR(qApp->applicationFilePath()); //use argv[0]

  for (int i = 1; i < qApp->argc(); i++){
     if(QString(qApp->argv()[i]) == "--debug_database")
       _Debug_Database = true;
  }
}

/** Read property for _Debug_OpenHBCI
*/
bool DEBUG_DATABASE(){
  return _Debug_Database;
}

/** Returns the prfix for the library path for libchipcard etc.
*/
QString LIB_PREFIX(){
  return _Lib_Prefix;
}

/**Exports LANGUAGE=de, if no LANGUAGE system variable is set.
*/
void EXPORT_LANGUAGE(){
    //Check for an existing LANGUAGE variable
    FILE * language = popen("echo $LANGUAGE", "r");
    char buf [16];
    buf[0] ='\0'; //terminate
    if(language){
      fgets(buf, sizeof(buf), language);
      buf[strlen(buf) -1] ='\0'; //remove \n
      pclose(language);
    }
    if(strlen(buf) == 0) //no LANGUAGE set, export de
      system("export LANGUAGE=de");
}

/**
 * The MoneyPenny linux live CD uses Knoppix where you can use \c sudo
 * on shell level WITHOUT entering a password. In contrast to this,
 * on other non-live-CD linux systems, you MUST enter a password.
 * This function offers a small workaround for the \c sudo command.
 * If you want to execute a system command with root rights, you can
 * prepend the string returned from this function to the system command.
 * The function automatically determines if you are working on a live CD
 * or not and pops up a password input dialog if needed.
 * @ param msg The "Enter your password message for ..." text
 * @returns  system dependend sudo string
 */
QString sudo(const QString command, QWidget * parent, const QString & msg )
{
  bool    okPressed = true;

   QString rootPassword = MONEYPENNY_LIVE_CD_PASSWORD;
   QString result; 
   QString message;
   if(msg.isEmpty())
    message = QObject::tr("Please enter the root password!");
   else message = msg;
  if ( !USE_LIVE_CD_FEATURES )
  {
    //
    // Since we are not operating under Knoppix, we ask the user for the root password
    //
    rootPassword =  QInputDialog::getText(parent,
                      QObject::tr("Information"),
                      message,
                      QLineEdit::Password,
                      0,
                      &okPressed
                      );
    }
  result  = QString("echo ") + rootPassword + " | sudo -S " + command;
  #ifdef USE_LIVE_CD_FEATURES
    qDebug("sudo command: %s", qPrintable(result));
  #endif
  return result;
}
/** Convenience function returning a char 
  */
const char * c_sudo(const QString command, QWidget * parent, const QString & msg)
{
  return qPrintable (sudo(command, parent, msg));
}




class ExNotImplemented: public std::exception
{
  virtual const char* what() const throw()
  {
    std::cerr << "***************Exeption: Not Implented**************\n";
    return "Error: Functionality is NOT (YET)IMPLEMENTED";
  }
};

/** Throws an exception of typ ExNotImplemented
*/
void throw_ExNotImplemented(){
  throw ExNotImplemented(); 
}
