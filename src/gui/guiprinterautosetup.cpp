/***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file contains the automatic printer setup implementation.
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   STATUS: File has been reviewed and commented
 *
 *   $Id: guiprinterautosetup.cpp,v 1.3 2011-01-14 20:05:38 hbci Exp $
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
#include "guiprinterautosetup.h"
#include "../printing/printersetupdb.h"
#include "globals.h"
#include "../printing/globalsprintersetup.h"
using namespace LinuxPrinterSetup;

// Qt includes
#include <QFile>
#include <QDir>
#include <QListWidget>
#include <QTextStream>
#include <QMessageBox>
#include <QRegExp>
#include <QtXml/QDomDocument>
#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QCursor>
#include <QDialog>
#include <QPixmap>

// Other includes
#include <stdlib.h>
using namespace std;


/**
 * Constructs a new printer auto setup dialog
 *
 * @see QWidget for parameter documentation
 */
GuiPrinterAutoSetup::GuiPrinterAutoSetup( QWidget * parent )
    : QWidget(parent), GuiPrinterAutoSetupBase()
{
  qDebug("Constructor GuiPrinterAutoSetup");
  setupUi(this);
  setObjectName("GuiPrinterAutoSetup");
  m_domDoc                  = new PrinterSetupDb( LPS_DB_DOCUMENT_NAME );
  m_translationRecommended  = tr("recommended", "This will be a appended to any recommended printer");

  connect(buttonBack, SIGNAL(clicked()), this, SIGNAL(back()));
}


/**
 * This destructor 'takes care' of manually allocated memory
 */
GuiPrinterAutoSetup::~GuiPrinterAutoSetup()
{
  if ( m_domDoc )
    delete m_domDoc;

  qDebug("Destructor GuiPrinterAutoSetup finished");
}


/**
 * Shows the widget and its child widgets.
 * This method might take a while until it returns because the XML
 * printer driver database is loaded and the printer detection is
 * performed.
 */
void GuiPrinterAutoSetup::showEvent(QShowEvent * se)
{
  QWidget::showEvent(se);

  emit showSmallHelp("smallPrinterAutoSetup.html");
  QString errMsg;

  labelStatus->setPalette(PALETTE_YELLOW());
  labelStatus->setText(tr("Loading database..."));

  setCursor(Qt::WaitCursor);

  setWindowTitle(tr("Automatic Printer Setup"));
  qApp->processEvents();

  listWidgetPrinters->clear();
  listWidgetDrivers->clear();
  QString fileName = MP_DIR() + MONEYPENNY_DATA_DIR + LPS_DB_FILE;
  qDebug("Now loading: %s", qPrintable(fileName));

  try
  {
    // Load the XML database
    QFile file( fileName );

    if ( !file.exists() || !file.open( QIODevice::ReadOnly ) )
      throw QString("Could not open file for reading:") + " " + fileName;

    QString errMsg;
    int     errLine;
    int     errCol;
    bool    ok      = m_domDoc->setContent(&file, false, &errMsg, &errLine, &errCol);

    file.close();

    if ( !ok )
      throw LpsError(QString("An internal error in the XML file %1 at line %2 in column %3 occured."
                             "<br />The error message from the system was: %4").arg(
                       LPS_DB_FILE, QString::number(errLine),QString::number(errCol),errMsg));

    // Initiate the autodetection process
    on_buttonRestart_clicked();
  }
  catch ( LpsError e )
  {
    labelStatus->setPalette( PALETTE_RED() );
    labelStatus->setText(e);
    enableWidgets(NoneFlag ^ HelpButtonFlag ^ BackButtonFlag);
  }
  unsetCursor();
}


/**
 * This slot method is called, when the user clicks on the "Save Settings"
 * button. The current printer configuration is 'saved' and the user
 * is presented with the opportunity to print a test page.
 *
 * @see LinuxPrinterSetup::installPrinter
 * @see LinuxPrinterSetup::testPrintRequest
 */
void GuiPrinterAutoSetup::on_buttonSave_clicked()
{
  try
  {
    //----------------------------------------------------------------------
    // Try to get all information about the selected printer.
    //----------------------------------------------------------------------
    QString       manufacturer  = listWidgetPrinters->currentItem()->text().section(" / ", 0, 0);
    QString       product       = listWidgetPrinters->currentItem()->text().section(" / ", 1, 1);
    if(listWidgetDrivers->currentItem() == NULL)
    {
      if(listWidgetDrivers->count() == 1) //only one driver
      {
        listWidgetDrivers->setCurrentRow(0); //select it
      }
      else 
      {
        QMessageBox::information(this, tr("Information"), tr("Please select a driver!"));
        return;
      }
    }
    QString       driver        = listWidgetDrivers->currentItem()->text().replace(m_translationRecommended, "recommended");
    QString       printer       = listWidgetPrinters->currentItem()->text();
    QDomNodeList  drivers       = m_printerMap[printer].drivers;
    QDomElement   driverElement;

    uint i = 0;
    while ( driverElement.isNull()   &&   (i < drivers.length()) )
    {
      QDomElement element = drivers.item(i).toElement();

      if ( element.attribute("nickName") == driver ) //we have found it
        driverElement = element;

      i++;
    }

    if ( driverElement.isNull() )
      throw LpsError(QString("The driver element was NULL."));

    QString nickName        = driverElement.attribute("nickName");
    QString pcFileName      = driverElement.attribute("pcFileName");
    QString languageVersion = driverElement.attribute("languageVersion");
    QString ppdFilePath     =m_domDoc->documentElement().attribute("path");
    ppdFilePath     += driverElement.attribute("ppdFilePath");
    QString deviceUri;

    if ( m_printerMap[ listWidgetPrinters->currentItem()->text() ].connectionType == ParPortConnection )
      deviceUri = "parallel:/dev/lp0";
    else
      deviceUri = "usb:/dev/usb/lp0";

    //----------------------------------------------------------------------
    // Next: Setup the printer using a CUPS CLI-command
    //----------------------------------------------------------------------
    QDomElement modelEle = m_domDoc->getModel(manufacturer, "", product);

    QString printerName = "";

    if ( !modelEle.isNull() )
      printerName = modelEle.attribute("name");
    else
      printerName = LPS_PRINTER_NAME;

    installPrinter(this, printerName, deviceUri, ppdFilePath);

    //----------------------------------------------------------------------
    // Ask the user whether a testpage shall be printed
    //----------------------------------------------------------------------
    testPrintRequest(this, printerName);

    labelStatus->setText("<p>" + tr("Your printer has been set up successfully.") + "</p>");
    labelStatus->setPalette(PALETTE_GREEN());

  }
  catch ( LpsNotice e )
  {
    labelStatus->setPalette(PALETTE_YELLOW());
    labelStatus->setText(e);
    qDebug("%s", qPrintable(e));
  }
  catch ( LpsError e )
  {
    labelStatus->setPalette(PALETTE_RED());
    labelStatus->setText(e);
    qDebug("%s", qPrintable(e));
  }
  unsetCursor();
  emit printerInstalled();
}


/**
 * This slot method sends a signal to the surrounding GUI that the help
 * page for automatic printer setup shall be displayed.
 */
void GuiPrinterAutoSetup::on_buttonHelp_clicked()
{
  emit helpRequest("printerAutoSetup.html");
}


/**
 * This slot method is called when the user changes the selection in the
 * driver list box. This event enables certain features of the program like
 * the "Save Settings" button.
 *
 * @param item  a pointer to the driver item that has been selected or NULL
 */
void GuiPrinterAutoSetup::on_listWidgetDrivers_currentItemChanged
     ( QListWidgetItem * currentItem, QListWidgetItem *  )
{
  QListWidgetItem * printerItem = listWidgetPrinters->currentItem();

  if ( !currentItem || !printerItem )
    return;

  labelStatus->setText(  tr("Driver:") + " <b>" + currentItem->text()+"</b><br>" +
                          tr("Printer:")+ " <b>" + printerItem->text()+"</b><br>" +
                          tr("Click \"Save Settings\", if you want to use these settings.")
                       );

  labelStatus->setPalette(PALETTE_YELLOW());
}


/**
 * This slot method is called when the user changes the selection in the
 * printer list box. The reaction to this event is, that the driver list
 * box gets filled with appropriate drivers that might work with the selected
 * printer.
 *
 * @param item  a pointer to the printer item that has been selected or NULL
 */
void GuiPrinterAutoSetup::on_listWidgetPrinters_currentItemChanged
     ( QListWidgetItem * currentItem, QListWidgetItem *  )
{
  if( !currentItem ) // clicking into nirvana
    return;

  enableWidgets(AllFlags^DriversFlag);
  listWidgetDrivers->clear();

  QString manufacturer  = currentItem->text().section(" / ", 0, 0);
  QString product       = currentItem->text().section(" / ", 1, 1);

  qDebug("GuiPrinterAutoSetup::on_listWidgetPrinters_currentItemChanged:Searching drivers for: %s", qPrintable(product));


  //
  // Why did I use a reference below? -> To avoid doing the same thing more than one.
  //
  // If the user already clicked on the printer that is currently focused in the printer
  // list box, we've already queried the database to find associated printer drivers.
  // If this is the first time, the user clicked on the printer in the printer driver
  // list box, we have to query the database for the first time for the selected printer.
  // And we have to make sure that the returned information is stored for upcoming
  // selections of the same printer.
  //
  QDomNodeList & drivers = m_printerMap[ currentItem->text() ].drivers;

  if ( drivers.length() <= 0 )
    drivers = m_domDoc->getDrivers(product); // matches products OR models

  for ( uint i=0; i<drivers.length(); i++ )
  {
    QDomElement driverElement = drivers.item(i).toElement();

    if ( !driverElement.isNull() )
    {
      //
      // Insert driver into driver list box and replace the "recommended" string
      // with a localized translation.
      //
      QString driverName = driverElement.attribute("nickName");

      qDebug ("GuiPrinterAutoSetup::on_listWidgetPrinters_highlighted: Found driver %s", qPrintable(driverName));

      listWidgetDrivers->addItem(driverName.replace("recommended", m_translationRecommended));

      //
      // If the last inserted item contained the "recommended" string, pre-select it.
      //
      uint count = listWidgetDrivers->count();
      if ( listWidgetDrivers->item(count - 1)->text().contains(m_translationRecommended) )
      {
        listWidgetDrivers->setCurrentRow(count - 1);
        enableWidgets(AllFlags);
      }
    }
  }

  uint count = drivers.length();
  if ( count <= 0 )
  {
    labelStatus->setPalette( PALETTE_YELLOW() );
    labelStatus->setText(  tr("No appropriate driver found for this device:") +
                            " <b>" + currentItem->text() + "</b> <br>" +
                            tr("Please choose a different device!")
                         );
    enableWidgets(AllFlags ^ SaveSettingsButtonFlag ^ DriversFlag);
  }
  else 
  {
    labelStatus->setPalette( PALETTE_GREEN() );
    labelStatus->setText(  tr("Please select a driver from the list.") );
    enableWidgets(AllFlags);
  }


  if( !listWidgetDrivers->currentItem() && count > 0 )
  {
    listWidgetDrivers->setCurrentItem(0);
    enableWidgets(AllFlags);
  }
}


/**
 * This slot method is called when the user clicks on the "Restart Autodetection"
 * button. On the other side, this method is also always called from within the
 * show function to initialize the printer detection process.
 */
void GuiPrinterAutoSetup::on_buttonRestart_clicked()
{
  enableWidgets(NoneFlag);

  listWidgetPrinters->clear();
  listWidgetDrivers->clear();
  m_printerMap.clear();
  int nPrinters = 0;

  try
  {
    //
    // Never change the order of this function calls.
    // If you connected the same printer via usb AND parallel port, it's safer
    // to use the usb rather than the parallel port. The parallel port printer
    // is shown in the list, although it might have been disconnected unlike
    // the usb printer which will disappear from the list, the next time you
    // enter a new autodetection cycle.
    //
    nPrinters += findParPortPrinters();
    nPrinters += findUsbPrinters();
  }
  catch ( LpsError e )
  {
    labelStatus->setPalette(PALETTE_RED());
    labelStatus->setText(e);
    enableWidgets(AllFlags ^ DriversFlag ^ SaveSettingsButtonFlag);
    return;
  }

  if ( nPrinters > 0 )
  {
    PrinterMap::Iterator it;

    for ( it = m_printerMap.begin(); it != m_printerMap.end(); ++it )
    {
      listWidgetPrinters->addItem( it.key() );
      qDebug("%s", qPrintable(it.key()) );
    }

    //
    // If we found more than one printer/device, select the first from the list.
    //
    enableWidgets(AllFlags  ^ DriversFlag ^ SaveSettingsButtonFlag);
    listWidgetPrinters->setCurrentItem(0);
  }
  else
  {
    labelStatus->setPalette(PALETTE_YELLOW());

    labelStatus->setText(  "<p>"+
                            tr("No printers found! Make sure your printer is connected and switched on.") + "<br>" +
                            tr("If you use a parallel port printer, connect it to your computer before booting.") +
                            "<br>" +
                            tr("You can also try the <i>Show all devices</i>-checkbox. ") +
                            "</p>"
                         );

    enableWidgets(AllFlags ^ PrintersFlag ^ DriversFlag ^ SaveSettingsButtonFlag);
  }
}


/**
 * This slot method is called when the user clicks on the "Show all devices" check box.
 * From now on, calls to \c on_pushButton_restart_clicked will effect that all devices
 * are shown in the printers list box and not only those regarded as printers.
 *
 * @param state the checkbox's state after the user has clicked on it (checked or unchecked)
 */
void GuiPrinterAutoSetup::on_checkBoxShowAllDevices_stateChanged( int state )
{
  if ( state == Qt::Checked )
  {
    QMessageBox::information( this,
                              LPS_APP_NAME,
                              tr("Please find out yourself, which of the devices in the list could be your printer.")  +
                              "<br>" +
                              tr("You should then switch to Manual Printer Setup.")
                            );
  }

  on_buttonRestart_clicked();
}


/**
 * This slot method is called when the user clicks on the push button
 * "More Driver Information". It opens a popup with details about the selected
 * driver.
 *
 * @see LinuxPrinterSetup::showPrinterDriverDetails
 */
void GuiPrinterAutoSetup::on_buttonDriverDetails_clicked()
{
  if(!listWidgetDrivers->currentItem())
   return;

  try
  {
    showPrinterDriverDetails(  listWidgetDrivers->currentItem()->text().replace(m_translationRecommended, "recommended"), this  );
  }
  catch (LpsError e)
  {
    labelStatus->setText(e);
    labelStatus->setPalette(PALETTE_YELLOW());
  }
}


/**
 * This slot method sends a signal to the surrounding GUI that the manual
 * printer setup dialog shall be shown
 */
void GuiPrinterAutoSetup::on_buttonManualSetup_clicked()
{
  emit raiseWidget(PRINTER_MANUAL_SETUP_WIDGET);
}


/**
 * This function finds all connected USB printers and
 * shows them in the printer list box.
 *
 * @returns the number of found USB printers
 * @see     findParPortPrinters
 */
int GuiPrinterAutoSetup::findUsbPrinters()
{
  //
  /// Opens the file \c /sys/bus/usb/devices which contains information
  /// about connected usb devices.
  //
  QDir usbDevicesDir( LPS_USB_DEVICE_DIR );

  if ( !usbDevicesDir.exists() )
  {
    throw LpsError(tr( "Could not find usb-device dir:") 
                    + usbDevicesDir.dirName());
  }

  int nPrintersFound = 0;

  //
  // Read all subdirectories
  //
  QStringList subDirs = usbDevicesDir.entryList(); 
  for ( int nSubDir = 2; nSubDir < subDirs.count(); nSubDir ++)
  {
     if(readUsbInfo(usbDevicesDir.absolutePath() + "/" + subDirs[nSubDir]))
       nPrintersFound++;
  }// end while
  return nPrintersFound;
}

/**Read the information in a subdir of the USB-devices dir
*/
bool GuiPrinterAutoSetup::readUsbInfo(const QString & subDir){
    QString     manufacturerName;
    QString     productName;

    QFile file ( subDir +"/" +"manufacturer");
    if(!file.exists())
      return false; 
    if(file.open(QIODevice::ReadOnly)){
      manufacturerName = file.readAll().trimmed();
      fixManufacturer(manufacturerName);
      file.close();
    }
    else return false;

    file.setFileName(subDir +"/" +"product");
    if(!file.exists())
      return false; 
    if(file.open(QIODevice::ReadOnly)){
      productName = file.readAll().trimmed();
      file.close();
    }

    qDebug("GuiPrinterAutoSetup::findUsbPrinters: Found printer: %s %s",
           qPrintable(manufacturerName), qPrintable(productName));

    QDomNodeList drivers;

    //
    // A device has to meet at least one of the following three criteria
    // before it is inserted into the printers list box:
    //
    // 1. The user wants to see all connected devices (checkBox_showAllDevices is checked)
    // 2. We found the manufacturer of the device in our printer driver DB
    // 3. We found the product of the device somewhere in the DB
    //
    if (  checkBoxShowAllDevices->isChecked() ||
          m_domDoc->manufacturerExists(manufacturerName) ||
          (drivers = m_domDoc->getDrivers(productName)).length()
       )
    {

      PrinterDevice device;

      device.nativeManufacturer = manufacturerName;
      device.connectionType     = UsbConnection;
      device.drivers            = drivers;
      device.nativeProduct      = productName;

      m_printerMap[ manufacturerName +" / "+ productName ] = device;
      return true;
    }//end if checkBox....
    else return false;
}

/**
 * This function finds all connected parallel port printers and
 * shows them in the printer list box.
 *
 * @returns the number of found parallel port printers
 * @see     findUsbPrinters
 */
int GuiPrinterAutoSetup::findParPortPrinters()
{
  int nPrintersFound = 0;

  QDir deviceDir(LPS_PARPORT_DEVICE_DIR); // typically, this point to /proc/sys/dev/parport/

  if ( !deviceDir.exists() || !deviceDir.isReadable() )
    throw LpsError(QString("The following directory could not be found or is not readable: ") + deviceDir.absolutePath());

  deviceDir.setFilter( QDir::Dirs );
  QStringList subdirs = deviceDir.entryList();

  // Run through all directories that are in /proc/sys/dev/parport/ and search for parportX directories
  for ( QStringList::Iterator subDirIt = subdirs.begin();
        subDirIt != subdirs.end();
        ++subDirIt
      )
  {
      QString dirName = *subDirIt;

      // Filter parport dirs
      if ( dirName!="." && dirName!=".." && dirName.contains("parport") ) //this is a parportX dir,
                                                                          //enter it and search for an autoprobe file
      {
        QString absolutePath = deviceDir.absolutePath() + "/" + dirName;

        QDir dirEntry( absolutePath );

        if ( !dirEntry.exists() || !dirEntry.isReadable() )
          throw LpsError(QString("The following directory could not be found or is not readable: ") + dirEntry.absolutePath());

        dirEntry.setFilter( QDir::Files );
        QStringList files = dirEntry.entryList();

        QFile deviceFile(absolutePath + "/autoprobe");

        if ( !deviceFile.exists() || !deviceFile.open(QIODevice::ReadOnly) )
          throw  LpsError(QString("Could not open device file for reading: ") 
                          + deviceFile.fileName());

        QTextStream stream( &deviceFile );

        int     status                  = 0; ///< contains a bit pattern that iforms about the current stage of information collection
        int     statusManufacturerDone  = 1; ///< a status flag the informs, if the manufacturer has been extracted (1) or not (0)
        int     statusModelDone         = 2; ///< a status flag the informs, if the model has been extracted (1) or not (0)
        int     statusAllDone           = 3; ///< a status flag the informs, if all needed information for the current device have been extracted (1) or not (0)

        QString manufacturer            = "";
        QString model                   = "";

        //
        // Read the autoprobe file from the beginning to the end
        //
        QString text = stream.readAll();
        deviceFile.close();
        QStringList lines = text.split("\n");
        int nLine = 0;
        while ( nLine < lines.count())
        {
          //
          // Extract the file line by line and parse each, until
          // we have found all needed information (status==statusAllDone).
          //
          QString line = lines[nLine];
          nLine ++;

          if ( line.startsWith("MANUFACTURER:") )
          {
            manufacturer = line.remove("MANUFACTURER:").remove(";");
            status      |= statusManufacturerDone;
          }
          else if ( line.startsWith("MODEL:") )
          {
            model   = line.remove("MODEL:").remove(";");
            status |= statusModelDone;
          }
          if ( status == statusAllDone )
          {
            fixManufacturer(manufacturer);

            qDebug("GuiPrinterAutoSetup::findParPortPrinters: Found printer: %s %s",
                   qPrintable( manufacturer), qPrintable(model));

            QDomNodeList drivers;

            //
            // A device has to meet at least one of the following three criteria
            // before it is inserted into the printers list box:
            //
            // 1. The user wants to see all connected devices (checkBox_showAllDevices is checked)
            // 2. We found the manufacturer of the device in our printer driver DB
            // 3. We found the product of the device somewhere in the DB
            //
            if (  checkBoxShowAllDevices->isChecked() ||
                  m_domDoc->manufacturerExists(manufacturer) ||
                  (drivers=m_domDoc->getDrivers(model)).length()
               )
            {
                status                    = 0;

                PrinterDevice device;

                device.nativeManufacturer = manufacturer;
                device.connectionType     = ParPortConnection;
                device.nativeProduct      = model;
                device.drivers            = drivers;

                m_printerMap[manufacturer +" / "+ model] = device;

                nPrintersFound++;
            }
          }//end if status == allDone
        }//end while
      } //end if dirname contains parport
  }//end for

  return nPrintersFound;
}


/**
 * This method en-/disables widgets according to the appropriate bitmask.
 *
 * @param wf          any combination of \c WidgetFlag flags
 * @see   WidgetFlag
 */
void GuiPrinterAutoSetup::enableWidgets( int wf )
{
  // list boxes
  listWidgetDrivers->setEnabled(          wf & DriversFlag            );
  listWidgetPrinters->setEnabled(         wf & PrintersFlag           );

  // labels
  label_drivers->setEnabled(            wf & DriversFlag            );
  label_printers->setEnabled(           wf & PrintersFlag           );

  // push buttons
  buttonDriverDetails->setEnabled( wf & DriversFlag            );
  buttonHelp->setEnabled(          wf & HelpButtonFlag         );
  buttonBack->setEnabled(          wf & BackButtonFlag         );
  buttonRestart->setEnabled(       wf & RestartButtonFlag      );
  buttonManualSetup->setEnabled(   wf & ManualSetupButtonFlag  );
  buttonSave->setEnabled(  wf & SaveSettingsButtonFlag );
}


/**
 * Some PPD files are known to contain unusable manufacturer names
 * that need to be renamed due to compatibility with the printer
 * driver database.
 * A famous example: \c Hewlett-Packard will be renamed to \c HP .
 *
 * @param manufacturer  the manufacturer name that needs to be renamed
 */
void GuiPrinterAutoSetup::fixManufacturer( QString & manufacturer )
{
  if ( manufacturer == "Hewlett-Packard" )
    manufacturer = "HP";
}
