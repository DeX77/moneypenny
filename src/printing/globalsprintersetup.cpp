/***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file contains the implementation of globally used types, functions
 *   and constants for the Linux Printer Setup project.
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   STATUS: File has been reviewed and commented
 *
 *   $Id: globalsprintersetup.cpp,v 1.3 2011-01-14 20:05:38 hbci Exp $
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

// App includes
#include "globalsprintersetup.h"

// MoneyPenny includes
#include "globals.h"
#include "hobit_string.h"

// Qt includes
#include <QString>
#include <QTextStream>
#include <QMessageBox>
#include <QListWidget>
#include <QCursor>
#include <QInputDialog>
#include <QFileInfo>
#include <QDir>

// Other includes
#include <stdlib.h>
using namespace std;

namespace LinuxPrinterSetup
{


/**
 * Constructs a printer device object without setting any members.
 */
PrinterDevice::PrinterDevice()
{
}


/**
 * Constructs a printer device object and sets the members.
 *
 * @param nativeManufacturer
 * @param nativeProduct
 * @param connectionType
 * @param drivers
 */
PrinterDevice::PrinterDevice( const QString & nativeManufacturer,
                              const QString & nativeProduct,
                              ConnectionType connectionType,
                              const QDomNodeList & drivers)
{
        this->nativeManufacturer  = nativeManufacturer;
        this->nativeProduct       = nativeProduct;
        this->connectionType      = connectionType;
        this->drivers             = drivers;
}


/**
 * Constructs a LPS notice object and sets the message to \a str .
 *
 * @param str
 */
LpsNotice::LpsNotice( const char * str ) : QString(str)
{
}


/**
 * Same as above, except for the type of the \a str parameter.
 * Constructs a LPS notice object and sets the message to \a str .
 *
 * @param str
 */
LpsNotice::LpsNotice( const QString & str ) : QString(str)
{
}


/**
 * Constructs a LPS error object and sets the message to \a str .
 *
 * @param str
 */
LpsError::LpsError( const char * str ) : QString(str)
{
}


/**
 * Same as above, error for the type of the \a str parameter.
 * Constructs a LPS notice object and sets the message to \a str .
 *
 * @param str
 */
LpsError::LpsError( const QString & str ) : QString(str)
{
}




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
void showPrinterDriverDetails( const QString & driverName, QWidget * parent )
{
  QString       fullName = driverName.section("/", 1, 1);
  QStringList sections = fullName.split(",");
  if(sections.count() == 0){
    QMessageBox::information(parent, QObject::tr("Information"), QObject::tr("No details available for this driver."));
    return;
  }
  QString xmlFileName = sections[0];
  FILE        * pipe        = NULL;
  QTextStream * reader      = NULL;

  try
  {
    ///
    /// This is how we present the XML driver information file:
    /// We pass parameters to a system call to \c xsltproc which does the transformation of the
    /// XML file into a displayable string.
    ///

    QString xmlFilePath = QString(LPS_DRIVER_DETAILS_DIR + xmlFileName + ".xml").remove(" (recommended)");
    QString params      = "--nonet --stringparam commentTitle " + QObject::tr("Comment");
    QString cmd         = "xsltproc "+params+" "+MP_DIR() + MONEYPENNY_DATA_DIR + LPS_DRIVER_STYLESHEET_PATH+" " + xmlFilePath;
    pipe                = popen(qPrintable(cmd), "r");

    if ( !pipe )
    {
      throw "Error during xsltproc execution! Could not execute process:<br />" + cmd;
    }

    reader          = new QTextStream(pipe, QIODevice::ReadOnly);

    QString details = reader->readAll().replace("&gt;", ">").replace("&lt;", "<");
    if(details.isEmpty())
     details =  QObject::tr("No information available.");

    QMessageBox::information(parent, QObject::tr("Driver Details"), details);
  }
  catch ( LpsError e )
  {
    //
    // NEVER change the order of these deletions/closings!!!
    //
    if (reader)
      delete reader;
    if (pipe)
      pclose(pipe);

    throw e;
  }

  //
  // NEVER change the order of these deletions/closings!!!
  //
  if (reader)
    delete reader;
  if (pipe)
    pclose(pipe);
}


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
 * @throws  LpsError    if something \c lpr command fails
 */
void testPrintRequest( QWidget * widget, const QString & printerName )
{
  //
  // Ask the user if a test print is requested
  //
  QString question = QObject::tr("Your printer has been configured successfully.") + "<br>" +
                     QObject::tr("Would you like to print a testpage?");

  int answer = QMessageBox::question(widget,
                                     LPS_APP_NAME,
                                     question,
                                     QMessageBox::Yes,
                                     QMessageBox::No,
                                     QMessageBox::NoButton);

  if ( answer == QMessageBox::Yes )
  {
    //
    // Build the path to test print file. Replace blanks and / in printerName with _
    //
    QString name = QString(printerName).replace(" ", "_");
    name = name.replace("/", "_");
    QString testFile = MP_DIR() + MONEYPENNY_DATA_DIR + "/" + LPS_TESTPAGE_PS_SRC;
  //  QString cmdLpr = "lpr -P "+ name +" -C "LPS_TESTPAGE_JOBNAME" "+testFile;
    QString cmdLpr = "lp -d "+ name + " " +testFile;
    qDebug("Printing Testpage: %s", qPrintable(cmdLpr));

    //
    // Now print the test page.
    // Due to system() call, the UI is blocked.
    //
    widget->setCursor(Qt::WaitCursor);
    if ( system( qPrintable(cmdLpr) ) != 0 )
    {
      widget->unsetCursor();
      throw LpsError("Error during lpr execution! Process failed: " + cmdLpr );
    }
    widget->unsetCursor();

    //
    // Open preview window
    //
    QDialog dlg;
    dlg.setFixedSize(LPS_TESTPAGE_WIDTH, LPS_TESTPAGE_HEIGHT);
    QPixmap pxm(MP_DIR() + MONEYPENNY_DATA_DIR + "/" + LPS_TESTPAGE_IMG_SRC);
    pxm = pxm.scaled(LPS_TESTPAGE_WIDTH, LPS_TESTPAGE_HEIGHT);

    QPalette palette;
    palette.setBrush(widget->backgroundRole(), QBrush(pxm));
    dlg.setPalette(palette);   
    dlg.setWindowTitle(QObject::tr("Testprint Preview"));
    dlg.exec();
  }
}


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
void installPrinter( QWidget * widget,
                     const QString & printerName,
                     const QString & deviceUri,
                     const QString & ppdFilePath )
{
  //
  /// Build install command string and replace the blanks in \c m_printername with
  /// underscores, otherwise \c lpadmin will fail.
  /// @todo find out on which systems \c sudo() is really needed (SuSE is gay)
  //
  QString name = QString(printerName).replace(" ", "_");
  name = name.replace("/", "_");
  QString cmdLpadmin = sudo(QString(LPS_LPADMIN_PATH) + "lpadmin -p " + name 
                              + " -E -v " + deviceUri + " -P " + ppdFilePath,
                            widget, 
                            QObject::tr("Please enter the system password to install your printer."));
  qDebug("Installing printer with command: %s", qPrintable(cmdLpadmin));

  //
  // Install the printer.
  // Due to system() call, the UI is blocked.
  //
  widget->setCursor(Qt::WaitCursor);
  if (  system(qPrintable(cmdLpadmin)) != 0 )
  {
    widget->unsetCursor();
    qDebug("GuiPrinterManualSetup::on_pushButton_saveSettings_clicked: Failed command: %s", qPrintable(cmdLpadmin));
    throw LpsError("Error during lpadmin execution! The following command returned an error: " + cmdLpadmin);
  }

  system("sync");

  widget->unsetCursor();
}

/**
 * Sets the OUT parameter in /etc/cups/cups-pdf.conf to \a newDir
 * @param newDir the new directory for pdf files generated by cups
 */
bool setPs2PdfTargetDir(const QString & newDirName){
 QDir newDir(newDirName);
 QString homeDir = getenv("HOME");
 QString cupsPdfConfName= QString (LPS_CUPS_PDF_CONF);
 if(!USE_LIVE_CD_FEATURES)
  return false; //Do not interfere with the OS config!
   //cupsPdfConfName=homeDir + "/monix501" + cupsPdfConfName; //try it on the remaster image. For Debugging only!!
 QFileInfo fileInfoCupsPdfConf(cupsPdfConfName);

 //Do we have a cups-pdf.conf?
 if(!fileInfoCupsPdfConf.exists()){
  qDebug("LinuxPrinterSetup::setPs2PdfTargetDir: No %s file available on system.", LPS_CUPS_PDF_CONF);
  return false;
 }

 //Can we create the target directory?
 if(!newDir.exists())
  if(!newDir.mkdir(newDir.absolutePath())){
   qDebug("LinuxPrinterSetup::setPs2PdfTargetDir:Could not create target directory: %s", qPrintable(newDir.absolutePath()));
   return false;
  }

 //Can we write to a file in target directory?
 QFile * testFile = new QFile(newDir.absolutePath()+ "/Cups_PDF_testfile.txt()");
 
 if(testFile->exists())
   testFile->remove();
  if(!testFile->open(QIODevice::ReadWrite)){
   qDebug("LinuxPrinterSetup::setPs2PdfTargetDir:Could not create testfile: %s", qPrintable(testFile->fileName()));
   return false;
  }
  QTextStream s(testFile);
  s <<"Testfile for cups-pdf configuration"; // Perform a real write operation
  if(!testFile->isOpen()){
   qDebug("LinuxPrinterSetup::setPs2PdfTargetDir:Could not write to testfile: %s", qPrintable(testFile->fileName()));
   return false;
  } 
  testFile->close();
  testFile->remove(); // we do not need it any longer.
 
 //Now we are shure, that we can use the desired directory. Set the OUT Parameter
  //first copy the config file to our home directory, so that we have write access
  QString homeFileName =  homeDir + "/" + fileInfoCupsPdfConf.fileName();
  QString copyCommand = "cp "+ cupsPdfConfName + " " + homeFileName;
  if(system(qPrintable(copyCommand))!= 0){
   qDebug("LinuxPrinterSetup::setPs2PdfTargetDir: Copy command failed: %s", qPrintable(copyCommand));
   return false;
  }
  system("sync"); //Make shure, that everything is in place before we continue.
  QString startBlock ="\\n[\\t\\s]*Out";
  QString endBlock = "\\n";
  QString newBlock = " " + newDirName;
  HoBIT_String::replaceTextBlock(startBlock, endBlock, newBlock, homeFileName);

  //Now copy modified cups-pdf.conf to /etc/cups 
  copyCommand = sudo("cp "+ homeFileName + " " + cupsPdfConfName,
                     NULL, 
                     QObject::tr("Please enter system password.")); //root rights required
  if(system(qPrintable(copyCommand)) != 0){ 
   qDebug("LinuxPrinterSetup::setPs2PdfTargetDir: Copy command failed: %s", qPrintable(copyCommand));
   return false;
  }

  system(qPrintable("rm " + homeFileName)); //we do not need it any more
  qDebug("LinuxPrinterSetup::setPs2PdfTargetDir: Cups pdf output directory has been set to: %s", qPrintable(newDirName));
  return true;
}


}; //end of namespace LinuxPrinterSetup
