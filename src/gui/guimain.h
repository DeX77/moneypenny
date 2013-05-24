/***************************************************************************
                          guimain.h  -  description
                             -------------------
    begin                : Do Nov 4 2004
    copyright            : (C) 2004 by Marcel Mulch, 2010 Walter Roth
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

#ifndef GUIMAIN_H
#define GUIMAIN_H

#include <QDialog>
#include <QStack>
 
class QTreeWidgetItem;
class QTableWidget;
class QAction;
class QProcess;

#include <openhbci2/pointer.h>
#include <openhbci2/date.h>
using namespace HBCI;
 
#include "ui_guimainbase.h"
#include "globals.h"
#include "hobit_connector.h"

class DlgEnterPinKeypad;
class DlgEnterPin;
class GuiNotImplemented;
class GuiBalances;
class GuiSplash;
class GuiTransactionList;
class GuiGetTransactions;
class GuiTransactionPrinter;
class GuiTransactionInfo;
class GuiTools;
class GuiShowBankData;
class GuiTransferTemplates;
class GuiTransferForm;
class GuiDatedTransfers;
class GuiStandingOrders;
class GuiStartStandingOrder;
class GuiDatedSingleTransfer;
class GuiHelpWindow;
class GuiIniLetter;
class GuiKnownBanks;
class GuiLogin;
class GuiLog;
class GuiAccountInformation;
class GuiHbciWizard;
class GuiChangeAccountName;
class GuiTransactionInfo;
class GuiJobs;
class GuiAddAccount;
class GuiHardwareConfig;
class GuiDialIn;
class GuiIniLetter;
class GuiTransferTemplates;
class GuiWelcome;
class GuiLoginPassword;
class GuiFindMedium;
class GuiDataExport;
class GuiSqlQuery;
class GuiSqlQueryResult;
class GuiPrinterAutoSetup;
class GuiPrinterManualSetup;
class GuiPrinterDriverToXml;
class GuiTransactionPrinter;
class GuiEntropy;
class GuiImport;
class GuiProgressIndicator;

class HoBIT;
class HoBIT_Connector;
class HoBIT_Crypt;
class HoBIT_SQL;

namespace banking{
  class Account;
  class TransactionItem;
  class Transaction;
  class Transfer;
  class TransactionItemList;
  class TransactionList;
}


/**
* This is MoneyPenny's Main Widget. All other widgets are constructed and controlled from here. 
* See buildMainApplication and createMenu for details.
*
* WARNING: The Hobit engine runs the banking kernel in a separate thread to prevent GUI freezing.
* Communication with the kernel engine is carried out via asynchronous signals sent by _Hobit and slots
* connected to these signals. Refer to connectHobit() for details.
* Whenever a user action is required during execution of the banking kernel (e.g. enter a PIN or a TAN, 
* verfiy a fingerprint) the _Hobit engine blocks the banking kernel thread using a QWaitCondition.
* ALL slots that handle such user actions have to call _Hobit->wakeKernelThread() when user interaction
* is complete and valid data have been copied to the respective locations. Refer to slotRequestXXXX functions
* for examples.
* While the banking kernel is running, all user actions have to be blocked. This is performed by launching
* the application modal progress dialog _GuiProgressIndicator. Refer to slotSendNow, slotKernelThreadStarted  * and slotKernelThreadFinished for details.
*/
class GuiMain : public QDialog, public Ui::GuiMain
{
  Q_OBJECT

public:

  /**
  *constructor for GuiMain (main window),
  *sets statusbar, menulist, refreshes informations, widgets, slots, etc. 
  *@param crypt pointer on the HoBIT_Crypt interface
  *@param parent pointer on current QWidget
  *@param name title of the GuiMain dialog
  */
  GuiMain();

  /** starts building the main application -- real constructor */
  virtual void buildMainApplication();

  /**
  *destructor of GuiMain
  */
  ~GuiMain();

  /** Opens a dialog which asks the user wether the stored jobs should
  * be send or should not. If the user answers "Yes" the Job Queue will
  * be executed. Otherwise The jobs will be stored for later use.
  */
  int askForSend();
  
  /**
  *Building Menu by adding items and setting labels
  */
  void createMenu();


  /**
  *refresh account-balance
  */
  void updateBalanceStatus();

  /**
  *Raise widget with list of known banks
  */
  void slotListKnownBanks();

  /**
  *Switch Account by raise login-widget
  */
  void switchAccount();

  /** Loads the configuration files. 
  */
  bool loadEnvironment();
  
  /**
  *function for closing
  */
  void closeEvent(QCloseEvent * ce);
  
  /** Read property of HoBIT_Connector m_Connector. 
  */
  HoBIT_Connector * getConnector()const;

  /** Read property of QString _CryptDir. 
  */
  const QString& getCryptDir();
  
  /** Write property of QString _CryptDir. This should be on the removable medium.
  */
  void setCryptDir( const QString& _newVal);
  
  /** Read property of QString _ConfigDir. This should be on the ramdisk.
  */
  const QString& getConfigDataDir()const;
  
  /** Write property of QString _ConfigDir.  This should be on the ramdisk.
  */
  void setConfigDataDir( const QString& _newVal);
  
  /** Write property of Storage _Storage. 
  */
  void setStorage( const StorageType& _newVal);
  
  /** Read property of QString _MountPoint. 
  */
  const QString& getMountPoint()const;
  
  /** Write property of QString _MountPoint. 
  */
  void setMountPoint( const QString& _newVal);

  /** shows the splash screen */
  void showSplashScreen();

  /** hides the splash screen */
  void hideSplashScreen();
  
  /** returns the splash screen */
  GuiSplash * getSplashScreen();
    
  /**Search for the removable medium. Returns true on success.*/
  bool findMedium(bool readNewMedium);
  
  /**ask the user for the local data password.*/
  void login();

protected:  
  
  /** Output of window size
  */
  void resizeEvent (QResizeEvent * re);
  /** Decrypts the data on the removeable medium and copies them to /home/moneypenny_data
  */
  bool decryptData();
  /** Crypts the data in /home/moneypenny_data and copies them to the removeable medium.
  */
  bool encryptData();

  /** Asks the user, whether he wants to close withouts saving.
  */
  void reject();
  /**Create main configuration directory
  */
  void createConfigDir();
  /** Executes command associated to item.
  */
  void executeCommand(QTreeWidgetItem * item);
  /**Loads all OpenHBCI datafiles selected in GuiFindmedium and merges them with MP2 data
  */
  bool loadLegacyData();
private:
  //dialogs
  DlgEnterPinKeypad * _DlgEnterPinKeypad;
  DlgEnterPin * _DlgEnterPin;
  // widgets
  GuiNotImplemented * _GuiNotImplemented;
  GuiTools* _GuiTools;
  GuiShowBankData * _GuiShowBankData;
  GuiWelcome* _guiwelcome;
  //GuiBalances* _guibalances;
  GuiAccountInformation* _GuiAccountInformation;
  GuiTransactionList* _GuiTransactionList;
  GuiTransactionPrinter* _guitransactionprinter;
  GuiGetTransactions* _GuiGetTransactions;
  GuiIniLetter* _GuiIniLetter;
  GuiTransferForm* _GuiTransferForm;
  GuiDatedTransfers* _GuiDatedTransfers;
  GuiDatedSingleTransfer * _GuiDatedSingleTransfer;
  GuiStandingOrders* _guistandingorders;
  GuiHelpWindow * _GuiHelpWindow;
  GuiKnownBanks * _GuiKnownBanks;
  GuiLogin* _GuiLogin;
  QWidget* _guiNullWidget; // a dummy widget for debugging
  GuiHbciWizard* _GuiHbciWizard;
  GuiChangeAccountName* _guichangeaccountname;
  GuiJobs* _GuiJobs;
  GuiAddAccount* _GuiAddAccount;
  GuiHardwareConfig* _GuiHardwareConfig;
  GuiDialIn* _guidialin;
  GuiLog * _GuiLog;
  GuiTransferTemplates* _GuiTransferTemplates;
  GuiTransactionInfo* _GuiTransactionInfo;
  GuiSplash * _GuiSplash;
  GuiFindMedium * _GuiFindMedium;
  GuiLoginPassword * _GuiLoginPassword;
  GuiDataExport* _GuiDataExport;
  GuiSqlQuery* _guisqlquery;
  GuiSqlQueryResult* _guisqlqueryresult;
  GuiPrinterDriverToXml * _GuiPrinterDriverToXml;
  GuiPrinterAutoSetup * _GuiPrinterAutoSetup;
  GuiPrinterManualSetup * _GuiPrinterManualSetup;
  GuiTransactionPrinter * _GuiTransactionPrinter;
  GuiImport * _GuiImport;
  GuiProgressIndicator * _GuiProgressIndicator;
  
  // List view Items
  QTreeWidgetItem* _PaymentItem;
  QTreeWidgetItem* _JobItem;
  QTreeWidgetItem* _ConfigItem;
  QTreeWidgetItem* _ConnectItem;

  QTreeWidgetItem* _AccountItem;
  QTreeWidgetItem* _ChangeBankItem;  
  QTreeWidgetItem* _AddBankItem;  
  QTreeWidgetItem* _UpdateAccountList;
  QTreeWidgetItem* _SelectAccountItem;
  QTreeWidgetItem* _AccountBalanceItem;
  QTreeWidgetItem* _TurnOverItem;
  QTreeWidgetItem* _GetTransactionsItem;
  QTreeWidgetItem* _ShowTransactionsItem;
  QTreeWidgetItem* _ShowDatedTransfersItem;
  QTreeWidgetItem* _ShowStandingOrdersItem;
  QTreeWidgetItem* _PrintTransactionsItem;
  QTreeWidgetItem* _ExportTransactionsItem;
  QTreeWidgetItem* _SingleTransferItem;
  QTreeWidgetItem* _ImportItem;
  QTreeWidgetItem* _DatedTransferItem;
  QTreeWidgetItem* _TransferTemplateItem;
  QTreeWidgetItem* _StandingOrderItem;
  QTreeWidgetItem* _SendNowItem;
  QTreeWidgetItem* _HbciItem;
  QTreeWidgetItem* _ChangeLoginPasswordItem;
  QTreeWidgetItem* _ChangeHbciPasswordItem;
  QTreeWidgetItem* _CreateBackupMediumItem;
  QTreeWidgetItem* _ToolboxItem;
  QTreeWidgetItem* _ShowDataItem;
  QTreeWidgetItem* _HardwareItem;
  QTreeWidgetItem* _HelpItem;
  QTreeWidgetItem* _UpdateItem;
  QTreeWidgetItem* _ExitItem;
  QTreeWidgetItem* _KnownBanksItem;
  QTreeWidgetItem* _JobQueueItem;
  QTreeWidgetItem* _JobProtocolItem;
  QTreeWidgetItem* _ClickedItem; //the item that was sent to on_treeWidget_itemClicked()
  QTimer* _timer;

  HoBIT_Connector* _Connector;
  HoBIT_Crypt* _Crypt;

  QAction* balanceAction;
  
 public slots: // Public slots 

  /**
  * Runs associated slot after pressing an item in main menu
  * Calls executeCommand
  */
  void on_treeWidgetMenu_itemClicked( QTreeWidgetItem * item, int column);
  /**
  * Runs associated slot after pressing an item in main menu
  * Calls executeCommand
  */
  void on_treeWidgetMenu_itemActivated( QTreeWidgetItem * item, int column);
  /** Activate corresponding treewidgetitem
   */
  void on_buttonAccounts_clicked();
  /** Activate corresponding treewidgetitem
   */
  void on_buttonSingleTransfer_clicked();
  /** Activate corresponding treewidgetitem
   */
  void on_buttonShowTransactions_clicked();
  /** Activate corresponding treewidgetitem
   */
  void on_buttonUpdate_clicked();
  /** Activate corresponding treewidgetitem
   */
  void on_buttonSendNow_clicked();
  /** Activate corresponding treewidgetitem
   */
  void on_buttonHelp_clicked();
  /** Activate corresponding treewidgetitem
   */
  void on_buttonToolbox_clicked();
  /**
  *Raise AccountLogin-Dialog or HBCI-Wizard
  */
  void slotAccountLogin();
  /** Stores the knoppix configuration.
  */
  bool slotSaveOsConfig();

  /** calls the knoppix restore script.
  */
  int slotRestoreOsConfig();
  /**
  *refresh informations, lists and widgets
  */
  void slotRefresh();
  /**
  *Raises transferform to edit a template
  */
  void slotRaiseTransferTemplate( banking::Transfer * transfer);
   /** deletes the old hobit object and creates a new one */
   void renewHoBIT();

   /** create all the connectoions for Hobit
   */
   void connectHobit();

  /** shows data export dialog */
  void slotExport();
  
  /**
  *Raise widget for ini-letter
  */
  void slotShowIniLetter();
  /**
  *Raises the toolbox-widget on the WidgetStack
  */
  void slotToolbox();
  /**
  *Raises the ShowData-widget on the WidgetStack
  */
  void slotShowAccountData();
  /**
  *Raises the Welcome-widget on the WidgetStack and shows 
  *update informations.
  */
  void slotUpdateProgram();

  /**
  *Raises the HBCI-Widget on the WidgetStack
  */
  void slotHbciSetup();

  /**
  *Raises the Widget, which shows quick information about all hbci accounts
  */
  void slotAccounts();

  /** Get a new account list from bank server.
  */
  void slotUpdateAccountList();
  
   /**
  *Asks for execution of the job list and sends if requested.
  */
  void slotSend();
  
  /**
  *starts execution of the job list. Sets result to true on success. 
  */
  void slotSendNow(bool * result, const QString & reason, bool showJobList = true);
  /**
  *starts execution of the job list. 
  */
  void slotSendNow();

  /**
  *Select the accounts for which the transactionlists are to be updated 
  */
  void slotGetTransactions();

  /**
  *shows a dialog with the turnovers 
  */
  void slotShowTransactions();

  /**
  *shows a list with the standing orders 
  */
  void slotShowStandingOrders();

  /**
  *shows a list with the dated transfers 
  */
  void slotShowDatedTransfers();

  /**
  *creates a single transfer 
  */
  void slotSingleTransfer();

  /**
  *calls a transfer template. This is a user defined mask for frequently used transfers. 
  */
  void slotTransferTemplate();
/**
 * Opens GuiImport
 */
  void slotImport();
  /**
  *shows dated transfers widget 
  */
  void slotDatedTransfer();

  /**
  * Performed after Setup Wizard has finished
  */
  void slotSetupSuccessful(HBCI_MODE mode);
  
  /**
  *No descriptions 
  */
  void slotStandingOrder();

  /**
  *No descriptions 
  */
  void slotUpdate();

  /**
  *Call slotHelp(QString&)
  */
  void slotHelp();
  /**
  *Open the help window
  *@param helpfile a helping QString
  */
  void slotHelp(const QString & helpfile);

  /**
  *timer done 
  */
  void slotTimerDone();

  /**
  *Prints the transactions.
  */
  void slotPrintTransactions(banking::TransactionList *transactionList, const QDate & from, const QDate & to);

  /**
  *Shows the job list
  */
  void slotShowJobs();

  /**
  *Raise jobs widget and show progress dialog with protocol of last run. 
  */
  void slotShowJobProtocol();

  /**
  *Raise hardware widget in CONNECT mode 
  */
  void slotDialIn();

  /**
  *raises the add account widget 
  */
  void slotAddAccount();
  
  /** Deletes the account accountId.
  */
  void slotDeleteAccount(QString accountId);

  /**
  *raises the hw config dialog 
  */
  void slotHardwareConfig();

  /**
  *shows welcome widget 
  */
  void slotShowWelcome(const QString & smallHelpFile);
  
  /**
  *closes the application 
  */
  void slotClose();
  /**
  * Change bank by shutting down and restarting with new medium 
  */
  void slotChangeBank();

  /**Shuts down current session and runs the select-bank loop.
  */
  void slotHbciSetupAborted();

  /**
  *set account to be used from moneypenny after login 
  *@param bankCode QString for the bankCode
  *@param ca QString for the current account
  */
  void slotSetCurrentAccount(const QString &bankCode, const QString &ca);

   /**
  *Raises widget with given id
  *Builds the name of the small help file from widget name
  *@param id id of the widget which should be raised 
  */
  void slotRaiseWidget(int id);
  
  /**
  *Deactivate Buttons and List
  */
  void slotDeactivateMenuFunctions();

  /**
  *Activate Buttons and List
  */
  void slotActivateMenuFunctions();
  
  /** Adds a log text to the log text widget
  */
  void slotAddLogText(QString logText);

  /** Run a test connection to www.google.de or another url
  */
  void slotTestInternet(bool showSuccessMessage, const QString & url, int port, HoBIT_Connector::ConnectionState * state);
  
  /**
  *Show last visible widget
  */
  void slotShowLastVisibleWidget();
  
  
  /**
  *Restart the Application
  */
  void slotRestart();

 /**
  *shows a transaction information dialog
  */
  void slotShowTransactionInfo(const banking::TransactionItem & trc);

  /**
  *shows a transaction information dialog
  */
  void slotShowStandingOrderInfo(banking::Transfer * t);
  
  /** shows information about a single dated transaction
  */
  void slotShowDatedTransferInfo(banking::Transfer * t);
  /** Shows a messagebox with the error messages.
  */
  void slotShowErrorMessages(const QString & msg);
  /** Shows a messagebox with the messages.
  */
  void slotShowMessages(const QString & msg);
  /** Import an RDH2 formatted key. 
  */
  void slotImportRDH2Key(const QString & fileName);
  
  /** Shows a dialog to change the login password.
  */
  void slotChangeLoginPassword();

  /** Shows a dialog to change the HBCI password.
  */
  void slotChangeHbciPassword();
  
  /** Shows a dialog to create a backup medium.
   * Writes a copy of the present data to a new storage medium
   */
  void slotCreateBackupMedium();

  /** Shows a dialog to send an sql query.
  */
  void slotSqlQuery();

  /** Exectutes the sqlquery.
  */
  void slotExecuteSqlQuery(QString query);

  /** Sets Sets _AutoShutdown Mode. 
  */
  void slotSetShutdownMode(bool autoShutdown);
  
  /** Calls the printer setup program
  */
  void slotAddPrinter();

  /** generate sql database */
  void slotMakeSQL(QString filename);
  
  /** Sets the timeOut value in _Connector
  */
  void slotSetTimeOut(int newTimeOut);
  
  /** Raises the printer driver setup widget.
  */
  void slotBuildPrinterDriverDatabase();
  /** Displays helpFile in the small help widget at the lower left.
  */
  void slotShowSmallHelp(const QString & helpFile);
  /**Deletes OpenHbci-LogFiles
  */
  void slotDeleteLogFiles();
  /** Updates _CurrentWizardPage and contents of small help widget
  */
  void slotWizardPageChanged(int newPage);
  /**Shuts down current session and calls HBCI wizard to setup a new bank.
  */
  void slotAddBank();
  /**Dispatches printer installation message to chuild widgets.
   */
  void slotPrinterInstalled();
  /** Display "insert card" message
   */
  void slotInsertCardRequest();
  /** Show PIN Dialog for cardreader input
   */
  void slotInputPinViaKeypadRequest();
  /** Show PIN Dialog for cardreader input
   */
  void slotInputPinViaKeyboardRequest(QString * pin);
  /** Hide PIN Dialog for cardreader input
   */
  void slotPinWasOk();
  /** Hide PIN Dialog for cardreader input and show warning
   */
  void slotPinWasNotOk(int triesLeft);
  /** Show PIN Dialog for HBCI-PIN-TAN
   */
  void slotPinRequest(QString * pin);
  /** Show TAN Dialog for HBCI-PIN-TAN
   */
  void slotTanRequest(QString bankmessage, QString * tan);
  /** Set the path for certificates to data dir/certificates
  */
  void slotCertificatePathRequest(QString * certificatePath);
  /**Called when kernel thread is started.
    *Calls GuiProgressIndicator->exec().
   */
  void slotKernelThreadStarted();
  /** Called when kernel thread is finished. If no error occurred, ok is true
   *  Hides progress indicator.
   */
   void slotKernelThreadFinished(bool ok);
   /**Called when a message is about to be sent by kernel.
     * Adds msg to _GuiProgressIndicator.
     */
   void slotSendingMessage(QString msg);
   /**Called when a response without error was received by kernel.
     * Adds msg to _GuiProgressIndicator.
     */
    void slotResponseOk(QString msg);
   /**Called when a response with error message was received by kernel.
    * Adds msg to _GuiProgressIndicator.
    */
    void slotResponseError(QString msg);
   /**
     * Called when the kernel caught an exception.
     */
    void slotCaughtKernelException(exception e);
private:

  /** Starts the cups printer daemon.
  */
  bool startCupsd();

   /** Storage is the storage type for the config directory (USB or floppy)
  */
  StorageType _Storage;
  /** mount point of the removable device
  */
  QString _MountPoint;
  
  /** Exit Code of the Application 
  */
  ExitCode _exitCode;
  /** Flag for automatic OS-shutdown on Knoppix CD
  */
  bool _AutoShutdown;
  /** Flag for saving Knoppix config
  */
  bool _SaveKnoppixConfig;
  /** The directory for moneypenny's temporary data files.
  */
  QString _ConfigDataDir; 
  /** The directory for moneypenny's encrypted data files.
  */
  QString _CryptDir; 
  /** The printer setup process
  */
  QProcess * _AddPrinterProcess;
  /** The stack with the widgets theat have already been called.
  */
  QStack<int> * _WidgetCallStack;
  /**The currently displayed small help file
  */
  QString _SmallHelp;
  /** The present page of the HBCI Wizard
  */
  int _CurrentWizardPage;

 /** The array with the page indexes of the widgets on the stack
 *   The indexes may not be identical to the enums in globals.h, 
 *   due to variable number of added widgets on the stacked_Widget. 
 */
  int _PageIndex [LAST_WIDGET];
 };



#endif
