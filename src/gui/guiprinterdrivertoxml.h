/***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file contains the "Printer Driver 2 XML" definition.
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   STATUS: File has been reviewed and commented
 *
 *   $Id: guiprinterdrivertoxml.h,v 1.2 2010-09-28 16:50:27 hbci Exp $
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

#ifndef GUIPRINTERDRIVER2XML_H
#define GUIPRINTERDRIVER2XML_H

// App includes
#include "ui_guiprinterdrivertoxmlbase.h"
#include "globals.h"
#include "../printing/globalsprintersetup.h"
#include "../printing/printersetupdb.h"
using namespace LinuxPrinterSetup;

// Forward declarations
class QDomDocument;
class QDomElement;
class QDir;

/**
 * This class implements the underlying functionality of the printer driver
 * to xml dialog. It provides means for parsing of PPD files saving the
 * collected information to a file.
 *
 * @author  Konrad Wilhelm Kleine, ported by Walter Roth
 */

class GuiPrinterDriverToXml :public QWidget, public Ui::GuiPrinterDriverToXmlBase
{

Q_OBJECT

public:


  /**
   * Constructs a new printer driver 2 xml gui
   *
   * @param parent   a pointer to the parent widget
   * @param name     the name of the widget
   * @see QWidget
   */
  GuiPrinterDriverToXml( QWidget * parent = 0);


  /**
   * This destructor takes care of manually allocated memory.
   */
  ~GuiPrinterDriverToXml();


public slots:


  /**
   * This slot method is called when this dialog is shown.
   * In initializes all widgets located in this dialog with sensable state (e.g. Saving is diabled).
   */
  void showEvent(QShowEvent *);


  /**
   * This slot method is called when the widget recieves
   * a close signal. Eventually running build processes are aborted!
   */
  void closeEvent( QCloseEvent * e );


protected slots:


  /**
   * This slot method is called, when the user clicks on the "Build" button.
   * It initiates the parsing of all PPD files located under the previously set
   * CUPS model directory.
   */
  void on_buttonBuild_clicked();


  /**
   * This slot method pops up a file dialog for setting the save path of the XML db.
   * If the file you select or enter does not exist, it will be created on-the-fly.
   */
  void on_buttonBrowseSavePath_clicked();


  /**
   * This slot method pops up a file dialog for setting the CUPS model directory.
   * You should select a directory that contains folders of printer manufacturers (e.g. Alps, HP, ...).
   * One of the most common folders on Debian/Ubuntu is: /usr/share/cups/model/
   * Until we haven't found a mechanism to determine a this path automatically,
   * you must find it yourself if you have other distributions.
   *
   * @todo  automatically determine the path to the CUPS model dir
   */
  void on_buttonBrowseCupsDir_clicked();


  /**
   * This slot method is called, when the user clicks on the abort button to
   * abort the building of the XML db and it does what says.
   */
  void on_buttonAbort_clicked();


  /**
   * This slot method is called, when the user clicks on the back button to
   * return to the main program. (This is MoneyPenny specific)
   */
  void on_buttonBack_clicked();


  /**
   * This slot method is called when the user clicks on the "Save" button.
   * It saves the DOM datastructure to a XML file previously selected by the user.
   * If the file specified does not exists, it will be created on-the-fly.
   */
  void on_buttonSave_clicked();


signals:

    /**
     * This signal is fired, when the user clicks on the "Back" button.
     * It is handled by the sourrounding UI and shall bring the user back to
     * the page that she/he visited before.
     */
    void back();


private:


  /**
   * This method is called when at some state some program part wants to report something.
   * It appends the message to the log widget.
   *
   * @param msg     the message to log
   * @param isError whether the message is an error or not
   */
  void logMessage( const QString & msg, bool isError = false );


  /**
   * Runs recursively through all subdirectories in "dir" and parses
   * every file it founds.
   *
   * @param   dir         the entry point (CUPS model dir) for recursion
   * @throws  LpsError    when an error occured or the user hitted the abort-button (this will end the recursion)
   * @see     parseFile()
   */
  void traverseDir( QString  dir );


  /**
   * Trys to figure out if it is a Postscript Printer Description (PPD).
   * If it is a PPD-file, it is decompressed and read for further
   * processing with addDriver().
   *
   * @param   filePath
   * @throws  LpsError    if an error occured (wrong PPD-file-signature, popen failed, not all needed information found)
   * @see     addDriver()
   */
  void parseFile( QString  filePath );


  /**
   * Adds a driver to the internal XML DOM database.
   *
   * @param drv the driver structure
   */
  void addDriver( const PpdInfo & drv );


private:


  /**
   * This method enables widgets according to the appropriate bitmask.
   *
   * @param wf see WidgetFlag for orable widget flags
   */
  void enableWidgets( int wf );


private:

  /**
   * ORable bitmasks for en-/disabling widgets.
   * You can use \c enableWidgets() with any combination of this widget flags.
   *  E.g.:  \c enableWidgets(AllFlags^SaveSettingsButtonFlag); will enable
   *        all widgets of the automatic printer setup dialog except the
   *        "Save settings button".
   *
   * @see enableWidgets()
   */
  enum WidgetFlag {
    NoneFlag       = 0,
    ProgressFlag   = 1,
    BuildFlag      = 2,
    SaveFlag       = 4,
    AllFlag        = 7
  };

  bool m_buildCanceled;       ///< Useful for influencing the build process

  QString m_CupsModelDir;     ///< The directory containing the PPD files. This may have subdirectories.

  PrinterSetupDb *m_domDoc;   ///< The DOM document used for XML db building/saving
};

#endif

