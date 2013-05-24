/***************************************************************************
                          adminjobfactory.h  -  description
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

#ifndef ADMINJOBFACTORY_H
#define ADMINJOBFACTORY_H

#include "hobit.h"
#include "aqmoney2/aqmapi.h"
#include "aqmoney2/adminjobs.h"
#include <openhbci2.h>
//#include "structs.h"


// predeclaration of class hobit
class HoBIT;



/**This class includes methods for creating and queing AdminJobs
  *@author Marcel Mulch
  */

class AdminJobFactory : public QObject{
Q_OBJECT //neccessary for translations and signals
signals:
 void logTextToShow(QString logText);
public: 
    AdminJobFactory(AqMoneyAPI* api);
    ~AdminJobFactory();

    /**
    *  executes hobits admin job queue
    */
    void updateJobQueue(Outbox* queue);

    /** show job results */
    HobitError showJobResults();

    /** method which creates and enqueues a new job get keys
    */
     HobitError createJobGetKeys();

    /**
    * generates a job which retrieves the system id of the bank server
    */
    HobitError createJobGetSystemId();

  /**
    * generates a job which retrieves a list of our accounts
    */
    HobitError createJobGetAccounts();

    /**
    * generates a job which sends our own public keys to
    * the bank server.
    */
    HobitError createJobSendKeys();


    /**
    * generates a job which retrieves reports
    * the bank server.
    */
    HobitError createJobGetReports();
    
   
   /** returns the current admin job queue */
   list<Pointer<AdminJob> > queue() {
   return _adminjobs;
   }
   
   
    /** deletes a certain job from the job list */
   void deleteJob(int number){
   int pos = 0;
   list<Pointer<AdminJob> >::iterator it;
   
   for (it = _adminjobs.begin();  it != _adminjobs.end(); it++){
       if (pos == number){
       _adminjobs.remove(*it);
    	break;
       }
       pos++;
   }
 }

    
    
private:
AqMoneyAPI * _api;
//Pointer<HoBIT>  _hobit;

  // Hobits Pre-Queue for storing jobs
list<Pointer<AdminJob> > _adminjobs;
};

#endif


