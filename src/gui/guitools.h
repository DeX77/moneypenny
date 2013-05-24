/***************************************************************************
                          guitools.h  -  description
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

#ifndef GUITOOLS_H
#define GUITOOLS_H

#include <QWidget>

#include "ui_guitoolsbase.h"
#include "hobit.h"

#include <list>
using namespace std;

#include <openhbci2.h>
using namespace HBCI;


class GuiMain;
class GuiCreateUser;
class GuiHbciVersion;

/**
class for dialogs to create new user or change the current hbci-version
*/
class GuiTools : public QWidget, public Ui::GuiTools
{
  Q_OBJECT
public:

  /**
  *this is the constructor for create new user
  *
  *@param main
  *@param parent the parent dialog of this widget
  *@param name widgets title
  *@param hobit
  */
  GuiTools(QWidget *parent);
  ~GuiTools();

   /** Updates the data listview.
  */
  void refresh();
  /** Sets the timeOut value in lineEditTimeOut
  */
  void setTimeOut(int newTimeOut);
  
signals:
 void testInternet(bool showSuccessMessage, const QString & url, int port, HoBIT_Connector::ConnectionState * state);
 void helpRequest(const QString& );
 void importRDH2Key(const QString & fileName);
 void sqlquery();
 void setShutdownMode(bool autoShutdown);
 void back();
 void timeOutChanged(int newTimeOut);
 void saveKnoppixConfiguration();
 void buildPrinterDriverDatabase();
 void deleteLogFiles();
 void raiseWidget(int);
 void refreshRequest(); 
 void send();
 void showIniLetter();
 
public slots: // Public slots
  /**
  * retrieves the system id of the bank server 
  */
  void on_buttonGetSystemId_clicked();

  /**
  * retrieves a list of our accounts 
  */
  void on_buttonGetAccounts_clicked();
  /**
  * sends our own public keys to the bank server 
  */
  void on_buttonKeyChange_clicked();
  /**
  * generates the customers iniletter 
  */
  void on_buttonIniLetter_clicked();
  /** Run a test connection to www.google.de
  */
  void on_buttonConnectionTest_clicked();
  /** Start the hardware setup dialog
  */
  void on_buttonConsole_clicked();
  /** Emits signal help request.
  */
  void on_buttonHelp_clicked();
 /** Emits signal sqlquery
  */
  void on_buttonSQLQuery_clicked();
  /** Fires signal setShutdownMode
  */
  void on_checkBoxShutdown_stateChanged(int state);
  /** Emits signal back
  */
  void on_buttonBack_clicked();
  /** Fires signal timeOutChanged
  */
  void on_lineEditTimeOut_textChanged();
  /**
  * slot which retrieves the bank servers public keys 
  */
  void on_buttonGetKeys_clicked();
  /** Saves the knoppix configuration
  */
  void on_buttonSaveOsConfig_clicked();
  /** Fires signal buildPrinterDriverDatabase
  */
  void on_buttonBuildPrinterDriverDatabase_clicked();
  /** Emits deleteLogFiles
  */
  void on_buttonDeleteLogFiles_clicked();
  
private:
 /**
  * updates the list of users 
  */
  void updateList();
  GuiCreateUser* _guicreateuser;
  //GuiIniLetter* _guiiniletter;
  GuiHbciVersion* _guichangeversion;
};

#endif
