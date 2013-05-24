/***************************************************************************
                          databaseaccess.cpp  -  description
                             -------------------
    begin                : Fr Nov 5 2004
    copyright            : (C) 2004 by Marcel Mulch
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

#include "databaseaccess.h"
#include "globals.h"
#include "hobit_string.h"
#include "openhbcisupport.h"
#include "bankingjobitem.h"

#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

#include "kernel/helpers/bank.h"


#include <QString>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextStream>

#define Tag_TransferTemplates "TransferTemplates"
#define Tag_TransactionList "Transactions"
#define Tag_DatedTransfers "DatedTransfers"
#define Tag_StandingOrders "StandingOrders"
#define Tag_BankList "Banks"


// hobit includes
#include "hobit_interactor.h"
#include "hobit_auth.h"
#include "hobit_validator.h"
#include "hobit_connector.h"

//a reference for empty strings
string Empty_String = "";

DatabaseAccess::DatabaseAccess(Pointer<AqMoneyAPI> api){
 // _api = api;
 // _OpenHbciSupport.setApi(api);
  _EmitStatusMessages = true;
}

DatabaseAccess::~DatabaseAccess(){
 clearLists();
 qDebug("Destructor DatabaseAccess");
}



/**  This method creates a new User configuration, if not already available
  *  Sets _CurrentBank to bankCode.
  */
  bool DatabaseAccess::createUser( int passportType,
                   const QString & passportFile,
                   const QString & country,
                   const QString &bankName,
                   const QString & bic,
                   const QString & bankCode,
                   BankType bankType,
                   int hbciVersion,
                   const QString &rdhVersion,
                   const QString & userId,
                   const QString & customerId,
                   const QString & serverName){

  banking::Bank * bank = bankByCode(qPrintable(bankCode));
  bool newBank = (bank == NULL);
  if (newBank) //create a new bank
    bank = new banking::Bank(qPrintable(bic),
                             qPrintable(bankCode),
                             bankType,
                             qPrintable(serverName), //or IP
                             hbciVersion,
                             "",  //no separate IP
                             qPrintable(bankName));
  else { //modify existing bank
    bank->setHbciVersion(hbciVersion);
    bank->setServerName(serverName.toLatin1().data());
  }
  bank->setRdhVersion(qPrintable(rdhVersion));

  banking::User * user = bank->userById(qPrintable(userId));
  if(!user){ //create a new user
    bank->addUser( new banking::User(qPrintable(userId),
                                   qPrintable(customerId),
                                   passportType,
                                   qPrintable(passportFile),
                                   qPrintable(bankCode)));
  }
  else{//set new values to existing user
    user->setCustomerId(qPrintable(customerId));
    user->setPassportType(passportType);
    user->setPassportFileName(qPrintable(passportFile));
    user->setBankCode(qPrintable(bankCode));
  }

  if(newBank)
    _BankList.push_back(bank);
  setCurrentBankCode(qPrintable(bankCode)); //make bank current
  return true;
}

/**Copy new system id from BPD to bank
 */
void DatabaseAccess::updateSystemId(){
   banking::Bank * bank = bankByCode(_CurrentBankCode);
   if(bank)
    bank->updateSystemId();
}

/** Reads the data from the chipcard. There MAY be userId, bankid, server. Returns the number of valid user data sets.
*   If no valid set is found, strings in set 1 (the first record) are used. Else the data of the last valid set are used.
*/
int DatabaseAccess::readCardData(int & country, QString & bankId, QString & userId, QString & server, QString & cardnumber){

 return 0;
}



/** Returns a std::list which includes all registered users for the current bank
*/
const banking::UserList * DatabaseAccess::users(){
   if(currentBank() == NULL)
      return NULL;
   return currentBank()->userList();
}

/** removes all existing users from the gwen database */
void DatabaseAccess::deleteAllBanks(){
  qDebug ("DatabaseAccess::deleteAllBanks() not implemented!");
  throw_ExNotImplemented();
}


/** Method to change used hbci version
*   checks if an overhanded hbci version is supported
*   by the bank and returns false if not

This does not work!! messageengine has to be renewed! WR


*/
bool DatabaseAccess::setHbciVersion(int version)
{
   banking::Bank * bank = currentBank();
   if(bank)
     return bank->setHbciVersion(version); 
   else return false;
 }

/** Writes transactions to XML file. returns true on success 
*/
bool DatabaseAccess::saveTransactionList(){
   if(_DataDir.empty()){
    cerr << "DatabaseAccess::saveTransactionList: Error: No datadir set."<<endl;
    return false;
   }
   bool result = true;
    banking::Account * curracc = currentAccount();
    if( curracc && curracc->transactionList() && (!curracc->transactionList()->empty() ) ){
      result = saveBankingJobList (
             Tag_TransactionList, 
             transactionsFileName(_BankDir),
             (banking::BankingJobList *)curracc->transactionList());
    }
  system("sync"); // make shure that everything is on disk
  return result;
}

/** Reads Gwen data and stores data in list _TransactionList.
 *  Returns number of transactions read.
 */
int DatabaseAccess::addLegacyTransactionList(string targetDir){
  int nTransactions = 0;
  banking::Bank * bank = bankByCode(_CurrentBankCode);
  if( !bank )
     return 0; //bank does not exist
  //try Gwen
  currentAccount()->setTransactionList(_OpenHbciSupport.gwenTransactionList(currentAccountId()));

  //count transactions in list
  banking::TransactionList::const_iterator it;
  for(it = currentAccount()->transactionList()->begin();
      it != currentAccount()->transactionList()->end(); 
      it ++)
    nTransactions ++; 
  if(nTransactions == 0)
    return 0;

 //save the list in targetDir
 string oldDataDir = _DataDir;
  setDataDir(targetDir);
  if( bank->currentAccount())
     saveTransactionList();
  setDataDir( oldDataDir );
  return nTransactions;
}

/** Reads BBBBBBB_AAAAAAAAAAA_transactions.xml and stores data in list _TransactionList.
 *  BBB is BankCode. AAA is AccountId.
 *  Returns number of transactions read.
 */
int DatabaseAccess::readTransactionList(){
   int nTransactions = 0;
   if(_DataDir.empty()){
    cerr << "DatabaseAccess::readTransactionList: Error: No datadir set."<<endl;
    return 0;
   }
  if(!currentAccount())
   return 0;
  if(!currentAccount()->transactionList()) //have we got old transactions?
   currentAccount()->setTransactionList(new banking::TransactionList()); //no, create a fresh list
  clearTransactionList();
  Date date1990 = Date(1,1,1990);
   banking::Transaction transaction;
  nTransactions = readBankingJobList(Tag_TransactionList, 
                         transaction.className(), 
                         transactionsFileName(_BankDir), 
                         (banking::BankingJobList *)currentAccount()->transactionList());
  currentAccount()->removeDuplicateTransactions();
  return nTransactions;
}

 /** Constructs a new list containing all transactions of an account which were made during
 *  a specific time periode. List must be deleted by caller.
 */
banking::TransactionList * DatabaseAccess::listTransactions(
                                                  const DateTime &fromDate,
                                                  const DateTime &toDate){
  banking::TransactionList * transactions = new banking::TransactionList();
  if(currentAccount()){
    banking::TransactionList * transactionList = currentAccount()->transactionList();
    banking::TransactionList::iterator it = transactionList->begin();
    while (it != transactionList->end()){
     banking::Transaction * transaction = (*it);
     DateTime transactionDate = transaction->bookingDate();
     if(transactionDate >= fromDate && transactionDate <= toDate )
       transactions->push_back(new banking::Transaction(*transaction));
     it++;
    }
  }
  return transactions;
}
 /** Constructs a new list containing all dated transfers of an account. 
 *List must be deleted by caller.
 */
banking::TransferList * DatabaseAccess::listDatedTransfers()
{
  banking::TransferList * newTransferList = new banking::TransferList();
  if(currentAccount() && currentAccount()->datedTransferList() != NULL){
    banking::TransferList * transferList = currentAccount()->datedTransferList();
    banking::TransferList::iterator it = transferList->begin();
    while (it != transferList->end()){
     banking::Transfer * transfer = (*it);
     newTransferList->push_back(new banking::Transfer(*transfer));
     it++;
    }
  }
  return newTransferList;
}

/**Returns an account list for all banks and all users.
*/ 
const banking::AccountList * DatabaseAccess::accounts(){
/* if(!currentBank())
  return NULL;
 if(!currentBank()->currentUser())
  return NULL;
 return &(currentBank()->currentUser()->accounts());
*/
 buildAllAccountsList();
 return & _AllAccounts;
}

/**Builds the list of all accounts for all banks and all users.
*/
void DatabaseAccess::buildAllAccountsList(){
 _AllAccounts.clear();
 banking::BankList::const_iterator itBank;
 for(itBank = _BankList.begin(); itBank != _BankList.end(); itBank++){
   banking::Bank * bank = (*itBank);
   banking::UserList::const_iterator itUser;
   for(itUser = bank->userList()->begin(); itUser != bank->userList()->end(); itUser++){
     banking::User * user = (*itUser);
     banking::AccountList::const_iterator itAccount;
     for(itAccount = user->accounts().begin(); itAccount != user->accounts().end(); itAccount++){
        banking::Account * account = (*itAccount);
        _AllAccounts.push_back(account);
     }//end account loop
   }//end user loop
 }//end bank loop
}


/** Returns the date of the last stored transaction of current account
  * !!!!WARNING: This date may be in the future!!!!
  */
DateTime DatabaseAccess::getLastDateOfTransactions()
{
  banking::TransactionList::iterator it;
  DateTime d; 
  if(!currentAccount())
    return d; //no account, return default date
  banking::TransactionList * transactionList = currentAccount()->transactionList();
  if(transactionList->empty()) //empty list, return default date
    return d;

  //now go ahead
  it = transactionList->end();
  it --; //point to the last transaction
  d = (*it)->bookingDate(); //take the date of the last transaction in the list
  if(it ==  transactionList->begin()) //only one transaction, return it's date
    return d;
  int transactionsToTest = 10; //only test the last 10 transactions for a later date
  for(it = it; (it != transactionList->begin()) && (transactionsToTest > 0); it--){
    if((it != transactionList->begin()) && ((*it)->bookingDate() > d))
      d = (*it)->bookingDate();
    else transactionsToTest--;
  }
  return d;
}

/**Exports current list of transactions to file as CSV formatted data.
*/
bool DatabaseAccess::exportCSV(QFile * file, char fieldSeparator, char textSeparator,
               const DateTime &fromDate,
               const DateTime &toDate){
     QString toExport;
     banking::TransactionList * transactionList = listTransactions(fromDate, toDate);
     banking::TransactionList::const_iterator it;
     for (it = transactionList->begin(); it != transactionList->end(); it++){
       banking::Transaction * transaction = (*it);
       toExport += QString(transaction->toCSV(fieldSeparator, textSeparator).c_str()) + '\n';
     }
     delete transactionList; 
    QTextStream s(file);
//    s.setEncoding(_Encoding);
    s << toExport;
    return toExport.length() > 0;
}

/**Exports all data to file as SQL formatted data.
*/
bool DatabaseAccess::exportSQL(QFile * file){
QString toExport = "DatabaseAccess::exportSQL: Not yet implemented";
    QTextStream s(file);
//    s.setEncoding(_Encoding);
    s << toExport;

/*
 list<Pointer<Account> > accounts = _Hobit->listAccounts();
// get a list of all accounts managed by moneypenny
 list<Pointer<Account> >::iterator account;


// create list of all transactions
list<Pointer<Transaction> > transactions;

// create list of all transactions
list<Pointer<DatedTransfer> > datedtransfers;

// create list of all transactions
list<Pointer<StandingOrder> > standingorders;

   // now get transaction lists for all accounts
   for (account = accounts.begin(); account != accounts.end(); account++) {

	// get Account ID and SubID
    	QString accountId = account->ref().accountId().c_str();


	// create list of all transactions
	list<Pointer<Transaction> > currenttransactions = _Hobit->listTransactions(  0,
                            			       				    "*",
                                        					    accountId.toAscii().data(),
                                        					    "*",
                                        					    QDate(1980, 1,1),
                                        					    QDate::currentDate().addYears(1));

	// create list of all transactions
	list<Pointer<DatedTransfer> > currentdatedtransfers = _Hobit->listDatedTransfers(0, 
											"*",
											 accountId.toAscii().data(),
											 "*");

	// create list of all transactions
	list<Pointer<StandingOrder> > currentstandingorders =  _Hobit->listStandingOrders(  0, 
											   "*",
											   accountId.toAscii().data(),
											    "*"); 


	// merge transactions of all accounts to one large list
	list<Pointer<Transaction> >::iterator transit;
        for (transit = currenttransactions.begin(); transit != currenttransactions.end(); transit++){
	transactions.push_back(*transit);
	}
	
	
	// merge dated transfers of all accounts to one large list
	list<Pointer<DatedTransfer> >::iterator datit;
	for (datit = currentdatedtransfers.begin(); datit != currentdatedtransfers.end(); datit++){
	datedtransfers.push_back(*datit);
	}

	
	// merge standing orders of all accounts to one large list
	list<Pointer<StandingOrder> >::iterator standit;
	for (standit = currentstandingorders.begin(); standit != currentstandingorders.end(); standit++){
	standingorders.push_back(*standit);
	}	
        
	_sql->setAll(transactions, datedtransfers, standingorders);
	_sql->writeSqlFile(filename); 
	 }
*/  
    return toExport.length() > 0;
}

/**
* Method that reads various bank data files and returns 
* a list of banking::Bank-Objects.
* If file extension is .csv, the ZKA -banklist fints_institute.csv (ISO-8859) is read.
* If file-extension is .txt, the Bundesbank BLZ-list file (ISO-8859)is read.
* If file-extension is .hbd, the MP-specific bankdata.hbd file containing combined data from
* fints_institute.csv and Bundesbank list is read.
*/
QList<banking::Bank> DatabaseAccess::readBankFile(const QString &filename) 
{
   // First create list
    banking::Bank * bank;
    QList<banking::Bank> banks;
    QFile file(filename);
    QFileInfo fileInfo(file);
   // ifstream stream (qPrintable(filename));
    qDebug("Reading bankfile: %s", qPrintable(filename));
    if ( file.open( QIODevice::ReadOnly ) ) {
        if(fileInfo.completeSuffix() =="txt" || fileInfo.completeSuffix() =="csv"){ //prepare a Textstream and read from it
           QByteArray text = file.readAll();
           QTextStream stream (&text);
           QString codecName = "ISO-8859-15";
           QTextCodec *codec = QTextCodec::codecForName(qPrintable(codecName));
           if(!codec)
            codec = QTextCodec::codecForLocale();
           if(codec){
             stream.setAutoDetectUnicode(false);
             stream.setCodec(codec);
             qDebug("Using text codec for: %s", codec->name().data());
           }
           else qDebug("Could not find a codec for:%s ", codecName.toLatin1().data());
           
           int nLine = 0;
           QString line;
           line = stream.readLine(); // read first line 
           qDebug("bankdata: %s", qPrintable(line));
           nLine ++;

           //Read the Bundesbank list
           if(fileInfo.completeSuffix() =="txt"){
            if(line.indexOf("10000000") == 0){ //check first line 
              stream.seek(0); //read first line again
              while ( !stream.atEnd() ) {
                  line = stream.readLine(); // read next line 
                  bank = new banking::Bank();
                  if(bank->attributesFromTxtLine(banking::to8Bit(line))) //this converts to 8 bit chars using _BankingToEncoding
                    banks.push_back(*bank);
                  else delete bank; //invalid line
                  nLine++; 
              }
             }
             else qDebug("File format error in bank data file: %s", filename.toLatin1().data());
            }
            //Read the original FinTS list, which MUST NOT be redistributed
            else if(fileInfo.completeSuffix() =="csv"){
                if(line.indexOf("Nr.;") == 0) //check start of first line 
                  while ( !stream.atEnd() ) {
                    line = stream.readLine(); // read line excluding '\n'
                    bank = new banking::Bank();
                    if(bank->attributesFromCsvLine(banking::to8Bit(line)))
                      banks.push_back(*bank);
                    else delete bank; //invalid line
                    nLine++; 
                  }
                else qDebug("File format error in bank data file: %s", filename.toLatin1().data());
             }
        }//end if text format

        else if (fileInfo.completeSuffix() =="hbd") { //MP bank data file
         ifstream ifStream(qPrintable(filename)); //we need an STL stream here. No codec required.
         while ( !ifStream.eof() ) {
              bank = new banking::Bank();
              if(bank->attributesFromDataStream(ifStream))
                banks.push_back(*bank);
              else {
                qDebug("File format error in bank data file: %s", filename.toLatin1().data());
                delete bank; //invalid line
              }
          }
        }
        file.close();
    }//end if fileOpen
    else qDebug( "Bank data file not found: %s", filename.toLatin1().data()); 
   return banks;
}

/**
* Writes the hbd file format. Required due to license of FinTS_institute file.
*
*/
bool DatabaseAccess::writeHbdBankFile(const QString &filename, QList<banking::Bank> & bankList) 
{
    QFile file (filename);
    QFileInfo fileInfo(file);
    if(fileInfo.completeSuffix() !="hbd"){
     qDebug("DatabaseAccess::writeHbdBankFile: File-extension must be .hbd! Did not write: %s", filename.toLatin1().data());
     return false;
    } 
    if ( ! file.open( QIODevice::WriteOnly ) ) {
     qDebug("DatabaseAccess::writeHbdBankFile: No write access. Did not write: %s", filename.toLatin1().data());
     return false;
    }
    if (bankList.empty()){
     qDebug("DatabaseAccess::writeHbdBankFile: Empty bank list! Did not write: %s", filename.toLatin1().data());
     return false;
    }

    ofstream stream(qPrintable(filename));
    QList<banking::Bank>::iterator bit;
    bool ok = true;
    for (bit = bankList.begin(); (bit != bankList.end()) && ok; bit++)
    {
      banking::Bank & bank = *bit;
      ok = bank.attributesToDataStream(stream);
    }// end for
    if (!ok)
    {
     qDebug("DatabaseAccess::writeHbdBankFile: Error while writing bank data. File is not complete: %s", filename.toLatin1().data());
     return false;
    }
    return true;
}

/** returns the hash fingerprint of the institute after resceiving the bank servers public keys */
/*QString DatabaseAccess::getBankFingerPrint(){

Error err;
  list<Pointer<Customer> > custs;
  Pointer<Customer> cust;
  Pointer<Medium> medium;
  Pointer<MediumRDHBase> mediumrdh;
  Pointer<User> user;
  Pointer<Bank> bank;
  string modulus;
  string exponent;
  string hash;
  int keyversion;
  int keynumber;
  QString result;
  bool isuser;
  string userId;
  string tmp;
  bool usecrypt;
  QString hint = tr("\nPlease check your User-Id");

  custs=_api.ref().getCustomers();
      
  if (custs.empty()) {
    qDebug( "Error: No matching customers found");
    return QString(tr("Error: No matching customers found.\n") + hint).toLatin1().data();
  }
  
  if (custs.size()>1) {
    qDebug ("Error: Ambiguous customer specification. ");
    return tr("Error: Ambiguous customer specification.\n").toLatin1().data();
  }
  
  cust=custs.front();
  user=cust.ref().user();
  medium=user.ref().medium();
  bank=user.ref().bank();
  if (medium.ref().securityMode()!=HBCI_SECURITY_RDH) {
    qDebug( "Error: Not in RDH mode.\n");
    return tr("Error: Not in RDH mode.\n").toLatin1().data();
  }
  
  mediumrdh=medium.cast<MediumRDHBase>();

  // mount medium
  err=mediumrdh.ref().mountMedium();
  if (!err.isOk()) {
    qDebug( "Error while trying to mount the security medium");
    return  QString(tr("Error mounting medium (%s)\n")+
            err.errorString().c_str() + hint).toLatin1().data();
  }

  // select context
  err=mediumrdh.ref().selectContext(bank.ref().country(),
                                    bank.ref().bankCode(),
                                    user.ref().userId());
  if (!err.isOk()) {
    mediumrdh.ref().unmountMedium();
    qDebug( "Error selecting user context\n %s",err.errorString().c_str());
    return QString(
          tr("Error selecting user context\n") +
          err.errorString().c_str() + hint).toLatin1().data();
  }

  try {
    if (!mediumrdh.ref().userPubSignKey().isValid()) {
      qDebug( "Error: No sign key, please create keys.");
      return tr("Error: No sign key, please create keys.\n").toLatin1().data();
    }
    if (!mediumrdh.ref().userPubCryptKey().isValid()) {
      qDebug( "Error: No crypt key, please create keys.");
      return tr("Error: No crypt key, please create keys.\n").toLatin1().data();
    }

      // change the following lines to generate a users ini letter:
      usecrypt=!mediumrdh.ref().hasInstSignKey();
      if(usecrypt && mediumrdh.ref().cryptKeyVersion()==0) //Invalid Crypt key
       throw(Error(string("Keyfile error"),string("No bank keys available"), HBCI_ERROR_CODE_MEDIUM));
      hash=mediumrdh.ref().getInstIniLetterHash(usecrypt);
      exponent=mediumrdh.ref().getInstIniLetterExponent(usecrypt);
      modulus=mediumrdh.ref().getInstIniLetterModulus(usecrypt);
      keynumber=-1;
      keyversion=-1;
      isuser=false;
      userId=bank.ref().bankCode();
      // end of change block

      mediumrdh=medium.cast<MediumRDHBase>();

      result=HoBIT_String::dumpHexString(hash).c_str();
  }

      catch (Error xerr) {
    DBG_ERROR(0, "Error: %s", xerr.errorString().c_str());
    QString error = "Error: ";
    error += xerr.errorString().c_str();
    return error;
  }

  return result;
} 

*/

/** returns the hash fingerprint of the own public key 
*/
/*QString DatabaseAccess::getMyOwnFingerPrint(){

Error err;
  list<Pointer<Customer> > custs;
  Pointer<Customer> cust;
  Pointer<Medium> medium;
  Pointer<MediumRDHBase> mediumrdh;
  Pointer<User> user;
  Pointer<Bank> bank;
  string modulus;
  string exponent;
  string hash;
  int keyversion;
  int keynumber;
  QString result;
  bool isuser;
  string userId;
  string tmp;

  custs=_api.ref().getCustomers();

  if (custs.empty()) {
    fprintf(stderr,
            "%s",
            tr("Error: No matching customers found.\n").toLatin1().data());
    return "Error: No matching customers found";
  }

  if (custs.size()>1) {
    fprintf(stderr,
            "%s",
            tr("Error: Ambiguous customer specification.\n").toLatin1().data());
    return "Error: Ambiguous customer specification. ";
  }

  cust=custs.front();
  user=cust.ref().user();
  medium=user.ref().medium();
  bank=user.ref().bank();
  if (medium.ref().securityMode()!=HBCI_SECURITY_RDH) {
    fprintf(stderr,
            "%s",
            tr("Error: Not in RDH mode.\n").toLatin1().data());
    return "Error: Not in RDH mode.\n";
  }

  mediumrdh=medium.cast<MediumRDHBase>();

  // mount medium
  err=mediumrdh.ref().mountMedium();
  if (!err.isOk()) {
    fprintf(stderr,
            tr("Error mounting medium (%s)\n").toLatin1().data(),
            err.errorString().c_str());
    return "Error while trying to mount the security medium";
  }

  // select context
  err=mediumrdh.ref().selectContext(bank.ref().country(),
                                    bank.ref().bankCode(),
                                    user.ref().userId());
  if (!err.isOk()) {
    mediumrdh.ref().unmountMedium();
    fprintf(stderr,
            tr("Error selecting user context (%s)\n").toLatin1().data(),
            err.errorString().c_str());
      QString error = "Error selecting user context";
      error += err.errorString().c_str();
      return error;
  }

  try {
    if (!mediumrdh.ref().userPubSignKey().isValid()) {
      fprintf(stderr,
              "%s",
              tr("Error: No sign key, please create keys.\n").toLatin1().data());
      return "Error: No sign key, please create keys.";
    }
    if (!mediumrdh.ref().userPubCryptKey().isValid()) {
      fprintf(stderr,
              "%s",
              tr("Error: No crypt key, please create keys.\n").toLatin1().data());
      return "Error: No crypt key, please create keys.";
    }

      // change the following lines to generate a bank hash:
      hash=mediumrdh.ref().getUserIniLetterHash();
      exponent=mediumrdh.ref().getUserIniLetterExponent();
      modulus=mediumrdh.ref().getUserIniLetterModulus();
      keynumber=mediumrdh.ref().getUserKeyNumber();
      keyversion=mediumrdh.ref().getUserKeyVersion();
      isuser=true;
      userId=user.ref().userId();
      // end of change block

      mediumrdh=medium.cast<MediumRDHBase>();

      result=HoBIT_String::dumpHexString(hash).c_str();
  }

      catch (Error xerr) {
    DBG_ERROR(0, "Error: %s", xerr.errorString().c_str());
    QString error = "Error: ";
    error += xerr.errorString().c_str();
    return error;
  }

  return result;
} 
*/


 /** changes the account description */
bool DatabaseAccess::changeAccountName(const string& accid, const string& name){
  banking::User * user = currentUser();
  if(!user)
   return false;
  banking::Account * account= user->accountById(accid);
  if(!account)
   return false;
  account->setAccountName(name);
  return true;
}

/**Deletes the account and rebuilds AllAccounts list.
*/
bool DatabaseAccess::deleteAccount(const QString & accountId){
  banking::User * user = currentUser();
  if(!user)
   return false;
  user->deleteAccount(qPrintable(accountId));
  buildAllAccountsList();
  return true;
}

/** creates a new account. Strips leading zeros from accountId, if not 00000000 
  * @param accountId: the account number
  * @param accountSubId: Hbci allows subids for accounts 
  * 			 this parameter is normaly not needed
  * @param accountName:  you can give your account an description
  *
  */		
bool DatabaseAccess::addAccount(const string &accountId, const string &accountSubId, const string& accountName){
  string bankCode;
  string bic;
  if(currentBank()){
   bankCode= currentBank()->bankCode();
   bic = currentBank()->bankBic();
  }
  else return false;
  string accId = accountId;
  if(accId != MONEYPENNY_DUMMY_ACCOUNT_ID) //00000000
  {
    while(accId[0]=='0') //strip any leading zeros
     accId.erase(0,1);
  }

  if(currentUser()){
    banking::Account * account = new banking::Account(bankCode, bic, accountId, accountName);
    bool result = currentUser()->addAccount(account);
    if(result)
         buildAllAccountsList();
    return result;
  }
  else return false;
}

/** writes all templates and banks to XML files. returns true on success 
* ( transactions are written in evaluateResponseData )
*/
bool DatabaseAccess::saveLists(){
   if(_DataDir.empty()){
    cerr << "DatabaseAccess::saveLists: Error: No datadir set."<<endl;
    return false;
   }
   else setDataDir(_DataDir); //Create all missing or deleted directories
   //first save transactions of current account
   bool transactionsOk = saveTransactionList();
      //save template list
   bool templatesOk = true;
   if( ! _TemplateList.empty() )
     templatesOk = saveBankingJobList(Tag_TransferTemplates, 
                                _BankDir + MONEYPENNY_TEMPLATE_FILE_XML,
                                (banking::BankingJobList *) & _TemplateList);
    // save bank list
    bool banksOk = saveBankList(_BankDir + MONEYPENNY_BANK_FILE_XML);

   return transactionsOk && templatesOk && banksOk;
}


/** Polymorphic save function. Used for DomObjects and all their child classes.
 *  @param listName is the tag name of the root tag in the file
 *  @param fileName is the name of the target file including the directory
 *  @param listToSave is a pointer to the list that is to be saved. Type casting to (list <BankingJob * > *) is required
 */
bool DatabaseAccess::saveBankingJobList
       (const string & listName, const string & fileName, banking::BankingJobList * listToSave){
   bool result = true;;
   banking::BankingJobList::iterator it;
   cout << "DatabaseAccess::saveBankingJobList: Saving " << listName << endl;
   if(listToSave->empty()){
     cerr << "Error: List " << listName <<" is empty."<< endl;
     return false;
   }
   C_FTSC_Xerces & xerces = C_FTSC_Xerces::getInstance();
   xerces.initialize();
   xerces.setEncoding(C_FTSC::XE_ISO88591);

   DOMDocument * doc = xerces.createNewDocument();
   XMLCh tempStr[100];
   XMLString::transcode(listName.c_str(), tempStr, 99);
   DOMElement*   root = doc->createElement(tempStr);
   doc->appendChild(root); //the first appended child becomes the document root
   root = doc->getDocumentElement();

   for (it= listToSave->begin(); it!= listToSave->end(); it++) {
      banking::BankingJob * job = (*it);
      if(job){
        DOMElement * element = job->toDomElement(doc);
        root->appendChild(element);
      }
   } // for

   cerr << "Saving file: " << fileName << endl;
   xerces.setWriterTarget(C_FTSC::WT_File, fileName);
   result = xerces.serialize(root, NULL);
   if(!result)
    cerr<< "DatabaseAccess::saveBankingJobList: Error: Saving file " << fileName << " failed." << endl;
   return result;	
}

/** Saves the list of all banks.
 *  @param fileName is the name of the target file including the directory
 */
bool DatabaseAccess::saveBankList(const string & fileName){
   bool result = true;;
   banking::BankList::iterator it;
   cerr << "Saving banklist"  << endl;
   C_FTSC_Xerces & xerces = C_FTSC_Xerces::getInstance();
   xerces.initialize();
   xerces.setEncoding(C_FTSC::XE_ISO88591);
   DOMDocument * doc = xerces.createNewDocument();
   XMLCh tempStr[100];
   XMLString::transcode(Tag_BankList, tempStr, 99);
   DOMElement*   root = doc->createElement(tempStr);

   for (it= _BankList.begin(); it!= _BankList.end(); it++) {
      banking::Bank * bank = (*it);
      DOMElement * element = bank->toDomElement(doc);
      root->appendChild(element);
   } // for

   cerr << "Saving file: " << fileName << endl;
   xerces.setWriterTarget(C_FTSC::WT_File, fileName);
   result = xerces.serialize(root, NULL);
   if(!result)
    cerr<< "DatabaseAccess::saveBankList: Error: Saving file " << fileName << " failed." << endl;
   return result;	
}

/** Polymorphic read function. Used for BankingJobs and all their child classes.
 * Reads file fileName and fills listToFill with all XML elements of type className.
 * Returns number of jobs read.
 *  @param listName is the tag name of the root tag in the file
 *  @param className is  the tag name of the class to be read
 *  @param fileName is the name of the target file including the directory
 *  @param listToFill is a pointer to the list that is to be read. Type casting to (BankingJobList *) is required
*/
int DatabaseAccess::readBankingJobList
       (const string & listName, const string & className, const string & fileName, banking::BankingJobList * listToFill){
   int nJobs = 0;
   //dummy objects to access the virtual className function
   banking::BankingJob bankingJob;
   banking::Transfer transfer;
   banking::Transaction transaction;

   if(_EmitStatusMessages){
     QFile xmlFile (fileName.c_str());
     int fileSize = xmlFile.size();
     emit statusMessage(QObject::tr("Reading an XML file of size: ") + QString::number(fileSize));
   }

   cerr << "DatabaseAccess::readBankingJobList: Loading file: " << fileName << endl;
   C_FTSC_Xerces & xerces = C_FTSC_Xerces::getInstance();
   xerces.initialize(); 
   xerces.setEncoding(C_FTSC::XE_ISO88591);
   xerces.setBuilderTarget(C_FTSC::BT_File, fileName.c_str());
   DOMDocument * doc = xerces.parse();
   if (doc == NULL) {
      cerr << "DatabaseAccess::readBankingJobList:Error: Loading failed for file: " << fileName << endl;
      cerr << "Error description see above." << endl;
      return 0;
    }
   DOMElement* root;

   // Check for correct list node
   root= doc->getDocumentElement();
   // should always have a valid root but handle gracefully if it does not
   if (!root){
     cerr << "DatabaseAccess::readBankingJobList:Error: Format error in root node of XML file: " << fileName << endl;
     return 0;
   }
   string listNameInFile = XMLString::transcode(root->getTagName());
   if(listNameInFile != listName){
     cerr << "DatabaseAccess::readBankingJobList:Error: Wrong list name in XML file: " << fileName;
     cerr << " Expected name: "<<listName << endl;
     return 0;
   }

   //evaluate child nodes and fill list
   if(_EmitStatusMessages)
     emit statusMessage(QObject::tr("Evaluating transactions."));
   banking::BankingJob * item = NULL;
   DOMNodeList * jobList = root->getElementsByTagName(XMLString::transcode(className.c_str()));
   int listLength = jobList->getLength();
       item = new banking::Transaction();
   for(int i = 0; i < listLength; i++)
   {
     if(_EmitStatusMessages)
       emit processingJob(i);
     DOMElement * job = (DOMElement *)jobList->item(i);
     //use the right constructor
     if (className == bankingJob.className())
       item = new banking::BankingJob();
     else if (className == transfer.className())
       item = new banking::Transfer();
     else if (className == transaction.className())
       item = new banking::Transaction();
     //now read data
     item->fromDomElement(job);
     listToFill->push_back(item);
     nJobs ++;
   }
  return nJobs;
}

/** 
 * Reads file fileName and fills _BankList with all XML elements of type Bank.
 *  @param fileName is the name of the target file including the directory
 * Returns false on error.
 */
bool DatabaseAccess::readBankList (const string & fileName){
   //dummy object to access the virtual className function
   cerr << "DatabaseAccess::readBankList: Loading bankdata file: " << fileName << endl;
   banking::Bank dummyBank;
   C_FTSC_Xerces & xerces = C_FTSC_Xerces::getInstance();
   xerces.initialize();
   xerces.setEncoding(C_FTSC::XE_ISO88591);
   xerces.setBuilderTarget(C_FTSC::BT_File, fileName.c_str());
   DOMDocument * doc = xerces.parse();
   if (doc == NULL) {
      cerr << "DatabaseAccess::readBankList:Error: Loading failed for file: " << fileName << endl;
      return false;
    }

   // Check for correct list node
   DOMElement * root  = doc->getDocumentElement();
   // should always have a valid root but handle gracefully if it does not
   if (root == NULL){
     cerr << "DatabaseAccess::readBankList:Error: Format error in root node of XML file: " << fileName << endl;
     return false;
   }
   string listNameInFile = XMLString::transcode(root->getTagName());
   if(listNameInFile != Tag_BankList ){
     cerr << "DatabaseAccess::readBankList:Error: Wrong list name in XML file: " << fileName;
     cerr << " Expected name: "<< Tag_BankList << endl;
     return false;
   }
   //evaluate child nodes and fill list
   DOMNodeList * bankNodeList = root->getElementsByTagName(XMLString::transcode(dummyBank.className()));

   banking::Bank * bank = NULL;
   for(uint i = 0; i< bankNodeList->getLength(); i++)
   {
     DOMElement * bankElement = (DOMElement *)bankNodeList->item(i);
     bank = new banking::Bank();
     //now read data
     bank->fromDomElement(bankElement);
     cerr << "DatabaseAccess::readBankList: Found: " << bank->bankName() << endl;
     bank->debug();
     _BankList.push_back(bank);
   }//end for
   if(_BankList.empty())
    {
     cerr << "DatabaseAccess::readBankList:Error: No banks found in file: " << fileName;
     return false;
    }

    //finally make first bank (and its first user) current
    bank = *_BankList.begin();
    setCurrentBankCode(bank->bankCode());
    return true;
}

/** Compare function for sorting transfers according to recipient name (_OtherName)
*/
bool compareTransfersByName(banking::Transfer * first, banking::Transfer * second){
  return first->otherName1() +  first->otherName2() < second->otherName1() + second->otherName2();
}

/** reads stored templates and banklist from files */
bool DatabaseAccess::readLists(){
    if(_DataDir.empty()){
     cerr << "DatabaseAccess::readLists: Error: No datadir set."<<endl;
     return false;
    }
    clearLists();
    bool result = true;
    //read transfer templates
    banking::Transfer transfer;
    string templateFile = _BankDir + MONEYPENNY_TEMPLATE_FILE_XML;
    if( !readBankingJobList
         (Tag_TransferTemplates, transfer.className(), templateFile, (banking::BankingJobList *) &_TemplateList)){ 
       qDebug("DatabaseAccess::readLists: Loading template list failed. File was: %s", templateFile.c_str());
       //keep result true, because templates are optional
    }
    else _TemplateList.sort(compareTransfersByName);
   string bankFile = _BankDir + MONEYPENNY_BANK_FILE_XML;
    if(! readBankList(bankFile)){
       qDebug("DatabaseAccess::readLists: Loading bank list failed. File was: %s", bankFile.c_str());
     result = false;
    }
    buildAllAccountsList();
    return result;
}

/** Reads legacy data from _DataDir 
 */
bool DatabaseAccess::readOpenHbciData(const QString & dataDir, const QString & keyFileDir){
  Pointer<AqMoneyAPI> aqMoneyApi;
  Pointer<HoBIT_Interactor> _Interactor;
  Pointer<HoBIT_Auth> _Auth;
  Pointer<HoBIT_Validator> _Validator;

  aqMoneyApi = new AqMoneyAPI(false);
  aqMoneyApi.ref().setSystemName("Legacy_MoneyPenny");
  aqMoneyApi.ref().setSystemVersion(VERSION);
  aqMoneyApi.ref().setApplicationName("Legacy_MoneyPenny_14");
  aqMoneyApi.ref().setAapplicationDataDir(qPrintable(dataDir)); //take care of Aa

  // creating and setting interactor and authentificator
    _Interactor = new HoBIT_Interactor(NULL, aqMoneyApi.ptr());
    _Auth = new HoBIT_Auth(aqMoneyApi.ptr(), NULL);
    aqMoneyApi.ref().setInteractor(_Interactor.cast<Interactor>());
    aqMoneyApi.ref().setAuthentificator(_Auth.cast<Auth>());

  _OpenHbciSupport.setApi(aqMoneyApi);
  banking::Bank * bank =_OpenHbciSupport.readData(qPrintable(dataDir), qPrintable(keyFileDir));
  if(bank)
    addBank(bank);
  return bank != NULL;
}

/** adding transfer template */
void DatabaseAccess::addTransferTemplate(banking::Transfer * temp){
  _TemplateList.push_back(temp);
  _TemplateList.sort(compareTransfersByName);
}

/** deletes a transfer template. */  
void DatabaseAccess::deleteTransferTemplate( int row) {
  banking::TransferList::iterator tit;
  int i = 0;
  for (tit = _TemplateList.begin(); tit != _TemplateList.end() && i <= row; tit++){ //run through the list
     if(i == row)
       _TemplateList.remove(*tit);
     i++;
  }
}

/** Clears all lists. Deletes objects in lists.
*/
void DatabaseAccess::clearLists(){
  clearBankList();
  clearTemplateList();
}

/** Clears template list. Deletes objects in list.
*/
void DatabaseAccess::clearTemplateList(){
  banking::TransferList::iterator itTemplates;
  for (itTemplates = _TemplateList.begin(); itTemplates != _TemplateList.end(); itTemplates++){
    if(* itTemplates)
     delete (* itTemplates);
  }
  _TemplateList.clear();
}

/** Clears transaction list. Deletes objects in list.
*/
void DatabaseAccess::clearTransactionList(){
 if(currentAccount())
   currentAccount()->clearTransactionList();
}

/** Clears bank list. Deletes objects in list.
*/
void DatabaseAccess::clearBankList(){
  banking::BankList::iterator itBanks;
  for (itBanks = _BankList.begin(); itBanks != _BankList.end(); itBanks++){
    if(* itBanks)
     delete (* itBanks);
  }
  _BankList.clear();
}

/**Updates Balances and adds transactions to _TransactionLists of all accounts
* Updates and saves all transactionlists for current user.
* Only one set of transactiondata is kept in memory at a time.
*/
bool DatabaseAccess::evaluateResponseData(banking::BankingJobList jobList){
    banking::User * user = currentUser();
    if(!user)
      return false;
    string oldAccountId = currentAccountId(); //save setting 
    bool result = true;
   try{
	   //account loop, iterate over all accounts for current user
	   banking::AccountList::const_iterator itAcc;
	   for (itAcc = user->accounts().begin(); itAcc !=user->accounts().end(); itAcc++){
		   banking::Account * account = (*itAcc);
		   setCurrentAccountId(account->accountId()); //reads transactions from file into account
		   if(!user->evaluateResponseData(jobList, account)){ //see what we have got for the account and add it
			   result = false;
			   cout << "DatabaseAccess::evaluateResponseData:Error while evaluating response data for:" << account->accountId() << endl;
		   }
		   else {
			   cout << "DatabaseAccess::evaluateResponseData: Updating data for:" << account->accountId() << endl;
			   saveTransactionList();//save updated transactionlist
		   }
	   }
	   system("sync"); //make shure that the new data are on the disk
	   if(!oldAccountId.empty())
		   setCurrentAccountId(oldAccountId); //reactivate old account
	   buildAllAccountsList();
   }
   catch(exception & e)
   {
	   cerr << "DatabaseAccess::evaluateResponseData: Caught exception: " << e.what()<< endl;
	   throw;
   }
    return result;
}

/** Returns bank with given bankCode. Returns NULL, if bank is not in list.
*/
banking::Bank * DatabaseAccess::bankByCode(const string & bankCode){
 banking::Bank * result = NULL;
 banking::BankList::iterator it;
 if(_BankList.empty())
  return NULL;
 for(it = _BankList.begin(); it != _BankList.end(); it++)
   if((*it)->bankCode() == bankCode)
     result = (*it);
 return result; 
}

/**Adds bank to the list, if it is not already in the list
*/
bool  DatabaseAccess::addBank(banking::Bank * bank){
 if(!bank)
  return false;
 if(! bankByCode(bank->bankCode())){
   _BankList.push_back(bank);
   _CurrentBankCode = bank->bankCode(); //activate bank
   buildAllAccountsList();
   return true;
 }
 else return false;
}

/**Deletes bank, if it is in the list
*/
bool DatabaseAccess::deleteBank(const string & bankCode){
  banking::Bank * bank = bankByCode(bankCode);
  if(bank){
   _BankList.remove(bank);
   delete bank;
   saveBankList(_BankDir + MONEYPENNY_BANK_FILE_XML);
   system("sync");
   buildAllAccountsList();
   return true;
  }
  else return false;
}

/** Builds the transactionsFile name
*/
string DatabaseAccess::transactionsFileName(const string & directory){
 string result = directory +_CurrentBankCode + "_"+ currentAccountId()  
      + "_" + MONEYPENNY_TRANSACTIONS_FILE_XML;
 return result;
}

/** Sets the current account to accountId, if it is in the list of the current bank.
  * Saves transaction list for old account.
  * Reads list of transactions for new account.
  * Call setCurrentAccountId("") to unset an account.
  * Returns true on success.
  */
bool DatabaseAccess::setCurrentAccountId(const string & accountId){
  banking::Bank * bank = bankByCode(_CurrentBankCode);
  if( !bank )
     return false; //bank does not exist

  if( bank->currentAccount())
     saveTransactionList();

  if( !bank->setCurrentAccountId(accountId))
     return false; //account does not exist

  readTransactionList();

  //set the keyfile path for the current user
  banking::User * user = bank->currentUser();
  if( !user ) 
    return false; //no user
  user->setPassportFilePath(_PassportFileDir + "/");

  return true;
}

/**Returns current account. May return NULL.
*/
banking::Account * DatabaseAccess::currentAccount(){
  banking::Bank * bank = bankByCode(_CurrentBankCode);
  if(bank)
   return bank->currentAccount();
  else return NULL;
}

/**Returns current account id. May return an empty string.
*/
const string & DatabaseAccess::currentAccountId(){
  if(currentAccount())
     return currentAccount()->accountId();
  else return Empty_String;
}

/**Returns current account IBAN. May return an empty string.
*/
string DatabaseAccess::currentIban(){
  if(currentAccount())
     return currentAccount()->iban();
  else return Empty_String;
}

/**Returns current account name. May return an empty string.
*/
string DatabaseAccess::currentAccountName(){
   banking::Account * account = currentAccount();
   if(account)
     return account->accountName();
   else return "";
}

/** Sets the current bank code, if bank is in list.
  * Returns true on success.
  */
bool DatabaseAccess::setCurrentBankCode(const string & bankCode){
  if(bankByCode(bankCode)){
    _CurrentBankCode = bankCode;
    return true;
  }

  return false;
}

/**Returns current bank.May return NULL.
 */
banking::Bank * DatabaseAccess::currentBank(){
  return bankByCode(_CurrentBankCode);
}

/**Returns current bankCode.May return an empty string.
 */
const string & DatabaseAccess::currentBankCode(){
  return _CurrentBankCode;
}

/**Returns current BIC.May return an empty string.
 */
const string & DatabaseAccess::currentBic(){
  banking::Bank * bank = currentBank();
  if(bank)
   return bank->bankBic();
  else return Empty_String;
}

/**Returns current user.May return NULL.
 */
banking::User * DatabaseAccess::currentUser(){
  banking::Bank * bank = currentBank();
  if(bank)
   return bank->currentUser();
  else return NULL;
}

/**Returns passport type for current user. Returns 0, if not known.
*/
int DatabaseAccess::passportType(){
  banking::User * user =currentUser();
  if(user)
    return user->passportType();
  else return 0;
}

/** Set the top directory for XML files. Adds / if required.
 * Creates newDataDir and its subdirectory _BankDir if they do not exist.
 */
void DatabaseAccess::setDataDir(const string & newDataDir){
  _DataDir = newDataDir;
  if(_DataDir[_DataDir.length() - 1] != '/')
   _DataDir += "/";
  _BankDir = (_DataDir + MONEYPENNY_BANKDATA_DIR + "/").c_str();
  QDir dir(_DataDir.c_str());
  if(!dir.exists()){
    dir.mkdir(_DataDir.c_str());
    system("sync"); //make shure, that the new dir is written
  }
  dir.setPath(_BankDir.c_str());
  if(!dir.exists()){
    dir.mkdir(_BankDir.c_str());
    system("sync"); //make shure, that the new dir is written
  }
}

/**Sets keyfile directory to be used for all users
 */
 void DatabaseAccess::setPassportFileDir(const string & newVal){
  _PassportFileDir = newVal;
  banking::Bank * bank = currentBank();
  banking::User * user = NULL;
  if(bank)
     user = bank->currentUser();
  if( user ) //set the keyfile path for the current user
     user->setPassportFilePath(_PassportFileDir + "/");
}



