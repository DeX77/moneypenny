/***************************************************************************
                          accountjobfactory.h  -  description
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

#ifndef ACCOUNTJOBFACTORY_H
#define ACCOUNTJOBFACTORY_H

#include "hobit.h"
#include "hobit_validator.h"
#include "hobit_creator.h"
//#include "accountjobxmlwriter.h"
#include "aqmoney2/aqmapi.h"
#include "aqmoney2/accountjobs.h"
#include "aqmoney2/account.h"
#include <openhbci2.h>


// predeclaration of class hobit
class HoBIT;
class AccountJobXMLWriter;


/**
  *@author Marcel Mulch
  */

class AccountJobFactory : public QObject {
Q_OBJECT //neccessary for translations
signals:
   void logTextToShow(QString logText);
public: 
	AccountJobFactory(AqMoneyAPI* api);
	~AccountJobFactory();

 void updateJobQueue(Outbox* queue);
 HobitError showJobResults();
 HobitError createJobGetBalance();
 HobitError createJobGetTransactions(Date fdate,
                                     Date tdate,
                                     int country,
                                     const string& bankId,
                                     const string& accountId);


 HobitError createJobGetStandingOrders(int country,
                                       const string& bankId,
                                       const string& accountId);

 HobitError createJobGetDatedTransfers(int country,
                                                           const string& bankId,
                                                           const string& accountId,
                                                           Date fromdate,
                                                           Date todate);

                                                        
 HobitError createJobSingleTransfer(bool skipBad,
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
                                                      const Date& date);


   /** deletes a dated transfer from the transfer queue of the bankserver */
  HobitError createJobDeleteDatedTransfer(bool skipBad,
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
                                                                const string& jobId);


  HobitError createJobSingleDebitNote(bool skipBad,
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
                                                         const Date& date);


    /** creates a new standing Order */
  HobitError createJobStartStandingOrder(bool skipBad,
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
                                                            int day);


  /** deletes a standing  order from the bankserver*/
  HobitError createJobDeleteStandingOrder( bool skipBad,
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
                                                                  bool weekly);

   /** returns the current account job queue */
   list<Pointer<AccountJob> > queue() {
   return _accountjobs;
   }

   /** deletes a certain job from the job list */
   void deleteJob(int number){
        int pos = 0;
  	list<Pointer<AccountJob> >::iterator it;
   
   	for (it = _accountjobs.begin();  it != _accountjobs.end(); it++){
          if (pos == number){
            _accountjobs.remove((*it));
             break;
         }
         pos++;
        }//end for
   }
 
private:
// Hobits Pre-Queue for storing jobs
AqMoneyAPI * _api;
//Pointer<HoBIT>  _hobit;
//Pointer<AccountJobXMLWriter> _writer;
list<Pointer<AccountJob> > _accountjobs;


};

#endif
