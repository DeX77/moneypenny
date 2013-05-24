/***************************************************************************
 *   Copyright (C) 2005-2006 by Jens Lukowski, Walter Roth 2009
 *   jens.lukowski@fh-swf.de
 *   Modified by Walter Roth 2006
 *   This file contains the implementation of the Printersetup GUI
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   $Id: guiprintermanualsetup.cpp,v 1.3 2011-01-14 20:05:38 hbci Exp $
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
#include "guiprintermanualsetup.h"
#include "../printing/printersetupdb.h"
#include "globals.h"

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


/**
 * Constructs a new printer manual setup dialog.
 *
 * @see QWidget for parameter documentation
 */
GuiPrinterManualSetup::GuiPrinterManualSetup( QWidget* parent)
    : QWidget( parent), GuiPrinterManualSetupBase()
{
  setupUi(this);
  setObjectName("GuiPrinterManualSetup");
  m_db                      = new PrinterSetupDb( LPS_DB_DOCUMENT_NAME );
  m_translationRecommended  = tr( "recommended", "This will be a appended to any recommended printer" );
}


/**
 * This destructor 'takes care' of manually allocated memory.
 */
GuiPrinterManualSetup::~GuiPrinterManualSetup()
{
  if ( m_db )
    delete m_db;

  qDebug("Destructor GuiPrinterManualSetup finished");
}


/**
 * This method en-/disables widgets according to the appropriate bitmask.
 *
 * @param wf          any combination of \c WidgetFlag flags
 * @see   WidgetFlag
*/
void GuiPrinterManualSetup::enableWidgets( int wf )
{
  // list boxes
  listWidgetModels->setEnabled(             wf & ModelsFlag );
  listWidgetDrivers->setEnabled(            wf & DriversFlag );
  listWidgetManufacturers->setEnabled(      wf & ManufacturersFlag );

  // labels
  labelDrivers->setEnabled(              wf & DriversFlag );
  labelModels->setEnabled(               wf & ModelsFlag );
  labelManufacturers->setEnabled(        wf & ManufacturersFlag );

  // combo boxes
  comboBoxPorts->setEnabled(             wf & PortsFlag );

  // push buttons
  buttonDriverDetails->setEnabled(   wf & DetailsFlag );
  buttonSave->setEnabled(    wf & SaveSettingsFlag );
}


/**
 * This slot is called if an item in listWidgetManufacturers is highlighted.
 * It fills the listWidgetModels with the associated models outof the database.
 *
 * @param item  The highlighted item in listWidgetManufacturers.
 */
void GuiPrinterManualSetup::on_listWidgetManufacturers_itemActivated( QListWidgetItem * item )
{
  if ( !item ) // If the manufacturer is not highlighted correctly
    return;

  listWidgetModels->clear();
  listWidgetDrivers->clear();

  // Get the Models to the highlightet manufacturer from the db
  QDomNodeList models = m_db->getModels( item->text() );

  // Fill the listWidgetModels
  for ( uint i=0; i<models.length(); i++ )
  {
    QDomElement modelElement = models.item( i ).toElement();

    if ( !modelElement.isNull() )
    {
      listWidgetModels->addItem( modelElement.attribute( "name" ) );
    }
  }
  listWidgetModels->setSortingEnabled(true);

  // No models in listWidgetModels?
  if ( listWidgetModels->count() <= 0 )
  {
    labelStatus->setPalette(PALETTE_RED());
    labelStatus->setText( tr( "No model found for this manufacturer." ));
    enableWidgets( AllFlags ^ SaveSettingsFlag ^ DriversFlag ^ DetailsFlag ^ ModelsFlag );
  }

  enableWidgets( AllFlags ^ PortsFlag ^ DriversFlag ^ DetailsFlag ^ SaveSettingsFlag );

  labelStatus->setPalette( PALETTE_YELLOW() );
  labelStatus->setText( tr( "Please choose a model now." ) );
}


/**
 * This slot is called if an item in listWidgetModels is highlighted.
 * It fills the listWidgetDrivers with the associated drivers outof the database.
 *
 * @param item  The highlighted item in on_listWidgetModels.
 */
void GuiPrinterManualSetup::on_listWidgetModels_itemActivated( QListWidgetItem * item )
{
  // Get the manufacturer out of the listWidgetManufacturers
  QListWidgetItem * selectedManufacturerItem = listWidgetManufacturers->currentItem();

  // Is the model item correctly highlighted
  if ( !item || !selectedManufacturerItem )
    return;

  listWidgetDrivers->clear();


  // Get the drivers from the database
  QDomNodeList drivers = m_db->getDriversByModel( selectedManufacturerItem->text(), item->text()  );


  for ( uint i=0; i<drivers.length(); i++ )
  {
    QDomElement driverElement = drivers.item( i ).toElement();

    if ( !driverElement.isNull() )
    {
      //replace "recommended" for the user. This must be undone later for selecting the driver.
      listWidgetDrivers->addItem( driverElement.attribute( "nickName" ).replace( "recommended", m_translationRecommended ) );

      uint count = listWidgetDrivers->count();

      // If the driver is recommended, highlight it!
      if ( listWidgetDrivers->item( count - 1 )->text().contains( m_translationRecommended ) )
      {
        listWidgetDrivers->setCurrentRow( count - 1 );
        enableWidgets( AllFlags );
      }
    }
  }
  listWidgetDrivers->setSortingEnabled(true);

  // No driver in drivers list box?
  if ( listWidgetDrivers->count() <= 0 )
  {
    labelStatus->setPalette( PALETTE_RED());
    labelStatus->setText( tr( "No driver found for this model." ));
    enableWidgets( AllFlags ^ SaveSettingsFlag  ^ DriversFlag ^ DetailsFlag );
  }
  // No item selected? Then select the first one!
  else if( !listWidgetDrivers->currentItem() )
  {
    enableWidgets( AllFlags ^ SaveSettingsFlag ^ PortsFlag );
    listWidgetDrivers->setCurrentItem( 0 );
  }

  editPrinterName->setText( item->text().replace( " ", "_" ) );
}


/**
 * This slot is called if an item in listWidgetDrivers is highlighted.
 * It fills the comboBoxPorts with the on this machine founded ports to
 * connect a printer.
 *
 * @param item  The highlighted item in listWidgetDrivers.
 */
void GuiPrinterManualSetup::on_listWidgetDrivers_itemActivated( QListWidgetItem * item )
{
  // Is the driver item correctly highlighted (prevents clicking in nirvana)
  if ( !item )
    return;

  labelStatus->setPalette(PALETTE_YELLOW());

  // No port selected!
  if( comboBoxPorts->currentText() == "" )
  {
    enableWidgets(AllFlags ^ SaveSettingsFlag);
    labelStatus->setPalette(PALETTE_YELLOW() );
    labelStatus->setText( tr( "Driver:" ) +" " + item->text() + "<br>" +
                           tr( "Port:" ) + " " + tr( "Not yet selected." ) );
  }
  // Port selcted
  else
  {
    enableWidgets(AllFlags);
    labelStatus->setPalette(PALETTE_GREEN());
    labelStatus->setText( tr( "Driver:" ) +" " + item->text() + "<br>" +
                           tr( "Port:" ) + " " + comboBoxPorts->currentText() );
  }
}


/**
 * This slot calls the right help page for manual printer setup.
 */
void GuiPrinterManualSetup::on_buttonHelp_clicked()
{
  emit helpRequest( "printerManualSetup.html" );
}


/**
 * This slot emits signal back().
 */
void GuiPrinterManualSetup::on_buttonBack_clicked()
{
  emit back();
}

/**
 * This slot is called, when the user clicks on the "Save Settings"
 * button. Then the current printer configuration is saved and the user may
 * decide whether to print a test page or not.
 *
 * @throws LpsNotice  if no printername is given.
 * @throws LpsError   if the driver element is null.
 */
void GuiPrinterManualSetup::on_buttonSave_clicked()
{
  try
  {
    // If no printername is given throw exception.
    if ( editPrinterName->text() == "" )
      throw LpsNotice( tr( "Printer is unnamed. Please type in a name." ) );

    //undo the translation, because the driver name is untranslated in list.
    QString driverName = listWidgetDrivers->currentItem()->text().replace( m_translationRecommended, "recommended" );

    // Get driver element
    QDomElement driver = m_db->getDriverByModel(
                           listWidgetManufacturers->currentItem()->text(),
                           listWidgetModels->currentItem()->text(),
                           driverName );

    // If no driver element is null throw exception.
    if ( driver.isNull() )
      throw LpsError( "The driver element was NULL." );

    // Build command to be executed and run it
    QString printerName = editPrinterName->text();
    QString deviceUri   = comboBoxPorts->currentText();
    QString ppdFilePath = m_db->documentElement().attribute("path");
    ppdFilePath += driver.attribute("ppdFilePath");

    // Run lpadmin to install printer
    installPrinter( this, printerName, deviceUri, ppdFilePath );

    // Ask the user if a testpage shall be printed
    testPrintRequest( this, printerName );

    labelStatus->setText( "<p>" + tr( "Your printer has been set up successfully." ) + "</p>");
    labelStatus->setPalette(PALETTE_GREEN());
  }
  catch ( LpsNotice e )
  {
    labelStatus->setPalette( PALETTE_YELLOW() );
    labelStatus->setText( e );
    return;
  }
  catch ( LpsError e )
  {
    labelStatus->setPalette( PALETTE_RED());
    labelStatus->setText( e );
    return;
  }

  labelStatus->setText( tr( "<p>Your printer has been configured successfully!</p>" ) );
  labelStatus->setPalette( PALETTE_GREEN() );
  emit printerInstalled();
}


/**
 * Shows the widget and its child widgets.
 * This method might take a while until it returns because the XML
 * printer driver database is loaded.
 *
 *@throws LpsError  if the xml db could not been opened
 *@throws LpsError  if the content of the databes could not been set
 */
void GuiPrinterManualSetup::showEvent(QShowEvent * se)
{
  QWidget::showEvent(se);

  emit showSmallHelp( "smallPrinterManualSetup.html" );
  labelStatus->setPalette( PALETTE_YELLOW() );
  labelStatus->setText(tr( "Loading database..." ));
  setCursor( Qt::WaitCursor );
  setWindowTitle( tr( "Manual Printer Setup" ) );
  qApp->processEvents();

  enableWidgets( NoneFlag | ManufacturersFlag );

  // Only valid characters for printername
  QRegExp rx( "[a-zA-Z_0-9]+" );
  QValidator * validator = new QRegExpValidator( rx, this );
  editPrinterName->setValidator( validator );

  // Clear all list boxes
  listWidgetDrivers->clear();
  listWidgetModels->clear();
  listWidgetManufacturers->clear();

  // Set the default printername
  editPrinterName->setText( LPS_PRINTER_NAME );
  QString fileName= MP_DIR() + MONEYPENNY_DATA_DIR + LPS_DB_FILE;
  QFile file( fileName );

  try
  {
    if ( !file.open( QIODevice::ReadOnly ) )
      throw LpsError( tr( "Could not open the database file." ) );
    if ( !m_db->setContent( &file ) )
      throw LpsError( tr( "Could not set content of xml database." ) );
  }
  catch ( LpsError e )
  {
    labelStatus->setPalette( PALETTE_RED() );
    labelStatus->setText( e );
    qDebug( "%s", qPrintable(e) );
    enableWidgets( NoneFlag );
  }

  // Fill the manufacturers list box
  QDomNodeList manufacturers = m_db->getManufacturers();
  for ( uint i=0; i<manufacturers.length(); i++ )
  {
    QDomElement manufacturersElement = manufacturers.item( i ).toElement();
    if ( !manufacturersElement.isNull() )
    {
      listWidgetManufacturers->addItem( manufacturersElement.attribute( "name" ) );
    }
  }
  listWidgetManufacturers->setSortingEnabled(true);

  // No manufacturers in listWidgetManufacturers?
  if ( listWidgetManufacturers->count() <= 0 )
  {
    labelStatus->setPalette( PALETTE_RED() );
    labelStatus->setText( tr( "No manufacturers found." ) );
    enableWidgets( NoneFlag );
  }
  else
  {
    labelStatus->setPalette( PALETTE_YELLOW() );
    labelStatus->setText( tr( "Please select your printer's manufacturer name from the manufacturer list box." ) );
  }

  readPorts();
  unsetCursor();
}


/**
 * This slot is called if comboBoxPorts is activated.
 * It enables the pushButton_saveSettings.
 *
 * @param itemtext  The selected text in the comboBoxPorts as QString.
 */
void GuiPrinterManualSetup::on_comboBoxPorts_activated( const QString & itemtext )
{
  // Is no port selected / activated?
  if ( itemtext == "" )
  {
    enableWidgets( AllFlags ^ SaveSettingsFlag );
  }
  else
  {
    enableWidgets( AllFlags );
    labelStatus->setPalette( PALETTE_YELLOW() );
    labelStatus->setText( tr( "To save these settings and create the printer click on Save settings." ) );
  }
}


/**
 * This slot is called when the user clicks on pushButton_driverDetails
 * ("More Driver Information"). It opens a popup with details about the
 * selected driver.
 */
void GuiPrinterManualSetup::on_buttonDriverDetails_clicked()
{
  try
  {
    showPrinterDriverDetails( listWidgetDrivers->currentItem()->text().replace( m_translationRecommended, "recommended" ), this );
  }
  catch ( LpsError e )
  {
    labelStatus->setText( e );
    labelStatus->setPalette( PALETTE_YELLOW() );
  }
}


/**
 * Reads the ports using lpinfo and puts the results into comboBoxPorts.
 *
 * @throws LpsError if the ports could not be read
 */
void GuiPrinterManualSetup::readPorts()
{
  FILE * lpinfo = NULL;
  comboBoxPorts->clear();

  lpinfo = popen( qPrintable(sudo( "/usr/sbin/lpinfo -v", this, 
    QObject::tr("Please enter the system password to read the printer configuration."))), "r" );

  QTextStream* stream = NULL;

  try
  {
    if ( !lpinfo )
    {
      throw LpsError( tr( "Could not read ports" ) );
    }
    stream = new QTextStream(lpinfo,QIODevice::ReadOnly);
    QString line;

    while( !stream->atEnd() )
    {
      line = QString( stream->readLine() );

      if ( !line.isEmpty() && !line.contains( "network" ) ){
        QStringList params = line.split(" "); //the device-url is the second parameter
        comboBoxPorts->addItem(params[1]);
      }
    }
  }
  catch ( LpsError e )
  {
    labelStatus->setPalette( PALETTE_RED() );
    labelStatus->setText(e);
    qDebug( "%s", qPrintable(e) );
  }

  //
  // Never change the order of these deletions/closings!!!
  //
  if ( stream )
    delete stream;
  if ( lpinfo )
    pclose( lpinfo );

  comboBoxPorts->setCurrentIndex( 0 );
}
