/***************************************************************************
                          hobit.cpp  -  description
                             -------------------
    begin                : Do Nov 4 2004
    copyright            : (C) 2004, 2008 by Marcel Mulch, Walter Roth
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

#include "hobit.h"
//#include "keyfilerdh2.h"
#include "keyfileopenhbci.h"
#include "hobit_transfertemplate.h"
#include "accountjobfactory.h"
#include "adminjobfactory.h"
#include "../dta/dtagenerator.h"
#include "kernel/helpers/C_uarray.h"
#include "kernel/global/HBCI_exceptions.h"

#include <QtDebug>
#include <QFile>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QValidator>

#include "../banking/bankingjobitem.h"

#include "../gui/dlgstatusmessage.h"

#include <openhbci2/hbci.h>
#include <openhbci2/auth.h>
using namespace HBCI;

/* Default path for bank data */
#define BANKDATA_PATH "/usr/local/share/ktoblzcheck"


/** The singleton HobIT. Use it with the hobit() function.
*/
HoBIT* HoBIT::theHoBIT = NULL;

/** Creates a new HoBIT engine. For parameters see private constructor above.
*/
void HoBIT::newHoBIT(bool readonly,
QWidget *parent,
const QString &openhbcifile,
const QString &aqmoneyfile,
const QString &templatefile,
const QString &hbcidatadir,
const QString &configdatadir,
const QString &applicationname,
HoBIT_Connector * connector)
{
  if(theHoBIT != NULL)
    delete theHoBIT;
  theHoBIT = new HoBIT(readonly,
                        parent,
                        openhbcifile,
                        aqmoneyfile,
                        templatefile,
                        hbcidatadir,
                        configdatadir,
                        applicationname,
                        connector);
}

/** Deletes the Hobit engine. 
*/ 
void HoBIT::deleteHoBIT()
{
 if(theHoBIT)
  delete theHoBIT;
 theHoBIT = NULL;
}

/** Returns a pointer to the Hobit engine. Will throw an execption, if no HoBIT engine exists.
*/ 
HoBIT * HoBIT::hobit()
{
  if(theHoBIT == NULL)
   throw(ExHbciError("HoBIT banking engine not initialized."));
  return theHoBIT;
}


HoBIT::HoBIT(bool readonly,
             QWidget *parent,
             const QString &openhbcifile,
             const QString &aqmoneyfile,
             const QString &templatefile,
             const QString &hbcidatadir,
             const QString &configdatadir,
             const QString &applicationname,
             HoBIT_Connector * connector)
{
     qDebug("Constructor HoBIT start"); 
     _ReadOnly = readonly;
     _Parent = parent;  
     _OpenHbciFile = openhbcifile; 
     _AqMoneyFile = aqmoneyfile; 
     _TemplateFile = templatefile;
     _HbciDataDir =  hbcidatadir;
     _ConfigDataDir = configdatadir;
     _ApplicationName = applicationname;
     _Connector = connector;
     _AdminJobFactory = NULL;
     _AccountJobFactory=NULL;
     _Database = NULL;
     _JobManager = NULL;

     qDebug() <<"HbciDataDir is: " <<_HbciDataDir;
     qDebug() << "OpenHbciFile is: " << _OpenHbciFile << "\n";
     qDebug() << "AqMoneyFile is: " << _AqMoneyFile << "\n";
     qDebug() << "TemplateFile is: " << _TemplateFile << "\n";

     createApi();
     qDebug("Constructor HoBIT finished"); 
}


HoBIT::~HoBIT(){
  //removeConfigDataDir();
  if(_AdminJobFactory)
    delete _AdminJobFactory;
  if(_AccountJobFactory)  
    delete _AccountJobFactory;
  if(_Database)  
    delete _Database;
  if(_JobManager)
   delete _JobManager;
  qDebug("Destructor HoBIT finished");
}


/**
*  clears job queues for account and admin
*   jobs.
*/
void HoBIT::clearJobLists(){
  _JobManager->clearLists();
}
/**Create a filename form date. Create path if not yet available.
*/
QString HoBIT::createLogFilename()
{
  QDateTime time = QDateTime::currentDateTime();
  QString sTime = time.toString("yyyy_MM");
  //make log directory if neccessary
  QString logDirName = _ConfigDataDir + "/"+ MONEYPENNY_LOG_DIR + "/" + sTime;
  QDir logDir(logDirName);
  if( !logDir.exists())
    logDir.mkpath(logDirName);
  sTime = time.toString("dd_hh_mm_");
  return logDirName + "/" + sTime + MONEYPENNY_HBCI_LOGFILE;
}

/**
*  executes both job queues for account and admin
*   jobs. Results of admin jobs are analyzed by slotAnalyzeAdminJobs().
*/
bool HoBIT::executeJobLists(const QString & pin){
  bool result = _JobManager->executeJobLists(
                   qPrintable(pin),
                   createLogFilename(),
                   _Database->bankDir().c_str()
                  );
/*  if(result)
    result = _Database->evaluateResponseData(_JobManager->executedAdminJobs());*/
 
  return result;
}

/**Called when jobmanager has valid jobs to analyze.
 */
void HoBIT::slotAnalyzeAdminJobs()
{
  try
  {
    _Database->evaluateResponseData(_JobManager->executedAdminJobs()); //analyse while kernel thread is waiting
    _JobManager->wakeKernelThread();
  }
  catch(exception & e)
  {
    qDebug("HoBIT::slotAnalyzeAdminJobs(): Caught exception: %s", e.what());

    _JobManager->wakeKernelThread();
    emit caughtException(e);
  }
  catch(...)
  {
    qDebug("HoBIT::slotAnalyzeAdminJobs(): Caught an unknown exception.");
    _JobManager->wakeKernelThread();
    emit caughtException(exception());
  }
}

/**Called when the user pressed the cancel button in GuiProgressIndicator
 */
void HoBIT::slotCancelKernelAction()
{
  _JobManager->cancelKernelAction();
}


/** Wake up thread. To be called when kernel is waiting for input.
 */
void HoBIT::wakeKernelThread()
{
  _JobManager->wakeKernelThread();
}

/**  This method creates a new User configuration from the data in user and bank
  *  Parameter data override data on medium. 
  */
bool  HoBIT::copyUserAndBank( const banking::User * user, const banking::Bank * bank){
   bool result = true;

   int passportType = user->passportType();
   QString passportFile = user->passportFile().c_str();
   QString bic = bank->bankBic().c_str();
   QString country = bank->country().c_str();
   QString bankCode = bank->bankCode().c_str();
   QString bankName = bank->bankName().c_str();
   int hbciVersion= bank->hbciVersion();
   QString rdhVersion = bank->rdhVersion().c_str();
   QString userId = user->userId().c_str();
   QString customerId = user->customerId().c_str();
   QString serverName = bank->server().c_str();

   BankType bankType = findBankType ( serverName, bankName, passportType);

   result =  _Database->createUser( passportType,
                                   passportFile,
                                   country,
                                   bankName,
                                   bic,
                                   bankCode,
                                   bankType,
                                   hbciVersion,
                                   rdhVersion,
                                   userId,
                                   customerId,
                                   serverName);

  if(result){ //data ok, try to create a new user and bank
    setUserDefinedBankName(bankName);

    //copy accounts from user
    banking::AccountList::const_iterator itAccount;
    for(itAccount = user->accounts().begin(); itAccount != user->accounts().end(); itAccount++){
       banking::Account * account = (*itAccount);
       addAccount(account->accountId().c_str(), 
                  "", //account->accountSubId().c_str(), 
                  account->accountName().c_str());
    }//end account loop
  }

  return result;
}

/**  This method creates a new User configuration.
  *  Parameter data override data on medium. 
  */
bool HoBIT:: createUser( int passportType,
                         const QString &passportFile,
                         const QString & country,
                         const QString &bankName,
                         const QString &bic,
                         const QString &bankCode,
                         BankType bankType,
                         int hbciVersion,
                         const QString &rdhVersion,
                         const QString &userId,
                         const QString &customerId,
                         const QString &serverName){

   return  _Database->createUser( passportType,
                                   passportFile,
                                   country,
                                   bankName,
                                   bic,
                                   bankCode,
                                   bankType,
                                   hbciVersion,
                                   rdhVersion,
                                   userId,
                                   customerId,
                                   serverName);
}

/**
  * Generates a DTA file from jobs in Jobqueue that match type.
  * Returns file contents as a QString.
  */
QString HoBIT::generateDTAFile(banking::BankingJob::JobType type)
{
  return DTAGenerator::generateDTAFile(type);
}

/**Copy new system id from BPD to bank.
  * Call this function after BPD has been updated.
 */
void HoBIT::updateSystemId(){
  _Database->updateSystemId();
}

/** Loads the OpenHBCI environment and writes data to targetDir
*/
bool HoBIT::loadLegacyData( const QString & legacyConfigDataDir, const QString & passportFileDir)
{
 //Read OpenHBCI data, if available
  if(! _Database->readOpenHbciData(legacyConfigDataDir + "/", passportFileDir))
    return false;
  this->saveEnvironment(); 
  const banking::AccountList * accountList = accounts();
  banking::AccountList::const_iterator itAccount = accountList->begin();
  banking::Account * account;
 while(itAccount != accountList->end())
 {
   account = (*itAccount);
   setCurrentAccountId(account->bankCode().c_str(), account->accountId().c_str());
   _Database->addLegacyTransactionList(qPrintable(legacyConfigDataDir));
   itAccount++;
 }
  return true;
}

/** Loads MP2 database and the OpenHBCI environment when the program is started 
*/
bool HoBIT::loadEnvironment(const QString & passportFileDir){
   bool result = true;
   _Database->setDataDir(qPrintable(_ConfigDataDir));
   if (!_Database->readLists()) {
    qDebug("Error reading MP2 lists.");
    result = false;
   }
   else {
      result = true;
    }

//Finally set the passport file directory
  setPassportFileDir(passportFileDir);
  return result;
}


/** Saves the OpenHBCI environment and data  */
bool HoBIT::saveEnvironment(){
    bool result = true;
    QString configDirName(QString(_ConfigDataDir + "/").toLatin1().data());
    qDebug() << "Writing banking data.\n";
    result = _Database->saveLists();
    return result;
}

/** shows the logwindow and adds a logtext*/
void HoBIT::showLogText(QString string){
  emit logTextToShow(string);
}

/** creates a new security medium using the given information 
* @ param keyfilename The full path to the new keyfile. This is not used for chipcard passport types.
* @ param newPassword the password for a keyfile. This is not used for chipcard passport types.
* @ param rdhVersion 1, 2 or 10 according to user settings
*/
bool  HoBIT::createNewKeyfile   (
                           C_uarray entropy,
                           const QString & rdhVersion,
                           int passportType,
                           const QString & keyFileName,
                           const QString & newPassword,
                           const QString & country,
                           const QString & bankCode,
                           int hbciVersion,
                           const QString & serverName,
                           const QString & userId,
                           const QString & customerId)
{
   QString logFileName = createLogFilename();
   if(! _JobManager->createNewKeyfile(rdhVersion,
                               passportType,
                               entropy,
                               bankCode,
                               country,
                               serverName,
                               userId,
                               newPassword,
                              "1",  //keynumber
                               keyFileName,
                               logFileName)) 
    return false; 

//   _JobManager->setBank(_Database->currentBank());
   else return true;
}


  //-------------------------------------------------------------------------
  //  DB-Access Operations
  //-------------------------------------------------------------------------

  /** Returns a std::list which includes all registered users as a struct
    */
  /** Returns a std::list which includes all registered users for the current bank
*/
const banking::UserList * HoBIT::users(){
  return _Database->users();
}


/** removes all existing banks from the gwen database */
void HoBIT::deleteAllBanks(){
  qDebug("Deleting Banks");
    _Database->deleteAllBanks();
  qDebug("Banks deleted");
}

/**Deletes bank, if it is in the list
*/
bool HoBIT::deleteBank(const QString & bankCode){
     qDebug("Deleting Bank: %s", qPrintable(bankCode));
     return _Database->deleteBank(qPrintable(bankCode));
}

  /** searches the api for a special bank */
 /* Pointer<HBCI::Bank> HoBIT::findBank(int country, const string& instId){
    return _Api.ref().findBank(country, instId);
  }
*/
/** Return the type of bank. Returns BANKTYPE_UNKNWON, if
  * banktype could not be detected.
  */
BankType HoBIT::findBankType( const QString & server, const QString & bankName, int passportType)
{
  bool isSparkasse = false;
  bool isVolksbank = false;
  bool isPrivateBank = false;
  BankType bankType = BANKTYPE_UNKNOWN;

  if(bankName.indexOf("Sparkasse",0, Qt::CaseInsensitive) >=0
   || bankName.indexOf("Spk",0, Qt::CaseInsensitive) >=0)
  isSparkasse = true;
  if(bankName.indexOf("Volksbank",0, Qt::CaseInsensitive) >=0
   || bankName.indexOf("VB",0, Qt::CaseInsensitive) >=0
   || bankName.indexOf("Raiffeisenbank",0, Qt::CaseInsensitive) >=0
   || bankName.indexOf("Hypo",0, Qt::CaseInsensitive) >=0
   || bankName.indexOf("Genossenschaftsbank",0, Qt::CaseInsensitive) >=0
   || server.indexOf(".GAD",0, Qt::CaseInsensitive) >=0
   || server.indexOf(".Fiducia",0, Qt::CaseInsensitive) >=0)
  isVolksbank = true;
  if(bankName.indexOf("Dresdner Bank",0, Qt::CaseInsensitive) >=0
   || bankName.indexOf("Deutsche Bank",0, Qt::CaseInsensitive) >=0)
  isPrivateBank = true;
  if(bankName.indexOf("Postbank",0, Qt::CaseInsensitive) >=0)
    bankType = BANKTYPE_POSTBANK;

  if(isSparkasse && ! isVolksbank && !isPrivateBank)
    bankType = BANKTYPE_SPARKASSE;
  else if(!isSparkasse &&  isVolksbank && !isPrivateBank)
    bankType = BANKTYPE_GAD;
  else if(!isSparkasse && ! isVolksbank && isPrivateBank)
    bankType = BANKTYPE_PRIVATE;

  if(passportType & PASSPORT_SECURITY_DDV) //DDV is always Sparkasse
    bankType = BANKTYPE_SPARKASSE;
  return bankType;
}

/** Returns the bankId BLZ
*/
QString HoBIT::bankCode()const
{
  return _Database->currentBankCode().c_str();
}

/** changes the used hbci version */
bool HoBIT::setHbciVersion(int newVersion){
    return _Database->setHbciVersion(newVersion);
}
  
  
  /** creates a new account.Strips leading zeros from accountId, if not 00000000 
  * @param name: Name of the account owner
  * @param accountId: the account number
  * @param accountSubId: Hbci allows subids for accounts 
  * 			 this parameter is normaly not needed
  * @param accountName:  you can give your account an description
  *
  */		
bool HoBIT::addAccount(const QString &accountId, const QString &accountSubId, const QString& accountName){

 return _Database->addAccount(qPrintable(accountId), qPrintable(accountSubId), banking::to8Bit(accountName));
}
  
  
 /** changes the account description */
  void HoBIT::changeAccountName(const string& accid, const string& name){
  _Database->changeAccountName(accid, name);
  saveEnvironment();
  }

/** get a read-only list of all accounts */
const banking::AccountList * HoBIT::accounts()const{
     return _Database->accounts();
}
  
  
  /** searches the aqmapi for a single account
   */
/*  Pointer<Account> HoBIT::findAccount(const QString& accid)
  {
    Pointer<Account> no;
    list<Pointer<Account> > accounts;
    list<Pointer<Account> >::iterator accit;
    accounts =  _Api.ref().getAccounts(0,
                                       "*",
                                       accid.toStdString(),
                                       "*");

    if (accounts.size()< 1){
            qDebug() << "No matching accounts found\n";
            return no;
    }

    if (accounts.size()>1){
            qDebug() << "Problem in data - to many possible accounts\n";
            return no;
    }

    accit = accounts.begin();
    return *accit;	
  }
*/
  /** Deletes account accountId of current user.
  */
  bool HoBIT::deleteAccount(QString accountId){
      return _Database->deleteAccount(accountId);
  }

/** Set _FromDate and _ToDate. 
 * 
 */
void HoBIT:: setFromTo(const QDate &fromDate, const QDate &toDate)
 {
  _FromDate = fromDate;
  _ToDate = toDate;
 }

/** Returns a list of transactions between _FomDate and _ToDate. List must be deleted by caller.
 */
banking::TransactionList * HoBIT:: listTransactions()
 {
  DateTime fromdate = DateTime(_FromDate.year(), _FromDate.month(),_FromDate.day() );
  DateTime todate = DateTime(_ToDate.year(), _ToDate.month(), _ToDate.day() );
  return _Database->listTransactions (fromdate, todate);
 }


/** Returns all dated transfers for current account
*/
banking::TransferList * HoBIT::listDatedTransfers()
{
  return _Database->listDatedTransfers();
}


/** Returns all standing orders for account
*/
banking::TransferList *  HoBIT::listStandingOrders
 ( int country,
   const QString &bankCode,
   const QString &accountId,
   const QString &accountSubId)
{
  return NULL;

 //   return _Database->listStandingOrders(country, bankCode, accountId, accountSubId);

}

/**Exports current list of transactions to file as CSV formatted data.
*/
bool HoBIT::exportCSV(QFile * file, char fieldSeparator, char textSeparator){
  DateTime fromDate = DateTime(_FromDate.year(), _FromDate.month(),_FromDate.day() );
  DateTime toDate = DateTime(_ToDate.year(), _ToDate.month(), _ToDate.day() );
 return _Database->exportCSV(file, fieldSeparator, textSeparator, fromDate, toDate);
}

/**Exports all data to file as SQL formatted data.
*/
bool HoBIT::exportSQL(QFile * file){
 return _Database->exportSQL(file);
}

/** generates the customers iniletter
*/
QString HoBIT::createIniLetter(const QString & password, const QString & customerName){
     _JobManager->setBank(_Database->currentBank());
     _JobManager->clearLists();
     return _JobManager->createIniLetter(password, customerName, _Database->bankDir().c_str());
}

/** Returns the date of the last stored transaction of current account
  * !!!!WARNING: This date may be in the future!!!!
  */
QDate HoBIT::lastDateOfTransactions()
{
     DateTime d= _Database->getLastDateOfTransactions();
     QDate lastDate = QDate(d.yearNumber(), d.monthNumber(), d.dayNumber());
     return lastDate;
}

/**
  * returns a list of all known banks. Banks are 
  * returned as banking::Bank-objects. 
  */
  const QList<banking::Bank > & HoBIT::listOfAllKnownBanks(){
      if( _AllBanksList.isEmpty()){  
         QString filename = MP_DIR()  + QString(MONEYPENNY_DATA_DIR) + "/"+ QString(MONEYPENNY_ALL_BANKDATA_FILE);
         _AllBanksList = _Database->readBankFile(filename);
      }
//      int banks = _AllBanksList.count(); //for debugging
      return _AllBanksList;
}

/** Searches the list of all banks for a bank with bankcode bankCode, that has a BIC entry.
 * If no BIC is found, an empty string is returned. Pretty slow. But used only once.
 */
QString HoBIT::getBicForBankCode( const QString & bankCode)
{
  bool found = false;
  int i = 0;
  string bc = qPrintable(bankCode);
  string result = "";
  if(_AllBanksList.isEmpty()) //read list, if not available
    _AllBanksList = listOfAllKnownBanks();
  while ( ! found && i < _AllBanksList.count() ){
    if( _AllBanksList[i].bankCode() == bc && ! _AllBanksList[i].bankBic().empty()){
     result = _AllBanksList[i].bankBic();
     found = true;
    }
    i++;
  }
  return QString(result.c_str());
}

/**
  * returns a list of all known banks, that support HBCI. Banks are 
  * returned as banking::Bank objects. Reads and updates bank files automatically, if
  * a file fints-institute.csv exists. 
  * !!!The fints-institute files MUST NOT be distributed. Copyright by ZKA!!!
  */
  QList<banking::Bank> HoBIT::listOfKnownHbciBanks(){
  if(_HbciBankList.empty()){ //not yet read, read now
    QString filename = MP_DIR()  + QString(MONEYPENNY_DATA_DIR) 
                       + "/" + QString(MONEYPENNY_HBCI_BANKDATA_FILE); //hbd file
    QFile file(filename);
    if(!file.exists()){ //no hbd file, try to create one from csv file
      filename = MP_DIR()  + QString( MONEYPENNY_DATA_DIR ) 
                         + "/"+ QString( MONEYPENNY_OLD_HBCI_BANKDATA_FILE );
      file.setFileName(filename); 
      if(file.exists()){ //evaluate csv file and create the fast binary hbd format file
        qDebug("HoBIT::listOfKnownHbciBanks: Updating HBCI-bank list from %s \n",
                qPrintable(filename));
        _HbciBankList = _Database->readBankFile(filename.toLatin1().data());
        _AllBanksList = listOfAllKnownBanks(); //Read the list, if not already done
        //Get the BICs for the banks from Bundesbank list bankdata.txt
        for(int nBank = 0; nBank < _HbciBankList.count();nBank++){
          banking::Bank * hbciBank = &_HbciBankList[nBank];
           hbciBank->setBankBic( qPrintable(getBicForBankCode( hbciBank->bankCode().c_str() )) );
           qDebug("Adding BIC to bank: %i", nBank);
        }
        //We have got the data, now write the .hbd file
        filename = MP_DIR()  + QString(MONEYPENNY_DATA_DIR) + "/" + QString(MONEYPENNY_HBCI_BANKDATA_FILE);
        file.setFileName(filename); 
        bool ok = true;
        if(!file.open(QIODevice::WriteOnly))
         QString rootPassword = QInputDialog::getText( _Parent, tr("Information"),
          tr("Can not update bank data file. No write access. \nEnter root password to let MoneyPenny try to write updated bankdata file."),
           QLineEdit::Password,
           0,
           &ok);
        if(ok){ //try to write  
             ok = _Database->writeHbdBankFile(filename.toLatin1().data(), _HbciBankList);
        }
        if(!ok){
            QMessageBox::critical(_Parent, tr("Unrecoverable Error"), tr("Writing bankdata failed. Aborting now."));
            exit(1);
        }
       _HbciBankList = _Database->readBankFile(filename.toLatin1().data()); 
    }
    }
   //hbd file exists, read it
    else {
       filename = MP_DIR()  + QString(MONEYPENNY_DATA_DIR) + "/" + QString(MONEYPENNY_HBCI_BANKDATA_FILE);
       file.setFileName(filename); 
       _HbciBankList = _Database->readBankFile(filename.toLatin1().data());
    }
  }// end list is empty
  return _HbciBankList;
}


 /** returns the hash fingerprint of the institute after resceiving the bank servers public keys 
 */
  QString HoBIT::bankFingerPrint(){
    return _JobManager->bankFingerprint();
  }

  
  /** returns all transfer templates */
  banking::TransferList HoBIT::listTransferTemplates(){
     return _Database->listTransferTemplates();
  }

/** adds a transfer template to database
*/
void HoBIT::addTransferTemplate(banking::Transfer * transferTemplate){
   _Database->addTransferTemplate(transferTemplate);
}


 /** creates a new transfer template 
*/
void HoBIT::deleteTransferTemplate(int row)
{
   _Database->deleteTransferTemplate(row);
}

//-------------------------------------------------------------------------
//  AdminJobs
//-------------------------------------------------------------------------

/** returns the account job queue */
  QList<Pointer<AdminJob> > HoBIT::getAdminJobQueue(){
    return QList<Pointer<AdminJob> >::fromStdList(_AdminJobFactory->queue());
  }

  /** deleteAdminjob */
  void HoBIT::deleteAdminJob(int number){
    _AdminJobFactory->deleteJob(number);
  }

/** creates a job get keys, which will be executed directly */
int HoBIT::createJobFirstKeyRequest(){
 _JobManager->setBank(_Database->currentBank());
 _JobManager->clearLists();
 return _JobManager->createJobFirstKeyRequest();
}


/** creates a job get systemid for a new key
*/
int HoBIT::createJobGetNewSystemId(){
 _JobManager->setBank(_Database->currentBank());
 _JobManager->clearLists();
 return _JobManager->createJobGetNewSystemId();
}


/** creates a job to resynchronize an already active key
*/
int HoBIT::createJobResynchronize(){
 _JobManager->setBank(_Database->currentBank());
 return _JobManager->createJobResynchronize();
}


/** creates a job to get UPD and BPD */
int HoBIT::createJobGetUPD(){
 _JobManager->setBank(_Database->currentBank());
 return _JobManager->createJobGetUPD();
}


/** creates a job send keys, which will be queued */
int HoBIT::createJobFirstKeySend(){
 _JobManager->setBank(_Database->currentBank());
 return _JobManager->createJobFirstKeySend();
}

/** creates a job change keys which will send a new public key
*   to the bank server 
* This job is stored in the Account Job Queue
*/
int HoBIT::createJobKeyChange(){
 _JobManager->setBank(_Database->currentBank());
 return _JobManager->createJobKeyChange();
}


//-------------------------------------------------------------------------
//  Account Jobs
//-------------------------------------------------------------------------

/** returns the account job queue */
 QList<Pointer<AccountJob> > HoBIT::getAccountJobQueue(){
  return QList<Pointer<AccountJob> >::fromStdList(_AccountJobFactory->queue());
 }


/** deleteAccountjob */
  void HoBIT::deleteAccountJob(int number){
   _AccountJobFactory->deleteJob(number);
  }

/** creates jobs to get balance to retrieve the balances of all 
accounts hobit maintains. Returns number of jobs.
*/
int HoBIT::createJobGetBalance(const QString & accountId){
 _JobManager->setBank(_Database->currentBank());
 return _JobManager->createJobGetBalance(accountId);
}

/** creates a job get get transaction which creates a job for retrieving all transactions
*belonging to a specific account during a defined time period. Returns number of jobs. 
*
* @ param fdate the date of the first transaction to get
* @ param tdate the date of the last transaction to get
* @ param accountId the id of the account to get transactions for. * gets all
*/
int HoBIT::createJobGetTransactions(QDate fdate, QDate tdate, const QString& accountId){
 _JobManager->setBank(_Database->currentBank());
  return _JobManager->createJobGetTransactions(fdate, tdate, accountId);
}


/** creates a job to get dated transfers, 
*/
int HoBIT::createJobGetDatedTransfers(QDate fromDate, QDate toDate, const QString& accountId){
  /*if(! HoBIT_Bank::supportsJob(getConfigDataDir() + "/" + MONEYPENNY_OPENHBCI_FILE, "HITUBS"))
    return HobitErrorInvalidArgs; 
*/
  return _JobManager->createJobGetDatedTransfers(fromDate, toDate, accountId);

}


/** creates a job get standing orders
 */
int HoBIT::createJobGetStandingOrders(const string& accountId){
/*  if(!HoBIT_Bank::supportsJob(getConfigDataDir() + "/" + MONEYPENNY_OPENHBCI_FILE, "HIDABS"))  
    return HobitErrorInvalidArgs
 */
 return 0;
}


/** Create a transfer (single transfer, dated transfer or standing order) job.  Returns number of transfers in list.
 */
int HoBIT::createJobTransfer (banking::Transfer * transfer)
{
  _JobManager->setBank(_Database->currentBank());
  return _JobManager->createJobTransfer(transfer);
}



/** Deletes a  dated transfer 
*/
int HoBIT::createJobDeleteDatedTransfer(banking::Transfer * transfer){
 /*if(!HoBIT_Bank::supportsJob(getConfigDataDir() + "/" + MONEYPENNY_OPENHBCI_FILE, "HITULS"))  
   return HobitErrorInvalidArgs;
*/
 return 0;
}


  /** Deletes a  dated transfer */
int HoBIT::createJobDeleteStandingOrder(banking::TransferItem * transfer){

/*if(!HoBIT_Bank::supportsJob(getConfigDataDir() + "/" + MONEYPENNY_OPENHBCI_FILE, "HIDALS"))  
  return HobitErrorInvalidArgs;
  */
 return 0;
}

/** creates a new standing Order 
*/

int HoBIT::createJobStartStandingOrder(banking::TransferItem * transfer){
  return 1;
}


/** Removes the data directory using a simple rm
 */ 
void HoBIT::removeUncryptedEnvironment()
{
   QString rm = "rm -r " + _ConfigDataDir;
   system(rm.toLatin1().data());
}


/** Returns the name of the bank for the given id. Returns an empty string, if no bank could be found.
*/
QString HoBIT::getBankNameForId(const QString & code){
  bool bankFound = false;
  string result = "";
  string sCode = qPrintable(code);
  QList<banking::Bank> banks = listOfAllKnownBanks();
  QList<banking::Bank>::iterator bit;
  for (bit = banks.begin(); bit != banks.end() && !bankFound; bit++)
  {
    string bankCode = (*bit).bankCode();

    if (bankCode == sCode)
    {
      result = (*bit).bankName();
      bankFound = true;
    }
  }//end for
  return QString(result.c_str());
 }
 
/** Returns the name (url) of the bank server
*/
QString HoBIT::currentBankServer(){
    if(_Database->currentBank() != NULL)
      return _Database->currentBank()->server().c_str();
    else return "";
}


/**Creates the aqmoney engine
*/ 
void HoBIT::createApi(){
     // setting up version
     qDebug("Creating AqMoney API"); 

    _Api = new AqMoneyAPI(_ReadOnly);
    _Api.ref().setSystemName(_ApplicationName.toLatin1().data());
    _Api.ref().setSystemVersion(VERSION);
    _Api.ref().setApplicationName((_ApplicationName + " " + VERSION).toLatin1().data());

  // creating and setting interactor and authentificator
    _Interactor = new HoBIT_Interactor(_Parent, _Api.ptr());
    _Auth = new HoBIT_Auth(_Api.ptr(), _Parent);
    _Api.ref().setInteractor(_Interactor.cast<Interactor>());
    _Api.ref().setAuthentificator(_Auth.cast<Auth>());
 
   //initialize HBCI_Cern registry
  	#if DEBUG_C_EVENTREGISTRY
   QString path = MP_DIR() + MONEYPENNY_DATA_DIR;
   C_EventRegistry::readMessageDefinesFile(qPrintable(path));
  #endif
   // creating and setting members
   _Database = new DatabaseAccess(_Api);
   _Database->setDataDir(qPrintable(_ConfigDataDir));
   connect(_Database, SIGNAL(statusMessage (const QString &)),
           this, SIGNAL(statusMessage(const QString&)));
   connect(_Database, SIGNAL(processingJob(int)),
           this, SIGNAL(processingJob(int)));
   // now setting directories and file names
   //top directory is crypt::_ConfigDataDir 
   _Api.ref().setAapplicationDataDir((_ConfigDataDir + "/" + _HbciDataDir).toLatin1().data()); //take care of Aa !!
  
   // creating JobFactories
   _AccountJobFactory = new AccountJobFactory(_Api.ptr());
   _AdminJobFactory = new AdminJobFactory(_Api.ptr());

   // create validator 
   _Validator = new HoBIT_Validator(_Api.ptr());

  // create Job lists
   _JobManager = new JobManager();

   //relay the signals from jobmanager
   connect(_JobManager, SIGNAL(jobStatusChanged(banking::BankingJob *)), 
            this, SIGNAL(jobStatusChanged(banking::BankingJob *)));
   connect(_JobManager, SIGNAL(sentAdminJob(int, banking::BankingJob*)), 
           this, SIGNAL(sentAdminJob(int, banking::BankingJob*)));
   connect(_JobManager, SIGNAL(sentTransferJob(int, banking::BankingJob*)), 
           this, SIGNAL(sentTransferJob(int, banking::BankingJob*)));
   connect(_JobManager, SIGNAL(insertCardRequest()), 
           this, SIGNAL(insertCardRequest()));
   connect(_JobManager, SIGNAL(insertCardRequest()), 
           this, SIGNAL(insertCardRequest()));
   connect(_JobManager, SIGNAL(inputPinViaKeypadRequest()), 
           this, SIGNAL(inputPinViaKeypadRequest()));
   connect(_JobManager, SIGNAL(inputPinViaKeyboardRequest(QString *)), 
           this, SIGNAL(inputPinViaKeyboardRequest(QString *)));
   connect(_JobManager, SIGNAL(pinWasOk()), 
           this, SIGNAL(pinWasOk()));
   connect(_JobManager, SIGNAL(pinWasNotOk(int)), 
           this, SIGNAL(pinWasNotOk(int)));
   connect(_JobManager, SIGNAL(pinRequest(QString *)), 
           this, SIGNAL(pinRequest(QString *)));
   connect(_JobManager, SIGNAL(tanRequest(QString, QString *)), 
           this, SIGNAL(tanRequest(QString, QString *)));
   connect(_JobManager, SIGNAL(certificatePathRequest(QString *)), 
           this, SIGNAL(certificatePathRequest(QString *)));
   connect(_JobManager, SIGNAL(verifyBankKeyHash(string *)), 
           this, SIGNAL(verifyBankKeyHash(string *)));
   connect(_JobManager, SIGNAL(errorMessagesReceived(const QString &)), 
           this, SIGNAL(errorMessagesReceived(const QString &)));
   connect(_JobManager, SIGNAL(messagesReceived(const QString &)), 
           this, SIGNAL(messagesReceived(const QString &)));
   connect(_JobManager, SIGNAL(kernelThreadStarted()), 
           this, SIGNAL(kernelThreadStarted()));
   connect(_JobManager, SIGNAL(kernelThreadFinished(bool)), 
           this, SIGNAL(kernelThreadFinished(bool)));
   connect(_JobManager, SIGNAL(adminJobsToAnalyze()), 
           this, SLOT(slotAnalyzeAdminJobs()));
   connect(_JobManager, SIGNAL(sendingMessage(QString)), 
           this, SIGNAL(sendingMessage(QString)));
   connect(_JobManager, SIGNAL(responseOk(QString)), 
           this, SIGNAL(responseOk(QString)));
   connect(_JobManager, SIGNAL(responseError(QString)), 
           this, SIGNAL(responseError(QString)));
   connect(_JobManager, SIGNAL(cardDataRead(int, QString, QString, QString, QString)), 
           this, SIGNAL(cardDataRead(int, QString, QString, QString, QString)));
   connect(_JobManager, SIGNAL(caughtException (exception)), 
           this, SIGNAL(caughtException (exception)));
   connect(_JobManager, SIGNAL(overridableErrorDetected(bool *, const QString &)), 
           this, SIGNAL(overridableErrorDetected(bool *, const QString &)));
}


/** Returns the current bank */
const banking::Bank * HoBIT::bank()const{
    return _Database->currentBank();	
}

/** Returns the current user */
banking::User * HoBIT::user(){
    return _Database->currentUser();	
}

/**Returns the user's name or an empty string.
 */
QString HoBIT::userName(){
  banking::User * user = _Database->currentUser();
  if(user)
     return _Database->currentBankCode().c_str();
  else return "Error: No user available";
}

/** Returns the type of the bank (Volksbank, Sparkasse...)
 */
BankType HoBIT::bankType()const{
  BankType result = BANKTYPE_UNKNOWN;
  if(bank())
    result = bank()->bankType();
  return result;
}

/** Returns the bankCode (BLZ)
*/
QString HoBIT::bankName()const{
 banking::Bank * bank = _Database->currentBank();	
 if(bank) 
   return  bank->bankName().c_str();
 else return "";
}

/**Returns last error messages from the bank.
*/
const QString & HoBIT::bankMessages(){
  return _JobManager->bankMessages();
}

/** Import an RDH2 formatted key. Format is specified in "FinTS-Format der RDH2 Diskette" by PPI, Hamburg. see www.hbci-kernel.org
*/
bool HoBIT::importRDH2Key(const QString & fileName, const QString & password){
  //Under construction!!!
  qDebug("\nFunction HoBIT::importRDH2Key");
 // KeyFileRDH2 * keyFileRDH2 = new KeyFileRDH2(fileName, password);
  
  KeyFileOpenHbci * keyFileOpenHbci = new KeyFileOpenHbci(fileName, password);
  if(!keyFileOpenHbci->exists())
    return false;
  keyFileOpenHbci->readFile();
  qDebug("\nFunction HoBIT::importRDH2Key: keyFile read");
  
  delete keyFileOpenHbci;
  qDebug("HoBIT::importRDH2Key");
  return true;  
    
}

/** Returns true for acceptable and intermediate accountId strings
*/
bool HoBIT::validateAccountIdString(const QString & accountId){
  // regular expression allows only digits from 0 to 9 with a length of 0 to 15
 QRegExp rx( "[0-9]{0,15}" );
 QRegExpValidator v( rx, 0 );
 int pos = 0;
 QString localAccountId = accountId;
 return (v.validate(localAccountId, pos)==QValidator::Acceptable)  || (v.validate(localAccountId, pos)==QValidator::Intermediate);
}

/** Read property of AdminJobFactory m_AdminJobFactory. 
*/
AdminJobFactory * HoBIT::getAdminJobFactory()const{
	return _AdminJobFactory;
}
/** Read property of AccountJobFactory m_AccountJobFactory. 
*/
AccountJobFactory * HoBIT::getAccountJobFactory()const{
	return _AccountJobFactory;
}


/** returns the current HBCI mode */
/*HBCI_MODE HoBIT::getHbciMode(){
     list<Pointer<Customer> > custs=_Api.ref().getCustomers();
     list<Pointer<Customer> >::iterator cit;
     Pointer<Medium> medium;
    
      if (custs.size() == 0) {
            qDebug() << "No customers found. " << custs.size()<<"\n";
	    return HBCI_MODE_ERROR;
        }
      else {
          if(custs.size() > 1)
            qDebug() << custs.size() << " customers found. Using first customer for security medium check." << endl;
          cit=custs.begin();
          medium=(*cit).ref().user().ref().medium();
	}

	if (medium.ref().securityMode() == HBCI_SECURITY_RDH) 
         return HBCI_MODE_RDH;
	else if (medium.ref().securityMode() == HBCI_SECURITY_DDV) 
           return HBCI_MODE_DDV;
	else return HBCI_MODE_ERROR;
}
*/

/** Returns true, if no jobs are in job queues
*/
bool HoBIT::areJobQueuesEmpty(){
  return _JobManager->pendingJobs() == 0;
}

/** Returns the current bank */
/*Pointer<Bank> HoBIT::getBank(){

    list<Pointer<Bank> > banks;
    Pointer<Bank> bank;
           
    banks =  _Api.ref().banks();
    
    if (banks.size()< 1){
            cerr << "No matching bank found\n";
            return bank;
    }
                                       
    if (banks.size()>1){
            cerr << "Problem in data - to many possible banks\n";
            return bank;
    }
    
    bank = *banks.begin();
    return bank;	
}
*/

/** Returns the user from OpenHbci database. Return value should be checked with isValid().
*/
/*Pointer<User> HoBIT::getUser(){
  Pointer<Bank> bank = getBank();;
  Pointer<User> user; //constructor creates an invalid Pointer
  list<Pointer<User> > users;
   
  if (bank.isValid()){  
    users = bank.ref().users();    
    if(users.size() > 0){
      user = *users.begin();
    }
  }
  return user;
}
*/

/** Returns the user's name
*/
/*QString HoBIT::getUserName(){
    Pointer<Account> account;

    if (currentAccountId() != "") {
      account = findAccount( currentAccountId().toLatin1().data() );
      if (account.isValid())
        return QString( account.ref().userName().c_str());
      else return tr("No username found");
    }
    else return tr("No account and no username found");
}
*/
/** Returns the currently used medium from OpenHbci database. Return value should be checked with isValid().
*/
/*Pointer<Medium> HoBIT::getMedium(){
   Pointer<Medium> medium;
   Pointer<User> user = getUser();
   if(user.isValid())
     medium = user.ref().medium();
   return medium; 
}
*/
/** Returns the currently used complete medium path (including the filename) from OpenHbci database. 
*/
/*QString HoBIT::getMediumPath(){
   Pointer<Medium> medium = getMedium();
   if(medium.isValid())
    return QString(medium.ref().mediumName().c_str());
   else return "Invalid medium"; 
}
*/


/** Returns the first customer from the user's customer list. Return value should be checked with isValid().
*/
/*Pointer<Customer> HoBIT::getCustomer(){
   Pointer<User> user = getUser();
   list<Pointer<Customer> > customers;
   Pointer<Customer> customer;
   if(user.isValid()){
     customers = user.ref().customers();     
     if (customers.size() > 0)        
       customer = *customers.begin();
   } 
   return customer; 
 }
 */
/** Returns true, if cardreader is up.
*/
bool HoBIT::isCardReaderOk(){
 bool result = false;

 return result;
}



/** Reads the data from the chipcard. There MAY be userid, bankid, server. 
*/
int HoBIT::readDDVCardData(int & country, QString & bankId, QString & userId, QString & serverUrl, QString & readerName){
 return _JobManager->readDDVCardData(country, bankId, userId, serverUrl, readerName);
}

/** Writes the data to record recordNumber on the chipcard. 
 * Will ask for PIN via signal inputPinViaKeyboardRequest or inputPinViaKeypadRequest.
 * There MUST be userid, bankid, server.
 * Returns true on success.
 */
bool HoBIT::writeDDVCardData(int recordNumber, const QString & bankName, const QString & bankId, 
const QString & userId, const QString & serverUrl)
{
 return _JobManager->writeDDVCardData(recordNumber, bankName, bankId, userId, serverUrl);
}

/** Returns true for a valid account
*/
bool HoBIT::isCurrentAccountValid()
{
  const banking::AccountList * accountList= accounts();
 //check whether selected account is valid
  if(currentAccountId().isEmpty())
    return false; //no account selected
  bool accountIsValid = !accountList->empty(); //no accounts
  if(accountIsValid)
    accountIsValid = currentAccountId().toDouble() != 0.0; //accountId is not 00000000
  return accountIsValid;
}

/** Read property of QString _CurrentAccount. 
*/
QString HoBIT::currentAccountId()const{
	return _Database->currentAccountId().c_str();
}
/** Read property of QString _CurrentAccount. 
*/
QString HoBIT::currentIban()const{
	return _Database->currentIban().c_str();
}

/** Read property of QString _CurrentAccountName. 
*/
QString HoBIT::currentAccountName()const{
  return _Database->currentAccountName().c_str();
}

/** Write property of QString _CurrentAccount. 
*/
void HoBIT::setCurrentAccountId( const QString &bankCode, const QString& _newVal){
   _JobManager->clearLists();
   _Database->setCurrentBankCode(qPrintable(bankCode));
   if(_Database->currentBank()->bankBic().empty()) //automatic update of BIC
      _Database->currentBank()->setBankBic(qPrintable(getBicForBankCode(qPrintable(bankCode))));
   _Database->setCurrentAccountId(qPrintable(_newVal));
}
/** Write property of QString CurrentAccountName. 
*/
void HoBIT::setCurrentAccountName(const QString& _newVal){
  currentAccount()->setAccountName(banking::to8Bit(_newVal));
}

/** Set the security profile version for new bank connections. 
* DO NOT call this function for existing connections. 
*/
void HoBIT::setSecProfileVersion(const QString & newVal){
  _JobManager->setSecProfileVersion(newVal);
}

/** Sets the current path to the passport file. This is NOT persistent!
*/
void HoBIT::setPassportFileDir(const QString & dir){
  _Database->setPassportFileDir(qPrintable(dir));
}


/** Returns temporary config data directory.
*/
QString  HoBIT::getConfigDataDir(){
  return _ConfigDataDir;
}

/**Sets the name of the current bank, as edited by the user
*/
void  HoBIT::setUserDefinedBankName(const QString & newName){
  _UserDefinedBankName = newName;
  _Database->currentBank()->setBankName(qPrintable(newName));
}
/**Returns the name of the current bank, as edited by the user
*/
const QString &  HoBIT::getUserDefinedBankName(){
 return _UserDefinedBankName;
}

/**Returns jobmanager
*/
JobManager * HoBIT::jobManager(){
  return _JobManager;
}

/**Returns database
*/
DatabaseAccess * HoBIT::database(){
 return _Database;
};
 
/**Returns current account. May return NULL.
*/
banking::Account * HoBIT::currentAccount(){
  return _Database->currentAccount();
}

/**Returns current bank. May return NULL.
*/
banking::Bank * HoBIT::currentBank(){
  return _Database->currentBank();
}
/**Returns current bankcode. May return an empty string.
*/
QString HoBIT::currentBankCode(){
  return QString(_Database->currentBankCode().c_str());
}
/**Returns current BIC. May return an empty string.
*/
QString HoBIT::currentBic(){
  return QString(_Database->currentBic().c_str());
}

/**Saves the present key data. This is required after getting the bank's public keys.
 */
bool HoBIT::saveKeyfile(const QString & filename){
 return _JobManager->saveKeyfile(filename);
}

/**Returns a pointer to the pending transfer at index. Returns NULL if index is out of range.
*/
 const banking::Transfer * HoBIT::pendingTransfer(int index)
{
  return _JobManager->pendingTransfer(index);
}

/**Returns number of pending transfers. 
*/
int HoBIT::pendingTransfers()
{
  return _JobManager->pendingTransfers();
}

/**Returns the number of new jobs in the lists
 */
int HoBIT::pendingJobs()
{
  return _JobManager->pendingJobs();
}

/**Returns number of pending debitnotes. 
*/
int HoBIT::pendingDebitNotes()
{
  return _JobManager->pendingDebitNotes();
}
