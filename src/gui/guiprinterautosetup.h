/***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file contains the automatic printer setup definition.
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   STATUS: File has been reviewed and commented
 *
 *   $Id: guiprinterautosetup.h,v 1.2 2010-09-28 16:50:27 hbci Exp $
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

#ifndef GUIPRINTERAUTOSETUP_H
#define GUIPRINTERAUTOSETUP_H

// App includes
#include "ui_guiprinterautosetupbase.h"
#include "../printing/globalsprintersetup.h"

using namespace LinuxPrinterSetup;

// Qt includes
//#include <qmap.h>

// Forward Declarations
class QDomDocument;
class QDomNode;
class QDomElement;
class QDomNodeList;
class PrinterSetupDb;


/**
 * This class implements the underlying functionality of the automatic
 * printer setup dialog. It can autodetect and setup printers and print
 * test pages on them.
 * Simply call the \c show() method and the rest is done by the class
 * itself.
 * @note    There are some signal/slots, that are directly connected to
 *          the MoneyPenny GUI ( \c helpRequest() , \c back() , \c showSmallHelp() ,
 *          \c on_pushButton_help_clicked() ).
 * @author  Konrad Wilhelm Kleine
 */

class GuiPrinterAutoSetup : public QWidget, public Ui::GuiPrinterAutoSetupBase
{
  Q_OBJECT

public:


  /**
   * Constructs a new printer auto setup dialog
   *
   * @see QWidget for parameter documentation
   */
  GuiPrinterAutoSetup( QWidget * parent = 0 );


  /**
   * This destructor 'takes care' of manually allocated memory
   */
  ~GuiPrinterAutoSetup();


public slots:


  /**
   * Shows the widget and its child widgets.
   * This method might take a while until it returns because the XML
   * printer driver database is loaded and the printer detection is
   * performed.
   */
  void showEvent(QShowEvent * se);


signals:


  /**
   * This signal is fired, when the user clicks on the "Back" button.
   * It is handled by the sourrounding UI and shall bring the user back to
   * the page that she/he visited before.
   */
  void back();


  /**
   * This signal is fired, when the user clicks on the "Help" button.
   * The user will be presented with a help page where afore the printer
   * auto setup dialog could be seen.
   *
   * @param page  the HTML filename of the help page to be displayed
   */
  void helpRequest( const QString & page );


  /**
   * This signal is fired, whenever this GUI is shown. A small help/info
   * will be seen in the bottom left corner.
   *
   * @param page  the HTML filename of the help page to be displayed
   */
  void showSmallHelp( const QString & page );


  /**
   * This signal is fired when the user clicks on the button to switch
   * to the manual printer setup. It is a hint at the sourrounding GUI
   * to open the manual printer setup dialog. The \a widgetNumber
   * argument will be \c PRINTER_MANUALSETUP_WIDGET .
   *
   * @param widgetNumber  an integer number that globally identifies widgets
   * @see   globals.h
   */
  void raiseWidget( int widgetNumber );

  /**
   * Signal is fired after a printer has been installed. All QPrinters in
   * the application should be restarted.
   */
  void printerInstalled();

protected slots:


  /**
   * This slot method is called, when the user clicks on the "Save Settings"
   * button. The current printer configuration is 'saved' and the user
   * is presented with the opportunity to print a test page.
   *
   * @see LinuxPrinterSetup::installPrinter
   * @see LinuxPrinterSetup::testPrintRequest
   */
  virtual void on_buttonSave_clicked();


  /**
   * This slot method sends a signal to the surrounding GUI that the help
   * page for automatic printer setup shall be displayed.
   */
  virtual void on_buttonHelp_clicked();


  /**
   * This slot method is called when the user changes the selection in the
   * driver list box. This event enables certain features of the program like
   * the "Save Settings" button.
   *
   * @param item  a pointer to the driver item that has been selected or NULL
   */
  virtual void on_listWidgetDrivers_currentItemChanged
     ( QListWidgetItem * currentItem, QListWidgetItem * previous );


  /**
   * This slot method is called when the user changes the selection in the
   * printer list box. The reaction to this event is, that the driver list
   * box gets filled with appropriate drivers that might work with the selected
   * printer.
   *
   * @param item  a pointer to the printer item that has been selected or NULL
   */
  virtual void on_listWidgetPrinters_currentItemChanged
     ( QListWidgetItem * currentItem, QListWidgetItem * previous );


  /**
   * This slot method is called when the user clicks on the "Restart Autodetection"
   * button. On the other side, this method is also always called from within the
   * show function to initialize the printer detection process.
   */
  virtual void on_buttonRestart_clicked();


  /**
   * This slot method is called when the user clicks on the "Show all devices" check box.
   * From now on, calls to \c on_pushButton_restart_clicked will effect that all devices
   * are shown in the printers list box and not only those regarded as printers.
   *
   * @param state the checkbox's state after the user has clicked on it (checked or unchecked)
   */
  virtual void on_checkBoxShowAllDevices_stateChanged( int state );


  /**
   * This slot method is called when the user clicks on the push button
   * "More Driver Information". It opens a popup with details about the selected
   * driver.
   *
   * @see LinuxPrinterSetup::showPrinterDriverDetails
   */
  virtual void on_buttonDriverDetails_clicked();


  /**
   * This slot method sends a signal to the surrounding GUI that the manual
   * printer setup dialog shall be shown
   */
  virtual void on_buttonManualSetup_clicked();


private:


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
  friend void LinuxPrinterSetup::showPrinterDriverDetails( const QString & driverName , QWidget * parent);


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
  friend void LinuxPrinterSetup::installPrinter(  QWidget * widget,
                                                  const QString & printerName,
                                                  const QString & deviceUri,
                                                  const QString & ppdFilePath
                                               );


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
  friend void LinuxPrinterSetup::testPrintRequest(  QWidget * widget,
                                                    const QString & printerName
                                                 );


  /**
   * This function finds all connected USB printers and
   * shows them in the printer list box.
   *
   * @returns the number of found USB printers
   * @see     findParPortPrinters
   */
  int findUsbPrinters();

  /**Read the information in a subdir of the USB-devices dir
  * Returns true, if subdir contains printer infos
  */
  bool readUsbInfo(const QString & subDir);

  /**
   * This function finds all connected parallel port printers and
   * shows them in the printer list box.
   *
   * @returns the number of found parallel port printers
   * @see     findUsbPrinters
   */
  int findParPortPrinters();


  /**
   * ORable bitmasks for en-/disabling widgets.
   * You can use \c enableWidgets() with any combination of this widget flags.
   * E.g.:  \c enableWidgets(AllFlags^SaveSettingsButtonFlag); will enable
   *        all widgets of the automatic printer setup dialog except the
   *        "Save settings button".
   *
   * @see enableWidgets()
   */
  enum WidgetFlag {
    NoneFlag                  = 0,
    DriversFlag               = 1,
    PrintersFlag              = 2,
    SaveSettingsButtonFlag    = 4,
    HelpButtonFlag            = 8,
    RestartButtonFlag         = 16,
    ManualSetupButtonFlag     = 32,
    BackButtonFlag            = 64,
    AllFlags                  = 127
  };


  /**
   * This method en-/disables widgets according to the appropriate bitmask.
   *
   * @param wf          any combination of \c WidgetFlag flags
   * @see   WidgetFlag
   */
  void enableWidgets( int wf );


  /**
   * Some PPD files are known to contain unusable manufacturer names
   * that need to be renamed due to compatibility with the printer
   * driver database.
   * A famous example: \c Hewlett-Packard will be renamed to \c HP .
   *
   * @param manufacturer  the manufacturer name that needs to be renamed
   */
  void fixManufacturer( QString & manufacturer );


private:

  PrinterMap        m_printerMap;             ///< contains all printers found during autodetection
  PrinterSetupDb  * m_domDoc;                 ///< a pointer to the DOM object that handles the underlying DB
  QString           m_translationRecommended;

};

#endif

