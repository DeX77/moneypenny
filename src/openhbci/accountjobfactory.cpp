/***************************************************************************
                          accountjobfactory.cpp  -  description
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

#include "accountjobfactory.h"

AccountJobFactory::AccountJobFactory(AqMoneyAPI* api){
  _api = api;
  //_hobit = hobit;
//  _writer = new AccountJobXMLWriter(hobit,api,"/home/test");
}
AccountJobFactory::~AccountJobFactory(){
 qDebug("Destructor AccountJobFactory");
}


/** This method creates the HoBIT-JobQueue  */
void AccountJobFactory::updateJobQueue(Outbox* queue){

  list<Pointer<AccountJob> >::iterator accjobit;
 
 // adding accountjobs
    for (accjobit=_accountjobs.begin(); accjobit!=_accountjobs.end(); accjobit++) {
      queue->addJob((*accjobit).cast<OutboxJob>());
    } // for
}


/** shows results of the account job execution */
HobitError AccountJobFactory::showJobResults(){

list<Pointer<AccountJob> >::iterator accjobit;
  Error err;
  int resultCode = 0;
  bool errorFlag = false;
  int nJob = 0;

// results for accountjobs
    for (accjobit=_accountjobs.begin(); accjobit!=_accountjobs.end(); accjobit++) {
      nJob ++;
      cerr << "Accountjob number: " << nJob<< endl;

      bool isBad;
      list<MsgJob::Result>::const_iterator rit;
      QString resultMessage;
      for (rit=(*accjobit).ref().results().begin();
           rit!=(*accjobit).ref().results().end();
           rit++) {
           resultCode =  (*rit).resultCode();
           resultMessage += " " + QString((*rit).resultMessage().c_str()) + " ";
           fprintf(stderr, I18N("Result: %d %s\n"),
                   (*rit).resultCode(),
                   (*rit).resultMessage().c_str());
      }
      isBad=(*accjobit).ref().result()==HBCI_JOB_RESULT_FAILED;
      if(isBad)
       resultMessage += tr("Errorcode: ") + QString::number(resultCode);
      QString resultString = QString((*accjobit).ref().resultString(!isBad).c_str()) + "<br> " + resultMessage;
      emit logTextToShow(resultString);

      if (!isBad) {
        err=(*accjobit).ref().commit();
        if (!err.isOk()) {
            emit logTextToShow(err.errorString().c_str());
          errorFlag=true;
        }
      }
      else {
        emit logTextToShow(tr("Error in job"));
        errorFlag=true;
      }
    } // for
 cerr << "AccountJobFactory::showJobResults: Clearing job list" << endl;
    _accountjobs.clear();
 cerr << "AccountJobFactory::showJobResults: End of account jobs" << endl << endl;
 if (errorFlag)
    return HobitErrorExecution;
  return HobitErrorSuccess;
}


/** method which creates and enqueues a new job get reports
  */
HobitError AccountJobFactory::createJobGetBalance() {

  Error err;
  list<Pointer<Account> > accs;
  list<Pointer<Account> >::iterator ait;
  list<Pointer<JobGetBalance> > jobs;
  list<Pointer<JobGetBalance> >::iterator jit;
  Pointer<JobGetBalance> job;

  accs= _api->getAccounts();

  if (accs.empty()) {
    emit logTextToShow(tr("No accounts found"));
    return HobitErrorInvalidArgs;
  }

    for (ait=accs.begin(); ait!=accs.end(); ait++) {
      Pointer<Customer> cust;

      cust=_api->findCustomer((*ait).ref().country(),
                                                (*ait).ref().serverBankCode(),
                                                (*ait).ref().customer());
      if (!cust.isValid()) {
        DBG_ERROR(0, "Customer \"%s\" not found, aborting",
                  (*ait).ref().customer().c_str());
        emit logTextToShow(tr("Could not find customer, aborting"));
        return HobitErrorInconsistentSystem;
      }
      job=new JobGetBalance(cust, *ait);
      emit logTextToShow(tr("Adding job: Get Balance"));
      DBG_INFO(0, "Adding job for account %d%s%s to queue",
               (*ait).ref().country(),
               (*ait).ref().bankCode().c_str(),
               (*ait).ref().accountId().c_str());
              _accountjobs.push_back(job.cast<AccountJob>());
    } // for

  return HobitErrorSuccess;
}


HobitError AccountJobFactory::createJobGetTransactions(Date fdate,
                                                       Date tdate,
                                                       int country,
                                                       const string& bankId,
                                                       const string& accountId){
  Error err;
  list<Pointer<Account> > accs;
  list<Pointer<Account> >::iterator ait;
  list<Pointer<JobGetTransactions> > jobs;
  list<Pointer<JobGetTransactions> >::iterator jit;
  Pointer<JobGetTransactions> job;

  accs =_api->getAccounts(country,
                                             bankId,
                                             accountId);
  if (accs.empty()) {
    emit logTextToShow(tr("No matching accounts found"));    
    return HobitErrorInvalidArgs;
  }

    for (ait=accs.begin(); ait!=accs.end(); ait++) {
      Pointer<Customer> cust;
      Date fromDate;

      if (!fdate.isValid()) {
        int year, month, day;

        err=(*ait).ref().storage().ref().getLastDay(year, month, day);
        if (err.isOk()) {
          char dbuffer[16];

          snprintf(dbuffer, sizeof(dbuffer), "%04d%02d%02d",
                   year, month, day);
          fromDate=Date(dbuffer);
        }
      }
      else
        fromDate= fdate;

      cust=_api->findCustomer((*ait).ref().country(),
                              (*ait).ref().serverBankCode(),
                              (*ait).ref().customer());
      if (!cust.isValid()) {
         emit logTextToShow(tr("No matching customer found"));         
        return HobitErrorInconsistentSystem;
      }
      job=new JobGetTransactions(cust, *ait, fromDate, tdate);
      _accountjobs.push_back(job.cast<AccountJob>());
      } // for

  return HobitErrorSuccess;
}


HobitError AccountJobFactory::createJobGetDatedTransfers(int country,
                                                         const string& bankId,
                                                         const string& accountId,
                                                         Date fromdate,
                                                         Date todate){
  Error err;
  list<Pointer<Account> > accs;
  list<Pointer<Account> >::iterator ait;
  Pointer<Outbox> queue;
  list<Pointer<JobGetDatedTransfers> > jobs;
  list<Pointer<JobGetDatedTransfers> >::iterator jit;
  Pointer<JobGetDatedTransfers> job;
  
  accs=_api->getAccounts(country, bankId, accountId);

  if (accs.empty()) {
    emit logTextToShow(tr("No matching Accounts found!"));
    return HobitErrorInvalidArgs;
  }

      for (ait=accs.begin(); ait!=accs.end(); ait++) {
      Pointer<Customer> cust;

      cust=_api->findCustomer((*ait).ref().country(),
                                               (*ait).ref().serverBankCode(),
                                               (*ait).ref().customer());
      if (!cust.isValid()) {
        emit logTextToShow(tr("Customer not found"));
        return HobitErrorInconsistentSystem;
      }

      job=new JobGetDatedTransfers(cust, *ait, fromdate, todate);
     emit logTextToShow(tr("Adding job: Get Dated Transfers"));
     _accountjobs.push_back(job.cast<AccountJob>());
     
    } // for

return HobitErrorSuccess;
}




HobitError AccountJobFactory::createJobGetStandingOrders(int country,
                                                         const string& bankId,
                                                         const string& accountId){
  Error err;
  list<Pointer<Account> > accs;
  list<Pointer<Account> >::iterator ait;
  list<Pointer<JobGetStandingOrders> > jobs;
  list<Pointer<JobGetStandingOrders> >::iterator jit;
  Pointer<JobGetStandingOrders> job;

  accs=_api->getAccounts(country,
                              bankId,
                              accountId);
  if (accs.empty()) {
      emit logTextToShow(tr("No matching accounts found"));   
    return HobitErrorInvalidArgs;
  }

    for (ait=accs.begin(); ait!=accs.end(); ait++) {
      Pointer<Customer> cust;

      cust=_api->findCustomer((*ait).ref().country(),
                                   (*ait).ref().serverBankCode(),
                                   (*ait).ref().customer());
      if (!cust.isValid()) {
        DBG_ERROR(0, "Customer \"%s\" not found, aborting",
                  (*ait).ref().customer().c_str());
        emit logTextToShow(tr("Customer data are invalid"));                   
        return HobitErrorInconsistentSystem;
      }

      emit logTextToShow(tr("Adding job: Get Standing Orders"));                   
      job=new JobGetStandingOrders(cust, *ait);
      DBG_INFO(0, "Adding job for account %d%s%s to queue",
               (*ait).ref().country(),
               (*ait).ref().bankCode().c_str(),
               (*ait).ref().accountId().c_str());
      _accountjobs.push_back(job.cast<AccountJob>());
    } // for


return HobitErrorSuccess;
}

HobitError AccountJobFactory::createJobSingleTransfer(bool skipBad,
                                                           int country,
                                                           const string &bankCode,
                                                           const string &accountId,
                                                           const string &accountSubId,
                                                           int otherCountry,
                                                           const string& otherBankCode,
                                                           const string& otherAccount,
                                                           const list<string> &otherName,
                                                           const list<string> &purpose,
                                                           const  Value&  value,
                                                           int textkey,
                                                           const Date& date){
  Error err;
  list<Pointer<Account> > accs;
  list<Pointer<JobSingleTransfer> > jobs;
  list<Pointer<JobDatedSingleTransfer> > djobs;
  list<Pointer<JobSingleTransfer> >::iterator jit;
  list<Pointer<JobDatedSingleTransfer> >::iterator djit;
  Pointer<JobSingleTransfer> job;
  Pointer<JobDatedSingleTransfer> djob;
  GWEN_DB_NODE *skippedXA = 0;
//  GWEN_DB_NODE *badXA = 0;
//  GWEN_DB_NODE *goodXA = 0;
  HoBIT_Validator validator(_api);
  Pointer<Account> acc;
  Pointer<Customer> cust;
  bool skip = false;
  GWEN_DB_NODE *xa = 0;

/*  accs=_api->getAccounts(country, //this does not work with leading zeros
                         bankCode,
                         accountId,
                         accountSubId);
*/
      list<Pointer<Account> > accounts = _api->getAccounts();
      list<Pointer<Account> >::iterator  accit;
      for (accit=accounts.begin(); accit!=accounts.end(); accit++)
      {
        if(accountId ==(*accit).ref().accountId())
          acc = *accit;
      }

      if (!acc.isValid()) {
        emit logTextToShow(tr("No matching accounts found"));
        return HobitErrorInvalidArgs;
      }
/*      if (accs.size()>1) {
        emit logTextToShow(tr("Wrong accounts - please check your input"));
        return HobitErrorInvalidArgs;
      }
      acc=accs.front();
*/
      // check account
         if(validator.checkAccount(otherCountry, otherBankCode, otherAccount)){
         emit logTextToShow(tr("The recipient's account seems to be invalid"));
         if (skipBad) {
            skip=true;
          }
        }

       cust=_api->findCustomer(acc.ref().country(),
                                                 acc.ref().serverBankCode(),
                                                 acc.ref().customer());

      if (!cust.isValid()) {
          DBG_ERROR(0, "Customer \"%s\" not found, aborting",
                    acc.ref().customer().c_str());
          emit logTextToShow(tr("The customer has not been found"));         
          return HobitErrorInconsistentSystem;
      }
        xa=HoBIT_Creator::makeTransaction(acc,
                                          otherCountry,
                                          otherBankCode,
                                          otherAccount,
                                          otherName,
                                          purpose,
                                          value,
                                          textkey,
                                          date,
                                          "");
      if (!skip) {
          // if dated transfer
             if (date.isValid()) {
              djob=new JobDatedSingleTransfer(cust, acc);
              err= validator.checkTransaction(xa, djob.ref().paramData());
                   if (!err.isOk()) {
                         skip=true;
                    } 
                  else {
                            err=djob.ref().setTransaction(xa);
                            if (!err.isOk()) {
                                skip=true;
                              }
                            else {
                                 DBG_INFO(0, "Adding dated job for account %d/%s/%s to queue",
                                          acc.ref().country(),
                                          acc.ref().bankCode().c_str(),
                                          acc.ref().accountId().c_str());
                                         _accountjobs.push_back(djob.cast<AccountJob>());
                                     }
                                }
                  }
      // if single transfer
           else {
                       job=new JobSingleTransfer(cust, acc);
                       err=validator.checkTransaction(xa, job.ref().paramData());
                                if (!err.isOk()) {
                                              skip=true;
                                              }
                                  else {
                                        	      err=job.ref().setTransaction(xa);
                                                 if (!err.isOk()) {
                                                                          skip=true;
                                                          }
                                                  else {
                                                                DBG_INFO(0, "Adding job for account %d/%s/%s to queue",
                                                                acc.ref().country(),                     
                                                                acc.ref().bankCode().c_str(),
                                                                acc.ref().accountId().c_str());
                                                               _accountjobs.push_back(job.cast<AccountJob>());
                                                              
                                                          }
                                                   }
                                            }

      }
      
     if (skip) {
          if (!skippedXA)
            skippedXA=GWEN_DB_Group_new("skipped");
            GWEN_DB_AddGroup(skippedXA, GWEN_DB_Group_dup(xa));
              }

  GWEN_DB_Group_free(xa);
  xa=0;
                                                                                    
return HobitErrorSuccess;
}




/** deletes a dated transfer from the transfer queue of the bankserver */
HobitError AccountJobFactory::createJobDeleteDatedTransfer(bool skipBad,
                                                           int country,
                                                           const string &bankCode,
                                                           const string &accountId,
                                                           const string &accountSubId,
                                                           int otherCountry,
                                                           const string& otherBankCode,
                                                           const string& otherAccount,
                                                           const list<string> &otherName,
                                                           const list<string> &purpose,
                                                           const  Value&  value,
                                                           int textkey,
                                                           int textkeyExt,
                                                           const Date& date,
                                                           const string& jobId){

  Error err;
  list<Pointer<Account> > accs;
  list<Pointer<JobDeleteDatedSingleTransfer> > jobs;
  list<Pointer<JobDeleteDatedSingleTransfer> >::iterator jit;
  Pointer<JobDeleteDatedSingleTransfer> job;
  HoBIT_Validator validator(_api);
  GWEN_DB_NODE *skippedXA = 0;
//  GWEN_DB_NODE *badXA = 0;
//  GWEN_DB_NODE *goodXA = 0;


      Pointer<Account> acc;
      Pointer<Customer> cust;
      bool skip;
      GWEN_DB_NODE *xa;

      skip=false;
      xa=0;
      accs=_api->getAccounts(country,
                                                bankCode,
                                                accountId);
      if (accs.empty()) {
        emit logTextToShow(tr("No matching accounts found"));
        return HobitErrorInvalidArgs;
      }
      if (accs.size()>1) {
        emit logTextToShow(tr("Ambigious account description"));
        return HobitErrorInvalidArgs;
      }

      acc=accs.front();

      // check account
      if (!validator.checkAccount(otherCountry,
                                              otherBankCode,
                                              otherAccount)) {
        if (skipBad) {
          skip=true;
        }
      }

      cust=_api->findCustomer(acc.ref().country(),
                                               acc.ref().serverBankCode(),
                                               acc.ref().customer());
      if (!cust.isValid()) {
        DBG_ERROR(0, "Customer \"%s\" not found, aborting",
                              acc.ref().customer().c_str());
        emit logTextToShow(tr("Inconsistent system: customer was not found"));
        return HobitErrorInconsistentSystem;
      }

      xa=HoBIT_Creator::makeTransaction(acc,
                                        otherCountry,
                                        otherBankCode,
                                        otherAccount,
                                        otherName,
                                        purpose,
                                        value,
                                        textkey,
                                        date,
                                        jobId);
      if (!skip) {
        try {
          job=new JobDeleteDatedSingleTransfer(cust, acc, xa);
          DBG_INFO(0, "Adding job for account %d/%s/%s to queue",
                   acc.ref().country(),
                   acc.ref().bankCode().c_str(),
                   acc.ref().accountId().c_str());
          emit logTextToShow(tr("Adding job: Delete Dated Transfer"));
          _accountjobs.push_back(job.cast<AccountJob>());
        } // try
        catch (Error xerr) {
        emit logTextToShow(xerr.errorString().c_str());
        skip=true;
        }
      } // if !skip

      if (skip) {
        if (!skippedXA)
          skippedXA=GWEN_DB_Group_new("skipped");
        GWEN_DB_AddGroup(skippedXA, GWEN_DB_Group_dup(xa));
      }

      GWEN_DB_Group_free(xa);
      xa=0;
 return HobitErrorSuccess;
}






  HobitError AccountJobFactory::createJobSingleDebitNote(bool skipBad,
                                                                                       int country,
                                                                                       const string &bankCode,
                                                                                       const string &accountId,
                                                                                       const string &accountSubId,
                                                                                       int otherCountry,
                                                                                       const string& otherBankCode,
                                                                                       const string& otherAccount,
                                                                                       const list<string> &otherName,
                                                                                       const list<string> &purpose,
                                                                                       const  Value&  value,
                                                                                       int textkey,
                                                                                       const Date& date){


  Error err;
  list<Pointer<Account> > accs;
  list<Pointer<JobSingleDebitNote> > jobs;
  list<Pointer<JobSingleDebitNote> >::iterator jit;
  HoBIT_Validator validator(_api);
  Pointer<JobSingleDebitNote> job;
  GWEN_DB_NODE *skippedXA = 0;
//  GWEN_DB_NODE *badXA = 0;
//  GWEN_DB_NODE *goodXA = 0;



      Pointer<Account> acc;
      Pointer<Customer> cust;
      bool skip;
      GWEN_DB_NODE *xa;

      skip=false;
      xa=0;
      accs=_api->getAccounts(country,
                                                bankCode,
                                                accountId,
                                                accountSubId);
      if (accs.empty()) {
          emit logTextToShow("No matching accounts found");
        return HobitErrorInvalidArgs;
      }
      if (accs.size()>1) {
            emit logTextToShow("Ambigious account declaration");
        return HobitErrorInvalidArgs;
      }
      acc=accs.front();
      // check account
      if (validator.checkAccount( otherCountry,
                                  otherBankCode,
                                  otherAccount)) {
        if (skipBad) 
          skip=true;
      }

      cust=_api->findCustomer(acc.ref().country(),
                                   acc.ref().serverBankCode(),
                                   acc.ref().customer());
      if (!cust.isValid()) {
        DBG_ERROR(0, "Customer \"%s\" not found, aborting",
                  acc.ref().customer().c_str());
        emit logTextToShow(tr("Customer was not found, aborting"));
        return HobitErrorInconsistentSystem;
      }

     xa=HoBIT_Creator::makeTransaction(acc,
                                       otherCountry,
                                       otherBankCode,
                                       otherAccount,
                                       otherName,
                                       purpose,
                                       value,
                                       textkey,
                                       date,
                                       "");

      if (!skip) {
          try {
               job= new JobSingleDebitNote(cust, acc, xa);
                DBG_INFO(0, "Adding job for account %d/%s/%s to queue",
                                        acc.ref().country(),
                                        acc.ref().bankCode().c_str(),
                                         acc.ref().accountId().c_str());
               _accountjobs.push_back(job.cast<AccountJob>());

       }  catch (Error xerr) {
          emit logTextToShow(xerr.errorString().c_str());
          skip=true;
        }
      } // if !skip

      if (skip) {
        if (!skippedXA)
          skippedXA=GWEN_DB_Group_new("skipped");
          GWEN_DB_AddGroup(skippedXA, GWEN_DB_Group_dup(xa));
      }

     GWEN_DB_Group_free(xa);
      xa=0;
                                                                                       
   return HobitErrorSuccess;
}



//------------------------------------------------------------------------------------------------------------
//      Standing Orders
//----------------------------------------------------------------------------------------------------------

    /** creates a new standing Order */
  HobitError AccountJobFactory::createJobStartStandingOrder(bool skipBad,
                                                            int country,
                                                            const string &bankCode,
                                                            const string &accountId,
                                                            const string &accountSubId,
                                                            int otherCountry,
                                                            const string& otherBankCode,
                                                            const string& otherAccount,
                                                            const list<string> &otherName,
                                                            const list<string> &purpose,
                                                            const  Value&  value,
                                                            int textkey,
                                                            const Date& date,
                                                            const Date& firstdate,
                                                            const Date& lastdate,
                                                            const int turnus,
                                                            bool weekly,
                                                            int day){

  Error err;
  HoBIT_Validator validator(_api);
  list<Pointer<Account> > accs;
  Pointer<Outbox> queue;
  list<Pointer<JobStartStandingOrder> > jobs;
  list<Pointer<JobStartStandingOrder> >::iterator jit;
  Pointer<JobStartStandingOrder> job;
  GWEN_DB_NODE *skippedXA = 0;
//  GWEN_DB_NODE *badXA = 0;
//  GWEN_DB_NODE *goodXA = 0;

      Pointer<Account> acc;
      Pointer<Customer> cust;
      bool skip;
      GWEN_DB_NODE *xa;

      skip=false;
      xa=0;
      accs=_api->getAccounts(country,
                                  bankCode,
                                  accountId);
      if (accs.empty()) {
        emit logTextToShow(tr("No accounts found"));
        return HobitErrorInvalidArgs;
      }
      if (accs.size()>1) {
        emit logTextToShow(tr("Ambiguous account specification.\n"));
        return HobitErrorInvalidArgs;
      }
      acc=accs.front();
      // check account
      if (validator.checkAccount(otherCountry,
                                 otherBankCode,
                                 otherAccount)) {
        if (skipBad) {
            emit logTextToShow(tr("The recipient's account seems to be invalid."));
          skip=true;
        }
      }

      cust=_api->findCustomer(acc.ref().country(),
                                   acc.ref().serverBankCode(),
                                   acc.ref().customer());
if (!cust.isValid()) {
          emit logTextToShow(tr("Customer seems to be invalid."));
        return HobitErrorInconsistentSystem;
      }

      xa= HoBIT_Creator::makeStandingOrder (   acc,
                                               otherCountry,
                                               otherBankCode,
                                               otherAccount,
                                               otherName,
                                               purpose,
                                               value,
                                               textkey,
                                               date,
                                               firstdate,
                                               lastdate,
                                               weekly,
                                               turnus,
                                               day);
      if (!skip) {
        try {
	      job=new JobStartStandingOrder(cust, acc, xa);
          DBG_INFO(0, "Adding job for account %d/%s/%s to queue",
                   acc.ref().country(),
                   acc.ref().bankCode().c_str(),
                   acc.ref().accountId().c_str());
          _accountjobs.push_back(job.cast<AccountJob>());
        } // try
    	catch (Error xerr) {
	     emit logTextToShow(xerr.errorString().c_str());
        skip=true;
        }
      } // if !skip

      if (skip) {
        if (!skippedXA)
          skippedXA=GWEN_DB_Group_new("skipped");
        GWEN_DB_AddGroup(skippedXA, GWEN_DB_Group_dup(xa));
      }
      GWEN_DB_Group_free(xa);
      xa=0;
      
return HobitErrorSuccess;
}


  /** deletes a standing  order from the bankserver*/
  HobitError AccountJobFactory::createJobDeleteStandingOrder( bool skipBad,
                                                              int country,
                                                              const string& bankCode,
                                                              const string& accountId,
                                                              int otherCountry,
                                                              const string& otherBankCode,
                                                              const string& otherAccountId,
                                                              const list<string>& otherName,
                                                              const list<string>& purpose,
                                                              const  Value&  value,
                                                              int textkey,
                                                              int textkeyExt,
                                                              const Date& date,
                                                              const Date& firstdate,
                                                              const Date& lastdate,
                                                              const string& jobId,
                                                              int turnus,
                                                              int day,
                                                              bool weekly){

  Error err;
  list<Pointer<Account> > accs;
  HoBIT_Validator validator(_api);
  list<Pointer<JobDeleteStandingOrder> > jobs;
  list<Pointer<JobDeleteStandingOrder> >::iterator jit;
  Pointer<JobDeleteStandingOrder> job;
  GWEN_DB_NODE *skippedXA = 0;
//  GWEN_DB_NODE *badXA = 0;
//  GWEN_DB_NODE *goodXA = 0;

      Pointer<Account> acc;
      Pointer<Customer> cust;
      bool skip;
      GWEN_DB_NODE *xa;

      skip=false;
      xa=0;
      accs=_api->getAccounts(country,
                                  bankCode,
                                  accountId);
      if (accs.empty()) {
        emit logTextToShow(tr("No matching accounts were found."));
        return HobitErrorInvalidArgs;
      }
      if (accs.size()>1) {
        emit logTextToShow(tr("Ambigious account specification"));
        return HobitErrorInvalidArgs;
      }
      acc=accs.front();
      // check account
      if (validator.checkAccount(otherCountry,
                                 otherBankCode,
                                 otherAccountId)) {
        if (skipBad) {
          skip=true;
          emit logTextToShow(tr("The recipient's account seems to be invalid."));
        }
      }

      cust=_api->findCustomer(acc.ref().country(),
                                   acc.ref().serverBankCode(),
                                   acc.ref().customer());
      if (!cust.isValid()) {
        emit logTextToShow(tr("Customer not found, aborting."));
        return HobitErrorInconsistentSystem;
      }

      xa= HoBIT_Creator::makeStandingOrder (   acc,
                                               otherCountry,
                                               otherBankCode,
                                               otherAccountId,
                                               otherName,
                                               purpose,
                                               value,
                                               textkey,
                                               textkeyExt,
                                               date,
                                               firstdate,
                                               lastdate,
                                               jobId,
                                               weekly,
                                               turnus,
                                               day);

      
      if (!skip) {
        try {
	  job=new JobDeleteStandingOrder(cust, acc, xa);
          emit logTextToShow(tr("Adding job: Delete Standing Order"));
          _accountjobs.push_back(job.cast<AccountJob>());
        } // try
        catch (Error xerr) {
         emit logTextToShow(xerr.errorString().c_str());
          skip=true;
        }
      } // if !skip

      if (skip) {
        if (!skippedXA)
          skippedXA=GWEN_DB_Group_new("skipped");
        GWEN_DB_AddGroup(skippedXA, GWEN_DB_Group_dup(xa));
      }
      GWEN_DB_Group_free(xa);
      xa=0;
                                                                                
      return HobitErrorSuccess;
}

