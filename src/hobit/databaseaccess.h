/***************************************************************************
                          databaseaccess.h  -  description
                             -------------------
    begin                : Fr Nov 5 2004
    copyright            : (C) 2004, 2007 by Marcel Mulch, Walter Roth
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATABASEACCESS_H
#define DATABASEACCESS_H




  /* Internationalization */
#ifdef ENABLE_NLS
# include <libintl.h>
# include <locale.h>
# define I18N(m) gettext(m)
#else
# define I18N(m) m
#endif
  

#include <openhbci2.h>
using namespace HBCI;

#include <QObject> //for translation system
#include <QList>
#include <QString>
#include <QFile>

/*
#include "aqmoney2/aqmapi.h"
#include "aqmoney2/account.h"
#include "hobit_string.h"
#include "gwentransaction.h"
#include "standingorder.h"
#include "datedtransfer.h"
*/
#include "hobit_transfertemplate.h"

#include "openhbcisupport.h"
#include "kernel/helpers/bank.h"
#include "kernel/helpers/transaction.h"

#include <list>

#include <qdatetime.h>
using namespace std;


/**                                                                  
  *@author Marcel Mulch, Walter Roth
  * This class contains the persistence functions for the banking data.
  * It keeps a list of banks and transfer templates. However, only one account of one bank
  * can be active at a time. When an account is activated,
  * the history list of transactions for this account is read into TransactionList.
  */


class DatabaseAccess: public QObject{
Q_OBJECT //neccessary for translations
public: 
	DatabaseAccess(Pointer <AqMoneyAPI> api);
	virtual ~DatabaseAccess(); 

/**  This method creates a new User configuration, if not already available
  *  Sets _CurrentBank to bankCode.
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
                   const QString & userId,
                   const QString & customerId,
                   const QString & serverName);

 /**Copy new system id from BPD to bank
  * Call this function after BPD has been updated.
  */
 void updateSystemId();

/** Reads the data from the chipcard. There MAY be userid, bankid, server. Returns the number of valid user data sets.
*   If no valid set is found, strings in set 1 (the first record) are used. Else the data of the last valid set are used.
*/
 int readCardData(int & country, QString & bankId, QString & userId, QString & server, QString & cardnumber);

/** creates a new account. Strips leading zeros from accountId, if not 00000000 
  * @param accountId: the account number
  * @param accountSubId: Hbci allows subids for accounts 
  * 			 this parameter is normaly not needed
  * @param accountName:  you can give your account an description
  *
  */		
virtual bool addAccount(
			      const string &accountId = "", 
			      const string &accountSubId = "", 
			      const string& accountName = "");

/**Deletes the account and rebuilds AllAccounts list.
*/
virtual bool deleteAccount(const QString & accountId);

/** Sets the current account to accountId, if it is in the list of the current bank.
  * Saves transaction list for old account.
  * Reads list of transactions for new account.
  * Call setCurrentAccountId("") to unset an account.
  * Returns true on success.
  */
bool setCurrentAccountId(const string & accountId);
/** Sets the current bank code, if bank is in list.
  * Calls setCurrentAccountId("") to save old account data.
  * Returns true on success.
  */
bool setCurrentBankCode(const string & bankCode);

/**Returns current bank.May return NULL.
 */
banking::Bank * currentBank();
/**Returns current bank.May return NULL.
 */
const string & currentBankCode();

/**Returns current BIC.May return an empty string.
 */
const string & currentBic();

/**Returns current user.May return NULL.
 */
banking::User * currentUser();

/**Returns current account id. May return an empty string.
*/
const string & currentAccountId();

/**Returns current account IBAN. May return an empty string.
*/
string currentIban();

/**Returns current account name. May return an empty string.
*/
string currentAccountName();

/**Returns current account. May return NULL.
*/
banking::Account * currentAccount();
/**Returns an account list for all banks and all users.
*/ 
const banking::AccountList * accounts();

/**Returns passport type for current user. Returns 0, if not known.
*/
int passportType();

/** Returns a std::list which includes all registered users for the current bank
*/
virtual  const banking::UserList * users();

/**Adds bank to the list, if it is not already in the list
*/
bool addBank(banking::Bank * bank);

/**Deletes bank, if it is in the list
*/
bool deleteBank(const string & bankCode);

/** Returns bank with given bankCode. Returns NULL, if bank is not in list.
*/
banking::Bank * bankByCode(const string & bankCode);

/** removes all existing users from the gwen database
*/                                                                                                                  
 virtual  void deleteAllBanks();


/** Method to change used hbci version
*   checks if version is supported
*   by the bank and returns false if not
*/
  virtual  bool setHbciVersion(int version);

/** Reads Gwen data and stores data in list _TransactionList.
 *  Returns number of transactions read.
 *  targetDir is the directory containing the new data
 */
int addLegacyTransactionList(string targetDir);

/** Reads transactions_CCC_BBBBBBB_AAAAAAAAAAA.xml and stores data in list _Transactions.
 *  CCC is country code, BBB is BankId, AAA is AccountId.
 *  Returns number of transactions read.
 */
virtual int readTransactionList();

/** Writes transactions to XML file. returns true on success 
*/
bool saveTransactionList();

 /** Constructs a new list containing all transactions of an account which were made during
 *  a specific time periode. List must be deleted by caller. 
 */
  virtual banking::TransactionList * listTransactions( const DateTime &fromDate,
                                                       const DateTime &toDate);
 /** Constructs a new list containing all dated transfers of an account. 
 *List must be deleted by caller.
 */
banking::TransferList * listDatedTransfers();

/** Returns the date of the last stored transaction of current account
  * !!!!WARNING: This date may be in the future!!!!
  */
DateTime getLastDateOfTransactions();

/**Exports current list of transactions to file as CSV formatted data.
*/
bool exportCSV(QFile * file, char fieldSeparator, char textSeparator,
               const DateTime &fromDate,
               const DateTime &toDate);

/**Exports all data to file as SQL formatted data.
*/
bool exportSQL(QFile * file);
  
  
/**
* Method that reads the ktoblzcheck-banklist and returns 
* a list of banking::Bank-Objects
*
* I was heavily inspired by the ktoblzcheck-source, when I 
* wrote this method
*
* @author Narcel Mulch
* @date 4.12.2004
*
*/
QList<banking::Bank> readBankFile(const QString &filename);

/**
* Writes the hbd file format. Required due to license of FinTS_institute file.
*
*/
bool writeHbdBankFile(const QString &filename, QList<banking::Bank> &bankList); 

 /** returns the hash fingerprint of the institute after receiving the bank servers public keys 
 */
//  QString getBankFingerPrint();

 /** returns the fingerprint of the current public key
 */
//  QString getMyOwnFingerPrint();

 /** changes the account description */
  bool changeAccountName(const string& accid, const string& name);

  /** writes all templates and banks to XML file. returns true on success 
  * ( transactions are written in evaluateResponseData )
  */
  bool saveLists();

  /** Reads data for all lists from _DataDir 
   */
  bool readLists();

  /** Reads legacy data from _DataDir 
   */
  bool readOpenHbciData(const QString & dataDir, const QString & keyFileDir);


  /**Updates Balances and adds transactions to _TransactionLists of all accounts
  * Updates and saves all transactionlists for current user.
  */
  bool evaluateResponseData(banking::BankingJobList jobList);

  /** Add a transfer template and re-sort the list.
  */
  void addTransferTemplate(banking::Transfer * temp);

  /** returns all transfer templates 
  */
  banking::TransferList listTransferTemplates(){ return _TemplateList; };

  /** Deletes a transfer template 
   */
  void deleteTransferTemplate(int row);
  /** Set the top directory for XML files. Adds / if required.
   *  Creates newDataDir and its subdirectory _BankDir if they do not exist.
   */
  void setDataDir(const string & newDataDir);
 /** Returns _BankDir. Call setDataDir() first!!
  */
  const string & bankDir(){ return _BankDir; };
 /**Sets kefile directory to be used for all users
  */
  void setPassportFileDir(const string & newVal);
 /** enable / disable status signals
  */
  void setEmitStatusMessages(bool newVal){_EmitStatusMessages = newVal;};

signals:
  /** To be used when long computations are performed. e.g. readBankingJobList, writeBankingJobList
   */
  void statusMessage(const QString & message);
  /** reports job number
   */
   void processingJob(int jobNumber);
private:
  /** Clears all lists. Deletes objects in lists.
  */
  void clearLists();
  /** Clears template lists. Deletes objects in list.
  */
  void clearTemplateList();
  /** Clears transaction list. Deletes objects in list.
  */
  void clearTransactionList();
  /** Clears bank list. Deletes objects in list.
  */
  void clearBankList();
  /**Builds the list of all accounts for all banks and all users.
  */
  void buildAllAccountsList();

  /** Polymorphic save function. Used for DomObjects and all their child classes.
   *  @param listName is the tag name of the root tag in the file
   *  @param fileName is the name of the target file including the directory
   *  @param listToSave is a pointer to the list that is to be saved. Type casting to (list <BankingJob * > *) is required
   */

  bool saveBankingJobList(const string & listName, const string & fileName, 
                          banking::BankingJobList * listToSave);

  /** Polymorphic read function. Used for BankingJobs and all their child classes.
   * Reads file fileName and fills listToFill with all XML elements of type className.
   * Returns number of jobs read.
   *  @param listName is the tag name of the root tag in the file
   *  @param className is  the tag name of the class to be read
   *  @param fileName is the name of the target file including the directory
   *  @param listToFill is a pointer to the list that is to be read. Type casting to (BankingJobList *) is required
   */
  int readBankingJobList(const string & listName, const string & className, const string & fileName, 
                           banking::BankingJobList * listToFill);
  /** Saves the list of all banks.
   *  @param fileName is the name of the target file including the directory
   */
  bool saveBankList(const string & fileName);

  /** 
   * Reads file fileName and fills _BankList with all XML elements of type Bank.
   *  @param fileName is the name of the target file including the directory
   * Returns false on error.
   */
  bool readBankList (const string & fileName);

  /** Builds the transactionsFile name
  */
  string transactionsFileName(const string & directory);

 /**For OpenHBCI compatibility*/ 
 Pointer<AqMoneyAPI> _api;

 OpenHbciSupport _OpenHbciSupport;
  /** The top directory for unencrypted data
  */
  string _DataDir;
  /** The directory for the keyfiles
  */
  string _PassportFileDir;
  /**The subdirectory (_DataDir/_BankDir) for the banking data
  */ 
  string _BankDir;
  //lists 
  /** List of all banks
   */
  banking::BankList _BankList;
 /**List of all accounts in all banks
  */
  banking::AccountList _AllAccounts;
  /**Global template list. May be used for all banks and accounts.
  */
  banking::TransferList _TemplateList;

  //user data
  /** The active account
   */
//  string _CurrentAccountId;
  /** The active bank
   */
  string _CurrentBankCode;
  /** Flag for status related signals
   */
  bool _EmitStatusMessages;
};
#endif
