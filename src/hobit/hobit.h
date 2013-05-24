/***************************************************************************
hobit.h-description
 -------------------
begin: Do Nov 4 2004
copyright: (C) 2004 by Marcel Mulch, 2011 Walter Roth
email:
 ***************************************************************************/

/***************************************************************************
 * *
 * This program is free software; you can redistribute it and/or modify*
 * it under the terms of the GNU General Public License as published by*
 * the Free Software Foundation; either version 2 of the License, or *
 * (at your option) any later version. *
 * *
 ***************************************************************************/

#ifndef HOBIT_H
#define HOBIT_H


// std includes
#include <string>
#include <list>
using namespace std;

// qt includes
#include <QMessageBox>
#include <qtextedit.h>
#include <QString>
#include <QDate>
//#include <qptrvector.h>

// hobit includes
#include "hobit_interactor.h"
#include "hobit_auth.h"
#include "hobit_validator.h"
#include "hobit_connector.h"
//#include "hobit_transfertemplate.h"
#include "databaseaccess.h"
#include "kernel/helpers/transaction.h"
#include "jobmanager.h"


// openhbci2 includes
#include <openhbci2.h>
using namespace HBCI;

// AqMoney includes
#include "aqmoney2/aqmapi.h"
#include "aqmoney2/accountjobs.h"
#include "aqmoney2/adminjobs.h"

//gwenhywfar includes
#include <gwenhywfar/directory.h>





/* Internationalization */
#ifdef ENABLE_NLS
# include <libintl.h>
# include <locale.h>
# define I18N(m) gettext(m)
#else
# define I18N(m) m
#endif

class AccountJobFactory;
class AdminJobFactory;
class DlgLogMessages;
class HoBIT_Auth;
class HoBIT_TransferTemplate;
class DlgStatusMessage;

/** The error codes for hobit functions.
*/
 
typedef enum {
    HobitErrorSuccess=0,
    HobitErrorArguments,
    HobitErrorLoad,
    HobitErrorSave,
    HobitErrorInvalidArgs,
    HobitErrorInconsistentSystem,
    HobitErrorExecution,
    HobitErrorReadingfile,
    HobitErrorWritingfile,
    HobitErrorSequence,
    HobitErrorNotConnected,
    HobitErrorInvalidMediumType
} HobitError;

namespace banking{
  class TransferItem;
}

/** The HoBIT-Class is the Main Interface between the Money-Penny Client
* and the HBCI_Cern engine. HoBIT contains methods for all hbci-usecases, that
* are executed by moneypenny. HoBIT is used as a singleton. However, it may be renewed completely
* using the static method newHoBIT. Used the static method hobit() to get a pointer to the HoBIT
* engine. This will throw an exception if no HoBIT engine is available. 
* DO NOT save the returned pointer. It may be deleted without notice.
*
* <b> How to use this class: </b>
*
* First you have to create an instance of the HoBIT-class using the static method newHoBIT:
*
* <p> newHoBIT(...);</p>
*
* Look at the description of the (private) base constructor to find out which 
* parameters will be needed. <br>
*
* After you have an existing instance of the HoBIT-Class, you have to
* create a new user, calling the hobit->createUser(...); <br>
*
* Now you can start to create jobs calling creator methods:
*
* <p>
* hobit->createJobFirstKeyRequest(); <br>
* hobit->createJobGetSystemId(); <br>
* hobit->createJobGetBalance(); <br>
* hobit->createJobGetTransactions(...); <br>
* hobit->createJobStartStandingOrder(...); <br>
* ... and so on...
* </p>
*
* HoBIT's Jobmanager stores jobs internally in 2 different job queues (AdminJobs and AccountJobs).
* To execute stored hbci-jobs you will just have to perform: 
*
* <p> hobit->executeJobQueue();
*
* This will execute both queues (AdministrationJobs and AccountJobs)
* <p>
* <i> <b>
* OpenHbci support is used for legacy data only. All bankingstuff is done via _Jobmanager.
* Datastorage is done via _Database
* 
* 
* </b> </i>
*<br>
*@author Marcel Mulch, Walter Roth
*
*@date 22.10.2004, 15.5. 2011
*/
class HoBIT : public QObject {
 Q_OBJECT
signals:
 void logTextToShow(QString logText); 

  //signals from jobmanager. connected in createApi
  /**Emitted when a callback for job execution was received.
  */
  void jobStatusChanged(banking::BankingJob * job);
  /**Emitted after job has been passed to GV_Api and job id has become valid.
  */
  void sentAdminJob(int row, banking::BankingJob * job);
  /**Emitted after job has been passed to GV_Api and job id has become valid.
  */
  void sentTransferJob(int row, banking::BankingJob * job);
  /**Emitted when a card must be inserted into reader.
   */
  void insertCardRequest();
 /**Emitted when a pin must be input via reader's keyboard.
   */
  void inputPinViaKeypadRequest();
  /**Emitted when a pin must be input via main keyboard.
   */
  void inputPinViaKeyboardRequest( QString * pin );
  /**Emitted when pin was ok.
   */
  void pinWasOk();
  /**Emitted when pin was NOT ok.
   */
  void pinWasNotOk(int triesLeft);
  /**Emitted when a PIN is required for PIN-TAN procedure.
   */
  void pinRequest( QString * pin );
  /**Emitted when a TAN is required for PIN-TAN procedure.
   */
  void tanRequest(QString bankmessage, QString * tan );
  /**Emitted when a certificate path is required for a PIN-TAN ssl connection.
   */
  void certificatePathRequest( QString * certificatePath );
  /** Emitted when api asks for hash verification. If hash is ok,
    * hash string must be modified (and returned) as "OK".
    */
  void verifyBankKeyHash(string * hash);
  /** Sent, when Error messages (9XXX) have been received
  */
  void errorMessagesReceived(const QString & messages);
  /** Sent, when messages have been received and no 9XXX message was detected
  */
  void messagesReceived(const QString & messages);
  /**Sent when thread is starting. Just for naming consistency.
    *Directly connected to QThread::started()
    */
  void kernelThreadStarted();
  /**Sent when thread is finished. 
    * @PARAM OK true, when no error was detected
    */
  void kernelThreadFinished(bool ok);
  /**Sent when a message is about to be sent by kernel.
   */
  void sendingMessage(QString msg);
 /**Sent when a response without error was received by kernel.
  */
  void responseOk(QString msg);
 /**Sent when a response with an error message was received by kernel.
  */
  void responseError(QString msg);
 /** Sent when a set of data has been read from a chipcard.
  */
  void cardDataRead(int country, QString bankId, QString userId, QString serverUrl, QString readerName);
 /**
   * Sent when jobmanager caught an exception
   */
  void caughtException(exception e);
 /**
   * Sent when a user- overrideable error warning was detected
   */
  void overridableErrorDetected(bool * override, const QString & msg);
  //end Jobmanager signals****************************************************

  //signals from Database****************************************************

  /** To be used when long computations are performed. e.g. readBankingJobList, writeBankingJobList
   */
  void statusMessage(const QString & message);
  /** reports job number
   */
   void processingJob(int jobNumber);

public slots:
   /**Called when jobmanager has valid jobs to analyze.
    */
   void slotAnalyzeAdminJobs();
   /**Called when the user pressed the cancel button in GuiProgressIndicator
    */
   void slotCancelKernelAction();

private:
/**
*base constructor for the HoBit QT/OpenHBCI-Interface
*
*In Order to work with HoBIT properly, you will have to
*create an HoBIT-Objects, which creates jobs and executes the
*job queues. HoBIT is the most important interface between
*the graphical user client and AqMoney2
*
*
* @param parent This is the qwidget which uses HoBIT. in most cases this
*		will be the main widget of the hbci-client
*
* @param openhbcifile (legacy) name of the file, which shall store the hbci-configuration
*			please consider, that this is ONLY THE FILENAME without any
*			path indication. The file will be created in the users main
*			directory. not used any more
*
* @param aqmoneyfilename (legacy) of the file, which shall store the accountinformation
*			please consider, that this is ONLY THE FILENAME without any
*			path indication. The file will be created in the users main
*			directory. not used any more
*
* @param applicationname (legacy) for use with aqmoney only, not used any more
* @param hbcidatadir the name of the directory where logfiles and transaction files will
*		 be stored. Please consider, that this should be only the name of the
*		 directory (for example ".moneypenny"). The directory will be created 
*		 in the users home directory. 
*
* @param configdatadir the name of the top folder in home directory for the uncrypted moneypenny data files
*/
HoBIT(bool readonly,
QWidget *parent,
const QString &openhbcifile,
const QString &aqmoneyfile,
const QString &templatefile,
const QString &hbcidatadir,
const QString &configdatadir,
const QString &applicationname,
HoBIT_Connector * connector);
	
/** base destructor (not of importance) 
*/	
virtual ~HoBIT();

/** The singleton HobIT. Use it with the hobit() function.
*/
static HoBIT * theHoBIT;

/** The singleton LegacyHobIT for loading OpenHBCI data. Use it with the legacyHobit() function.
*/
static HoBIT * theLegacyHoBIT;

public: 
/** Creates a new HoBIT engine. For parameters see private constructor above.
*/
static void newHoBIT(bool readonly,
QWidget *parent,
const QString &openhbcifile,
const QString &aqmoneyfile,
const QString &templatefile,
const QString &hbcidatadir,
const QString &configdatadir,
const QString &applicationname,
HoBIT_Connector * connector);

/** Returns a pointer to the Hobit engine. Will throw an execption, if no HoBIT engine exists.
*/ 
static HoBIT * hobit();

/** Deletes the Hobit engine. 
*/ 
static void deleteHoBIT();


/**Create a filename from date. Create path if not yet available.
*/
QString createLogFilename();

/** This method executes the JobManager's AdminJobs and AccountJobs lists.
* Jobs are not executed before this method is called. 
*/
bool executeJobLists(const QString & pin);

 /** Wake up thread. To be called when kernel is waiting for input.
 */
 void wakeKernelThread();

/**
*  clears job queues for account and admin
*   jobs.
*/
void clearJobLists();

/**  This method creates a new User configuration from the data in user and bank
  *  Parameter data override data on medium. 
  */
bool copyUserAndBank( const banking::User * user, const banking::Bank * bank);

/**  This method creates a new User configuration.
  *  Parameter data override data on medium. 
  */
bool createUser( int passportType,
                 const QString & passportFile,
                 const QString & country,
                 const QString &bankName,
                 const QString & bic,
                 const QString & bankCode,
                 BankType bankType,
                 int hbciVersion,
                 const QString &rdhVersion,
                 const QString &userId,
                 const QString &customerId,
                 const QString &serverName);
 
 /**Copy new system id from BPD to bank
   * Call this function after BPD has been updated.
   */
 void updateSystemId();

  /** Reads the data from the chipcard. There MAY be userid, bankid, server. 
    * Returns the number of valid user data sets.
    */
  int readDDVCardData(int & country, QString & bankId, QString & userId, QString & serverUrl, QString & readerName);

  /** Writes the data to record recordNumber on the chipcard. 
   * Will ask for PIN via signal inputPinViaKeyboardRequest or inputPinViaKeypadRequest.
   * There MUST be userid, bankid, server.
   * Returns true on success.
   */
  bool writeDDVCardData(int recordNumber, const QString & bankName, const QString & bankId, 
                        const QString & userId, const QString & serverUrl);

/** Returns true for a valid account
*/
bool isCurrentAccountValid();

/** Loads the OpenHBCI environment and writes data to targetDir
*/
bool loadLegacyData(const QString & legacyDataDir, const QString & passportFileDir);

/** 
* Loads the OpenHBCI environment when the Program is started 
* This method reads the configuration files stored in the 
* users home directory
* @param passportFileDir directory with the passport file. May be empty, if no passport file is required.
*/
bool loadEnvironment(const QString & passportFileDir);


/** Saves the OpenHBCI environment when the Program is shut 
* This method writes the environment to the configurationfiles 
* stored in the users home directory
*/
bool saveEnvironment();

/**Saves the present key data. This is required after getting the bank's public keys.
 */
bool saveKeyfile(const QString & filename);

/** adds a logtext */
void showLogText(QString string);



//-------------------------------------------------------------------------
//DB-AccessOperations
//-------------------------------------------------------------------------

/** Returns a std::list which includes all registered users for the current bank
*/
virtual  const banking::UserList * users();

/** removes all existing banks and users from the gwen database (config-files)
*/
void deleteAllBanks();

/**Deletes bank, if it is in the list
*/
bool deleteBank(const QString & bankCode);

/** searches the api for a special bank */
//Pointer<HBCI::Bank> findBank(int country, const string& instId);

/** Return the type of bank. Returns BANKTYPE_UNKNWON, if
  * banktype could not be detected.
  */
static BankType findBankType( const QString & server, const QString & bankName, int passportType);

/** changes the HBCI version to be used. Returns true if ok.
*/
bool setHbciVersion(int newVersion);


/** creates a new account.Strips leading zeros from accountId, if not 00000000 
* @param accountId: the account number
* @param accountSubId: Hbci allows subids for accounts 
* 			 this parameter is normaly not needed
* @param accountName:you can give your account an description
*
*/			 
virtual bool addAccount(
			const QString &accountId = MONEYPENNY_DUMMY_ACCOUNT_ID, 
			const QString &accountSubId = "", 
			const QString& accountName = tr("No account defined"));

/** Deletes account accountId of current user 
*/
bool deleteAccount(QString accountId);

/** changes the account description */
void changeAccountName(const string& accid, const string& name);



/** get a read-only list of all accounts 
*/
const banking::AccountList * accounts()const;



/** searches the aqmapi for a single account
*/
//Pointer<Account> findAccount(const QString &accid);



/** Returns a new list of transactios for the current account
 * during a time period specified  by _FromDate and _ToDate.
 * Use setFromTo() to set the dates.
 * List must be deleted by caller.
 */
banking::TransactionList * listTransactions();

/** Set _FromDate and _ToDate. 
 * 
 */
void setFromTo(const QDate &fromDate, const QDate &toDate);
 

/** returns a list of dated transfers 
 * concerning the current account. 
 * List must be deleted by caller.
 */ 
banking::TransferList * listDatedTransfers();


/** returns a list of standing orders
* concerning a single account. Call listStandingOrders(0, "*", "*", "*") to
* get all standing orders from all accounts.
*
* @param country the banks countrycode (280 for a german bank)
*		set to 0 for all banks
*
* @param bankCode the bankId (in german Bankleitzahl) of a bank
*		 set to "*" for all stored banks
*
* @param bankCode the accountId of an account
*		 set to "*" for all stored accounts of the bank
*
* @param accounSubId in HBCI accounts can have subids. in most 
* 			cases you should pass "" here
*/ 
banking::TransferList * listStandingOrders(int country,
const QString &bankCode,
const QString &accountId,
const QString &accountSubId);
 

/**Exports current list of transactions to file as CSV formatted data.
*/
bool exportCSV(QFile * file, char fieldSeparator, char textSeparator);

/**Exports all data to file as SQL formatted data.
*/
bool exportSQL(QFile * file);

/** returns a std::string, which describes the iniletter 
*/ 
QString createIniLetter(const QString & password, const QString & customerName);

/** Returns the date of the last stored transaction of current account
  * !!!!WARNING: This date may be in the future!!!!
  */
QDate lastDateOfTransactions();

/**
* returns a list of all known banks. Banks are 
* returned as banking::Bank-objects
* Please note that ktoblzcheck must be installed
* to use this function,
*/
const QList<banking::Bank> & listOfAllKnownBanks();

/**
* returns a list of all known banks that support HBCI. Banks are 
* returned as banking::Bank-objects
* Please note that fints_isnstute.csv must be installed
* to use this function,
*/
QList<banking::Bank> listOfKnownHbciBanks();

/** Searches the list of all banks for a bank with bankcode bankCode, that has a BIC entry.
 * If no BIC is found, an empty string is returned. Pretty slow. But used only once.
 */
QString getBicForBankCode(const QString & bankCode);

 /** returns the hash fingerprint of the institute after resceiving the bank servers public keys 
 */
QString bankFingerPrint();

/** Returns the name (url) of the bank server
*/
QString currentBankServer();
 

/** adds a transfer template to database. Transfer is owned by database.
*/
void addTransferTemplate(banking::Transfer * transferTemplate);
 			
/** deletes a transfer template 
*/
void deleteTransferTemplate(int row);

/** returns all transfer templates 
*/
banking::TransferList listTransferTemplates(); 

//-------------------------------------------------------------------------
//Administration Jobs
//------------------------------------------------------------------------

/** returns the account job queue 
*/
QList<Pointer<AdminJob> > getAdminJobQueue();

/** deletes a certain admin job from the adminjob queue 
*/
void deleteAdminJob(int number);

/** 
 * this method creates a job to get the servers public keys and puts 
 * it into the Admin Job Queue. 
 */
int createJobFirstKeyRequest();

/** creates a new job, which will retrieve a new system id for a NEW key
 *The system id is needed to identify the client system 
 *to the bank server.
*/
int createJobGetNewSystemId();

/** creates a new job, which will retrieve system id and
 * sequence number (signature id) for an already active key
*/
int createJobResynchronize();
 
/** Creates a job, which retrieves a list of all hbci accounts, 
*  the bank parameter data (BPD) and user Parameter Data (UPD)
*/
int createJobGetUPD();

/** creates a job send keys which will deliver the own public 
* keys to the bank server 
* This job is stored in the Account Job Queue
*/
int createJobFirstKeySend();
/** creates a job change keys which will send a new public key
*   to the bank server 
* This job is stored in the Account Job Queue
*/
int createJobKeyChange();


/** creates a new security medium using the given information. Creates a user, if not already available 
* @ param keyfilename The full path to the new keyfile. This is not used for chipcard passport types.
* @ param newPassword the password for a keyfile. This is not used for chipcard passport types.
*/
bool  createNewKeyfile   ( C_uarray entropy,
                           const QString & rdhVersion, 
                           int passportType,
                           const QString & keyFileName,
                           const QString & newPassword,
                           const QString & country,
                           const QString & bankCode,
                           int hbciVersion,
                           const QString & serverName,
                           const QString & userId,
                           const QString & customerId);


//-------------------------------------------------------------------------
//Account Jobs
//------------------------------------------------------------------------

/**
  * Generates a DTA file from jobs in Jobqueue that match type.
  * Returns file contents as a QString.
  */
QString generateDTAFile(banking::BankingJob::JobType type);

/** returns the account job queue */
QList<Pointer<AccountJob> > getAccountJobQueue();

/** deletes a certain account job from the accountjob queue */
void deleteAccountJob(int number);

/**Returns a pointer to the pending transfer at index. Returns NULL if index is out of range.
*/
 const banking::Transfer * pendingTransfer(int index);

/**Returns number of pending transfers. 
*/
int pendingTransfers();
/**Returns number of pending debitnotes. 
*/
int pendingDebitNotes();

/**Returns the number of new jobs in the lists
 */
int pendingJobs();

/**Returns jobmanager
*/
JobManager * jobManager();

/**Returns database
*/
DatabaseAccess * database();

/** creates jobs to get balance to retrieve the balances of all 
accounts hobit maintains. Returns number of jobs.
*/
int createJobGetBalance(const QString & accountId);

/** creates a job get get transaction which creates a job for retrieving all transactions
*belonging to a specific account during a defined time period. Returns number of jobs. 
*
* @ param fdate the date of the first transaction to get
* @ param tdate the date of the last transaction to get
* @ param accountId the id of the account to get transactions for. * gets all
*/
int createJobGetTransactions(QDate fdate, QDate tdate, const QString& accountId);

/** creates a job get get dated transfers which creates a job for retrieving 
*all dated transfers belonging to a specific account during a defined time period 
*
* @ param fromDate the date of the first transaction to get
* @ param toDate the date of the last transaction to get
* @ param accountId the id of the account to get transactions for
*/
int createJobGetDatedTransfers(QDate fromDate, QDate toDate, const QString& accountId);

/** creates a job get get standing orders which creates a job for retrieving 
*all standing orders belonging to a specific account 
* @ param accountId the id of the account to get transactions for
*/
int createJobGetStandingOrders(const string& accountId);
 
/** Creates a transfer (single transfer, dated transfer or standing order) job.  Returns number of transfers in list.
 */
int createJobTransfer(banking::Transfer * transfer);

 /** Creates a job to delete a dated transfer.
*/
int createJobDeleteDatedTransfer(banking::Transfer * transfer);
 
 /** creates a new job for starting a standing order
 */	
int createJobStartStandingOrder(banking::TransferItem * transfer);

 /** Deletes a standing order */
int createJobDeleteStandingOrder(banking::TransferItem * transfer);

/** Removes the data directory using a simple rm
*/ 
void removeUncryptedEnvironment();

/** Returns the name of the bank for the given code (BLZ). Returns an empty string, if no bank could be found.
*/
QString getBankNameForId(const QString & code);

/** Returns the active bank 
 */
const banking::Bank * bank()const;
/** Returns the active user
 */
banking::User * user();
/**Returns the user's name or an empty string.
 */
QString userName();
/** Returns the bankId BLZ
*/
QString bankCode()const;
/** Returns the type of the bank (Volksbank, Sparkasse...)
 */
BankType bankType()const;
/** Returns the bank's name
*/
QString bankName()const;
/**Returns last error messages from the bank.
*/
const QString & bankMessages();

/** Returns the currently used complete medium path (including the filename) from OpenHbci database. 
*/
//QString getMediumPath();

/** Import an RDH2 formatted key. Format is specified in 
* "FinTS-Format der RDH2 Diskette" by PPI, Hamburg. see www.hbc-kernel.org
*/
bool importRDH2Key(const QString & fileName, const QString & password);

/** Returns true for acceptable and intermediate accountId strings
*/
bool validateAccountIdString(const QString & accountId);

/** Read property of AdminJobFactory m_AdminJobFactory. 
*/
AdminJobFactory * getAdminJobFactory()const;

/** Read property of AccountJobFactory m_AccountJobFactory. 
*/
AccountJobFactory * getAccountJobFactory()const;

/** returns the current HBCI mode */
//HBCI_MODE getHbciMode();

/** Returns true, if no jobs are in job queues
*/
bool areJobQueuesEmpty();

/**Returns current account. May return NULL.
*/
banking::Account * currentAccount();
/**Returns current bank. May return NULL.
*/
banking::Bank * currentBank();
/**Returns current bankcode. May return an empty string.
*/
QString currentBankCode();
/**Returns current BIC. May return an empty string.
*/
QString currentBic();
/** Read property . 
*/
QString  currentAccountId()const;
/** Read property . 
*/
QString  currentIban()const;
/** Read property of QString CurrentAccountName. 
*/
QString currentAccountName()const;
/** Write property of QString CurrentAccount. 
*/
void setCurrentAccountId(const QString &bankCode, const QString& _newVal);
/** Write property of QString CurrentAccountName. 
*/
void setCurrentAccountName(const QString& _newVal);
/** Set the security profile version for new bank connections
*/
void setSecProfileVersion(const QString & newVal);

/** Sets the current path to the passport file. This is NOT persistent!
*/
void setPassportFileDir(const QString & dir);

/** Returns temporary config data directory.
*/
QString getConfigDataDir();
/**Sets the name of the current bank, as edited by the user
*/
void setUserDefinedBankName(const QString & newName);
/**Returns the name of the current bank, as edited by the user
*/
const QString &  getUserDefinedBankName();

private:
//OpenHBCI compatibility
/** Returns the current bank */
//Pointer<Bank> getBank();
/** Returns the user from OpenHbci database.
*/
//Pointer<User> getUser();

/** Returns the user's name
*/
//QString getUserName();

/** Returns the first customer from the user's customer list. Return value should be checked with isValid().
*/
//Pointer<Customer> getCustomer();

/** Returns the currently used medium from OpenHbci database. Return value should be checked with isValid().
*/
//Pointer<Medium> getMedium();

/** Returns true, if cardreader is up.
*/
bool isCardReaderOk();

//End OpenHBCI compatibility



/**Creates the aqmoney engine
*/ 
 void createApi();
/** deletes the aqmoney engine
*/
// void deleteApi();
 
private:
//These are owned by HoBIT. Pointer<> types are neccessary to match the aqMoney API.
//!!The Pointer<> class autodeletes the passed pointer!!
Pointer<AqMoneyAPI> _Api;
Pointer<HoBIT_Interactor> _Interactor;
Pointer<HoBIT_Auth> _Auth;
Pointer<HoBIT_Validator> _Validator;

DatabaseAccess * _Database;

//These are copies, which must not be deleted
HoBIT_Connector * _Connector;

//HoBIT_Crypt * _Crypt;

/**aqmoney flag
*/
bool _ReadOnly;

// qt dialogs
QWidget* _Parent;

// db directories
/** OpenHci data
*/
QString _OpenHbciFile;
/** AqMoney data, including accounts
*/
QString _AqMoneyFile;
/** Transfer template data
*/
QString _TemplateFile;
/** home
*/
//QString_HomeDir;
/** transaction data etc. GWEN database
*/
QString _HbciDataDir;
/**The top-directory for the temporary files
*/
QString _ConfigDataDir;
/**The directory for encrypted files
*/
 // string _CryptDir;
/** Hbci password
*/
 // QString_Password;

/** Name of the program
*/
QString _ApplicationName;

/** the list of all known banks.
*/
QList<banking::Bank> _AllBanksList;

/** the list of all banks, that support hbci.
*/
QList<banking::Bank> _HbciBankList;

/**The name of the current bank, as edited by the user
*/
QString _UserDefinedBankName;

// factories
AccountJobFactory * _AccountJobFactory;
AdminJobFactory * _AdminJobFactory;

//pending jobs manager
JobManager * _JobManager;

//The default date limits for transactionList()
QDate _FromDate;
QDate _ToDate;
};

#endif
