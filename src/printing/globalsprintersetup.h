/***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file contains the definition of globally used types, functions
 *   and constants for the Linux Printer Setup project.
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   STATUS: File has been reviewed and commented
 *
 *   $Id: globalsprintersetup.h,v 1.3 2011-01-14 20:05:38 hbci Exp $
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software Foundation,
 *   Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#ifndef GLOBALSPRINTERSETUP_H
#define GLOBALSPRINTERSETUP_H

// Qt includes
#include <QMap>
#include <QString>
#include <QtXml/QDomDocument>
//#include <qwidget.h>
class QWidget;

//
//====================================================================================
//
// Constants for the LPS automatic printer setup
//
//====================================================================================
//

#define LPS_USB_DEVICE_DIR     "/sys/bus/usb/devices"
#define LPS_PARPORT_DEVICE_DIR  "/proc/sys/dev/parport/"

//
//====================================================================================
//
// Global constants for LPS autmatic AND manual printer setup
// (Don't let the names fool you!)
//
//====================================================================================
//

#define LPS_DRIVER_DETAILS_DIR      "/usr/share/foomatic/db/source/driver/"
#define LPS_DB_FILE                 "/printerdata.xml"
#define LPS_CUPS_MODEL_DIR_DEFAULT  "/usr/share/ppd"
#define LPS_LPADMIN_PATH            "/usr/sbin/"
#define LPS_DRIVER_STYLESHEET_PATH  "/driverdetails.xslt"
#define LPS_APP_NAME                "PrinterSetup"
#define LPS_PRINTER_NAME            "LinuxPrinter"
#define LPS_DB_DOCUMENT_NAME        "CupsDriverDb"
#define LPS_URL                     "http://printersetup.sourceforge.net"
#define LPS_TESTPAGE_WIDTH          263
#define LPS_TESTPAGE_HEIGHT         341
#define LPS_TESTPAGE_IMG_SRC        "testpage.png"
#define LPS_TESTPAGE_PS_SRC         "testpage.ps"
#define LPS_TESTPAGE_JOBNAME        "Testpage"
#define LPS_CUPS_PDF_CONF           "/etc/cups/cups-pdf.conf"

/**
 * This namespace contains classes, type definitions and functions for usage
 * with the LinuxPrinterSetup project.
 */

namespace LinuxPrinterSetup
{


/**
 * The LPS project supports USB- and Parallel-Port-printers
 */
enum ConnectionType
{
  UsbConnection,
  ParPortConnection
};


/**
 * This class encapsulates technically relevant information of a connected printer.
 */

class PrinterDevice
{
public:

  /**
   * Constructs a printer device object without setting any members.
   */
  PrinterDevice();

  /**
   * Constructs a printer device object and sets the members.
   *
   * @param nativeManufacturer
   * @param nativeProduct
   * @param connectionType
   * @param drivers
   */
  PrinterDevice(  const QString & nativeManufacturer,
                  const QString & nativeProduct,
                  ConnectionType connectionType,
                  const QDomNodeList & drivers );

  // Publicly available members
  QString        nativeManufacturer;  //!< e.g. HP, Lexmark etc.
  QString        nativeProduct;       //!< e.g. Deskjet 670C
  ConnectionType connectionType;      //!< @see \c ConnectionType
  QDomNodeList   drivers;             //!< The list of drivers usable with this printer

};


typedef QMap< QString, PrinterDevice > PrinterMap;  //!< Used in LPS Auto Setup


typedef struct ppd_info
{
  QString ppdFilePath;
  QString product;
  QString nickName;
  QString modelName;
  QString manufacturer;
  QString languageVersion;
}
PpdInfo;


/**
 * This class represents a normal notice text.
 * It is used as an exception type.
 */

class LpsNotice : public QString
{
public:

  /**
   * Constructs a LPS notice object and sets the message to \a str .
   *
   * @param str
   */
  LpsNotice( const char * str );

  /**
   * Same as above, except for the type of the \a str parameter.
   * Constructs a LPS notice object and sets the message to \a str .
   *
   * @param str
   */
  LpsNotice( const QString & str );

};


/**
 * This class represents a warning/error text.
 * It is used as an exception type.
 */

class LpsError : public QString
{
public:

  /**
   * Constructs a LPS error object and sets the message to \a str .
   *
   * @param str
   */
  LpsError( const char * str );

  /**
   * Same as above, error for the type of the \a str parameter.
   * Constructs a LPS notice object and sets the message to \a str .
   *
   * @param str
   */
  LpsError( const QString & str );

};


//
//====================================================================================
//
// Shared functions
//
// The following functions are used by both printer setups, the automatic and manual.
// Therefore, they are outsourced from the concrete classes and made friends.
//
//====================================================================================
//


/**
 * This function tries to find the appropriate XML information file for the
 * \a driverName in the filesystem. If the file exists, it's information is
 * extracted and presentated in a message box.
 * Such information includes detailed information of the driver capabilities.
 *
 * @see     LPS_DRIVER_DETAILS_DIR
 * @param   driverName
 * @throws  LpsError                if \c xsltproc command fails
 */
void showPrinterDriverDetails( const QString & driverName, QWidget * parent );


/**
 * This function pops up a yes/no message box, that asks if one wants to
 * print test page. If "yes" is chosen, the page is send to the printer
 * that's supposed to actually print it.
 * You must supply the \a widget from which this function is called,
 * because it's mouse cursor is set to a sandclock/wait symbol.
 *
 * @see                 LPS_TESTPAGE_PS_SRC
 * @param   widget      calling widget
 * @param   printerName the name of the printer on CUPS level
 * @throws  LpsError    if something CUPS' \c lpr command fails
 */
void testPrintRequest( QWidget * widget, const QString & printerName );


/**
 * This function installs a printer with the name \a printerName , connected
 * to the device \a deviceUri and the driver file \a ppdFilePath .
 *
 * @param   widget      calling widget
 * @param   printerName the name of the new printer on CUPS level
 * @param   deviceUri   the connection type already formatted for use with \c lpadmin
 * @param   ppdFilePath the file path to the driver file
 * @throws  LpsError    if something CUPS' \c lpadmin command fails
 */
void installPrinter(  QWidget * widget,
                      const QString & printerName,
                      const QString & deviceUri,
                      const QString & ppdFilePath );

/**
 * Sets the OUT parameter in /etc/cups/cups-pdf.conf to \a newDirName
 * @param newDirName the new directory for pdf files generated by cups
 */
bool setPs2PdfTargetDir(const QString & newDirName);

}; /* End of namespace LinuxPrinterSetup */



/*! \mainpage The Linux Printer Setup for MoneyPenny
 *
 * \section intro_sec Introduction
 *
 * This is the technical source code documentation for the Linux Printer Setup for MoneyPenny.
 *
 * The Linux Printer Setup is a <A href="http://www.cups.org/" target="_blank">CUPS</A> and
 * <A href="http://trolltech.com" target="_blank">Qt</A> based tool to offer
 * an easy to use alternative to the standard and much more complex KDE print setup utility.
 * It shall bring more compfort and usability onto the KDE desktop by supplying autodetection
 * mechanisms and an access to the printer/driver database, that is at least 10 times faster
 * than KDE print setup utility.
 *
 * The Linux Printer Setup has been developed as a tool to run on a Linux LiveCD
 * in an embedded window mode of the Linux homebanking software
 * (<A href="http://www3.fh-swf.de/fbin/roth/money-penny_en.htm">MoneyPenny</A>).
 * It has been constructed to be very fast and well-engineered.
 *
 * It consists of 3 components:
 * <UL>
 *  <LI>A Database build tool (named "Printer Driver To XML"),</LI>
 *  <LI>a manual- and</LI>
 *  <LI>an automatic-printer-setup.</LI>
 * </UL>
 *
 * Each of the last two components can be used independently.
 * We are also thinking of a kcontrol integration for future releases.
 *
 * The Linux Printer Setup only handles USB and Parallel port printers.
 * People who must handle network and other printers are usualy able to do so,
 * but for the "normal user" who is not versed in the CUPS system, the methods
 * for setting up printers under linux are currently regarded as too complicated.
 *
 * PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                drivers' or simply 'drivers', we mean the PPD files
 *                and not the actual drivers.
 */

#endif
