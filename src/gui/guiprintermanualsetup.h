/***************************************************************************
 *   Copyright (C) 2005-2006 by Jens Lukowski, Walter Roth 2009
 *   jens.lukowski@fh-swf.de
 *
 *   This file contains the definition of the manual printersetup GUI
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   $Id: guiprintermanualsetup.h,v 1.2 2010-09-28 16:50:27 hbci Exp $
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
#ifndef GUIPRINTERMANUALSETUP_H
#define GUIPRINTERMANUALSETUP_H

// App includes
#include "ui_guiprintermanualsetupbase.h"
#include "../printing/globalsprintersetup.h"

// The namespace we use for the printersetup.
using namespace LinuxPrinterSetup;

// Forward declarations
class PrinterSetupDb;


/**
 * This class implements the underlying functionality of the manual
 * printer setup dialog. It can setup a printer and print test pages
 * on them.
 * Simply call the \c show() method and the rest is done by the class
 * itself.
 *
 * @note    There are some signal/slots, that are directly connected to
 *          the MoneyPenny GUI ( \c helpRequest , \c back , \c showSmallHelp ,
 *          \c on_pushButton_help_clicked ).
 * @author  Jens Lukowski
 */
class GuiPrinterManualSetup : public QWidget, public Ui::GuiPrinterManualSetupBase
{
  Q_OBJECT

public:


  /**
   * Constructs a new printer manual setup dialog.
   *
   * @see QWidget for parameter documentation
   */
  GuiPrinterManualSetup( QWidget* parent = 0 );


  /**
   * This destructor 'takes care' of manually allocated memory.
   */
  ~GuiPrinterManualSetup();


public slots:


  /**
   * Shows the widget and its child widgets.
   * This method might take a while until it returns because the XML
   * printer driver database is loaded.
   *
   *@throws LpsError  if the xml db could not been opened
   *@throws LpsError  if the content of the databes could not been set
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
  void helpRequest( const QString& page );


  /**
   * This signal is fired, whenever this GUI is shown. A small help/info
   * will be seen in the bottom left corner.
   *
   * @param page  the HTML filename of the help page to be displayed
   */
  void showSmallHelp( const QString& page );
  /**
   * Signal is fired after a printer has been installed. All QPrinters in
   * the application should be restarted.
   */
  void printerInstalled();
protected slots:


  /**
   * This slot is called if an item in listBox_drivers is highlighted.
   * It fills the comboBox_ports with the on this machine founded ports to
   * connect a printer.
   *
   * @param item  The highlighted item in listBox_drivers.
   */
  void on_listWidgetDrivers_itemActivated( QListWidgetItem * item );


  /**
   * This slot is called, when the user clicks on the "Save Settings"
   * button. Then the current printer configuration is saved and the user may
   * decide whether to print a test page or not.
   *
   * @throws LpsNotice  if no printername is given.
   * @throws LpsError   if the driver element is null.
   */
  void on_buttonSave_clicked();


  /**
   * This slot is called if an item in listBox_manufacturers is highlighted.
   * It fills the listBox_models with the associated models outof the database.
   *
   * @param item  The highlighted item in listBox_manufacturers.
   */
  void on_listWidgetManufacturers_itemActivated( QListWidgetItem * item );


  /**
   * This slot is called if an item in listBox_models is highlighted.
   * It fills the listBox_drivers with the associated drivers outof the database.
   *
   * @param item  The highlighted item in on_listBox_models.
   */
  void on_listWidgetModels_itemActivated( QListWidgetItem * item );


  /**
   * This slot calls the right help page for manual printer setup.
   */
  void on_buttonHelp_clicked();


  /**
   * This slot is called if comboBox_ports is activated.
   * It enables the pushButton_saveSettings.
   *
   * @param itemtext  The selected text in the comboBox_ports as QString.
   */
  void on_comboBoxPorts_activated( const QString & itemtext );


  /**
   * This slot is called when the user clicks on pushButton_driverDetails
   * ("More Driver Information"). It opens a popup with details about the
   * selected driver.
   */
  void on_buttonDriverDetails_clicked();


  /**
   * This slot emits signal back().
   */
  void on_buttonBack_clicked();


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
  friend void LinuxPrinterSetup::showPrinterDriverDetails( const QString & driverName, QWidget * parent );


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
   * Reads the ports using lpinfo and puts the results into comboBoxPorts.
   *
   * @throws LpsError if the ports could not be read
   */
  void readPorts();


  /**
   * This method en-/disables widgets according to the appropriate bitmask.
   *
   * @param wf          any combination of \c WidgetFlag flags
   * @see   WidgetFlag
   */
  void enableWidgets( int wf );


  /**
   * ORable bitmasks for en-/disabling widgets.
   * You can use \c enableWidgets() with any combination of this widget flags.
   * E.g.:  \c enableWidgets(AllFlags^SaveSettingsButtonFlag); will enable
   *        all widgets of the manual printer setup dialog except the
   *        "Save settings button".
   *
   * @see enableWidgets()
   */
  enum WidgetFlag {
    NoneFlag           = 0,
    ManufacturersFlag  = 1,
    ModelsFlag         = 2,
    DriversFlag        = 4,
    DetailsFlag        = 8,
    PortsFlag          = 16,
    SaveSettingsFlag   = 32,
    AllFlags           = 63
  };


  /**
   * A pointer to a QDom document for our database.
   */
  PrinterSetupDb * m_db;


  /**
   * The translation of the word "recommended". Set on show.
   */
  QString m_translationRecommended;
};

#endif

