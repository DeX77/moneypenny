/***************************************************************************
                          guimain.cpp  -  description
                             -------------------
    begin                : Do Nov 4 2004
    copyright            : (C) 2004 by Marcel Mulch, 2011 Walter Roth
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

#include "guimain.h"
#include "dlgenterpinkeypad.h"
#include "dlgenterpin.h"
#include "dlgstatusmessage.h"
#include "guisplash.h"
#include "guifindmedium.h"
#include "guiloginpassword.h" 
#include "guiwelcome.h"
#include "guilog.h"
#include "guihbciwizard.h"
#include "guiiniletter.h"
#include "guitools.h"
#include "guishowbankdata.h"
#include "guiaccountinformation.h"
#include "guitransactionlist.h"
#include "guitransferform.h"
#include "guidatedsingletransfer.h"
#include "guidatedtransfers.h"
#include "guijobs.h"
#include "guitransfertemplates.h"
#include "dlgenterpin.h"
#include "guitransactioninfo.h"
#include "guigettransactions.h"
#include "guinotimplemented.h"
#include "guidataexport.h"
#include "guitransactionprinter.h"
#include "guihelpwindow.h"
#include "guiknownbanks.h"
#include "guiprinterautosetup.h"
#include "guiprintermanualsetup.h"
#include "guiprinterdrivertoxml.h"
#include "guihardwareconfig.h"
#include "guientropy.h"
#include "guiaddaccount.h"
#include "guiimport.h"
#include "guiprogressindicator.h"

#include "datedtransfer.h"
#include "kernel/helpers/transaction.h"
#include "kernel/helpers/account.h"

#include "guilogin.h"

#include <QWidget>
#include <QDate>
#include <QTextStream>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCloseEvent>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>

#include "hobit_crypt.h"
#include "hobit.h"
#include "hobit_connector.h"
#include "hobit_sql.h"
#include "../keyfiles/keyfileopenhbci.h"

// openhbci2 includes
#include <openhbci2.h>
using namespace HBCI;


#include <iostream>
#include <exception>
using namespace std;


//Debugging defines
//#define DEBUG_PAGES
//#define DEBUG_DATABASE



/**
*constructor for GuiMain (main window),
*sets statusbar, menulist, refreshes informations, widgets, slots, etc. 
*@param crypt pointer on the HoBIT_Crypt interface
*@param parent pointer on current QWidget
*@param name title of the GuiMain dialog
*/
GuiMain::GuiMain() :QDialog(NULL), Ui::GuiMain()
{ 
  qDebug("Constructor GuiMain starts");
  setupUi(this);
  setObjectName("GuiMain");

  //Setup button connections, from right to left. See setupMainApplication for other connections
  //exit button is connected to close by Desinger
  connect(buttonHelp, SIGNAL(clicked()), this, SLOT(slotHelp()));
  connect(buttonToolbox, SIGNAL(clicked()), this, SLOT(slotToolbox()));
  connect(buttonSendNow, SIGNAL(clicked()), this, SLOT(slotSendNow()));
  connect(buttonDialIn, SIGNAL(clicked()), this, SLOT(slotDialIn()));
  connect(buttonUpdate, SIGNAL(clicked()), this, SLOT(slotUpdate()));
  connect(buttonShowTransactions, SIGNAL(clicked()), this, SLOT(slotShowTransactions()));
  connect(buttonSingleTransfer, SIGNAL(clicked()), this, SLOT(slotSingleTransfer()));
//  connect(buttonAccounts, SIGNAL(clicked()), this, SLOT(slotAccounts()));

  setWindowTitle(QString("MoneyPenny")+ " " + QString(VERSION));
  if(USE_LIVE_CD_FEATURES || DEBUG_HARDWARE)
    buttonDialIn->show();
  else  buttonDialIn->hide(); 
  _Crypt = new HoBIT_Crypt(); 
  
  qDebug("Crypt constructed.");
  
  createConfigDir();
 
  _CurrentWizardPage = 0; //Welcome page of wizard
  _AddPrinterProcess = NULL;
  _exitCode = EXITCODE_SUCCESS; // normal shutdown - user will be asked
  _AutoShutdown = true;
  _SaveKnoppixConfig = false;
   
   //set all child widgets with NULL
  _DlgEnterPinKeypad = NULL;
  _DlgEnterPin = NULL;

  _GuiTools = NULL;
  _GuiShowBankData = NULL;
  _guiwelcome = NULL;
  //_guibalances = NULL;
  _GuiAccountInformation = NULL;
  _GuiTransactionList = NULL;
  _GuiTransactionPrinter = NULL;
  _GuiGetTransactions = NULL;
  _GuiIniLetter = NULL;
  _GuiTransferForm = NULL;
//  _guisingletransfer = NULL;
//  _GuiDatedSingleTransfer = NULL;
  _GuiDatedTransfers = NULL;
  _guistandingorders = NULL;
//  _guistartstandingorder = NULL;
  _GuiHelpWindow = NULL;
  _GuiKnownBanks = NULL;
  _GuiLogin = NULL;
  _guiNullWidget = NULL;
  _GuiHbciWizard = NULL;
  _guichangeaccountname = NULL;
  _GuiJobs = NULL;
  _GuiAddAccount = NULL;
  _GuiHardwareConfig = NULL;
  _guidialin= NULL;
  _GuiLog = NULL;
  _GuiTransferTemplates = NULL;
  _GuiTransactionInfo = NULL;
  _GuiSplash = NULL;
  _GuiFindMedium = NULL;
  _GuiLoginPassword = NULL;
  _guisqlquery = NULL;
  _guisqlqueryresult = NULL;
  _GuiPrinterAutoSetup = NULL;
  _GuiPrinterManualSetup = NULL;
  _GuiPrinterDriverToXml = NULL;

   //Initialize splash and findMedium and LoginPassword only. They are needed immediately.
   //All other child widgets are initialized by buildMainApplication while the user looks at the splash.	
   _GuiSplash = new GuiSplash();
   qDebug("guiSplash constructed.");
   _GuiFindMedium = new GuiFindMedium(_GuiSplash );
   qDebug("guiFindMedium constructed.");
   _GuiLoginPassword= new GuiLoginPassword(_GuiSplash, _Crypt);
   qDebug("guiLoginPassword constructed.");
  
   for (int i = 0; i < LAST_WIDGET; i++) //initialize page index array with 0
     _PageIndex[i] = 0;

   _WidgetCallStack = new QStack<int>();
   _WidgetCallStack->push(0);
}

/**Create main configuration directory
*/
void GuiMain::createConfigDir(){
  QString homeDir = getenv("HOME"); //this is on the ramdisk, when running from CD
 
  _ConfigDataDir = homeDir + "/" + MONEYPENNY_MAIN_DIR;
  QDir mpHomeDir(_ConfigDataDir);
  
  if(mpHomeDir.exists()){  //cleanup required, dir exists because of unclean shutdown
   qDebug("Old config data dir found. Removing: %s", _ConfigDataDir.toLatin1().data());
   QString rm = "rm -r " + _ConfigDataDir;
   system(rm.toLatin1().data());
  }
  
  //now create a new empty dir    
  if(!mpHomeDir.mkdir(_ConfigDataDir)){
     QMessageBox::critical(NULL, QObject::tr("Error"), 
                           QObject::tr("Could not create main configuration data directory: ") + _ConfigDataDir + QObject::tr("Aborting"));
     qDebug("Could not create main configuration data directory: %s", _ConfigDataDir.toLatin1().data());
     exit (1);
   }
  
  qDebug("GuiMain::GuiMain: Using _ConfigDataDir: %s", _ConfigDataDir.toLatin1().data());
}


/** Builds the widgets for the application and creates the connections. 
  *  Name properties of widgets are used as help file names to control the contents
  *  of the small help window at the lower left. See slotRaiseWidget
  */  
void GuiMain::buildMainApplication(){
   qDebug("Start build application");
  _Connector = new HoBIT_Connector(this);
  HoBIT::newHoBIT( false,
                   this,
                   MONEYPENNY_OPENHBCI_FILE,
                   MONEYPENNY_AQMONEY_FILE,
                   MONEYPENNY_TEMPLATE_FILE,
                   MONEYPENNY_HBCIDATA_DIR,
                   _ConfigDataDir,
                   APPLICATION_NAME,
                   _Connector);

  _DlgEnterPinKeypad = new  DlgEnterPinKeypad (this);
  _DlgEnterPin = new   DlgEnterPin(this);

  _GuiNotImplemented = new GuiNotImplemented(stackedWidget );
  _PageIndex[NOT_IMPLEMENTED_WIDGET] = stackedWidget->addWidget(_GuiNotImplemented);
  connect(_GuiNotImplemented, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));

  _GuiLog = new GuiLog(stackedWidget );
  _PageIndex[LOG_WIDGET] = stackedWidget->addWidget(_GuiLog);
  connect(_GuiLog, SIGNAL(closeRequest()), this, SLOT(slotShowLastVisibleWidget()));

  _GuiIniLetter = new GuiIniLetter(stackedWidget);
  _PageIndex[INILETTER_WIDGET] = stackedWidget->addWidget( _GuiIniLetter );
  connect(_GuiIniLetter, SIGNAL(terminated()), this, SLOT(slotAccountLogin()));
  connect(_GuiIniLetter, SIGNAL(addPrinterRequest()), this, SLOT(slotHardwareConfig()));

  _GuiTools = new GuiTools(stackedWidget);
  connect(_GuiTools, SIGNAL(testInternet(bool, const QString& , int, HoBIT_Connector::ConnectionState * )), 
           this, SLOT(slotTestInternet(bool,const QString& , int , HoBIT_Connector::ConnectionState * )));
  connect(_GuiTools, SIGNAL(helpRequest(const QString& )), this, SLOT(slotHelp(const QString& )));
  connect(_GuiTools, SIGNAL(importRDH2Key(const QString & )), this, SLOT(slotImportRDH2Key(const QString & )));
  connect(_GuiTools, SIGNAL(sqlquery()), this, SLOT(slotSqlQuery()));
  connect(_GuiTools, SIGNAL(deleteLogFiles()), this, SLOT(slotDeleteLogFiles()));
  connect(_GuiTools, SIGNAL(setShutdownMode(bool)), this, SLOT(slotSetShutdownMode(bool)));
  connect(_GuiTools, SIGNAL(timeOutChanged(int)), this, SLOT(slotSetTimeOut(int)));
  connect(_GuiTools, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiTools, SIGNAL(saveKnoppixConfiguration()), this, SLOT(slotSaveOsConfig()));
  connect(_GuiTools, SIGNAL(buildPrinterDriverDatabase()), this, SLOT(slotBuildPrinterDriverDatabase()));
  connect(_GuiTools, SIGNAL(refreshRequest()), this, SLOT(slotRefresh()));
  connect(_GuiTools, SIGNAL(raiseWidget(int)), this, SLOT(slotRaiseWidget(int)));
  connect(_GuiTools, SIGNAL(send()), this, SLOT(slotSend()));
  connect(_GuiTools, SIGNAL(showIniLetter()), this, SLOT(slotShowIniLetter()));
  _PageIndex[TOOLBOX_WIDGET] = stackedWidget->addWidget( _GuiTools );

  _GuiPrinterDriverToXml= new GuiPrinterDriverToXml(stackedWidget);
  connect(_GuiPrinterDriverToXml, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));
  _PageIndex[PRINTER_DRIVER_TO_XML_WIDGET] = stackedWidget->addWidget( _GuiPrinterDriverToXml );

  _GuiShowBankData = new GuiShowBankData(stackedWidget);
  connect(_GuiShowBankData, SIGNAL(helpRequest(const QString& )), this, SLOT(slotHelp(const QString& )));
  connect(_GuiShowBankData, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));
  _PageIndex[ SHOW_ACCOUNTDATA_WIDGET] = stackedWidget->addWidget( _GuiShowBankData );

  _guiwelcome = new GuiWelcome( stackedWidget );
  connect(_guiwelcome, SIGNAL(back()), this, SLOT(slotAccountLogin()));
  _PageIndex[ WELCOME_WIDGET] = stackedWidget->addWidget( _guiwelcome );

  qApp->processEvents(); //keep gui alive
 
  _GuiAccountInformation = new GuiAccountInformation(stackedWidget);
  _PageIndex[ ACCOUNT_INFORMATION_WIDGET] = stackedWidget->addWidget( _GuiAccountInformation );
  connect(_GuiAccountInformation, SIGNAL(deleteAccount(QString)), this, SLOT(slotDeleteAccount(QString)));
  connect(_GuiAccountInformation, SIGNAL(addAccount()), this, SLOT(slotAddAccount()));
  connect(_GuiAccountInformation, SIGNAL(changeAccount()), this, SLOT(slotAccountLogin()));
  connect(_GuiAccountInformation, SIGNAL(showTransactions()), this, SLOT(slotShowTransactions()));
  connect(_GuiAccountInformation, SIGNAL(dataModified()), this, SLOT(slotRefresh()));

  _GuiTransactionList = new GuiTransactionList(stackedWidget);
  _PageIndex[ TRANSACTION_LIST_WIDGET ] = stackedWidget->addWidget(_GuiTransactionList);
  connect(_GuiTransactionList, SIGNAL(printTransactions(banking::TransactionList *, const QDate &, const QDate &)), 
          this, SLOT(slotPrintTransactions(banking::TransactionList *, const QDate &, const QDate &)));
  connect(_GuiTransactionList, SIGNAL(getTransactions()), this, SLOT(slotGetTransactions()));
  connect(_GuiTransactionList, SIGNAL( showDetails(const banking::TransactionItem &)), 
        this, SLOT(slotShowTransactionInfo(const banking::TransactionItem &)));
  connect(_GuiTransactionList, SIGNAL(exportData()), 
           this, SLOT(slotExport()));
  connect(_GuiTransactionList, SIGNAL(refreshRequest()), this, SLOT(slotRefresh()));
  connect(_GuiTransactionList, SIGNAL(helpRequest(const QString&)), this, SLOT(slotHelp(const QString&)));
  connect(_GuiTransactionList, SIGNAL(raiseWidget(int)), this, SLOT(slotRaiseWidget(int)));

  _GuiTransactionInfo = new GuiTransactionInfo(stackedWidget);
   _PageIndex[ TRANSACTION_INFORMATION_WIDGET ] = stackedWidget->addWidget(_GuiTransactionInfo);
  connect(_GuiTransactionInfo, SIGNAL(canceled()), this, SLOT(slotShowLastVisibleWidget()));
 
  _GuiGetTransactions = new GuiGetTransactions(stackedWidget);
  _PageIndex[ GET_TRANSACTIONS_WIDGET ] = stackedWidget->addWidget(_GuiGetTransactions );
  connect(_GuiGetTransactions, SIGNAL(raiseWidget(int)), this, SLOT(slotRaiseWidget(int)));
  connect(_GuiGetTransactions, SIGNAL(sendNow(bool *,const QString&, bool)), 
          this, SLOT(slotSendNow(bool *, const QString&, bool)));
  connect(_GuiGetTransactions, SIGNAL(canceled()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiGetTransactions, SIGNAL(jobsCreated()), this, SLOT(slotRefresh()));
  qDebug("GuiGetTransActions constructed");

  _GuiTransactionPrinter = new GuiTransactionPrinter(stackedWidget);
  _PageIndex[ TRANSACTIONS_PRINTER_WIDGET ] = stackedWidget->addWidget(_GuiTransactionPrinter );
  connect(_GuiTransactionPrinter, SIGNAL(helpRequest(const QString& )), this, SLOT(slotHelp(const QString& )));
  connect(_GuiTransactionPrinter, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));

  _GuiTransferForm = new GuiTransferForm(stackedWidget);
  _PageIndex[ TRANSFER_WIDGET ] = stackedWidget->addWidget(_GuiTransferForm);
  connect(_GuiTransferForm, SIGNAL(cancelled()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiTransferForm, SIGNAL(raiseWidget(int)), this, SLOT(slotRaiseWidget(int)));
  connect(_GuiTransferForm, SIGNAL(sendNow(bool *, const QString&, bool)), 
          this, SLOT(slotSendNow(bool *, const QString&, bool)));  
  connect(_GuiTransferForm, SIGNAL(refreshRequest()), this, SLOT(slotRefresh()));

  _GuiDatedSingleTransfer = new GuiDatedSingleTransfer(stackedWidget);
  _PageIndex[ DATED_SINGLE_TRANSFER_WIDGET ] = stackedWidget->addWidget(_GuiDatedSingleTransfer);
  connect(_GuiDatedSingleTransfer, SIGNAL(cancelled()), 
          this, SLOT(slotDatedTransfer()));
  connect(_GuiDatedSingleTransfer, SIGNAL(setCurrentWidget(int)), 
          this, SLOT(slotRaiseWidget(int)));
  connect(_GuiDatedSingleTransfer, SIGNAL(sendNow()), 
          this, SLOT(slotSendNow()));  
  connect(_GuiDatedSingleTransfer, SIGNAL(refreshRequest()), 
          this, SLOT(slotRefresh()));
 
  _GuiDatedTransfers = new GuiDatedTransfers(stackedWidget);
  _PageIndex[ DATED_TRANSFERS_WIDGET ] = stackedWidget->addWidget(_GuiDatedTransfers);
  connect(_GuiDatedTransfers, SIGNAL(setCurrentWidget(int)), 
          this, SLOT(slotRaiseWidget(int)));
  connect(_GuiDatedTransfers, SIGNAL(showInformation( const banking::TransferItem &)), 
          this, SLOT(slotShowTransactionInfo( const banking::TransferItem &)));
  connect(_GuiDatedTransfers, SIGNAL(newDatedTransferRequired()), 
          this, SLOT(slotDatedTransfer()));
/*
  _guistandingorders = new GuiStandingOrders(this, stackedWidget, "StandingOrders", HoBIT::hobit());
  stackedWidget->addWidget(_guistandingorders, STANDING_ORDERS_WIDGET);
  connect(_guistandingorders, SIGNAL(showInformation(Pointer<StandingOrder>)), this,
    SLOT(slotShowStandingOrderInfo(Pointer<StandingOrder>)));
  
  _guistartstandingorder = new GuiStartStandingOrder(stackedWidget, "StartStandingOrder", HoBIT::hobit());
  stackedWidget->addWidget(_guistartstandingorder, START_STANDING_ORDER_WIDGET);
  connect(_guistartstandingorder, SIGNAL(cancelled()), this, SLOT(slotStandingOrder()));
  connect(_guistartstandingorder, SIGNAL(setCurrentWidget(int)), this, SLOT(slotRaiseWidget(int)));
  connect(_guistartstandingorder, SIGNAL(sendNow()), this, SLOT(slotSendNow()));  
  connect(_guistartstandingorder, SIGNAL(refreshRequest()), this, SLOT(slotRefresh()));
  connect(_guistartstandingorder, SIGNAL(setCurrentWidget(int)), this, SLOT(slotRaiseWidget(int)));
 */

  qApp->processEvents(); //keep gui alive
  _GuiTransferTemplates = new GuiTransferTemplates(stackedWidget);
  _PageIndex[ TRANSFER_TEMPLATES_WIDGET ] = stackedWidget->addWidget(_GuiTransferTemplates);
  connect(_GuiTransferTemplates, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiTransferTemplates, SIGNAL(raiseWidget(int)), this, SLOT(slotRaiseWidget(int)));
  connect(_GuiTransferForm, SIGNAL(templateRequest(banking::BankingJob::JobType)),
          _GuiTransferTemplates , SLOT(slotSetTypeRadioButton(banking::BankingJob::JobType)));
  connect(_GuiTransferTemplates, SIGNAL(raiseTemplate(banking::Transfer*)),
          _GuiTransferForm, SLOT(slotShowTemplateData(banking::Transfer*)));
  
  _GuiHardwareConfig = new GuiHardwareConfig(stackedWidget);
  _PageIndex[ HARDWARE_WIDGET ] = stackedWidget->addWidget(_GuiHardwareConfig);
  connect(_GuiHardwareConfig, SIGNAL(finished()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiHardwareConfig, SIGNAL(raiseWidget(int)), this, SLOT(slotRaiseWidget(int)));
  qDebug("GuiHardwareConfig constructed");
/*
  _guidialin = new GuiDialIn(stackedWidget, "DialIn");
  stackedWidget->addWidget(_guidialin, DIALIN_WIDGET);
  connect(_guidialin, SIGNAL(finished()), this, SLOT(slotShowLastVisibleWidget()));
  qDebug("GuiDialIn constructed");
*/
  _GuiKnownBanks = new GuiKnownBanks(stackedWidget);
  _PageIndex[KNOWNBANKS_WIDGET] = stackedWidget->addWidget(_GuiKnownBanks);
  qDebug("GuiKnownBanks constructed");

  _GuiJobs = new GuiJobs(stackedWidget);
  _PageIndex[JOBS_WIDGET] = stackedWidget->addWidget(_GuiJobs);
  connect(_GuiJobs, SIGNAL(jobsCreated()), this, SLOT(slotRefresh()));
  connect(_GuiJobs, SIGNAL(sendNow(bool *, const QString&, bool)), 
          this, SLOT(slotSendNow(bool *, const QString&, bool)));
  connect(_GuiJobs, SIGNAL(editTransferJob(banking::Transfer *)), 
          this, SLOT(slotRaiseTransferTemplate(banking::Transfer *)));
  qDebug("GuiJobs constructed");

  _GuiHbciWizard = new GuiHbciWizard(this);
  _PageIndex[HBCI_WIZARD] = stackedWidget->addWidget( _GuiHbciWizard );
  connect(_GuiHbciWizard, SIGNAL(helpRequest(const QString& )), this, SLOT(slotHelp(const QString& )));
  connect(_GuiHbciWizard, SIGNAL(setupSuccessful(HBCI_MODE)), this, SLOT(slotSetupSuccessful(HBCI_MODE)));
  connect(_GuiHbciWizard, SIGNAL(setupInternet()), this, SLOT(slotHardwareConfig()));
  connect(_GuiHbciWizard, SIGNAL(showLog(int)), this, SLOT(slotRaiseWidget(int)));
  connect(_GuiHbciWizard, SIGNAL(testInternet(bool, const QString& , int, HoBIT_Connector::ConnectionState * )), 
           this, SLOT(slotTestInternet(bool, const QString& , int , HoBIT_Connector::ConnectionState * )));
  connect(_GuiHbciWizard, SIGNAL(renewHoBIT()), this, SLOT(renewHoBIT()));
  connect(_GuiHbciWizard, SIGNAL(addAccount()), this, SLOT(slotAddAccount()));
  connect(_GuiHbciWizard, SIGNAL(dialIn()), this, SLOT(slotDialIn()));
  connect(_GuiHbciWizard, SIGNAL(aborted()), this, SLOT(slotHbciSetupAborted()));
  connect(_GuiHbciWizard, SIGNAL(wizardPageChanged(int)), this, SLOT(slotWizardPageChanged (int)));
  connect(_GuiHbciWizard, SIGNAL(sendJobsNow(bool *, const QString&, bool)), this, SLOT(slotSendNow (bool *, const QString&, bool)));

  qDebug("GuiHbciWizard constructed and connected");

  _GuiAddAccount = new GuiAddAccount(stackedWidget);
  _PageIndex[ADD_ACCOUNT_WIDGET] = stackedWidget->addWidget(_GuiAddAccount);
  connect(_GuiAddAccount, SIGNAL(back()),
          this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiAddAccount, SIGNAL(accountAdded()),
          _GuiHbciWizard, SLOT(slotAccountAdded()));
  qDebug("GuiAddAccount constructed");

  _GuiHelpWindow = new GuiHelpWindow(stackedWidget, MP_DIR() + MONEYPENNY_HELP_DIR + "/index.html");
  _PageIndex[HELP_WIDGET] = stackedWidget->addWidget( _GuiHelpWindow );
  connect(_GuiHelpWindow, SIGNAL(cancelled()), this, SLOT(slotShowLastVisibleWidget()));

  qDebug("Creating guiLogin");

  _GuiLogin = new GuiLogin(stackedWidget);
  _PageIndex[LOGIN_WIDGET] = stackedWidget->addWidget( _GuiLogin );
  qDebug("%s", qPrintable(_GuiLogin->objectName()));
  connect(_GuiLogin, SIGNAL(setCurrentAccount(const QString &, const QString &)), 
          this, SLOT(slotSetCurrentAccount(const QString &, const QString &)));
  connect(_GuiLogin, SIGNAL(raiseWidget(int)), this, SLOT(slotRaiseWidget(int)));
  connect(_GuiLogin, SIGNAL(close()), this, SLOT(close()));

  _GuiDataExport = new GuiDataExport(stackedWidget);
  _PageIndex[EXPORT_WIDGET] = stackedWidget->addWidget( _GuiDataExport );
  connect (_GuiDataExport, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));

 /*
 _guisqlquery = new GuiSqlQuery(stackedWidget, "SqlQuery"); 
  stackedWidget->addWidget(_guisqlquery, SQL_QUERY_WIDGET);
  connect(_guisqlquery, SIGNAL(sqlQuery(QString)), this, SLOT(slotExecuteSqlQuery(QString)));

 _guisqlqueryresult = new GuiSqlQueryResult(stackedWidget, "SqlQueryResult");
  stackedWidget->addWidget(_guisqlqueryresult, SQL_RESULT_WIDGET);
  connect(_guisqlqueryresult, SIGNAL(goback()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_guisqlqueryresult, SIGNAL(exportdata(QTable*)), this, SLOT(slotExport(QTable*)));
*/
  _GuiPrinterAutoSetup = new GuiPrinterAutoSetup(stackedWidget);
  _PageIndex[PRINTER_AUTO_SETUP_WIDGET] = stackedWidget->addWidget( _GuiPrinterAutoSetup );
  connect(_GuiPrinterAutoSetup, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiPrinterAutoSetup, SIGNAL(helpRequest(const QString& )), this, SLOT(slotHelp(const QString&)));
  connect(_GuiPrinterAutoSetup, SIGNAL(showSmallHelp(const QString& )), this, SLOT(slotShowSmallHelp(const QString&)));
  connect(_GuiPrinterAutoSetup, SIGNAL(raiseWidget(int)), this, SLOT(slotRaiseWidget(int)));
  connect(_GuiPrinterAutoSetup, SIGNAL(printerInstalled()), this, SLOT(slotPrinterInstalled()));

  _GuiPrinterManualSetup = new GuiPrinterManualSetup(stackedWidget);
  _PageIndex[PRINTER_MANUAL_SETUP_WIDGET] = stackedWidget->addWidget( _GuiPrinterManualSetup );
  connect(_GuiPrinterManualSetup, SIGNAL(showSmallHelp(const QString& )), this, SLOT(slotShowSmallHelp(const QString&)));
  connect(_GuiPrinterManualSetup, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiPrinterManualSetup, SIGNAL(helpRequest(const QString& )), this, SLOT(slotHelp(const QString&)));
  connect(_GuiPrinterManualSetup, SIGNAL(printerInstalled()), this, SLOT(slotPrinterInstalled()));

  _GuiImport = new GuiImport(stackedWidget);
  _PageIndex[IMPORT_WIDGET] = stackedWidget->addWidget( _GuiImport );
  connect(_GuiImport, SIGNAL(back()), this, SLOT(slotShowLastVisibleWidget()));
  connect(_GuiImport, SIGNAL(importSuccessful()), this, SLOT(slotShowJobs()));

  slotRaiseWidget(LOGIN_WIDGET);

  createMenu();

  //start Timer
  _timer = new QTimer( this );
  connect( _timer, SIGNAL(timeout()), this, SLOT(slotTimerDone()) );
  _timer->start( 1000 ); // 1 second multishot timer

 //Progress indicator
 _GuiProgressIndicator = new GuiProgressIndicator(this);
 
  //show tcp-status
  textLabelConnection->setText(tr("Initialization"));
  _SmallHelp = "smallMain.html";

  connectHobit();

  qDebug("GuiMain::buildMainApplication finished.");
}


/**
*destructor of GuiMain
*/
GuiMain::~GuiMain()
{
  delete _GuiSplash; //_GuiSplash is a top level widget. Needs to be deleted.
  delete _Crypt;
  HoBIT::deleteHoBIT();
  if(_WidgetCallStack)
    delete _WidgetCallStack; 
  qDebug("Destructor GuiMain finished");
}

/**
*Raises the HBCI-Widget on the stackedWidget and resets the banking engine.
* This allows to setup a new bank from scratch. 
*/
void GuiMain::slotHbciSetup()
{
  
  if(QMessageBox::question(this,
      tr("Confirmation"),
      tr("Do you really want to renew the HBCI-setup for the following bank?")+ "\n"+
      tr("Selected bank:") +" "+HoBIT::hobit()->getBankNameForId(HoBIT::hobit()->bankCode()) + " "+
      tr("Bank code:") +" " +HoBIT::hobit()->bankCode(),
      tr("No"), tr("Yes"))!=0)
  {
      slotRaiseWidget(HBCI_WIZARD);
      if(!HoBIT::hobit()->bankCode().isEmpty())
        HoBIT::hobit()->deleteBank(HoBIT::hobit()->bankCode()); //remove bank
      renewHoBIT();
     // _GuiHbciWizard->evaluateStoredData();
      textLabelStatus->setText(tr("HBCI-Configuration"));
      slotRefresh();
  }
  else return;
}

/**
*Raises the toolbox widget 
*/
void GuiMain::slotToolbox()
{
  slotRaiseWidget(TOOLBOX_WIDGET);
  textLabelStatus->setText(tr("Toolbox"));
  slotRefresh();
}

/**
*Raises the Widget, which shows quick information about all hbci accounts 
*/
void GuiMain::slotAccounts()
{

  slotRaiseWidget(ACCOUNT_INFORMATION_WIDGET);
  textLabelStatus->setText(tr("Account Status"));
  //   _guibalances->updateList();
  _GuiAccountInformation->refresh();
}

/**
* asks for execution of the job list and sends if requested.
*/
void GuiMain::slotSend()
{
   switch (askForSend() ){
     case 0: break;
     case 1: slotSendNow(); break;
     case 2: {
               slotGetTransactions();
               slotSendNow(); 
               break;
             } 
     default: break;
    } 
}
/**
*starts execution of the job list . Sets result to true on success. 
*/
void GuiMain::slotSendNow()
{
   bool result;
   slotSendNow(&result, "", true);
}
/**
*starts execution of the job list . Sets result to true on success. 
*/
void GuiMain::slotSendNow(bool * result, const QString& reason, bool showJobList)
{
   qDebug("GuiMain::slotSendNow called.");
   bool ok = true;
   QString pin; //empty pin

   // check for sendable jobs
   if(HoBIT::hobit()->areJobQueuesEmpty())
   {
     QMessageBox::information(this, tr("Information"), tr("Job queue is empty. No jobs to be sent."));
     ok = false;
   }
   if (ok && showJobList)
      slotShowJobs(); //show the job lists and refresh
   else slotRefresh();//just refresh in background

   //check network
    if(ok){
      QString url = HoBIT::hobit()->currentBankServer();
      _Connector->setUrl(url);
      if(HoBIT::hobit()->user()->passportType() & PASSPORT_SECURITY_OTP)
         _Connector->setPort(443);
      else _Connector->setPort(3000);
      qDebug("\nServer: %s", qPrintable(url));
      _Connector->connectTCP();
      HoBIT_Connector::ConnectionState state = _Connector->connectionState();
      ok = (state == HoBIT_Connector::ConnectionConnected);
      _Connector->closeConnection();
    }

   if (ok){
   //try to get a pin for RDH
    if(HoBIT::hobit()->database()->passportType() & PASSPORT_KEYFILE){
        QString why = reason;
        if(why.isEmpty())
         why = tr("Your private key must be activated.");
        DlgEnterPin dlg(this, why);
        dlg.setPinBuffer(&pin);
        ok = dlg.exec();
      }
    }
    if(HoBIT::hobit()->database()->passportType() & PASSPORT_CHIPCARD) //TODO: Add RDH cards
     {
           int country;
           QString bankCode, userId, url, readerName;
          if(HoBIT::hobit()->database()->passportType() & PASSPORT_SECURITY_DDV)
          try{
               HoBIT::hobit()->readDDVCardData(country, bankCode, userId, url, readerName);
          }
          catch (exception & e){ //catch everything here and display a message
            QMessageBox::critical(this, tr("Error while trying to read card data."), banking::from8Bit(e.what()));
            return;
          }
          if(bankCode != HoBIT::hobit()->bankCode() || userId != HoBIT::hobit()->user()->userId().c_str()){
            QMessageBox::critical(this, tr("Error"), 
              tr("There is a wrong card in the reader!")
              + "\n" + tr("Bank data do not match current account. Please exchange card."));
            return;
          }
    }
    if (ok){
       // now go ahead
      // _GuiJobs->setButtonsEnabled(false);
       try{
          ok = HoBIT::hobit()->executeJobLists(pin);
       }
       catch (exception & e){ //catch everything here and display a message
         QMessageBox::critical(this, tr("Error during execution of joblists"), banking::from8Bit(e.what()));
         ok = false;
       }
    }

//     if (!ok && !HoBIT::hobit()->bankMessages().isEmpty())
//          QMessageBox::critical(this, tr("Error"), 
//           tr("An error was detected during execution of job lists. Please check job list for details.")
//           + tr("Error messages received form the bank:")+ "\n"
//           + HoBIT::hobit()->bankMessages());

//    _GuiJobs->setButtonsEnabled(true);

//     slotRefresh(); //finally update all gui elements

    if(result != NULL)
     * result = ok;
}

/**
*Select the accounts for which the transactionlists are to be updated 
*/
void GuiMain::slotGetTransactions()
{
  slotRaiseWidget(GET_TRANSACTIONS_WIDGET);
}


/**
*shows a dialog with the turnovers 
*/
void GuiMain::slotShowTransactions()
{
    slotRaiseWidget(TRANSACTION_LIST_WIDGET);
    textLabelStatus->setText(tr("Turnovers"));
    slotRefresh();
}

/**
*shows a list with the dated transfers 
*/
void GuiMain::slotShowDatedTransfers()
{
    slotRaiseWidget(DATED_TRANSFERS_WIDGET);
    textLabelStatus->setText(tr("Dated Transfers"));
    slotRefresh();
}

/**
*shows a list with the standing orders 
*/
void GuiMain::slotShowStandingOrders()
{
    slotRaiseWidget(STANDING_ORDERS_WIDGET);
    textLabelStatus->setText(tr("Standing Orders"));
    slotRefresh();
}

/**
*creates a single transfer 
*/
void GuiMain::slotSingleTransfer()
{
  _GuiTransferForm->setType(banking::Transfer::SingleTransfer);
  slotRaiseWidget(TRANSFER_WIDGET);
  textLabelStatus->setText(tr("Single Transfer"));
  slotRefresh();
}

/**
*creates a dated transfer
*/
void GuiMain::slotDatedTransfer()
{
  _GuiTransferForm->setType(banking::Transfer::DatedTransfer);
   slotRaiseWidget(TRANSFER_WIDGET);
   textLabelStatus->setText(tr("Dated Transfer"));
   slotRefresh();
}

/**
*calls a transfer template. This is a user defined mask for frequently used transfers. 
*/
void GuiMain::slotTransferTemplate()
{
  slotRaiseWidget(TRANSFER_TEMPLATES_WIDGET);
  textLabelStatus->setText(tr("Transfer Template"));
 }
/**
 * Opens GuiImport
 */
  void GuiMain::slotImport()
  {
	  slotRaiseWidget(IMPORT_WIDGET);
  }

/**
*creates a standing order
*/
void GuiMain::slotStandingOrder()
{
  slotRaiseWidget(NOT_IMPLEMENTED_WIDGET);
//  slotRaiseWidget(STANDING_ORDERS_WIDGET);
  textLabelStatus->setText(tr("Standing Order"));
  slotRefresh();
}


/**
*Update slots
*/
void GuiMain::slotUpdate()
{
  //HoBIT::hobit()->createJobGetAccounts();
  HoBIT::hobit()->createJobGetBalance("*");
  HoBIT::hobit()->createJobGetTransactions(QDate(2000, 1, 1), QDate::currentDate(), "*");
  HoBIT::hobit()->createJobGetDatedTransfers(QDate(), QDate(), "*");
  HoBIT::hobit()->createJobGetStandingOrders("*");
  slotSendNow(NULL, tr("Requesting all data available at the bank."));
}


/**
*Raise widget for undone jobs 
*/
void GuiMain::slotShowJobs()
{
  slotRaiseWidget(JOBS_WIDGET);
  textLabelStatus->setText(tr("Job-Queue"));
  slotRefresh();
}

/**
*Raise jobs widget and show progress dialog with protocol of last run. 
*/
void GuiMain::slotShowJobProtocol()
{
  slotRaiseWidget(JOBS_WIDGET);
  textLabelStatus->setText(tr("Job-Queue"));
  slotRefresh();
  _GuiProgressIndicator->setStatus(GuiProgressIndicator::DisplayLastProtocol);
  _GuiProgressIndicator->show();
}

/**
*Raise hardware widget in CONNECT mode 
*/
void GuiMain::slotDialIn()
{
 if(USE_LIVE_CD_FEATURES)
  system( qPrintable(sudo("pon dsl-provider", this)));
}

/**
*refresh informations, lists and widgets
*/
void GuiMain::slotRefresh()
{

  _GuiAccountInformation->refresh();
//  _GuiHbciWizard->updateAccountList();
  _GuiLogin->refresh();
  _GuiTools->refresh();
  _GuiTools->setTimeOut(_Connector->getTimeOut());
  _GuiTransactionList->refresh();
//  _GuiShowBankData->refresh();
//  _guistandingorders->refresh();
  _GuiDatedTransfers->refresh();
//  _GuiTransactionPrinter->refresh();
  _GuiTransferTemplates->refresh();
  _GuiTransactionInfo->refresh();
  _GuiTransferForm->refresh();
  _GuiDatedSingleTransfer->refresh();
//  _guistartstandingorder->refresh();
  _GuiJobs->refresh(); 
  _GuiJobs->setMountPoint(_MountPoint);
}

/**
*Building Menu by adding items and setting labels, bottom up
*/
void GuiMain::createMenu()
{  
   treeWidgetMenu->setSortingEnabled(false); //switch off sorting

  _ChangeBankItem= new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Change Storage Device"))); 

 // Qt::ItemFlags flags = _changeBankItem->flags(); //debug only
  _SelectAccountItem= new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Select an Account")));


  _AccountItem= new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Account Status")));
  _AccountBalanceItem= new QTreeWidgetItem(_AccountItem, QStringList(tr("Show Account Status")));
  _GetTransactionsItem = new QTreeWidgetItem(_AccountItem, QStringList(tr("Get Turnovers")));
  _ShowTransactionsItem = new QTreeWidgetItem(_AccountItem, QStringList(tr("Show Turnovers")));
  _ShowDatedTransfersItem = new QTreeWidgetItem(_AccountItem, QStringList(tr("Show Dated Transfers")));
  _ShowStandingOrdersItem = new QTreeWidgetItem(_AccountItem, QStringList(tr("Show Standing Orders")));

  _PrintTransactionsItem = new QTreeWidgetItem(_AccountItem, QStringList(tr("Print Turnovers")));
  _PrintTransactionsItem->setHidden(true);
  _ExportTransactionsItem = new QTreeWidgetItem(_AccountItem, QStringList(tr("Export Turnover Data")));
  _ExportTransactionsItem->setHidden(true);

  _PaymentItem = new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Payment")));
  _TransferTemplateItem = new QTreeWidgetItem(_PaymentItem, QStringList(tr("Transfer Template")));
  _SingleTransferItem = new QTreeWidgetItem(_PaymentItem, QStringList(tr("Single Transfer")));
  _DatedTransferItem = new QTreeWidgetItem(_PaymentItem, QStringList(tr("Dated Transfer")));
  _StandingOrderItem = new QTreeWidgetItem(_PaymentItem, QStringList(tr("Standing Orders")));
  _ImportItem = new QTreeWidgetItem(_PaymentItem, QStringList(tr("DTA Import")));

  _JobItem = new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Pending Jobs")));
  _JobQueueItem = new QTreeWidgetItem(_JobItem, QStringList(tr("Show Job-Queue")));
  _JobProtocolItem = new QTreeWidgetItem(_JobItem, QStringList(tr("Last Transmission Protocol")));
  _SendNowItem = new QTreeWidgetItem(_JobItem, QStringList(tr("Execute Job Queue")));

  if(USE_LIVE_CD_FEATURES)
   _ConnectItem = new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Internet Dial In")));
  else _ConnectItem = NULL;

  _ConfigItem = new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Setup")));
  if(USE_LIVE_CD_FEATURES || DEBUG_HARDWARE)
   _HardwareItem = new QTreeWidgetItem(_ConfigItem, QStringList(tr("Hardware")));
  else _HardwareItem = NULL;
  _HbciItem = new QTreeWidgetItem(_ConfigItem, QStringList(tr("HBCI-Setup")));
  _AddBankItem= new QTreeWidgetItem(_ConfigItem, QStringList(tr("Add Bank"))); 
  _UpdateAccountList= new QTreeWidgetItem(_ConfigItem, QStringList(tr("Update Account List"))); 
  _ShowDataItem = new QTreeWidgetItem(_ConfigItem, QStringList(tr("Show User Data")));
  _ToolboxItem = new QTreeWidgetItem(_ConfigItem, QStringList(tr("Toolbox")));
  _CreateBackupMediumItem = new QTreeWidgetItem(_ConfigItem, QStringList(tr("Create Backup Medium")));
  _ChangeHbciPasswordItem = new QTreeWidgetItem(_ConfigItem, QStringList(tr("Change HBCI-Password")));
  _ChangeLoginPasswordItem = new QTreeWidgetItem(_ConfigItem, QStringList(tr("Change Login-Password")));

  _HelpItem = new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Help")));
  _KnownBanksItem = new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("German Banks")));
  _UpdateItem = new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Program Update")));

  _ExitItem = new QTreeWidgetItem(treeWidgetMenu, QStringList(tr("Exit")));

  _ClickedItem = NULL;
}

/**
* Runs associated slot after pressing an item in main menu
* Calls executeCommand
*/
void GuiMain::on_treeWidgetMenu_itemClicked( QTreeWidgetItem * item, int ){
  _ClickedItem = item;
  if(item == NULL) //return on first activation
    return;
  else executeCommand(item);
}
/**
* Runs associated slot after pressing an item in main menu
* Calls executeCommand
*/
void GuiMain::on_treeWidgetMenu_itemActivated( QTreeWidgetItem * item, int ){
  if(item == NULL || _ClickedItem == item){ //return on first activation, do not run a command twice
    _ClickedItem = NULL;
    return;
  }
  else executeCommand(item);
  _ClickedItem = NULL;
}

/** Activate corresponding treewidgetitem
 */
void GuiMain::on_buttonAccounts_clicked()
{
  treeWidgetMenu->setCurrentItem(_AccountItem);
  slotAccounts();
}

/** Activate corresponding treewidgetitem
 */
void GuiMain::on_buttonSingleTransfer_clicked()
{
  treeWidgetMenu->setCurrentItem(_SingleTransferItem);
  slotSingleTransfer();
}

/** Activate corresponding treewidgetitem
 */
void GuiMain::on_buttonShowTransactions_clicked()
{
  treeWidgetMenu->setCurrentItem(_ShowTransactionsItem);
  slotShowTransactions();
}

/** Activate corresponding treewidgetitem
 */
void GuiMain::on_buttonUpdate_clicked()
{
  treeWidgetMenu->setCurrentItem(_UpdateItem);
  slotUpdate();
}

/** Activate corresponding treewidgetitem
 */
void GuiMain::on_buttonSendNow_clicked()
{
  treeWidgetMenu->setCurrentItem(_SendNowItem);
  slotSendNow();
}

/** Activate corresponding treewidgetitem
 */
void GuiMain::on_buttonHelp_clicked()
{
  treeWidgetMenu->setCurrentItem(_HelpItem);
  slotHelp();
}

/** Activate corresponding treewidgetitem
 */
void GuiMain::on_buttonToolbox_clicked()
{
  treeWidgetMenu->setCurrentItem(_ToolboxItem);
  slotToolbox();
}

/** Executes command associated to item.
*/
void GuiMain::executeCommand(QTreeWidgetItem * item)
{ 

  if(item == NULL) //return if NULL
    return;
  else if (treeWidgetMenu->currentItem() == _AccountItem ) slotShowWelcome("smallAccounts.html"); 
  else if (treeWidgetMenu->currentItem() == _ChangeBankItem ) slotChangeBank();
  else if (treeWidgetMenu->currentItem() == _SelectAccountItem ) slotAccountLogin();
  else if (treeWidgetMenu->currentItem() == _AccountBalanceItem ) slotAccounts();
  else if (treeWidgetMenu->currentItem() == _UpdateAccountList ) slotUpdateAccountList();
  
  else if (treeWidgetMenu->currentItem() == _GetTransactionsItem ) slotGetTransactions();
  else if (treeWidgetMenu->currentItem() == _ShowTransactionsItem ) slotShowTransactions();
  else if (treeWidgetMenu->currentItem() == _ShowDatedTransfersItem ) slotShowDatedTransfers();
  else if (treeWidgetMenu->currentItem() == _ShowStandingOrdersItem ) slotShowStandingOrders();
  else if (treeWidgetMenu->currentItem() == _PrintTransactionsItem ) slotShowTransactions();
  else if (treeWidgetMenu->currentItem() == _ExportTransactionsItem ) slotShowTransactions();
  
  else if (treeWidgetMenu->currentItem() == _PaymentItem )  slotShowWelcome("smallPayment.html"); 
  else if (treeWidgetMenu->currentItem() == _SingleTransferItem ) slotSingleTransfer();
  else if (treeWidgetMenu->currentItem() == _DatedTransferItem ) slotDatedTransfer();
  else if (treeWidgetMenu->currentItem() == _TransferTemplateItem ) slotTransferTemplate();
  else if (treeWidgetMenu->currentItem() == _StandingOrderItem ) slotStandingOrder();
  else if (treeWidgetMenu->currentItem() == _ImportItem ) slotImport();
   
  else if (treeWidgetMenu->currentItem() == _ConfigItem )  slotShowWelcome("smallConfig.html"); 
  else if (treeWidgetMenu->currentItem() == _HbciItem ) slotHbciSetup();
  else if (treeWidgetMenu->currentItem() == _AddBankItem ) slotAddBank();
  else if (treeWidgetMenu->currentItem() == _ChangeLoginPasswordItem ) slotChangeLoginPassword();
  else if (treeWidgetMenu->currentItem() == _ChangeHbciPasswordItem ) slotChangeHbciPassword();
  else if (treeWidgetMenu->currentItem() == _CreateBackupMediumItem ) slotCreateBackupMedium();
  else if (treeWidgetMenu->currentItem() == _HardwareItem ) slotHardwareConfig();
  else if (treeWidgetMenu->currentItem() == _ToolboxItem ) slotToolbox();
  else if (treeWidgetMenu->currentItem() == _ShowDataItem ) slotShowAccountData();
  
  else if (treeWidgetMenu->currentItem() == _ConnectItem )  slotDialIn(); 
  else if (treeWidgetMenu->currentItem() == _JobItem )  slotShowWelcome("smallJobs.html"); 
  else if (treeWidgetMenu->currentItem() == _JobQueueItem ) slotShowJobs();
  else if (treeWidgetMenu->currentItem() == _JobProtocolItem ) slotShowJobProtocol();
  else if (treeWidgetMenu->currentItem() == _SendNowItem ) slotSendNow(NULL, "");
  
  else if (treeWidgetMenu->currentItem() == _KnownBanksItem ) slotListKnownBanks();
  
  else if (treeWidgetMenu->currentItem() == _HelpItem ) slotHelp("index.html");

  else if (treeWidgetMenu->currentItem() == _UpdateItem ) slotUpdateProgram();
  
  else if (treeWidgetMenu->currentItem() == _ExitItem ) slotClose();

 //qDebug("executeCommand");
}


/**
*referring to statusbar:
*- get internet connectionState from Connector
*- update date and time
*/
void GuiMain::slotTimerDone()
{
  updateBalanceStatus();

  HoBIT_Connector::ConnectionState connectionState = _Connector->connectionState();
  switch (connectionState){
    case HoBIT_Connector::ConnectionUntested  : {
                                 textLabelConnection->setPalette(PALETTE_YELLOW());
                                 textLabelConnection->setText(" " +tr("Internet: Untested") + " ");
                                 break;
                               } 
    case HoBIT_Connector::ConnectionConnecting: {
                                 textLabelConnection->setPalette(PALETTE_YELLOW());
                                 textLabelConnection->setText(" " + tr("Internet: Connecting") + " ");
                                 break;
                               } 
    case HoBIT_Connector::ConnectionFailed    : {
                                 textLabelConnection->setPalette(PALETTE_RED());
                                 textLabelConnection->setText(" " + tr("Internet: Failed") + " ");
                                 break;
                               } 
    case HoBIT_Connector::ConnectionConnected : {
                                 textLabelConnection->setPalette(PALETTE_GREEN());
                                 textLabelConnection->setText(" " + tr("Internet: Online") + " ");
                                 break;
                               } 
    case HoBIT_Connector::ConnectionClosed    : {
                                textLabelConnection->setPalette(PALETTE_YELLOW());
                                textLabelConnection->setText(" " + tr("Internet: Offline") + " ");
                                break;
                               } 
    case HoBIT_Connector::ConnectionWasOk     : {
                                 textLabelConnection->setPalette(PALETTE_YELLOW());
                                 textLabelConnection->setText(" " + tr("Internet: Ok") + " ");
                                 break;
                               }

   default                   : {
                                 textLabelConnection->setPalette(PALETTE_YELLOW());
                                 textLabelConnection->setText(" " + tr("Internet: ??") + " ");
                                 break;
                               } 
  }
  QDate currentDate;
  QTime currentTime;
  QString date = currentDate.currentDate().toString(tr("yyyy-MM-dd", "The date format for the clock label"));
  QString time = currentTime.currentTime().toString("hh:mm");
  QString datetime = date + " - " + time;
  textLabelDate->setText(datetime);
}

/**
*refresh account-balance
*/
void GuiMain::updateBalanceStatus()
{
  banking::Account * account= HoBIT::hobit()->currentAccount();
  if (account)
  {
      DateTime d = account->balanceDate();
      QDate balanceDate = QDate(d.yearNumber(), d.monthNumber(), d.dayNumber());
      labelBalanceDate->setText(balanceDate.toString(tr("yyyy-MM-dd", "The date format for the balance date"))
       + tr(":", "The colon between balance date and balance amount") );

      labelAccount->setText(account->accountId().c_str());

      banking::Value balance = account->balance();
      QString sBalance = balance.readableString().c_str();
      labelBalance->setText(sBalance);
      if(balance.isCredit())
        labelBalance->setPalette(PALETTE_BLACK_TEXT());
      else labelBalance->setPalette(PALETTE_RED_TEXT()); 
   }
    else{ //no account
      labelBalanceDate->setText("");
      labelBalance->setText(tr("Unknown"));
      labelAccount->setText(tr("No Account"));
    }
}

/**
*Prints the transactions.
*/
void GuiMain::slotPrintTransactions(banking::TransactionList *transactionList, const QDate & from, const QDate & to)
{
   if (!_PrintTransactionsItem->isSelected()) //synchronize menu
    _PrintTransactionsItem->setSelected( true);
   if(transactionList == NULL){
    HoBIT::hobit()->setFromTo(from, to);
    _GuiTransactionPrinter->setTransactionList(HoBIT::hobit()->listTransactions());
   }
   else _GuiTransactionPrinter->setTransactionList(transactionList);
  slotRaiseWidget(TRANSACTIONS_PRINTER_WIDGET);
}

/**
*Call slotHelp (QString&)
*/
void GuiMain::slotHelp()
{
 slotHelp("index.html");
}

/**
*Open the help window
*@param helpfile a helping QString
*/
void GuiMain::slotHelp(const QString & helpfile)
{
  _GuiHelpWindow->setHelpfile (MP_DIR() + MONEYPENNY_HELP_DIR  + "/" +  helpfile);
  slotRaiseWidget(HELP_WIDGET);
}

/**
*Raise widget with list of known banks
*/
void GuiMain::slotListKnownBanks()
{
  slotRaiseWidget(KNOWNBANKS_WIDGET);
  textLabelStatus->setText(tr("german banks"));
}


/**
*set account to be used from moneypenny after login 
*@param ca QString for the current account
*/
void GuiMain::slotSetCurrentAccount(const QString &bankCode, const QString &ca)
{
  DlgStatusMessage dlg(this);
  connect(HoBIT::hobit(), SIGNAL(statusMessage(const QString&)),
          &dlg, SLOT(setLabelTopRightText(const QString&)));
  connect(HoBIT::hobit(), SIGNAL(processingJob(int)),
          &dlg, SLOT(setLabelBottomRightNumber(int)));
  dlg.show();
  HoBIT::hobit()->setCurrentAccountId(bankCode, ca);
  slotActivateMenuFunctions();
  slotRefresh();


  if (HoBIT::hobit()->users()->empty() || ! HoBIT::hobit()->isCurrentAccountValid()){//No User or no valid account: Most probably we need to continue RDH setup
           _GuiHbciWizard->evaluateStoredData();
           _GuiHbciWizard->showPage(GuiHbciWizard::CONNECT_PAGE);
           slotRaiseWidget(HBCI_WIZARD);
     }
  else{
     slotRaiseWidget(ACCOUNT_INFORMATION_WIDGET);
     textLabelStatus->setText(tr("Account "));
   }
}


/**
*Raise widget for ini-letter
*/
void GuiMain::slotShowIniLetter()
{
  QString password;
  DlgEnterPin dlg(this, tr("The password for your key is required to generate the Ini-Letter"));
  dlg.setPinBuffer(&password);
  QString iniLetter;
  try{
  if( dlg.exec()){
     QString customerName;
     banking::User * user = HoBIT::hobit()->user();
     if(user)
       customerName= user->userName().c_str();
     customerName = QInputDialog::getText(this, tr("Customer Name"), tr("Customer Name for Ini-Letter:"), QLineEdit::Normal, customerName);
     iniLetter = HoBIT::hobit()->createIniLetter(password, customerName);
  }
  else iniLetter = tr("No Password given. Could not create Ini-Letter.");
   _GuiIniLetter->setIniLetter(iniLetter);
  }
  catch(exception & e){
    QMessageBox::critical(this, tr("Error"), tr("Error when generating the ini-letter. Reason:") + e.what());
  }
  slotRaiseWidget(INILETTER_WIDGET);
}

/**
*Switch Account by raise login-widget
*/
void GuiMain::switchAccount()
{
  slotAccountLogin();
}

/**
*Show last visible widget
*/
void GuiMain::slotShowLastVisibleWidget()
{
  int widgetId = _WidgetCallStack->pop();
  stackedWidget->setCurrentIndex(widgetId);
}

/**
*Raise widget for add account
*/
void GuiMain::slotAddAccount()
{
  slotRaiseWidget(ADD_ACCOUNT_WIDGET);
}

/** Deletes the account accountid by deleting its account directory.
*/
void GuiMain::slotDeleteAccount(QString accountId){
 HoBIT::hobit()->deleteAccount(accountId);
 //renewHoBIT();
 slotRefresh();
 slotRaiseWidget(LOGIN_WIDGET);
}

/**
*Raise widget for hardware-configure-dialog
*/
void GuiMain::slotHardwareConfig()
{
  slotRaiseWidget(HARDWARE_WIDGET);
  textLabelStatus->setText(tr("Hardware Setup"));
  _SaveKnoppixConfig = true;
  qDebug("GuiMain::slotHardwareConfig: setting SaveKnoppixConfig flag");
}


/**
*Raise Login-Dialog and HBCI-Wizard
*/
void GuiMain::slotAccountLogin()
{
  treeWidgetMenu->setCurrentItem(_SelectAccountItem);
  slotDeactivateMenuFunctions();
  const banking::AccountList * accounts= HoBIT::hobit()->accounts();
  const banking::UserList * users = HoBIT::hobit()->users();
 //check whether selected account is valid
  bool accountIsValid = ! HoBIT::hobit()->currentAccountId().isEmpty(); //no account set
  if(accountIsValid)
    accountIsValid = ! HoBIT::hobit()->currentAccountId().toDouble() != 0.0; //accountId is 00000000
  if(users == NULL || accounts == NULL || users->empty()){ // no user, start from scratch
         _GuiHbciWizard->showPage(GuiHbciWizard::WELCOME_PAGE);
          slotRaiseWidget(HBCI_WIZARD);
  }
/*  else if (!users->empty() && !accountIsValid){//User, but no account: Most probably we need to continue RDH setup
         _GuiHbciWizard->evaluateStoredData();
         _GuiHbciWizard->showPage(GuiHbciWizard::CONNECT_PAGE);
         slotRaiseWidget(HBCI_WIZARD);
   }*/
  else //we have got an account, show the overview
  {
    _GuiLogin->refresh();
    slotRaiseWidget(LOGIN_WIDGET);
  }
}

/**
*Close main window after confirming
*/
void GuiMain::slotClose()
{
 _exitCode = EXITCODE_SUCCESS;
 close();
}

/**
* Change bank by shutting down and restarting with new medium 
*/
void GuiMain::slotChangeBank()
{
 _exitCode = EXITCODE_RUN_BANK_LOOP;
 close();
}

/**
*Raises widget with given id
*Builds the name of the small help file from widget name
*@param id id of the widget which should be raised 
*/
void GuiMain::slotRaiseWidget(int id)
{ 
  if((id >=0) && stackedWidget->currentWidget())
  {
    int currentPage = stackedWidget->currentIndex();
    if(currentPage != _PageIndex[id]){ 
       _WidgetCallStack->push(stackedWidget->currentIndex()); //save current page index
       stackedWidget->setCurrentIndex(_PageIndex[id]);  //finally raise the new widget
       QWidget * widget = stackedWidget->currentWidget();
       qDebug("Raising widget %i Name: %s", id, qPrintable(widget->objectName()));
       #ifdef DEBUG_PAGES
          for(int i = 0 ; i < stackedWidget->count(); i++){
            widget = stackedWidget->widget(i);
            qDebug("Page %i Name: %s", i, qPrintable(widget->objectName()));
           }
       #endif
       if(id == HBCI_WIZARD)
         slotWizardPageChanged(_GuiHbciWizard->currentPageIndex());
       else {
          QString helpFileName = widget->objectName();
          helpFileName.remove("Gui"); //strip leading Gui
          helpFileName = "small" + helpFileName + ".html";
         _SmallHelp = helpFileName; //Use the name as identifier for help file control
         slotShowSmallHelp(_SmallHelp); //now show it
       }
    }
  }
}

/** Updates _CurrentWizardPage and contents of small help widget
*/
void GuiMain::slotWizardPageChanged(int newPage){
    _CurrentWizardPage = newPage; 
    if(stackedWidget->currentWidget() == (QWidget*)_GuiHbciWizard){  //we are on top, show the small help file    
      //build the name of the small help file from widget name
      int page = _CurrentWizardPage;
      if(page < 0)
         page = 0; //do not call help for negative numbers
      QString helpFileName = _GuiHbciWizard->objectName();
      helpFileName.remove("Gui"); //strip leading Gui
      helpFileName = "small" + helpFileName + QString::number(page)+ ".html";
     _SmallHelp = helpFileName; //Use the name as identifier for help file control
      slotShowSmallHelp(_SmallHelp); //now show it
    }
}

/** Displays helpFile in the small help widget at the lower left.
*/
void GuiMain::slotShowSmallHelp(const QString & helpFile){
  _SmallHelp = helpFile;
  QString fileName = MP_DIR()  + MONEYPENNY_HELP_DIR + "/" + helpFile;
  QFile file( fileName); // Read the text from a file
  if ( file.open( QIODevice::ReadOnly ) )
  {
    QTextStream stream( &file );
    textEditHelp->setHtml( stream.readAll() );
    file.close();
  }
  else QMessageBox::warning(this, tr("File Error"), tr("Could not open help file : ") 
                                 + fileName + tr("<br>Please check your installation."));
}


/**
*function for closing
*/
void GuiMain::closeEvent(QCloseEvent * ce)
{
  if (! HoBIT::hobit()->areJobQueuesEmpty()){ //always ask for jobs
    if (QMessageBox::question( this,
                              tr("MoneyPenny Shutdown"),
                              tr("You have unsent jobs in your job queue. \nDo you really want to close MoneyPenny (Unsent jobs will be lost)?"),
                              tr("&Ok"), tr("&Cancel"),
                              QString::null, 0, 1 )){
	 ce->ignore();
	 return;
   }
  }
 
  //exit without saving
  if(_exitCode == EXITCODE_NO_SAVING){
    ce->accept();
    return;
  }

  if (_exitCode == EXITCODE_RESTART_SESSION){//immediate shutdown and restart.
    QMessageBox::information(this,
               tr("Information"),
               tr("MoneyPenny will now terminate the currrent session without saving data.") +"\n"+
               tr("After this it will start a new session.")); 
               HoBIT::hobit()->removeUncryptedEnvironment();
               ce->accept();
               QApplication::exit(_exitCode);
               return;
  } 


  //exit with saving
  QString sStorage;
  switch (_Storage){
    case STORAGE_FLOPPY  : sStorage = tr("floppy disk in the drive", 
                           "Replacement for sStorage in: please leave your  sStorage  and press Ok.");break;
    case STORAGE_USB     : sStorage = tr("USB stick plugged in", 
                           "Replacement for sStorage in: please leave your  sStorage  and press Ok.");    break;
    case STORAGE_UNKNOWN : sStorage = tr("storage device in place", 
                           "Replacement for sStorage in: please leave your  sStorage  and press Ok."); break;
    default              : sStorage = tr("storage device in place", 
                           "Replacement for sStorage in: please leave your  sStorage  and press Ok."); break;
  }
  bool bSaveData;
  if (_exitCode == EXITCODE_SUCCESS){
     int result = QMessageBox::question
               ( this,
               tr("MoneyPenny Shutdown"),
               tr("If you want to close MoneyPenny, \nplease leave your ") + sStorage + tr(" and press Ok."),
               tr("&Ok"), 
               tr("&Cancel"));
               bSaveData = result == 0; //First (ok) button has been clicked
   } 
  else if (_exitCode == EXITCODE_RUN_BANK_LOOP){
      int result =QMessageBox::question
               ( this,
               tr("MoneyPenny Changing Bank"),
               tr("MoneyPenny will now save all data for your present bank, \nplease leave your ") + sStorage + tr(" and press Ok."),
               tr("&Ok"), 
               tr("&Cancel"));
               bSaveData = result == 0 ;//First (ok) button has been clicked
  } 
  else bSaveData = false;
  if (bSaveData){  
        textLabelStatus->setText(tr("Saving data and configuration"));      
        qApp->processEvents();
        switch (_Storage){
          case STORAGE_FLOPPY  : sStorage = tr("floppy disk", 
                                 "This is the end of the sentence: Could not write to your floppy disk");    break;
          case STORAGE_USB     : sStorage = tr("USB stick", 
                                 "This is the end of the sentence: Could not write to yourUSB stick");       break;
          case STORAGE_UNKNOWN : sStorage = tr("storage device", 
                                 "This is the end of the sentence: Could not write to your storage device"); break;
          default              : sStorage = tr("storage device", 
                                 "This is the end of the sentence: Could not write to your storage device"); break;
        }

        if(_AutoShutdown && (_Storage == STORAGE_FLOPPY)) //do not save logfiles on a floppy, if everything was ok
          slotDeleteLogFiles();

        if(_SaveKnoppixConfig)
           slotSaveOsConfig();

        if(!HoBIT::hobit()->saveEnvironment()){ //save HBCI configuration to ConfigDataDir. 
          bool closeAnyway= QMessageBox::question( this,
                              tr("Error"),
                              tr(" <b>Saving of data failed.</b>"),
                              tr("&Cancel"), 
                              tr("&Close Anyway"),
                              QString::null, 0, 1 ) == 1;
          if(closeAnyway) 
            ce->accept();
          else ce->ignore();
          return;
        }

        //remove ConfigDataDir
        if(encryptData())  //First compress and encrypt ConfigDataDir.
          HoBIT::hobit()->removeUncryptedEnvironment();

        switch (_Storage){
          case STORAGE_FLOPPY  : sStorage = tr("floppy disk", 
                                 "Replacement for sStorage in: Please remove your  sStorage  now and press Ok."); break;
          case STORAGE_USB     : sStorage = tr("USB stick", 
                                 "Replacement for sStorage in: Please remove your  sStorage  now and press Ok."); break;
          case STORAGE_UNKNOWN : sStorage = tr("storage device", 
                                 "Replacement for sStorage in: Please remove your  sStorage  now and press Ok."); break;
          default              : sStorage = tr("storage device", 
                                 "Replacement for sStorage in: Please remove your  sStorage  now and press Ok."); break;
        }

       system("sync"); //make shure, that everything has been written
       bool ok = true;
          #ifdef LIVE_CD
         _AutoShutdown = QMessageBox::question
         ( this,
          tr("MoneyPenny Shutdown"),
          tr("Data have been encrypted and saved. \nPlease remove your ") + sStorage 
          + tr(" after the indicator lamp of the drive switches off.")
          +tr("Shall MoneyPenny switch off the computer now?")
          ,
          tr("&Yes, shutdown computer.")  , tr("&No, I want to continue working."))
         == 0 ;
        #else
       ok = QMessageBox::question
         ( this,
          tr("MoneyPenny Shutdown"),
          tr("Data have been encrypted and saved. \nPlease remove your ") + sStorage 
          + tr(" after the indicator lamp of the drive switches off and press Ok.")
          ,
          tr("&OK")  , tr("&Cancel"))
         == 0 ;
         #endif

       if(ok){
        textLabelStatus->setText(tr("Terminating session")); 
        if(USE_LIVE_CD_FEATURES && _AutoShutdown)
         _exitCode = EXITCODE_AUTOSHUTDOWN;
        QApplication::exit(_exitCode); 
       } else ce->ignore(); // cancel was selected after data saving
  }else ce->ignore(); // cancel was selected after first question   
   
}

/**
*Deactivate Buttons and List
*/
void GuiMain::slotDeactivateMenuFunctions()
{
  Qt::ItemFlags flagsDisabled = 0;
  buttonAccounts->setEnabled(false);
  buttonSingleTransfer->setEnabled(false);
  buttonShowTransactions->setEnabled(false);
  buttonUpdate->setEnabled(false);
  buttonSendNow->setEnabled(false);
  //buttonToolbox->setEnabled(false);
 
  _AccountItem->setFlags(flagsDisabled);
  _PaymentItem->setFlags(flagsDisabled);
  _JobItem->setFlags(flagsDisabled);
  //_ConfigItem->setFlags(flagsDisabled);
  if(_ConnectItem)
      _ConnectItem->setFlags(flagsDisabled);
  if(HoBIT::hobit()->accounts() && HoBIT::hobit()->accounts()->empty()){
      _SelectAccountItem->setFlags(flagsDisabled);
      _AccountBalanceItem->setFlags(flagsDisabled);
   }
  _AccountBalanceItem->setFlags(flagsDisabled);
  _UpdateAccountList->setFlags(flagsDisabled);
  
  _GetTransactionsItem->setFlags(flagsDisabled);
  _ShowTransactionsItem->setFlags(flagsDisabled);
  _PrintTransactionsItem->setFlags(flagsDisabled);
  _ExportTransactionsItem->setFlags(flagsDisabled);
  
  _SingleTransferItem->setFlags(flagsDisabled);
  _DatedTransferItem->setFlags(flagsDisabled);
  _TransferTemplateItem->setFlags(flagsDisabled);
  _StandingOrderItem->setFlags(flagsDisabled);
  
  //_jobQueueItem->setFlags(flagsDisabled);
  _JobProtocolItem->setFlags(flagsDisabled);
  _SendNowItem->setFlags(flagsDisabled);
  _GuiJobs->setButtonsEnabled(false);
  
  //_ChangeLoginPasswordItem->setFlags(flagsDisabled);
  //_ChangeHbciPasswordItem->setFlags(flagsDisabled);
  //_CreateBackupMediumItem->setFlags(flagsDisabled);
  //_ToolboxItem->setFlags(flagsDisabled);
  //_ShowDataItem->setFlags(flagsDisabled);
}

/**
*Activate Buttons and List
*/
void GuiMain::slotActivateMenuFunctions()
{
  Qt::ItemFlags flagsEnabled = Qt::ItemIsSelectable
                             | Qt::ItemIsDragEnabled
                             | Qt::ItemIsDropEnabled
                             | Qt::ItemIsUserCheckable
                             | Qt::ItemIsEnabled;
  
  buttonAccounts->setEnabled(true);
  buttonSingleTransfer->setEnabled(true);
  buttonShowTransactions->setEnabled(true);
  buttonUpdate->setEnabled(true);
  buttonSendNow->setEnabled(true);
  buttonToolbox->setEnabled(true);
 // treeWidgetMenu->setEnabled(true);
  
  _PaymentItem->setFlags(flagsEnabled);
  _JobItem->setFlags(flagsEnabled);
  _ConfigItem->setFlags(flagsEnabled);
  if(_ConnectItem) _ConnectItem->setFlags(flagsEnabled);

  _AccountItem->setFlags(flagsEnabled);
  _UpdateAccountList->setFlags(flagsEnabled);
  _SelectAccountItem->setFlags(flagsEnabled);
  _AccountBalanceItem->setFlags(flagsEnabled);
  _GetTransactionsItem->setFlags(flagsEnabled);
  _ShowTransactionsItem->setFlags(flagsEnabled);
  _PrintTransactionsItem->setFlags(flagsEnabled);
  _ExportTransactionsItem->setFlags(flagsEnabled);
  _SingleTransferItem->setFlags(flagsEnabled);
  _DatedTransferItem->setFlags(flagsEnabled);
  _TransferTemplateItem->setFlags(flagsEnabled);
  _StandingOrderItem->setFlags(flagsEnabled);
  _SendNowItem->setFlags(flagsEnabled);
  _HbciItem->setFlags(flagsEnabled);
  _ChangeLoginPasswordItem->setFlags(flagsEnabled);
  _ChangeHbciPasswordItem->setFlags(flagsEnabled);
  _CreateBackupMediumItem->setFlags(flagsEnabled);
  _ToolboxItem->setFlags(flagsEnabled);
  _ShowDataItem->setFlags(flagsEnabled);
  if(_HardwareItem) 
     _HardwareItem->setFlags(flagsEnabled);
  _HelpItem->setFlags(flagsEnabled);
  _UpdateItem->setFlags(flagsEnabled);
  _ExitItem->setFlags(flagsEnabled);
  _KnownBanksItem->setFlags(flagsEnabled);
  _JobQueueItem->setFlags(flagsEnabled);
  _JobProtocolItem->setFlags(flagsEnabled);
  _JobProtocolItem->setFlags(flagsEnabled);
  _GuiJobs->setButtonsEnabled(true);
}

/** Read property of HoBIT_Connector m_Connector. 
*/
HoBIT_Connector * GuiMain::getConnector()const{
	return _Connector;
}

/**
*shows welcome widget 
*/
void GuiMain::slotShowWelcome(const QString & smallHelpFile){
 textLabelStatus->setText("Welcome"); 
 slotRaiseWidget(WELCOME_WIDGET);
 slotShowSmallHelp(smallHelpFile); //override the settings made by slotRaiseWidget
}

/**Loads all OpenHBCI datafiles selected in GuiFindmedium and merges them into one MP2 dataset
*/
bool GuiMain::loadLegacyData()
{
  int result = 0;
  QString homeDir = getenv("HOME"); //this is on the ramdisk, when running from CD
  QDir dir(homeDir);
  dir.mkdir(QString("legacy_") + MONEYPENNY_MAIN_DIR);
  QString legacyConfigDataDir =  homeDir + "/" + "legacy_" + MONEYPENNY_MAIN_DIR;
  QStringList bankNames =  _GuiFindMedium->bankNames();

  for(int nBank = 0; nBank < bankNames.count(); nBank++ ){ //Bank loop
     QString bankName = bankNames[nBank];
     if(!bankName.isEmpty())
       bankName += "_"; //file  name is bankname_moneypenny.bf
     QString bankFileName = _CryptDir + "/" + bankName + MONEYPENNY_ENCRYPTED_HBCI_CONF_FILE;
     result =  _Crypt->decryptArchive(bankFileName, 
                                      legacyConfigDataDir, 
                                      _CryptDir + "/" + bankName + MONEYPENNY_HBCI_INIVECTOR_FILE);
     if(result < 0){
       QMessageBox::critical(stackedWidget, tr("Warning"), 
          tr("Decrypting MoneyPenny 1.X data failed for bank file:") + " " + bankFileName);
     }
    else //ok, evaluate data
    { 
      HoBIT::hobit()->loadLegacyData(legacyConfigDataDir, _CryptDir);
      encryptData();
      renewHoBIT();
    }
  } //end for bank
  return true;
}

/** Loads the configuration files. 
*/
bool GuiMain::loadEnvironment(){
   setCursor(Qt::WaitCursor);
   bool result = true;

   if(decryptData())
     result = HoBIT::hobit()->loadEnvironment(_CryptDir); 
   else result = false;
   if( _GuiFindMedium->isDataConvertingSelected() && !loadLegacyData()) 
       result = false;
  if(result){
   const banking::Bank * bank = HoBIT::hobit()->bank();
   if(bank && bank->hbciVersion() < 220){
     HoBIT::hobit()->setHbciVersion(220);
     QMessageBox::information(this, tr("Information"), 
       tr("The HBCI-version to be used with your bank has been updated to HBCI 2.2."));
   } 
  }
  if(result && USE_LIVE_CD_FEATURES) 
    slotRestoreOsConfig(); //this may fail, however it is not critical
 
  if(result)
    slotRefresh();
  unsetCursor();
  // added as there's a permanent wait cursor on OSX  (AG)
  setCursor(QCursor(Qt::ArrowCursor));
  return result;
}

/** Adds a log text to the log text widget
*/
void GuiMain::slotAddLogText(QString logText){

 _GuiLog->addLogText(logText);

}

/** Opens a dialog which asks the user wether the stores jobs should be
* send or should not. If the user answers "Yes" the Job Queue will be
*executed. Otherwise The jobs will be stored for later use.
*/
int GuiMain::askForSend(){
 QString caption = tr("Send jobs now?");
 QString question = tr("What would you like MoneyPenny to do?");
 QString later = tr("&Add to job queue \nfor sending later");
 QString now = tr("&Send immediately");
// QString nowAndGetTransactions = tr("Send immediately and \nget &transactions and balance");
 int result = 
  QMessageBox::question
  (  this, caption, question, later, now);//, nowAndGetTransactions);
     //this is considered to be too much asking
/* if (!result) QMessageBox::information
   ( this,
     tr("Job Queue"),
     tr("New Jobs have been queued. \nPlease do not forget to send them later! ") );
*/
 return result;
}

/** Read property of QString _ConfigDir. 
*/
const QString& GuiMain::getConfigDataDir()const{
  return _ConfigDataDir;
}

/** Write property of QString _ConfigDataDir. 
*/
void GuiMain::setConfigDataDir( const QString& _newVal){
  _ConfigDataDir=_newVal;
}  

/** Write property of QString _CryptDir. 
*/
void GuiMain::setCryptDir( const QString& _newVal){

  _CryptDir = _newVal;
  if(_Crypt)
    _Crypt->setCryptDir(_newVal);
  if(_GuiHbciWizard)
    _GuiHbciWizard->setMediumDir(_newVal); //initialize wizard with the same dir. User may modify this later
}  


/** Read property of QString _CryptDir. 
*/
const QString& GuiMain::getCryptDir(){
  return _Crypt->getCryptDir();
}  

/**
*Raises transferform to edit a template
*/
void GuiMain::slotRaiseTransferTemplate( banking::Transfer * transfer){
   if (transfer == NULL ){
     cerr << "GuiMain::slotRaiseTransferTemplate: Error:No template supplied." << endl;
     return;
   }
    //first set all edits, that are always used
   _GuiTransferForm->lineEditOtherName->setText(
             (transfer->otherName1() + transfer->otherName2()).c_str()
          );
   _GuiTransferForm->lineEditOtherAccountId->setText(transfer->otherAccountId().c_str());
   _GuiTransferForm->lineEditOtherBankCode->setText(transfer->otherBankCode().c_str());
   _GuiTransferForm->lineEditValue->setText(QString(transfer->value().readableString().c_str()));
   _GuiTransferForm->setPurpose(transfer->purposeLines());

    // do we need more data? Set them here
    switch (transfer->type()){
        case banking::Transfer::SingleTransfer:{
               //nothing more to do
             }
             break;

/*
        case DATED_SINGLE_TRANSFER_WIDGET:
       
               _GuiDatedSingleTransfer->lineEditOtherName->setText(name);
               _GuiDatedSingleTransfer->lineEditOtherAccountId->setText(accountId);
               _GuiDatedSingleTransfer->lineEditOtherBankCode->setText(bankId);
               _GuiDatedSingleTransfer->lineEditValue->setText(value);
               _GuiDatedSingleTransfer->setPurpose(purpose);
               slotRaiseWidget(DATED_SINGLE_TRANSFER_WIDGET);
               break;
        
        
               break;
        
        case START_STANDING_ORDER_WIDGET:
        
               _guistartstandingorder->lineEditOtherName->setText(name);
               _guistartstandingorder->lineEditOtherAccountId->setText(accountId);
               _guistartstandingorder->lineEditOtherBankCode->setText(bankId);
               _guistartstandingorder->lineEditValue->setText(value);
               _guistartstandingorder->setPurpose(purpose);
               slotRaiseWidget(START_STANDING_ORDER_WIDGET);
               break;
*/
       default:  break;
    }//end switch
  _GuiTransferForm->setType(transfer->type()); //tell the form, what type it is
  slotRaiseWidget(TRANSFER_WIDGET);
}
 

/** Write property of Storage _Storage. 
*/
void GuiMain::setStorage( const StorageType& _newVal){

   _Storage = _newVal;
   if(_GuiHbciWizard != NULL)
     _GuiHbciWizard->setStorageType(_Storage);

}

 
/** Run a test connection to www.google.de or another url
*/
void GuiMain::slotTestInternet(bool showSuccessMessage, const QString & url, int port, HoBIT_Connector::ConnectionState * state){
 
 QString oldUrl = _Connector->getUrl();
 int oldPort = _Connector ->getPort();
 
 HoBIT_Connector::ConnectionState oldState = _Connector->connectionState();
 if(oldState == HoBIT_Connector::ConnectionConnected) 
   _Connector->closeConnection();  
 
 _Connector->setUrl(url);
 _Connector->setPort(port);
 _Connector->connectTCP();
 * state = _Connector->connectionState();
 if( * state == HoBIT_Connector::ConnectionConnected){
   _Connector->closeConnection();
   * state = HoBIT_Connector::ConnectionWasOk;
   if(showSuccessMessage)
     QMessageBox::information(this, tr("Internet Connection Test"), 
     tr("Your internet setup seems to be Ok. Could connect to: ") + url + tr(" : Port: ") + QString::number(port));
 }
 else QMessageBox::warning(this, tr("Internet Connection Test"), 
       tr("Your internet setup does not work properly. Connection test failed. <br>") +
       tr("Tried to connect to: ") + url + tr(" : Port: ") + QString::number(port) );
 slotTimerDone(); //update Status Bar
 _Connector->setUrl(oldUrl);
 _Connector->setPort(oldPort);      
}


/** Read property of QString _MountPoint. 
*/
const QString& GuiMain::getMountPoint()const{
	return _MountPoint;
}

/** Write property of QString _MountPoint. 
*/
void GuiMain::setMountPoint( const QString& _newVal){
	_MountPoint = _newVal;
}

/** restart the application
*/
void GuiMain::slotRestart(){
   _exitCode = EXITCODE_RESTART;	
   slotClose();
}


/** shows information about a single transaction
*/

void GuiMain::slotShowTransactionInfo(const banking::TransactionItem & trc){
   _GuiTransactionInfo->setTransactionItem(trc);
   slotRaiseWidget(TRANSACTION_INFORMATION_WIDGET);
}

/** shows information about a single transaction
*/
void GuiMain::slotShowStandingOrderInfo(banking::Transfer * t){
/*
   _GuiTransactionInfo->showStandingOrder(sto);
   slotRaiseWidget(TRANSACTION_INFORMATION_WIDGET);
 */
}
/** shows information about a single dated transaction
*/
void GuiMain::slotShowDatedTransferInfo(banking::Transfer * t){
/*
   _GuiTransactionInfo->showDatedTransfer(dt);
   slotRaiseWidget(TRANSACTION_INFORMATION_WIDGET);
*/
}

/** Shows a messagebox with the error messages.
*/
void GuiMain::slotShowErrorMessages(const QString & msg)
{
  QMessageBox::critical(this, tr("Error Message Received"), 
                        tr("An error message was received! See message text marked in red.") + "\n");
}

/**
  * Called when the kernel caught an exception.
  */
 void GuiMain::slotCaughtKernelException(exception e)
{
  QMessageBox::critical(this, tr("Error"), tr("An exception occurred. Details:") + tr(e.what()) );
  setCursor(Qt::ArrowCursor); //remove any wait-cursor
}

/** Shows a messagebox with the messages.
*/
void GuiMain::slotShowMessages(const QString & msg)
{
  QString note;
  if(_GuiProgressIndicator->isCancelled())
  {
    note += tr("Sending of jobs was stopped.") + "\n";
//   note += tr("The dialog with the bank server was completed without errors.") +" "+
//                         tr("The following messages were sent and received:") + "\n";
   QMessageBox::information(this, tr("Messages Received"), note );
  }
}

/** Import an RDH2 formatted key. 
*/
void GuiMain::slotImportRDH2Key(const QString & fileName){
  QMessageBox::information(this, QString("MoneyPenny"), // ask user for password
                                    QString(tr("This function is not yet implemented completely!")));
/*   bool ok;
 
   QString password=QInputDialog::getText(QString("MoneyPenny"), // ask user for password
                                    QString(tr("Please input the password for the keyfile you want to import!")),
                                    QLineEdit::Password,
                                    0,
                                    &ok);
  if(ok)
   HoBIT::hobit()->importRDH2Key(fileName, password);
  HoBIT::hobit()->importRDH2Key(fileName, "Password");*/ // To be removed!!!!
}


/** Performed after HBCI Wizard has finished
*/
void GuiMain::slotSetupSuccessful(HBCI_MODE mode){
slotRefresh();
slotActivateMenuFunctions();
if (mode == HBCI_MODE_RDH_STEP1)
 slotShowIniLetter();
else slotAccountLogin();
}


/** shows the splash screen */
  void GuiMain::showSplashScreen(){
    if(!_GuiSplash->isVisible())
      _GuiSplash->showMaximized();
    hide(); //hide guimain
}

/** hides the splash screen */
  void GuiMain::hideSplashScreen(){
  _GuiSplash->hide();
  show(); //show main
}

/** returns the splash screen */
  GuiSplash * GuiMain::getSplashScreen(){
  return _GuiSplash;
}

/**Search for the removable medium*/
 bool GuiMain::findMedium(bool createBackupMedium){
  _GuiFindMedium->setWindowState(Qt::WindowMaximized);
  _GuiFindMedium->setMountBackupMedium(createBackupMedium);
  bool mediumOk = _GuiFindMedium->exec() == QDialog::Accepted;
  if(mediumOk){
     if(!createBackupMedium){
        createConfigDir();
     }
     setMountPoint(_GuiFindMedium->getMountPoint());
     setCryptDir(_GuiFindMedium->getCryptDirName()); // sets directory in _Crypt
     //set the output directory for the cups PDF converter to the removable medium
//ToDo:     LinuxPrinterSetup::setPs2PdfTargetDir(_MountPoint + "/"+ MONEYPENNY_PDF_DIR); 
     setStorage(_GuiFindMedium->getStorage());
     return true;
  }
  else {
     QMessageBox::warning(_GuiSplash, QObject::tr("Error"),QObject::tr("Can not use removable medium, aborting."));
     return false;
  }
 }

 /**ask the user for the local data password.*/
void GuiMain::login() {
  _GuiLoginPassword->checkForPasswordFile();
  bool passwordOk = _GuiLoginPassword->exec() == QDialog::Accepted;
   if(!passwordOk)
  exit (2);
}

/** deletes the old hobit object and creates a new one */
void GuiMain::renewHoBIT(){
  HoBIT::newHoBIT( false,
                   this,
                   MONEYPENNY_OPENHBCI_FILE,
                   MONEYPENNY_AQMONEY_FILE,
                   MONEYPENNY_TEMPLATE_FILE,
                   MONEYPENNY_HBCIDATA_DIR,
                   _ConfigDataDir,
                   APPLICATION_NAME,
                   _Connector);
   connectHobit();
 
  if(DEBUG_DATABASE())
     QMessageBox::information(this, tr("Programm stopped for debugging"), 
        tr("The program has been stopped for debugging.")+
        tr("The next step is loading the configuration from:") + _ConfigDataDir +"\n" +
        tr ("You may now edit the configuration files. Press the Continue button, when you have finished editing."),
        tr("Continue"));
  HoBIT::hobit()->loadEnvironment(_CryptDir + "/");
 slotRefresh();
}

/** shows data export dialog */
  void GuiMain::slotExport(){
   _GuiDataExport->setMountPoint(_MountPoint.toLatin1().data());
    slotRaiseWidget(EXPORT_WIDGET);
 }
 
/** Shows a dialog to change the login password.
*/
void GuiMain::slotChangeLoginPassword(){
   if(_GuiLoginPassword->checkForPasswordFile()){
     QMessageBox::warning(this, tr("Warning"), tr("Could not read your password verification file."));
   }
   bool passwordOk = _GuiLoginPassword->exec() == QDialog::Accepted;
   if(!passwordOk){
     QMessageBox::critical(this, tr("Error"), tr("Wrong Password!"));
   }
   else{
     _GuiLoginPassword->switchToEditPasswordMode();
     passwordOk = _GuiLoginPassword->exec() == QDialog::Accepted; //changes password in _Crypt
     if(passwordOk){
       if(encryptData()){
         QMessageBox::information(this, tr("Information"), tr("Your login password has been changed."));
       } 
       else{                 
         QMessageBox::critical(this, tr("Error"), tr("Could not save your data. Please make shure, that your medium is writeable and try again."));
      }
     }  
   }
}

/** Shows a dialog to change the HBCI password.
*/
void GuiMain::slotChangeHbciPassword(){
   slotRaiseWidget(HBCI_WIZARD);
   HoBIT::hobit()->saveEnvironment(); //save all data first
   encryptData();
   _GuiHbciWizard->changePassword();
}

/** Shows a dialog to create a backup medium.
 * Writes a copy of the present data to a new storage medium
 */
void GuiMain::slotCreateBackupMedium(){
    bool ok = true;
    KeyFileOpenHbci * keyfile = NULL;
    QString keyFileName = "No Keyfile";
    slotRaiseWidget(WELCOME_WIDGET);
    banking::User *user = HoBIT::hobit()->user();
    if(user && user->passportType() & PASSPORT_KEYFILE){
     QString password = QInputDialog::getText(stackedWidget,
                                              tr("Reading Keyfile for Backup"), 
                                              tr("Please enter the password for your keyfile."),
                                              QLineEdit::Password, "", &ok);
     keyFileName = user->passportFile().c_str();
     keyfile = new KeyFileOpenHbci(keyFileName, password);
     if(!ok || !keyfile->readFile()){
       QMessageBox::critical(this, tr("Error"), tr("Could not read keyfile: ") + keyFileName + tr("\n Wrong password or wrong file format?"));
       delete keyfile;
       keyfile = NULL;
       return;
     }
    }
    
    ok = QMessageBox::information(this, 
                                  tr("Creating Backup Medium"), 
                                  tr("Please remove your working medium now."),
                                  QMessageBox::Ok, QMessageBox::Cancel)== QMessageBox::Ok; 
    if(ok)
      ok = findMedium(true); //no reading of medium, create a backup
    HoBIT::hobit()->setPassportFileDir(_CryptDir); //copy the new path to database and user
    if(ok)
    {
     if(ok)
       ok = encryptData(); // this will also create the directory 
     if(ok && keyfile != NULL)
       ok = keyfile->writeFile(keyFileName);
   }
   if(keyfile != NULL)
     delete keyfile;
   if (ok){
       QMessageBox::information(this, tr("Creating Backup Medium"), 
                                      tr("Your configuration backup files have been written.") +"\n" +
                                      tr("To backup your transaction data, MoneyPenny will now close this session."),
                                  QMessageBox::Ok); 
        _exitCode = EXITCODE_RUN_BANK_LOOP; //save and run bank loop
   }
   else{
    QMessageBox::information(this, tr("Creating Backup Medium"), 
                               tr("Error while creating backup medium.") +"\n" +
                               tr("Please reinsert your old working medium when asked for it."),
                               QMessageBox::Ok); 
    _exitCode = EXITCODE_RESTART_SESSION; //do not save and run bank loop
   }
  close();
}


/** Shows a dialog to send an SQL-Query
*/
void GuiMain::slotSqlQuery(){
  slotRaiseWidget(SQL_QUERY_WIDGET);  
}

/** Executes the sql query
*/
void GuiMain::slotExecuteSqlQuery(QString query){  
/*
  
  // get Home directory path
  QString path = getenv("HOME");
  path += "/";
  path += MONEYPENNY_MAIN_DIR;

  QDir directory(path);
  if (directory.exists()){
	   path += "/";
	   path += MONEYPENNY_SQL_FILE_NAME;       
          // build the database
           slotMakeSQL(path);
 
    }
    else qDebug ("MoneyPenny configdatadir not found. Can not create an SQL file.");
	   	 
 
    slotRaiseWidget(SQL_RESULT_WIDGET);
    _guisqlqueryresult->executeQuery(query);
 */
}

/** Get a new account list from bank server.
*/
void GuiMain::slotUpdateAccountList(){
    HoBIT::hobit()->createJobGetUPD();
    slotSend();
}
/** Sets _AutoShutdown Mode. 
*/
void GuiMain::slotSetShutdownMode(bool autoShutdown){
 _AutoShutdown = autoShutdown;
}

/** Calls the printer setup program
*/
void GuiMain::slotAddPrinter(){
  slotRaiseWidget(HARDWARE_WIDGET);
}

/** Stores the knoppix configuration.
*/
bool GuiMain::slotSaveOsConfig(){
  qDebug("GuiMain::slotSaveOsConfig");
  if (USE_LIVE_CD_FEATURES)
  { //save OS config as knoppix.sh to ConfigDataDir
   
     //No automatic saving! Ask the user, if he wants the config to be saved!!!
     if(QMessageBox::question(this, tr("Save Hardware Configuration Security Request"), 
        tr("You called the hardware configuration page during this session. <br> Do you want to save the printer and internet configuration you setup?") +
        tr("MoneyPenny will restore a saved configuration automatically on next startup."), 
        tr("Yes"), tr("No")) == 0) //First button "Yes" was clicked
       {
           EXPORT_LANGUAGE();
           QString dirName = _ConfigDataDir + "/" + MONEYPENNY_OS_CONFIG_DIR;
           QDir dir(dirName);
           if(!dir.exists())
             dir.mkdir(dirName);
           const char * command = c_sudo("/usr/sbin/saveconfig-moneypenny " + dirName, this);  
           qDebug("Saving OS configuration with command: %s", command);
           system(command); 
           int cryptError = _Crypt->encryptArchive(MONEYPENNY_OS_CONFIG_DIR, 
                                                   _ConfigDataDir,
                                                   getCryptDir() + "/" + MONEYPENNY_ENCRYPTED_OS_FILE,
                                                   getCryptDir() + "/" + MONEYPENNY_OS_INIVECTOR_FILE); 
           if(cryptError){
            QMessageBox::information(this, tr("Error"), tr("Failed to encrypt and save hardware configuration."));
            qDebug("GuiMain::closeEvent: Failed to encrypt and save OS-config files. Error code: %i", cryptError);
           }
       }
       else QMessageBox::information(this, tr("Skipping Save Configuration"), tr("Setup changes will be lost."));  
       return true;
  }//end if LIVE_CD
  else return false;
}

/** Decrypts the crypted knoppix configuartion directory and then calls the knoppix restore script knoppix.sh.
*/
int GuiMain::slotRestoreOsConfig(){
  if(! USE_LIVE_CD_FEATURES) // just for safety
    return 0;

  int result = 0;  
  QString cryptedConfigFileName = getCryptDir() + "/" + MONEYPENNY_ENCRYPTED_OS_FILE;
  QString ivFileName = getCryptDir() + "/" + MONEYPENNY_OS_INIVECTOR_FILE;
  QString configFileDir = _ConfigDataDir + "/" + MONEYPENNY_OS_CONFIG_DIR;
  QFile knoppixFile(cryptedConfigFileName);
  if(!knoppixFile.exists()){
     result = -1;
     qDebug("No OS configuration available. Could not find: %s ", cryptedConfigFileName.toLatin1().data());
     startCupsd();    //Start cups daemon, because there is no script for starting it
     return result;
   } 
  result = _Crypt->decryptArchive(cryptedConfigFileName, _ConfigDataDir, ivFileName);
  if(result != 0){
    result = -2;
    QMessageBox::critical(this, tr("Error"), 
       tr("Decrypting Knoppix configuration failed. Most probably the file is damaged. \n Could not decrypt: ") + cryptedConfigFileName);
       qDebug("Decrypting Knoppix configuration failed. Could not decrypt: %s", qPrintable(cryptedConfigFileName));
    return result;
   } 
  QFile script( configFileDir + "/knoppix.sh");
  if(!script.exists()){
    result = -3;
    qDebug("Error: Can not restore OS-config. Could not find script: %s", script.fileName().toLatin1().data());
    startCupsd();    //Start cups daemon, because the script can't do it
    return result;
  } 
  else{
    QString chmodCommand = sudo("chmod +x " + script.fileName(), this);  
    QString restoreCommand = sudo (script.fileName() + " "  + configFileDir, this);// use configFileDir as $1
    system (chmodCommand.toLatin1().data());
    qDebug("Restoring OS-config using command: %s", restoreCommand.toLatin1().data());
    if (system(restoreCommand.toLatin1().data()) != 0) //this will also start cupsd
     result = -4;
    startCupsd();    //Start cups daemon, if the script did not do it
  }
  return result;
}

/** generate sql database */
void GuiMain::slotMakeSQL(QString filename){
} 
	
/** Starts the cups printer daemon.
*/
bool GuiMain::startCupsd(){
    if(system ("ps -A | grep cupsd") == 0) //cupsd is already running
      return true;
    QString cupsName = "/etc/init.d/cupsys"; //this is the startup skript in Knoppix
    QString startCommand = sudo(cupsName + " start", this);
    QFileInfo cupsFileInfo (cupsName);
    if (cupsFileInfo.isExecutable()){
      return system (startCommand.toLatin1().data())== 0;
    }  
    else {
      qDebug ("Can not start cups daemon using command: %s", startCommand.toLatin1().data());  
      return false;
    }  
}
/** Output of window size
*/
void GuiMain::resizeEvent (QResizeEvent * re){
  QString s = QString::number(re->size().width()) +" : " + QString::number(re->size().height());
  qDebug("Window resized to: %s", s.toLatin1().data());
}

/** Decrypts the data on the removeable medium and copies them to /home/moneypenny_data
*/
bool GuiMain::decryptData(){
    qDebug("GuiMain::decryptData");
    int result = 0;
    QFile mp2File(_CryptDir + "/" + MONEYPENNY2_ENCRYPTED_HBCI_CONF_FILE);
    if(mp2File.exists()){
       result =  _Crypt->decryptArchive(_CryptDir + "/" + MONEYPENNY2_ENCRYPTED_HBCI_CONF_FILE, 
                                         _ConfigDataDir, 
                                         _CryptDir + "/" + MONEYPENNY2_HBCI_INIVECTOR_FILE);
    }

    if(result < -1){
      QMessageBox::critical(this, QObject::tr("Fatal Error"), QObject::tr("Failed to decrypt data. Aborting") );
      return false;
    }
    else return true;
}


/** Encrypts the data in /home/moneypenny_data and copies them to the removeable medium.
*/
bool GuiMain::encryptData(){
     QDir testDir(_CryptDir);
     if(!testDir.exists())
      if(!testDir.mkdir(_CryptDir)){ //try to make the directory
        qDebug("GuiMain::encryptData: Failed to create crypt directory: %s", _CryptDir.toLatin1().data());
        QMessageBox::critical(this, tr("Error"),tr("Could not create directory:") + " " + _CryptDir);
        return false;
      }

    _Crypt->savePasswordMD();
    int val = _Crypt->encryptArchive( QString(MONEYPENNY_BANKDATA_DIR) + " " + QString(MONEYPENNY_LOG_DIR),
                                      _ConfigDataDir,
                                      _CryptDir + "/" + MONEYPENNY2_ENCRYPTED_HBCI_CONF_FILE,
                                      _CryptDir + "/" + MONEYPENNY2_HBCI_INIVECTOR_FILE); 
    if (val != 0) { 
       QMessageBox::critical(this, tr("Error"),tr("Error when encrypting data."));
       return false;
    }
    return true;
}

/** Sets the timeOut value in _Connector
*/
void GuiMain::slotSetTimeOut(int newTimeOut){
 if(newTimeOut != _Connector->getTimeOut())
   _Connector->setTimeOut(newTimeOut);
}

/** Asks the user, whether he wants to close withouts saving.
*/
void GuiMain::reject(){
 if(QMessageBox::question(this,
    tr("Confirmation"),
    tr("Do you want to close MoneyPenny without saving your data?"),
    tr("Yes"), //button 0 text
    tr("No"),  //button 1 text
    "",  //button 2 text
    1)         //default button
    == 0) // button 0 selected
    {
       HoBIT::hobit()->removeUncryptedEnvironment();
       _exitCode = EXITCODE_NO_SAVING;
       close();
    }
}
/**
*Raises the toolbox widget 
*/
void GuiMain::slotShowAccountData()
{
  treeWidgetMenu->setCurrentItem(_AccountBalanceItem);
  treeWidgetMenu->update();
  slotRaiseWidget(SHOW_ACCOUNTDATA_WIDGET);
  textLabelStatus->setText(tr("Show User Data"));
  slotRefresh();
}

/**
*Displays update Information.
*/
void GuiMain::slotUpdateProgram()
{
  QString msg = tr("This is MoneyPenny Version: ") + VERSION + "<br>";
  msg += tr("It was released on: ") + MONEYPENNY_RELEASE_DATE + "<br>";
  if(USE_LIVE_CD_FEATURES)
   msg += tr("The operating system on the CD is: ") + MONEYPENNY_CD_OS_VERSION + "<br>"; 
  msg += tr("Please have a look at http://www-in.fh-swf.de/moneypenny for availability of updates.") +"<br>";
 _guiwelcome->displayMessage(msg);
 slotRaiseWidget(WELCOME_WIDGET);
}

/** Raises the printer driver setup widget.
*/
void GuiMain::slotBuildPrinterDriverDatabase(){
 slotRaiseWidget(PRINTER_DRIVER_TO_XML_WIDGET);
}

/**Deletes OpenHbci-LogFiles
*/
void GuiMain::slotDeleteLogFiles(){
// QString logDir = _ConfigDataDir + "/" + MONEYPENNY_HBCIDATA_DIR + "/" 
//                  + QString::number(MONEYPENNY_DEFAULT_COUNTRY) + "/" + HoBIT::hobit()->bankCode() + "/logs";
 QString logDir = _ConfigDataDir +  "/" + MONEYPENNY_LOG_DIR;
 if( (_Storage == STORAGE_FLOPPY) || (QMessageBox::question(this, tr("Confirmation"), tr("Really delete all *.log files in:") +"\n" + logDir,
  QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)){
  QString rmCommand = "rm " + logDir + "/*.log";
  qDebug ("Deleting all log files. Command: %s", rmCommand.toLatin1().data());
  system (rmCommand.toLatin1().data());
 } 
}

/**Shuts down current session and calls HBCI wizard to setup a new bank.
*/
void GuiMain::slotAddBank(){
 if(QMessageBox::question(stackedWidget, tr("Add New Bank"),
   tr("MoneyPenny will now save your data and create an empty dataset for a new bank.") + "\n"
   +tr("Do you want to continue?"), 
   QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Cancel
 )!= QMessageBox::Ok)
  return;
 qDebug("slotAddBank\n");

 qApp->flush();
 slotRaiseWidget(WELCOME_WIDGET);
 textLabelStatus->setText(tr("Saving Data"));
 qApp->processEvents();

 if(HoBIT::hobit()->saveEnvironment()){
   if(encryptData()){
    renewHoBIT();
    textLabelStatus->setText(tr("Empty dataset created"));
    _GuiHbciWizard->showPage(GuiHbciWizard::WELCOME_PAGE);
    slotRaiseWidget(HBCI_WIZARD);
   } 
 }
}

/**Shuts down current session and runs the select-bank loop.
*/
void GuiMain::slotHbciSetupAborted(){
    _exitCode = EXITCODE_RESTART_SESSION;
    close();
}

/** Show PIN Dialog for cardreader input. Kernel thread is not waiting!
 */
void GuiMain::slotInputPinViaKeypadRequest(){
   if(_DlgEnterPinKeypad){
     _DlgEnterPinKeypad->show();
   }
   qApp->flush(); //send the paint event immediately
   qApp->processEvents();
}

/** Show PIN Dialog for cardreader input
 */
void GuiMain::slotInputPinViaKeyboardRequest(QString * pin){

   if(_DlgEnterPin){
     _DlgEnterPin->setPinBuffer(pin); //copy the pin address
     _DlgEnterPin->setLabelReason( tr("Please enter the PIN to activate your HBCI-card."));
     _DlgEnterPin->setLabelMessage(""); 
     _DlgEnterPin->setLabelKind( tr("PIN: "));
     _DlgEnterPin->setMinimumInputLength(5);
     _DlgEnterPin->exec();
   }
   else (* pin) = "";
   HoBIT::hobit()->wakeKernelThread();//we have copied valid input data, kernel thread may continue 
}

/** Show PIN Dialog for HBCI-PIN-TAN
 */
void GuiMain::slotPinRequest(QString * pin){

   if(_DlgEnterPin){
     _DlgEnterPin->setPinBuffer(pin); //copy the pin address
     _DlgEnterPin->setLabelReason( tr("Please enter the PIN for your account."));
     _DlgEnterPin->setLabelMessage(""); 
     _DlgEnterPin->setLabelKind( tr("PIN: "));
     _DlgEnterPin->setMinimumInputLength(5);
     _DlgEnterPin->exec();
   }
   else (* pin) = "";
   HoBIT::hobit()->wakeKernelThread();//we have copied valid input data, kernel thread may continue 
}

/** Show TAN Dialog for HBCI-PIN-TAN
 */
void GuiMain::slotTanRequest(QString bankmessage, QString * tan){

   if(_DlgEnterPin){
     _DlgEnterPin->setPinBuffer(tan); //copy the pin address
     _DlgEnterPin->setLabelReason( tr("Please enter the TAN for this transaction."));
     _DlgEnterPin->setLabelMessage( bankmessage );
     _DlgEnterPin->setLabelKind( tr("TAN: "));
     _DlgEnterPin->setMinimumInputLength(6);
     _DlgEnterPin->exec();
   }
   else (* tan) = "";
   HoBIT::hobit()->wakeKernelThread();//we have copied valid input data, kernel thread may continue 
}

/** Set the path for certificates to data dir/certificates
*/
void GuiMain::slotCertificatePathRequest(QString * certificatePath){
 * certificatePath = MP_DIR() + MONEYPENNY_DATA_DIR + "/" + MONEYPENNY_CERTIFICATE_DIR + "/"; 
 HoBIT::hobit()->wakeKernelThread();//we have copied valid input data, kernel thread may continue 
}

/** Hide PIN Dialog for cardreader input
 */
void GuiMain::slotPinWasOk(){
   if(_DlgEnterPinKeypad)
     _DlgEnterPinKeypad->hide();
   qApp->processEvents();
}

/** Hide PIN Dialog for cardreader input and show warning. Calls exit on user command.
 */
void GuiMain::slotPinWasNotOk(int triesLeft){
   if(_DlgEnterPinKeypad)
     _DlgEnterPinKeypad->hide();
   qApp->processEvents();

   QString msg ="<html><h2>"+ tr("WARNING") +"</h2>" +tr("The last PIN you input may have been wrong." )+"<br>";
   msg +=tr("Make shure that the right card is in your reader and check the PIN before you enter it again.") + "\n";
   msg +=tr("It is also possible, that the card and / or the reader are not connected properly or are defect.") +"\n";
   msg += "<b>" +tr("After the third wrong PIN the card will destroy itself.") + "</b><br>";
   msg += "<b>" +tr("Tries left on card:") + QString::number(triesLeft) + "</b><br>";
   msg += tr("It might be better to stop the program and have your card checked by your bank.")+ "</html>";
   if (QMessageBox::warning(NULL, 
                             tr("Warning"), 
                             msg,
                             tr("Abort program at once"), 
                             tr("Retry PIN input")) == 0){
      qDebug("*********Aborted due to wrong PIN*******");
      exit (EXITCODE_WRONG_PASSWORD);
    }
}

/**Dispatches printer installation message to child widgets.
 */
void GuiMain::slotPrinterInstalled(){
// _GuiTransactionPrinter->initPrinter();
 _GuiIniLetter->initPrinter();

}

/** Display "insert card" message and wait for the card to be inserted.
 */
void GuiMain::slotInsertCardRequest(){
  QMessageBox::information(this, tr("Chipcard Required"), tr("Insert your chipcard please!"));
  HoBIT::hobit()->wakeKernelThread();//card should be inserted now, kernel thread may continue and try again 
}

/**Called when kernel thread is started.
  *Calls GuiProgressIndicator->exec().
 */
void GuiMain::slotKernelThreadStarted()
{
   qDebug("GuiMain::slotKernelThreadStarted called. Blocking GUI.");
  _GuiProgressIndicator->setStatus(GuiProgressIndicator::Running);
  _GuiProgressIndicator->show(); //blocks the rest of the application
  _GuiJobs->setButtonsEnabled(false);
}

/** Called when kernel thread is finished. If no error occurred, ok is true  
  */
void GuiMain::slotKernelThreadFinished(bool ok)
{
 qDebug("GuiMain::slotKernelThreadFinished called. Releasing GUI.");
 _GuiProgressIndicator->setStatus(GuiProgressIndicator::Finished);
 _GuiJobs->setButtonsEnabled(true);
 slotRefresh(); //finally update all gui elements
}

/**Called when a message is about to be sent by kernel.
 * Adds msg to _GuiProgressIndicator.
 */
void GuiMain::slotSendingMessage(QString msg)
{
 _GuiProgressIndicator->addTextParagraph(msg);
 HoBIT::hobit()->wakeKernelThread();
}

/**Called when a response without error was received by kernel.
 * Adds msg to _GuiProgressIndicator.
 */
 void GuiMain::slotResponseOk(QString msg)
{
 _GuiProgressIndicator->addTextParagraph(msg);
  HoBIT::hobit()->wakeKernelThread();
}
/**Called when a response with error message was received by kernel.
 * Adds msg to _GuiProgressIndicator.
 */
 void GuiMain::slotResponseError(QString msg)
{
 _GuiProgressIndicator->addTextParagraph(tr("Error message from bank:") + " " + msg, Qt::red);
 HoBIT::hobit()->wakeKernelThread();
}


/** create all the connectoions for Hobit
*/
void GuiMain::connectHobit(){
  //connect HoBIT-signals for guimain
   connect(HoBIT::hobit(), SIGNAL(logTextToShow(QString)), 
           this, SLOT(slotAddLogText(QString)));

   connect(HoBIT::hobit(), SIGNAL(insertCardRequest()), 
            this, SLOT(slotInsertCardRequest()));

   connect(HoBIT::hobit(), SIGNAL(inputPinViaKeypadRequest()), 
           this, SLOT(slotInputPinViaKeypadRequest()));

   connect(HoBIT::hobit(), SIGNAL(inputPinViaKeyboardRequest(QString *)), 
           this, SLOT(slotInputPinViaKeyboardRequest(QString *)));

   connect(HoBIT::hobit(), SIGNAL(pinWasOk()), 
           this, SLOT(slotPinWasOk()));

   connect(HoBIT::hobit(), SIGNAL(pinWasNotOk(int)), 
           this, SLOT(slotPinWasNotOk(int)));

   connect(HoBIT::hobit(), SIGNAL(pinRequest(QString *)), 
           this, SLOT(slotPinRequest(QString *)));

   connect(HoBIT::hobit(), SIGNAL(tanRequest(QString, QString *)), 
           this, SLOT(slotTanRequest(QString, QString *)));

   connect(HoBIT::hobit(), SIGNAL(certificatePathRequest(QString *)), 
           this, SLOT(slotCertificatePathRequest(QString *)));

   connect(HoBIT::hobit(), SIGNAL(errorMessagesReceived(const QString &)), 
           this, SLOT(slotShowErrorMessages(const QString &)));

   connect(HoBIT::hobit(), SIGNAL(messagesReceived(const QString &)), 
           this, SLOT(slotShowMessages(const QString &)));

   connect(HoBIT::hobit(), SIGNAL(kernelThreadStarted()), 
           this, SLOT(slotKernelThreadStarted()));

   connect(HoBIT::hobit(), SIGNAL(kernelThreadFinished(bool)), 
           this, SLOT(slotKernelThreadFinished(bool)));

   connect(HoBIT::hobit(), SIGNAL(sendingMessage(QString)), 
           this, SLOT(slotSendingMessage(QString)));

   connect(HoBIT::hobit(), SIGNAL(responseOk(QString)), 
           this, SLOT(slotResponseOk(QString)));

   connect(HoBIT::hobit(), SIGNAL(responseError(QString)), 
           this, SLOT(slotResponseError(QString)));
  connect(HoBIT::hobit(), SIGNAL(caughtException(exception)), 
           this, SLOT(slotCaughtKernelException(exception)));

  //connections to GuiJobs
  connect(HoBIT::hobit(), SIGNAL(jobStatusChanged(banking::BankingJob *)),
          _GuiJobs, SLOT(slotUpdateJobStatus(banking::BankingJob *)));
  connect(HoBIT::hobit(), SIGNAL(sentAdminJob(int, banking::BankingJob *)),
          _GuiJobs, SLOT(slotSentAdminJob(int, banking::BankingJob *)));
  connect(HoBIT::hobit(), SIGNAL(sentTransferJob(int, banking::BankingJob *)),
          _GuiJobs, SLOT(slotSentTransferJob(int, banking::BankingJob *)));

 //connections to GuiProgressIndicator
  connect(_GuiProgressIndicator, SIGNAL(cancel()),
          HoBIT::hobit(), SLOT(slotCancelKernelAction()));
}
