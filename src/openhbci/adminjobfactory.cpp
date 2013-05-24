/***************************************************************************
                          adminjobfactory.cpp  -  description
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

#include "adminjobfactory.h"
#include <qobject.h>

AdminJobFactory::AdminJobFactory(AqMoneyAPI* api){
  _api = api;
}

AdminJobFactory::~AdminJobFactory(){
 qDebug("Destructor AdminJobFactory");
}


/** This method executes the HoBIT-JobQueue  */
void AdminJobFactory::updateJobQueue(Outbox* queue){
  list<Pointer<AdminJob> >::iterator admjobit;
  Error err;
     // adding accountjobs
     for (admjobit=_adminjobs.begin(); admjobit!=_adminjobs.end(); admjobit++) {
             queue->addJob((*admjobit).cast<OutboxJob>());
     } // for
}


/** shows results of the account job execution */
HobitError AdminJobFactory::showJobResults(){

list<Pointer<AdminJob> >::iterator admjobit;
  Error err;
  bool errorFlag = false;
  
// results for adminjobs
    for (admjobit=_adminjobs.begin(); admjobit!=_adminjobs.end(); admjobit++) {
      bool isBad;
      int resultCode = 0;
      int nJob = 0;
      QString resultMessage;
      Pointer<AdminJob> job = (*admjobit);
      list<MsgJob::Result>::const_iterator rit;
      for (rit=job.ref().results().begin();
           rit!=job.ref().results().end();
           rit++) {
           nJob ++;
           cerr << "Adminjob number: " << nJob<< endl;
           resultCode =  (*rit).resultCode();
           resultMessage += " " + QString((*rit).resultMessage().c_str()) + " ";
           fprintf(stderr, I18N("Result: %d %s\n"),
                   (*rit).resultCode(),
                   (*rit).resultMessage().c_str());        
      }

      if ((*admjobit).ref().result()!= HBCI_JOB_RESULT_FAILED) {
       QString resultString = QString((*admjobit).ref().resultString(true).c_str()) + "<br> ";
       emit logTextToShow(resultString);
      }
      else {
        DBG_ERROR(0, "Error in job");
        emit logTextToShow(tr("Error in job. Errorcode: ") + QString::number(resultCode) + "<br>" + resultMessage );
        errorFlag=true;
      }
      
      isBad=(*admjobit).ref().result()==HBCI_JOB_RESULT_FAILED;
  //    emit logTextToShow(QString((*admjobit).ref().resultString(!isBad).c_str()));

      if (!isBad) {
        err=(*admjobit).ref().commit();
        if (!err.isOk()) {
          emit logTextToShow(QString(err.errorString().c_str()) + "<br>" + resultMessage);
          errorFlag=true;
        }
      }
      else {
        emit logTextToShow(tr("Error in job"));
        errorFlag=true;
      }
    } // for 

   cerr << "AdminjobFactory::showJobResults: clearing job list" << endl <<endl;
   _adminjobs.clear();
   cerr << "AdminjobFactory::showJobResults: End of admin jobs" << endl <<endl;
 if (errorFlag)
    return HobitErrorExecution;
  return HobitErrorSuccess;

}


/** method which creates and enqueues a new job get keys
  */
HobitError AdminJobFactory::createJobGetKeys()
{
      Error err;
      list<Pointer<Customer> > custs;
      list<Pointer<Customer> >::iterator cit;
      list<Pointer<JobGetKeys> >::iterator jit;
      Pointer<JobGetKeys> job;
      bool errorFlag = false;

      custs=_api->getCustomers();
      if (custs.empty()) {
            emit logTextToShow(tr("No matching customers found"));
            cerr << "no customers found";
            return HobitErrorInvalidArgs;
        }

      for (cit=custs.begin(); cit!=custs.end(); cit++) {
        Pointer<Bank> bank;

        bank=(*cit).ref().user().ref().bank();
        job=new JobGetKeys(*cit);

        DBG_INFO(0, "Adding job for customer %d%s%s to queue",
               bank.ref().country(),
               bank.ref().bankCode().c_str(),
               (*cit).ref().custId().c_str());
        cerr << "Using HBCI Version: " << bank.ref().hbciVersion() << endl;       
       emit logTextToShow(tr("Adding Job: Get bank public key"));
        cerr << "no customers found";       
               
      _adminjobs.push_back(job.cast<AdminJob>());
    } // for
    
  if (errorFlag)
    return HobitErrorExecution;
  return HobitErrorSuccess;

}


/** method which creates and enqueues a new job get keys
  */
HobitError AdminJobFactory::createJobGetSystemId()
{
      Error err;
      list<Pointer<Customer> > custs;
      list<Pointer<Customer> >::iterator cit;
      list<Pointer<JobGetSystemId> >::iterator jit;
      Pointer<JobGetSystemId> job;
      bool errorFlag = false;

      custs=_api->getCustomers();
      if (custs.empty()) {
            emit logTextToShow(tr("No matching customers found"));
            cerr << "no customers found";
            return HobitErrorInvalidArgs;
        }

      for (cit=custs.begin(); cit!=custs.end(); cit++) {
        Pointer<Bank> bank;

        bank=(*cit).ref().user().ref().bank();
        job=new JobGetSystemId(*cit);

        DBG_INFO(0, "Adding job for customer %d%s%s to queue",
               bank.ref().country(),
               bank.ref().bankCode().c_str(),
               (*cit).ref().custId().c_str());
       emit logTextToShow(tr("Adding job: Get System Id"));
   
      _adminjobs.push_back(job.cast<AdminJob>());
    } // for

  if (errorFlag)
    return HobitErrorExecution;
  return HobitErrorSuccess;

}


/** creates an job, which retrieves a list of our accounts
  */
HobitError AdminJobFactory::createJobGetAccounts()
{
      Error err;
      list<Pointer<Customer> > custs;
      list<Pointer<Customer> >::iterator cit;
      list<Pointer<JobGetAccounts> >::iterator jit;
      Pointer<JobGetAccounts> job;
      bool errorFlag = false;

      custs=_api->getCustomers();
      if (custs.empty()) {
            emit logTextToShow(tr("No matching customers found"));
            cerr << "no customers found";
            return HobitErrorInvalidArgs;
        }

      for (cit=custs.begin(); cit!=custs.end(); cit++) {
        Pointer<Bank> bank;

        bank=(*cit).ref().user().ref().bank();
        job=new JobGetAccounts(*cit);

        DBG_INFO(0, "Adding job for customer %d%s%s to queue",
               bank.ref().country(),
               bank.ref().bankCode().c_str(),
               (*cit).ref().custId().c_str());
       emit logTextToShow(tr("Adding job: Get Accounts"));

      _adminjobs.push_back(job.cast<AdminJob>());
    } // for

  if (errorFlag)
    return HobitErrorExecution;
  return HobitErrorSuccess;

}


/** creates an job, which sends our own public keys
  * to the bank server
  */
HobitError AdminJobFactory::createJobSendKeys()
{
  list<Pointer<Customer> > custs;
  Pointer<Customer> cust;
  Pointer<JobSendKeys> job;
  Pointer<Medium> medium;
  Pointer<MediumRDHBase> mrdh;
  Pointer<User> user;
  Pointer<Bank> bank;
  Error err;
  bool errorFlag = false;

  custs=_api->getCustomers();
  
  if (custs.empty()) {
    fprintf(stderr,
            "%s",
            I18N("Error: No matching customers found.\n"));
    emit logTextToShow(tr("No matching customer found"));
    return HobitErrorInvalidArgs;
  }
  if (custs.size()>1) {
    fprintf(stderr,
            "%s",
            I18N("Error: Ambiguous customer specification.\n"));
     emit logTextToShow(tr("Error: Ambiguous customer specification"));
    return HobitErrorInvalidArgs;
  }
  
  cust=custs.front();
  user=cust.ref().user();
  medium=user.ref().medium();
  bank=user.ref().bank();
  if (medium.ref().securityMode()!=HBCI_SECURITY_RDH) {
    fprintf(stderr,
            "%s",
            I18N("Error: Not in RDH mode.\n"));
      emit logTextToShow(tr("Error: Not in RDH mode."));            
    return HobitErrorInvalidArgs;
  }
  mrdh=medium.cast<MediumRDHBase>();

  // mount medium
  err=mrdh.ref().mountMedium();
  if (!err.isOk()) {
    fprintf(stderr,
            I18N("Error mounting medium (%s)\n"),
            err.errorString().c_str());
    return HobitErrorExecution;
   emit logTextToShow(tr("Error mounting medium"));
  }

  // select context
  err=mrdh.ref().selectContext(bank.ref().country(),
                               bank.ref().bankCode(),
                               user.ref().userId());
  if (!err.isOk()) {
    mrdh.ref().unmountMedium();
    fprintf(stderr,
            I18N("Error selecting user context (%s)\n"),
            err.errorString().c_str());
    return HobitErrorExecution;
  }
  mrdh.ref().resetSEQ();

  // check if sign key is valid
  if (!mrdh.ref().userPubSignKey().isValid()) {
      emit logTextToShow(tr("Error, no sign key found"));
      return HobitErrorSequence;
    }
    if (!mrdh.ref().userPubCryptKey().isValid()) {
      emit logTextToShow(tr("Error, no crypt key found"));
      return HobitErrorSequence;
    }
    job=new JobSendKeys(cust,
                                        mrdh.ref().userPubSignKey(),
                                        mrdh.ref().userPubCryptKey());

   emit logTextToShow(tr("Adding job: Send Keys"));
    _adminjobs.push_back(job.cast<AdminJob>());
  
  if (errorFlag)
    return HobitErrorExecution;
  return HobitErrorSuccess;

}


/** method which creates and enqueues a new job get reports
  */
HobitError AdminJobFactory::createJobGetReports()
{
      Error err;
      list<Pointer<Customer> > custs;
      list<Pointer<Customer> >::iterator cit;
      list<Pointer<JobGetReports> >::iterator jit;
      Pointer<JobGetReports> job;
      bool errorFlag = false;

      custs=_api->getCustomers();
      if (custs.empty()) {
            emit logTextToShow(tr("No matching customers found"));
            cerr << "no customers found";
            return HobitErrorInvalidArgs;
        }

      for (cit=custs.begin(); cit!=custs.end(); cit++) {
        Pointer<Bank> bank;

        bank=(*cit).ref().user().ref().bank();
        job=new JobGetReports(*cit);

        DBG_INFO(0, "Adding job for customer %d%s%s to queue",
               bank.ref().country(),
               bank.ref().bankCode().c_str(),
               (*cit).ref().custId().c_str());
       emit logTextToShow(tr("Job get keys"));
        cerr << "no customers found";

      _adminjobs.push_back(job.cast<AdminJob>());
    } // for

  if (errorFlag)
    return HobitErrorExecution;
  return HobitErrorSuccess;

}



