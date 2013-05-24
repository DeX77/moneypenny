/***************************************************************************
                          guiconnections.cpp  -  description
                             -------------------
    begin                : Do Nov 4 2004
    copyright            : (C) 2004 by Marcel Mulch, Walter Roth
    email                : marcel@mulch-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "guitools.h"
#include "globals.h"


#include <QPushButton>
#include <QValidator>
#include <QFileDialog>
#include <QComboBox> 

/**
 *this is the constructor for create new user
 *
 *@param main
 *@param parent the parent dialog of this widget
 *@param name widgets title
 *@param hobit
 */
GuiTools::GuiTools(QWidget *parent)
    :QWidget(parent)
{
   setupUi(this);
   setObjectName("GuiTools");

  QValidator* validator = new QIntValidator( 1, 65355, this );
  lineEditPort->setValidator(validator);

  // hide sql functions if not needed
  if (! USE_MYSQL_FEATURES) groupBoxDataAnalysis->hide();
  if(!USE_LIVE_CD_FEATURES){
    checkBoxShutdown->hide();
    buttonSaveOsConfig->hide();
  }
 // if(USE_LIVE_CD_FEATURES || ! DEBUG_HARDWARE)
 //   buttonBuildPrinterDriverDatabase->hide(); //useless for a CD or a release on HDD
  qDebug("Constructor GuiTools");
}

GuiTools::~GuiTools()
{
  qDebug("Destructor GuiTools");
}

/**
* slot which retrieves the bank servers public keys 
*/
void GuiTools::on_buttonGetKeys_clicked()
{
  if(HoBIT::hobit()->createJobFirstKeyRequest() == HobitErrorSuccess)
    emit send();
  else (QMessageBox::critical(this, tr("Error"), 
         tr("Could not get keys. You need not get keys for a chipcard.")));
}

/**
* retrieves the system id of the bank server 
*/
void GuiTools::on_buttonGetSystemId_clicked()
{
  HoBIT::hobit()->createJobResynchronize();
  emit send(); 
}

/**
* sends our own public keys to the bank server 
*/
void GuiTools::on_buttonKeyChange_clicked()
{
  if(HoBIT::hobit()->createJobKeyChange() == 1) //one job must be created
    emit send();
  else (QMessageBox::critical(this, tr("Error"), 
         tr("Could not send keys. You need not send keys for a chipcard.")));  
}

/**
* retrieves a list of our accounts 
*/
void GuiTools::on_buttonGetAccounts_clicked()
{
  HoBIT::hobit()->createJobGetUPD();
   emit send(); 
 }


/**
* generates the customers iniletter 
*/
void GuiTools::on_buttonIniLetter_clicked()
{
  emit showIniLetter();
}

/** Run a test connection to www.google.de or another url
*/
void GuiTools::on_buttonConnectionTest_clicked(){
 HoBIT_Connector::ConnectionState state;
  //connected to guiMain::slotTestInternet  
 emit testInternet(true, //show success message
                   lineEditUrl->text(), lineEditPort->text().toInt(), &state); 
}

/** Start the hardware setup dialog
*/
void GuiTools::on_buttonConsole_clicked(){
   if(! USE_LIVE_CD_FEATURES){
     QMessageBox::critical(this, tr("Error"), 
     tr("Please use your operating system to run a shell."));
     return;
   } 
   else QMessageBox::information(this, tr("Information"), 
   tr("Moneypenny will now open a root shell for hardware setup"));

   system("sudo xterm");
}

/** Emits signal help request.
*/
void GuiTools::on_buttonHelp_clicked(){
 emit helpRequest("toolbox.html");
}


/** Emits signal sqlquery.
*/
void GuiTools::on_buttonSQLQuery_clicked(){
  emit sqlquery();
}

/** Fires signal setShutdownMode
*/
void GuiTools::on_checkBoxShutdown_stateChanged(int state){
 if(state == Qt::Checked)
  emit setShutdownMode(true);
 else emit setShutdownMode(false);
}

/** Emits signal back
*/
void GuiTools::on_buttonBack_clicked(){
  emit back();
}

/** Fires signal timeOutChanged
*/
void GuiTools::on_lineEditTimeOut_textChanged(){
  int newTimeOut = lineEditTimeOut->text().toInt();
  if(newTimeOut > 5) // do not allow too short times
   emit timeOutChanged(newTimeOut); 
}

/** Sets the timeOut value in lineEditTimeOut
*/
void GuiTools::setTimeOut(int newTimeOut){
 lineEditTimeOut->setText(QString::number(newTimeOut));
}

/** Saves the knoppix configuration
*/
void GuiTools::on_buttonSaveOsConfig_clicked(){
 emit saveKnoppixConfiguration();
}

/** Updates the data listview.
*/
void GuiTools::refresh(){
 //Do nothing
}

/** Fires signal buildPrinterDriverDatabase
*/
void GuiTools::on_buttonBuildPrinterDriverDatabase_clicked(){
 emit buildPrinterDriverDatabase();
}

/** Emits deleteLogFiles
*/
void GuiTools::on_buttonDeleteLogFiles_clicked(){
 emit deleteLogFiles();
}


