/***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file contains the "Printer Driver 2 XML" implementation.
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   STATUS: File has been reviewed and commented
 *
 *   $Id: guiprinterdrivertoxml.cpp,v 1.2 2010-09-28 16:50:27 hbci Exp $
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
#include "guiprinterdrivertoxml.h"
#include <globals.h>
using namespace LinuxPrinterSetup;

// Qt-includes
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLCDNumber>
#include <QGroupBox>
#include <QTextEdit>
#include <QtXml/QDomDocument>
#include <QDir>
#include <QProcess>
#include <QApplication>
#include <QLabel>
#include <QDateTime>
#include <QTextStream>
#include <QIODevice>

// Includes for popen handling
#include <errno.h>


/**
 * Constructs a new printer driver 2 xml gui
 *
 * @param parent   a pointer to the parent widget
 * @param name     the name of the widget
 * @see QWidget
 */
GuiPrinterDriverToXml::GuiPrinterDriverToXml( QWidget * parent)
    : QWidget(parent), GuiPrinterDriverToXmlBase()
{
  qDebug("%s","Constructor GuiPrinterDriverToXml");
  setupUi(this);
  setObjectName("GuiPrinterDriverToXml");
  m_domDoc        = NULL;
  m_CupsModelDir  = LPS_CUPS_MODEL_DIR_DEFAULT;

  enableWidgets(NoneFlag ^ BuildFlag);
}


/**
 * This destructor takes care of manually allocated memory.
 */
GuiPrinterDriverToXml::~GuiPrinterDriverToXml()
{
  qDebug("Destructor GuiPrinterDriverToXml");

  if ( m_domDoc )
    delete m_domDoc;

}


/**
 * This slot function is called whenever this dialog is shown.
 * In initializes all widgets located in this dialog with sensable state.
 */
void GuiPrinterDriverToXml::showEvent(QShowEvent *)
{
  QWidget::show();
  setWindowTitle("Cups Driver to XML");
  qApp->processEvents();

  editSavePath->setText(MP_DIR() + MONEYPENNY_DATA_DIR + LPS_DB_FILE);
  editCupsModelDir->setText(m_CupsModelDir);

  logMessage("System startup ok.");
}


/**
 * This slot function is called whenever the widget recieves
 * a close signal.Eventually running build processes are aborted!
 */
void GuiPrinterDriverToXml::closeEvent( QCloseEvent * e )
{
  on_buttonAbort_clicked();
  e->accept();
}


/**
 * This slot method is called, when the user clicks on the "Build" button.
 * It initiates the parsing of all PPD files located under the previously set
 * CUPS model directory.
 */
void GuiPrinterDriverToXml::on_buttonBuild_clicked()
{
  enableWidgets(NoneFlag ^ ProgressFlag);

  m_buildCanceled = false;

  m_CupsModelDir = editCupsModelDir->text();

  // make sure it ends with a "/"
  if ( m_CupsModelDir.right(1) != "/" )
    m_CupsModelDir += "/";

  logMessage(tr("Now building XML database from: %1 ...").arg(m_CupsModelDir));

  // until now, no drivers where found, so adjust the LCDs
  lcdNumberManufacturers->display(0);
  lcdNumberModels->display(0);
  lcdNumberDrivers->display(0);

  // prevent the gui from hanging up
  qApp->processEvents();

  // Constrcut a "clean" DOMDocument with a root element
  if ( m_domDoc )
    delete m_domDoc;

  m_domDoc                = new PrinterSetupDb(LPS_DB_DOCUMENT_NAME);
  QDomElement rootElement = m_domDoc->createElement(LPS_DB_DOCUMENT_NAME);
  rootElement.setAttribute("path", m_CupsModelDir);
  m_domDoc->appendChild(rootElement);

  try
  {
    traverseDir(m_CupsModelDir);
    logMessage("Build process complete! You can now save the db to a xml file.");
    enableWidgets(AllFlag ^ ProgressFlag);
  }
  catch(LpsError msg)
  {
    logMessage("Build process exited with error: " + msg, true);
    enableWidgets(NoneFlag ^ BuildFlag);
    return;
  }
}


/**
 * This slot method pops up a file dialog for setting the save path of the XML db.
 * If the file you select or enter does not exist, it will be created on-the-fly.
 */
void GuiPrinterDriverToXml::on_buttonBrowseSavePath_clicked()
{
  QString dir = editSavePath->text();

  if ( dir.length() <= 0 )
    dir = MONEYPENNY_DATA_DIR;

  QFileDialog* fd = new QFileDialog( this );

  fd->setDirectory(dir);
  fd->setFileMode( QFileDialog::AnyFile );

  if ( fd->exec() == QDialog::Accepted )
    editSavePath->setText( fd->selectedFiles()[0] );
}

/**
 * This slot method pops up a file dialog for setting the CUPS model directory.
 * You should select a directory that contains folders of printer manufacturers (e.g. Alps, HP, ...).
 * One of the most common folders on Debian/Ubuntu is: /usr/share/cups/model/
 * Until we haven't found a mechanism to determine a this path automatically,
 * you must find it yourself if you have other distributions.
 *
 * @todo  automatically determine the path to the CUPS model dir
 */
void GuiPrinterDriverToXml::on_buttonBrowseCupsDir_clicked()
{
  QString dir = editCupsModelDir->text();

  if ( dir.length() <= 0 )
    dir = LPS_CUPS_MODEL_DIR_DEFAULT;

  QFileDialog* fd = new QFileDialog( this );

  fd->setDirectory(dir);
  fd->setFileMode( QFileDialog::Directory );

  if ( fd->exec() == QDialog::Accepted )
    editCupsModelDir->setText( fd->selectedFiles()[0] );
}

/**
 * This slot method is called, when the user clicks on the abort button to
 * abort the building of the XML db and it does what says.
 */
void GuiPrinterDriverToXml::on_buttonAbort_clicked()
{
  m_buildCanceled = true;
  enableWidgets(NoneFlag ^ BuildFlag);
}

/**
 * This slot method is called, when the user clicks on the back button to
 * return to the main program. (This is MoneyPenny specific)
 */
void GuiPrinterDriverToXml::on_buttonBack_clicked()
{
  on_buttonAbort_clicked();
  emit back();
}

/**
 * This slot method is called when the user clicks on the "Save" button.
 * It saves the DOM datastructure to a XML file previously selected by the user.
 * If the file specified does not exists, it will be created on-the-fly.
 */
void GuiPrinterDriverToXml::on_buttonSave_clicked()
{
  //save result file
  QFile file( editSavePath->text() );

  if ( file.open( QIODevice::WriteOnly ) )
  {
    QTextStream stream( &file );
    m_domDoc->save( stream, 2);
    file.close();
    logMessage( "Saving complete!" );
  }
  else
  {
    logMessage("Could not open file for writing: " + editSavePath->text(), true);
  }
}


/**
 * This method is called when at some state some program part wants to report something.
 * It appends the message to the log widget.
 *
 * @param msg     the message to log
 * @param isError whether the message is an error or not
 */
void GuiPrinterDriverToXml::logMessage( const QString & msg,
                                        bool isError
                                      )
{
  // prepend a nice time information infront of every log entry
  QString str = QTime::currentTime().toString("[hh:mm:ss] ") + msg;

  QString color = "darkgreen";

  // change the format, depending on if the message is an error message or not
  if ( isError )
    color = "red";

  textEditLog->append( QString("<font color=\"%1\">%2</font>").arg(color, str) );
}


/**
 * Runs recursively through all subdirectories in "dir" and parses
 * every file it founds.
 *
 * @param   dir         the entry point (CUPS model dir) for recursion
 * @throws  LpsError    when an error occured or the user hitted the abort-button (this will end the recursion)
 * @see     parseFile()
 */
void GuiPrinterDriverToXml::traverseDir( QString  dir )
{
  // Due to some speed issues when stopping the build process, this string is used
  // at 3 different locations in this method. Therefore it is placed here once.
  QString buildCancelledString = tr("Build manually cancelled");

  // if canceled, abort this process by throwing a message as exception
  if ( m_buildCanceled )
    throw buildCancelledString;

  QDir currentDir( dir );

  if ( !currentDir.exists() || !currentDir.isReadable() )
  {
    throw LpsError(QString("The following directory could not be found or is not readable: ") + currentDir.dirName());
  }

  //======================================================================
  // Get all subdirs
  //======================================================================

  currentDir.setFilter( QDir::Dirs );
  QStringList subdirs = currentDir.entryList();

  for ( QStringList::Iterator subDirIt = subdirs.begin();
        subDirIt != subdirs.end();
        ++subDirIt
      )
  {
    if ( m_buildCanceled )
      throw LpsError(buildCancelledString);

      QString name = *subDirIt;

      if ( name!="." && name!=".." )
      {
        QString absPath = currentDir.absolutePath() + "/" + name;
        traverseDir(absPath);
      }
  }

  //======================================================================
  // Get all files for "dir"
  //======================================================================

  currentDir.setFilter( QDir::Files );
  QStringList files = currentDir.entryList();
  QString absPath;
  for ( QStringList::Iterator fileIt = files.begin();
        fileIt != files.end();
        ++fileIt )
  {
    if (m_buildCanceled)
      throw LpsError(buildCancelledString);

      QString name = *fileIt;

      if ( name!="." && name!=".." )
      {
        absPath = currentDir.absolutePath() + "/" + name;
        try
        {
          parseFile(absPath);
        }
        catch( LpsError e )
        {
          logMessage(e, true);
        }

      }
  }
}

/**
 * Trys to figure out if it is a Postscript Printer Description (PPD).
 * If it is a PPD-file, it is decompressed and read for further
 * processing with addDriver().
 *
 * @param   filePath
 * @throws  LpsError    if an error occured (wrong PPD-file-signature, popen failed, not all needed information found)
 * @see     addDriver()
 */
void GuiPrinterDriverToXml::parseFile( QString  filePath )
{
  //======================================================================
  // Make sure the file ends with ".ppd" or ".ppd.gz"
  //======================================================================

  if ( (filePath.indexOf(".ppd", Qt::CaseInsensitive) < 0) 
    && (filePath.indexOf(".ppd.gz", Qt::CaseInsensitive) < 0) )
  {
    logMessage("The following file has no valid PPD-file-signature: " + filePath, true);
    return;
  }

  //======================================================================
  // Read in the file (if compressed, decompress it first)
  //======================================================================

  QTextStream * reader = NULL;
  FILE        * pipe   = NULL; // used when opening with decompressing
  QFile       * file   = NULL; // used when opening without previous decompression

  if ( filePath.contains(".ppd.gz", Qt::CaseInsensitive) )
  {
    // The command to be executed for decompressing the PPD file
    QString gzcmd = "gunzip -cdf " + filePath;
    // gunzip exits with 0 if everything was ok, with 1 if an error
    // occured and with 2 if a warning occured.
    //
    // WE WON'T TOLERATE ANYTHING EXCEPT 0 (=OK) !!!
    //
    pipe = popen(qPrintable(gzcmd), "r");

    if ( !pipe )
        throw LpsError(
                        QString("Error during gunzip execution! Could not execute process: <br>") +
                        gzcmd +
                        QString("Errormessage: ") +
                        (QString) strerror(errno)
                      );

    reader = new QTextStream(pipe, QIODevice::ReadOnly);
  }
  else
  {
    file = new QFile(filePath);
    if ( !file->open(QIODevice::ReadOnly) )
    {
      throw LpsError(
                      QString("Could not open the file for reading: <br>") +
                      filePath +
                      " <br>" +
                      "Errormessage: " +
                      (QString)strerror(errno)
                    );
    }
    reader = new QTextStream(file);
  }
  //======================================================================
  // Read in and parse the extracted information from pipe
  //======================================================================

  // Run through every line of the file until we have all needed information
  // (this is the tricky part, that costed much of brain corniness :))

  PpdInfo info;

  bool        done    = false;
  QString     line    = QString::null;

  while ( !done && !reader->atEnd() )
  {
    line = reader->readLine();

    line = line.simplified().remove("\"");

    if ( line.startsWith("*Product:") )
      info.product = line.remove("*Product:").remove( " (" ).remove(")" ).trimmed();

    else if ( line.startsWith("*NickName:") )
    {
      info.nickName = line.remove("*NickName:");
      info.nickName = info.nickName.remove("\"").trimmed();
    }

    else if ( line.startsWith("*ModelName:") )
      info.modelName = line.remove("*ModelName: ").trimmed();

    else if ( line.startsWith("*Manufacturer:") )
      info.manufacturer = line.remove("*Manufacturer:").trimmed();

    else if ( line.startsWith("*LanguageVersion:") )
      info.languageVersion = line.remove("*LanguageVersion:").trimmed();

    if (  !info.product.isEmpty() && !info.nickName.isEmpty() &&
         !info.modelName.isEmpty() && !info.languageVersion.isEmpty() &&
         !info.manufacturer.isEmpty() )
      done = true;
  }

  //
  // Never change the order of the following deletions/closings!!!
  //

  // delete the stream that uses the pipe FIRST!!!
  if ( reader )
    delete reader;

  // SECONDLY, delete the pipe itself (otherwise you will get a segmentation fault)
  if ( pipe )
    pclose(pipe);
  if ( file )
    delete file;

  if (!done)
    throw LpsError( tr("PPD-file: %1 Not all neccessary informations were catched!").arg(qPrintable(filePath)) );

  QString ppdFilePath = filePath;
  info.ppdFilePath    = ppdFilePath.remove(m_CupsModelDir);

  addDriver(info);
}

/**
 * Adds a driver to the internal XML DOM database.
 *
 * @param drv the driver structure
 */
void GuiPrinterDriverToXml::addDriver( const PpdInfo & drv )
{
  QDomElement root = m_domDoc->documentElement();

  QDomElement manufacturerElement = m_domDoc->getManufacturer(drv.manufacturer);

  if ( manufacturerElement.isNull() )
  {
    manufacturerElement = m_domDoc->createElement("Manufacturer");
    manufacturerElement.setAttribute("name", drv.manufacturer);
    root.appendChild(manufacturerElement);

    // Update the manufacturer LCD
    lcdNumberManufacturers->display(lcdNumberManufacturers->intValue() + 1);
    qApp->processEvents();
  }

  QDomElement modelElement = m_domDoc->getModel(drv.manufacturer, drv.modelName, drv.product);

  if ( modelElement.isNull() )
  {
    modelElement = m_domDoc->createElement("Model");
    modelElement.setAttribute("name", drv.modelName);
    modelElement.setAttribute("product", drv.product);
    manufacturerElement.appendChild(modelElement);

    // Update the model LCD
    lcdNumberModels->display(lcdNumberModels->intValue() + 1);
    qApp->processEvents();
  }

  QDomElement driverEle = m_domDoc->getDriverByModel(drv.manufacturer, drv.modelName, drv.nickName);

  if ( driverEle.isNull() )
  {
    driverEle = m_domDoc->createElement("Driver");
    driverEle.setAttribute("nickName", drv.nickName);
    driverEle.setAttribute("languageVersion", drv.languageVersion);
    driverEle.setAttribute("ppdFilePath",  drv.ppdFilePath);
    modelElement.appendChild(driverEle);

    // Update the driver LCD
    lcdNumberDrivers->display(lcdNumberDrivers->intValue() + 1);
    qApp->processEvents();
  }

}

/**
 * This method enables widgets according to the appropriate bitmask.
 *
 * @param wf see WidgetFlag for orable widget flags
 */
void GuiPrinterDriverToXml::enableWidgets( int wf )
{
  groupBox_build->setEnabled(     wf & BuildFlag    );
  groupBox_progress->setEnabled(  wf & ProgressFlag );
  groupBox_save->setEnabled(      wf & SaveFlag     );
}


