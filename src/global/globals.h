/***************************************************************************
 *   Copyright (C) 2004,2007 by Walter Roth                                *
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

#ifndef GLOBALS_H
#define GLOBALS_H
 
#ifdef HAVE_CONFIG_H
 #include <config.h>
#else
 #define VERSION "3.0 BETA"
#endif

//#define MESSE
//#define LIVE_CD

#include <exception>
#include <iostream>

#include <QString>
#include <QFont>

/** Global constants, enums and defines for moneypenny
*/

/**Functions for main.cpp
 */
/** Calcualtes a font size, that makes the widgets fit on the avaliable screen.
 */ 
QFont calculateScreenFont();

/** Returns the name of the translation file for the local language. Returns
  * an empty string, if it does nor exist. Tries FORCED_LANGUAGE if FORCE_LANGUAGE
  * is defined.
  */
QString QM_FILE_NAME(QString baseName);
//Language setting independent of system settings 
//#define FORCE_LANGUAGE
//#define FORCED_LANGUAGE "ru"

 /**Global palettes for colored widget backgrounds
  */
/**Palette color settings, must be called once after QApplication has been constructed
 */
 void initPalettes();
/**Free the palettes. Must be called berfore exiting the program.
 */
 void freePalettes();

 /**Read property for predefined palette green
  */
 const QPalette & PALETTE_GREEN(); 
 /**Read property for predefined palette yellow
  */
 const QPalette & PALETTE_YELLOW(); 
 /**Read property for predefined palette orange
  */
 const QPalette & PALETTE_ORANGE(); 
 /**Read property for predefined palette red
  */
 const QPalette & PALETTE_RED(); 
 /**Read property for predefined palette red text color
  */
 const QPalette & PALETTE_RED_TEXT(); 
 /**Read property for predefined palette black text color
  */
 const QPalette & PALETTE_BLACK_TEXT(); 

/**Release data
 */
#define MONEYPENNY_RELEASE_DATE "NOT RELEASED"

//if OS_Version is updated, adjust MONEYPENNY_ENCRYPTED_KNOPPIX_FILE below!!
#define MONEYPENNY_CD_OS_VERSION "Ubuntu 10.04" 

#ifdef LIVE_CD
  #define APPLICATION_NAME "MoneyPenny_CD"
#else
  #define APPLICATION_NAME "MoneyPenny"
#endif
#define SHOW_PREDEFINED_CONFIGS false
#define MONEYPENNY_LIVE_CD_PASSWORD "moneypenny"
/** We detect Endian here
 */
const bool PLATFORM_IS_LITTLE_ENDIAN = * (unsigned short int *) ("\x00\x01") == 0x0100;

/**
* The next section describes the default path and file 
* names of all HoBIT / Moneypenny-Files
*/
/** To be called from main.cpp for relocating the resource and library tree
*/
void setMP_DIR(QString programFilePath); 
/**returns moneypenny's main resource data directory as an absolute path with trailing /
*/
QString MP_DIR(); 

/** Returns the prfix for the library path for libchipcard etc. including trailing /
*/
QString LIB_PREFIX();

/**Exports LANGUAGE=de, if no LANGUAGE system variable is set.
*/
void EXPORT_LANGUAGE();


/**
 * The MoneyPenny linux live CD uses Knoppix where you can use \c sudo
 * on shell level WITHOUT entering a password. In contrast to this,
 * on other non-live-CD linux systems, you MUST enter a password.
 * This function offers a small workaround for the \c sudo command.
 * If you want to execute a system command with root rights, you can
 * prepend the string returned from this function to the system command.
 * The function automatically determines if you are working on a live CD
 * or not and pops up a password input dialog if needed.
 * @param command the command to be executed as root 	
 * @param msg The "Enter your password message for ..." text
 * @returns  system dependend sudo string
 */
QString sudo(const QString command, QWidget * parent, const QString & msg = "");
/** Convenience function returning a char 
  */
const char * c_sudo(const QString command, QWidget * parent, const QString & msg = "");



//Installation directories
#define MONEYPENNY_HELP_DIR "help" 
#define MONEYPENNY_ICON_DIR "icons" 
#define MONEYPENNY_DATA_DIR "data" 
#define MONEYPENNY_DTA_DIR "dta"
#define MONEYPENNY_INTERNET_CONFIG_DIR "internet"
#define MONEYPENNY_PRINTERDATA_DIR "printerdata"

//Writable directories, final paths are set in guiFindMedium
#define MONEYPENNY_HBCIDATA_DIR "hbcidata"
#define MONEYPENNY_BANKDATA_DIR "bankdata"
#define MONEYPENNY_MAIN_DIR "moneypenny_data"
#define MONEYPENNY_LOG_DIR "hbci_logs" 
#define MONEYPENNY_PDF_DIR "moneypenny_pdf"
#define MONEYPENNY_BANKS_DIR "banks"
#define MONEYPENNY_OS_CONFIG_DIR "os_config"
#define MONEYPENNY_CERTIFICATE_DIR "certificates"

//Filenames
#define MONEYPENNY_OPENHBCI_FILE "openhbci.conf"
#define MONEYPENNY_AQMONEY_FILE "moneypenny.conf"
#define MONEYPENNY_TEMPLATE_FILE "templates.conf"

#define MONEYPENNY_HBCI_LOGFILE "hbci.log"
#define MONEYPENNY_TEMPLATE_FILE_XML "templates.xml"
#define MONEYPENNY_BANK_FILE_XML "banks.xml"
#define MONEYPENNY_TRANSACTIONS_FILE_XML "transactions.xml"
#define MONEYPENNY_TRANSLATIONS_DIR "translations"
#define MONEYPENNY_ENCRYPTED_HBCI_CONF_FILE "moneypenny.bf"
#define MONEYPENNY2_ENCRYPTED_HBCI_CONF_FILE "moneypenny2.bf"
#define MONEYPENNY_HBCI_INIVECTOR_FILE "iv.dat"
#define MONEYPENNY2_HBCI_INIVECTOR_FILE "iv2.dat"
#define MONEYPENNY_ENCRYPTED_OS_FILE "ubuntu_config.bf"
#define MONEYPENNY_OS_INIVECTOR_FILE "ubuntu_config_iv.dat"
#define MONEYPENNY_PASSWORD_FILE "pwmd.dat"
#define MONEYPENNY_ALL_BANKDATA_FILE "bankdata.txt"
#define MONEYPENNY_OLD_HBCI_BANKDATA_FILE "fints_institute.csv"
#define MONEYPENNY_HBCI_BANKDATA_FILE "hbcibankdata.hbd"
#define MONEYPENNY_KNOPPIX_MOUNTPOINT "/mnt/usb_moneypenny"
#define MONEYPENNY_SQL_FILE_NAME "dbmoneypenny.sql"
#define MONEYPENNY_READERS_FILE "readers.xml"


// SQL Connection
#define MYSQL_DRIVER 		 "QMYSQL3"  /** MySQL Driver */
#define MYSQL_DATABASE	 	 "dbmoneypenny"
#define MYSQL_USER 		     "moneypenny"
#define MYSQL_PASSWORD	 	 "moneypenny"
#define MYSQL_HOST		      "localhost"
#define USE_MYSQL_FEATURES 	 true

//Font names
//The Bitstream Charter and FreeSans font families must be available in /usr/X11R6/lib/X11/fonts/truetype
#define MONEYPENNY_PRINTER_FONT "Bitstream Charter" 
#define MONEYPENNY_SCREEN_FONT "FreeSans" 


/* region settings , 280 is Germany*/
#define MONEYPENNY_DEFAULT_COUNTRY 280   
#define MONEYPENNY_DUMMY_ACCOUNT_ID "00000000"

/* default name of the export file*/
#define MONEYPENNY_DEFAULT_EXPORT_FILE "mp_export.csv"
#define MONEYPENNY_DEFAULT_SQL_EXPORT_FILE "mp_export.sql"


/* default webinterface for cups - normaly port 631 on localhost */
#define CUPS_CONFIG_PAGE "http://localhost:631/"


/** the numbers of the widgets on the main widget stack
*/
typedef enum {
  NULL_WIDGET=0,
  NOT_IMPLEMENTED_WIDGET,
  WELCOME_WIDGET,
  HBCI_WIZARD,
  TOOLBOX_WIDGET,
  SHOW_ACCOUNTDATA_WIDGET,
  HARDWARE_WIDGET,
  DIALIN_WIDGET,
  LOGIN_WIDGET,
  // BALANCES_WIDGET,
  ACCOUNT_INFORMATION_WIDGET,
  TRANSACTION_LIST_WIDGET,
  GET_TRANSACTIONS_WIDGET,
  TRANSACTIONS_PRINTER_WIDGET,
  INILETTER_WIDGET,
  TRANSFER_WIDGET,
  ACCEPT_TRANSACTION_WIDGET,
  DATED_SINGLE_TRANSFER_WIDGET,
  DATED_TRANSFERS_WIDGET,
  STANDING_ORDERS_WIDGET,
  HELP_WIDGET,
  KNOWNBANKS_WIDGET,
  EDIT_ACCOUNT_NAME_WIDGET,
  TRANSFER_TEMPLATES_WIDGET,
  PRINTER_AUTO_SETUP_WIDGET,
  PRINTER_MANUAL_SETUP_WIDGET,
  JOBS_WIDGET,
  LOG_WIDGET,
  ADD_ACCOUNT_WIDGET,
  TRANSACTION_INFORMATION_WIDGET,
  EXPORT_WIDGET,
  SQL_QUERY_WIDGET,
  SQL_RESULT_WIDGET,
  PRINTER_DRIVER_TO_XML_WIDGET,
  ENTROPY_WIDGET,
  IMPORT_WIDGET,
  LAST_WIDGET 
} WIDGET;


/**The HBCI communication modes
*/
typedef enum{
  HBCI_MODE_ERROR,
  HBCI_MODE_DDV,
  HBCI_MODE_RDH_STEP1,
  HBCI_MODE_RDH,
  HBCI_MODE_PIN_TAN //corresponds to PASSPORT_TYPE_OTP
}HBCI_MODE;

/**The HBCI media types
*/
#define HBCI_MEDIUM_TYPE_DDV "DDVCard2"
#define HBCI_MEDIUM_TYPE_RDH "RDHFile"

 /**
* HoBIT uses ktoblzcheck to validate accounts 
* This means: If KtoBlzCheck does not know a 
* BankId, Transactions to those accounts will be
* skipped. During testing the default setting 
* musst be false, because KtoBlzCheck will tell
* hobit that the testbank does not exist
*/
#define SKIP_FALSE_TRANSACTIONS false



/**
* Money Penny comes as a linux version and on live cd
* The live cd version has special features which are not
* available in the linux version. set this param true
* to build a live-cd version.
*/
#ifdef DEBUG_HARDWARE_ON //use this flag to switch on the hardware page on SuSE
  #define DEBUG_HARDWARE true
#else
  #define DEBUG_HARDWARE false
#endif

#ifdef LIVE_CD
  #define USE_LIVE_CD_FEATURES true
#else
  #define USE_LIVE_CD_FEATURES false
#endif

#define MONEYPENNY_LIVE_CD_PASSWORD "moneypenny"
/** This block is used for UD-RW support. Still under construction
*/
#define UDRW_DEVICE "/dev/sda1"
#ifdef LIVE_UDRW
  #define USE_UDRW_FEATURES true
#else 
  #define USE_UDRW_FEATURES false
#endif

/***********************************************
* Later versions of MoneyPenny work fine under 
* Mac osX.  Because autorecognition of storage 
* media has to work different on darwin, Money
* Penny has a special option to be compiled on 
* Mac OsX
***********************************************/
#define OSX_MOUNT_PATH
#ifdef OSX
    #define BUILD_FOR_MAC_OSX true
#else
    #define BUILD_FOR_MAC_OSX false
#endif


/** The type of storage medium for the config and data.
*/
typedef enum{
 STORAGE_UNKNOWN,
 STORAGE_FLOPPY,
 STORAGE_USB,
 STORAGE_OSX,
 STORAGE_ZIP,
 STORAGE_MANUALLY
}StorageType;


/** Exit Codes of the Application
*/
typedef enum{
 EXITCODE_SUCCESS,
 EXITCODE_WRONG_PASSWORD,
 EXITCODE_FAILURE,
 EXITCODE_RESTART,
 EXITCODE_INSTALL_ERROR,
 EXITCODE_AUTOSHUTDOWN,
 EXITCODE_RUN_BANK_LOOP,
 EXITCODE_RESTART_SESSION,
 EXITCODE_NO_SAVING
}ExitCode;


/**Separator characters for CSV file export
*/
//                                              0    1    2    3    4     5
char MONEYPENNY_CSV_SEPARATORS(uint index); //{';', ',', ':','\t', '\'', '"'};
 
/** Flag for debugging database. Causes program to stop
  * after openhbci config has been decrypted. Set by 
  * parseCommandLine(); 
  */
bool DEBUG_DATABASE();

/** Parse command line arguments
  */
void parseCommandLine();

/** Throws an exception of typ ExNotImplemented
*/
void throw_ExNotImplemented();

#endif

