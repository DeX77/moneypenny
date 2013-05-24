 /***************************************************************************
 *   Copyright (C) 2004 by Walter Roth                                     *
 *   walter@amd2600-roth                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "openhbcisupport.h"
#include "kernel/helpers/transaction.h"
#include "kernel/helpers/bank.h"

// openhbci2 includes
#include <openhbci2.h>
using namespace HBCI;

#include "aqmoney2/aqmapi.h"
#include "aqmoney2/account.h"
#include "hobit_string.h"
#include "gwentransaction.h"
#include "standingorder.h"
#include "datedtransfer.h"
#include "hobit_transfertemplate.h"

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

#include <string>
#include <stdio.h>
using namespace std;

#include <QFileInfo>

// dump flags
#define HOBIT_DUMP_FLAGS_TRANSACTIONS   0x00010000
#define HOBIT_DUMP_FLAGS_BALANCE        0x00020000
#define HOBIT_DUMP_FLAGS_USERS          0x00040000
#define HOBIT_DUMP_FLAGS_ACCOUNT_HEADER 0x00080000
#define HOBIT_DUMP_FLAGS_STANDINGORDERS 0x00100000
#define HOBIT_DUMP_FLAGS_DATEDXFERS     0x00200000
#define HOBIT_DUMP_FLAGS_NOTEDXFERS     0x00400000

#define HOBIT_DUMP_FLAGS_DEFAULT \
  HOBIT_DUMP_FLAGS_ACCOUNT_HEADER | \
  AQM_TRANSACTION_MATCH_DEFAULT



 /** Constructs a new list containing all transactions of an account which were made during
 *  a specific time periode. List must be deleted by caller. DEPRECATED
 */

 banking::TransactionList * OpenHbciSupport::listGwenTransactions(int country,
                                                 const string &bankCode,
                                                 const string &accountId,
                                                 const string &accountSubId,
                                                 const Date &fromDate,
                                                 const Date &toDate){
  list<Pointer<Account> > accs;
  list<Pointer<Account> >::iterator ait;
  Pointer<Bank> bank;
  banking::Transaction * transaction;
  banking::TransactionList * result = new banking::TransactionList();
  Error err;
  GWEN_DB_NODE *xas;
  //bool first = true;

  accs =_api.ref().getAccounts(country, bankCode, accountId, accountSubId);

  if (accs.empty()) {
      cerr << "\nNo accounts found\n";
      return result;
  }
      
  for (ait=accs.begin(); ait!=accs.end(); ait++) {

     bank = _api.ref().findBank((*ait).ref().country(),
                                (*ait).ref().serverBankCode());
     if (!bank.isValid()){ 
        cerr << "\nInvalid Bank\n";
      return result;
     }
     xas=  (*ait).ref().storage().ref().getTransactions(MONEYPENNY_DEFAULT_COUNTRY,
                                                        "",
                                                        "",
                                                        fromDate,
                                                        toDate,
                                                        HOBIT_DUMP_FLAGS_DEFAULT);
     // if we got transactions
     if (xas) {
       GWEN_DB_NODE *n;
       n=GWEN_DB_GetFirstGroup(xas);
       if (n) {
                 cerr << "\n Transactions  found\n";
                 while(n) {
                   // get a single transaction
                    if (strcasecmp(GWEN_DB_GroupName(n), "transaction")==0) {
                      DBG_NOTICE(0, "Adding transaction "); 
                      transaction  = new GwenTransaction(n); 
                      result->push_back(transaction);
                    } // if transaction
                   n= GWEN_DB_GetNextGroup(n);
                 } // while n
             } // if (n)
       else return result; //return the empty list
       GWEN_DB_Group_free(xas);
     } // if transactions
 }  // end for
  return result;
}


/**Reads all transactions form the gwen database for account and returns a
 * new list of Transaction objects. List and Objects in this list must be deleted after use.
*/

banking::TransactionList * OpenHbciSupport::gwenTransactionList(const string & accountId){
  cerr << "DatabaseAccess::readTransactionList: Trying to evaluate OpenHBCI transaction data." << endl; 
  time_t simpleTime = time(NULL);
  struct tm * theTime = localtime(&simpleTime); //get the system time
  Date today = Date(theTime->tm_mday, theTime->tm_mon +1, theTime->tm_year + 1900);
  Date date1990 = Date(1,1,1990);
  banking::TransactionList * allTransactions = listGwenTransactions(0, "*", accountId, "*", date1990, today);
  return allTransactions; 
}

/** reads stored templates from file 
*/
 banking::TransferList * OpenHbciSupport::readGwenTransferTemplates(const string& templateFile){
  banking::TransferList * transferList = new banking::TransferList();
  Error err;
  GWEN_DB_NODE *db;
  GWEN_DB_NODE *n;
  GWEN_DB_NODE *gr;

  DBG_INFO(0, "Reading templates");

  if (access(templateFile.c_str(), F_OK)) {
    DBG_WARN(0,
             "Template file \"%s\" does not exist",
              templateFile.c_str());
  }
  else {
    DBG_INFO(0, "Loading Templates file \"%s\"",
             templateFile.c_str());

    db=GWEN_DB_Group_new("DatabaseAccess::readTransferTemplates");
    if (GWEN_DB_ReadFile(db, templateFile.c_str(),
			 GWEN_DB_FLAGS_DEFAULT|
			 GWEN_PATH_FLAGS_CREATE_GROUP)) {
      GWEN_DB_Group_free(db);
		 cerr <<  "OpenHbciSupport::readGwenTransferTemplates: Error loading Template config file" << endl;
      return transferList;
    }


  try {
  
  gr=GWEN_DB_GetGroup(db, GWEN_PATH_FLAGS_NAMEMUSTEXIST, "transfer_templates");
      if (gr) {
        n=GWEN_DB_GetFirstGroup(gr);
        while (n) {
          if (strcasecmp(GWEN_DB_GroupName(n), "template")==0) {
            // load templates

            HoBIT_TransferTemplate temp = HoBIT_TransferTemplate(n);
            banking::Transfer * transfer = new banking::Transfer(
                    banking::Transfer::NoType,
                    0.0, //value
                    qPrintable(temp.ourAccountId()),
                     "", //ourIban
                     "", //ourBankId(),
                     "", //ourBic
                     BANKTYPE_UNKNOWN,
                    qPrintable(temp.otherAccountId()),
                    "", //otherIban
                    qPrintable(temp.otherBankCode()),
                    "", //otherBic
                    qPrintable(temp.otherName()), //otherName1
                    "", //otherName2
                    temp.purposeStrings());

            transferList->push_back(transfer);

          } // if account
          else {
            DBG_INFO(0, "Unkown section \"%s\", ignoring",
                     GWEN_DB_GroupName(n));
          }
          n=GWEN_DB_GetNextGroup(n);
        } // while n
      } // if "templates"
  
  
      }	catch (Error xerr) {
           GWEN_DB_Group_free(db);
           cerr << "OpenHbciSupport::readGwenTransferTemplates: Exception during read" << endl;
           return transferList;
    }
    GWEN_DB_Group_free(db);
  }
  DBG_DEBUG(0, "Reading templates done.");
  return transferList;
}

/** Reads OppenHbci configuration data
*/
banking::Bank * OpenHbciSupport::readData(const string & dataDir, const string & keyFileDir){
    banking::Bank * bank = NULL;
    string configdir = dataDir + "/";
    cerr << "OpenHbciSupport::readData: Trying to load legacy OpenHBCI environment from MoneyPenny1.\n";
    deleteBankMessages(dataDir.c_str()); //bank messages may crash gwen 1.19.1 
    Error err = _api.ref().loadEnvironment( 
            string(configdir + MONEYPENNY_OPENHBCI_FILE),
            string(configdir + MONEYPENNY_AQMONEY_FILE));

   if (!err.isOk()) { //no OpenHbci data loaded
      DBG_ERROR(0, "Loading OpenHBCI environment failed. (%s)",
                err.errorString().c_str());
      return NULL;
   }
   cerr<<"OpenHbciSupport::readData: Trying to convert OpenHBCI data."<< endl;
   string templateFile = configdir + MONEYPENNY_TEMPLATE_FILE;
   cerr<<"OpenHbciSupport::readData: Trying to add OpenHBCI transfer templates... ";
   bool ok = !readGwenTransferTemplates(templateFile)->empty();
   if(!ok)
    cerr << "No OpenHBCI transfer templates found."<< endl;
   else cerr <<"OK" << endl;
   cerr << "OpenHbciSupport::readData: Trying to add OpenHBCI bank...";
   Pointer <Bank> openHbciBank = getBank();
   if(openHbciBank.isValid()){
      string bankCode = openHbciBank.ref().bankCode();
      string address = openHbciBank.ref().address();
      int hbciVersion = openHbciBank.ref().hbciVersion();
      string name = openHbciBank.ref().name();
      bank = new banking::Bank
         ("bic",  //bic
          bankCode,
          BANKTYPE_UNKNOWN,
          address, //server
          hbciVersion,
          "", // serverIpAddress,
          name);

     //add all users for this bank
     list<Pointer<User> > userList = _api.ref().getUsers();
     list<Pointer<User> >::iterator it;
     for(it = userList.begin(); it != userList.end(); it++){
        Pointer<User> openHbciUser = (*it);
        Pointer <Medium> medium =openHbciUser.ref().medium();
        int mediumType = medium.ref().securityMode();
        string mediumName = medium.ref().mediumName();//this includes a path
        QFileInfo fileInfo(mediumName.c_str());
        string fileName = qPrintable(fileInfo.fileName()); //remove the path
        //set the type of passport
        int passportType = PASSPORT_KEYFILE | PASSPORT_SECURITY_RDH | KF_OPENHBCI;
        if(mediumType == HBCI_SECURITY_DDV)
          passportType = PASSPORT_CHIPCARD | PASSPORT_SECURITY_DDV;
        string userName =    openHbciUser.ref().name();
        //create a user
        banking::User * user = new banking::User(
            openHbciUser.ref().userId(),
            openHbciUser.ref().userId(), //use as customerId
            passportType,
            fileName,
            bank->bankCode(),
            userName);
  
        //Now set the passport filepath
        user->setPassportFilePath(keyFileDir + "/");
  
         //Account loop
         list<Pointer<Account> > accounts = _api.ref().getAccounts();
         list<Pointer<Account> >::iterator  itAcc;
         for (itAcc = accounts.begin(); itAcc != accounts.end(); itAcc ++){ 
             banking::Account * account = 
                  new banking::Account(bank->bankCode(),
                                       (*itAcc).ref().accountId(),
                                       (*itAcc).ref().accountName(),
                                       (*itAcc).ref().userName());
             Value v = (*itAcc).ref().creditLine();
             account->setAccountLimit(v.toString());
             Account::Balance b = (*itAcc).ref().bookedBalance();
             v = b.value();
             Date d = b.date();
             DateTime balanceDate(d.year(), d.month(), d.day());
             account->setBalanceAndDate(v.toReadableString(), balanceDate);
             user->addAccount(account);
         } //end account loop
  
         bank->addUser(user);
     }//end user loop
     cerr << "Found bank: " << endl;
     bank->debug();
     qDebug("Loading OpenHbci data was successful. \n");
  }
  else{
   cerr << "No bank found. Completely new setup required" << endl;
   return NULL;
  }

 return bank;
}

/** Deletes bank messages from config file. Bank messages may cause gwen to crash. 
*/
void  OpenHbciSupport::deleteBankMessages(const QString & dataDir ){
   // remove account from moneypenny.conf
   QString msgStart = "[ \\t]*bankMsgs \\{\\s*\\n"; //skip the starting newline, it is needed in the file
   QString msgEnd = "\\s+\\} # bankMsgs\\s*\\n"; //include the trailing newline
   QString confFileName = dataDir + "/" + MONEYPENNY_AQMONEY_FILE;
   if(!HoBIT_String::removeTextBlock(msgStart, msgEnd, confFileName))
     return;
}

/** Returns the current bank */
Pointer<Bank> OpenHbciSupport::getBank(){
    list<Pointer<Bank> > banks;
    Pointer<Bank> bank;
    banks =  _api.ref().banks();

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
