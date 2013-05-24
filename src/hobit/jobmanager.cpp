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
#include <QString>
#include <QDebug>
#include <QDate>
#include <QApplication>
#include <QMetaType>

#include "globals.h"
#include "jobmanager.h"

#include "kernel/helpers/datetime.h"
#include "kernel/global/HBCI_exceptions.h"
#include "kernel/responses/C_SaldoabfrageRes.h"
#include "kernel/responses/C_UeberweisungRes.h"
#include "kernel/api/C_GV_API.h"

#include "hobit_string.h"
#include "../banking/bankingjobitem.h"
#include "../dta/dtagenerator.h"

#include <signal.h>

using namespace banking;

#define DEBUG_JOBMANAGER 1

// standard exceptions with an object ready to use
/***************************************/
#include <iostream>
#include <exception>
using namespace std;

class ExNoBank: public ExHbciError
{
 public:
  virtual const char* what() const throw()
  {
    QString text = QObject::tr("No bank available");
    cerr << "***************Exeption Ex_NoBank: " << qPrintable(text)<< endl;
    return qPrintable(text);
  }
};

class ExNoUser: public ExHbciError
{
 public:
  virtual const char* what() const throw()
  {
    QString text = QObject::tr("No user data available");
    cerr << "***************Exeption Ex_NoUser: " << qPrintable(text)<< endl;
    return qPrintable(text);
  }
};

class ExTooManyJobs: public ExHbciError
{
 public:
  virtual const char* what() const throw()
  {
    QString text = QObject::tr("Too many jobs in queue");
    cerr << "***************Exeption too many jobs: " << qPrintable(text)<< endl;
    return qPrintable(text);
  }
};

class ExInvalidBank: public ExHbciError
{
 public:
  virtual const char* what() const throw()
  {
    QString text = QObject::tr("Invalid bank data");
    cerr << "***************Exeption Ex_InvalidBank: " << qPrintable(text)<< endl;
    return qPrintable(text);
  }
};

class ExKernelError: public ExHbciError
{
 public:
  ExKernelError(QString text = "")throw() : ExHbciError(){_Text = text;}
  ~ExKernelError()throw(){};

  virtual const char* what() const throw()
  {
    cerr << "***************Exeption Ex_KernelError: " << qPrintable(_Text)<< endl;
    return banking::to8Bit(QObject::tr("HBCI Kernel Error:") + " " + _Text).c_str();
  }
  private:
   QString _Text;
};

/** The flag for registered exceptions
 */
static bool exceptionsRegistered = false;
/** Registers all types of exceptions that may occur and 
  * shall be sent through signal/slot mechanism to the GUI thread. See catch blocks in run().
  * Call only once.
  */
void JobManager::registerExceptionMetatypes(){
 if(exceptionsRegistered)
  return;
 exceptionsRegistered = true;
 qRegisterMetaType<exception>("exception");
 qRegisterMetaType<ExNoBank>("ExNoBank");
 qRegisterMetaType<ExNoUser>("ExNoUser");
 qRegisterMetaType<ExTooManyJobs>("ExTooManyJobs");
 qRegisterMetaType<ExInvalidBank>("ExInvalidBank");
 qRegisterMetaType<ExKernelError>("ExKernelError");

 //Exceptions from kernel
 qRegisterMetaType<ExHbciError>("ExHbciError");
 qRegisterMetaType<ExUnsupportedPassportType>("ExUnsupportedPassportType");
 qRegisterMetaType<ExNoChipCard>("ExNoChipCard");
 qRegisterMetaType<ExInvalidChipCardData>("ExInvalidChipCardData");
 qRegisterMetaType<ExUnsupportedChipCard>("ExUnsupportedChipCard");
 qRegisterMetaType<ExNoReader>("ExNoReader");
 qRegisterMetaType<ExWrongPin>("ExWrongPin");
 qRegisterMetaType<ExPinInputIncomplete>("ExPinInputIncomplete");
 qRegisterMetaType<ExNoBankKeys>("ExNoBankKeys");
 qRegisterMetaType<ExIncompleteSettings>("ExIncompleteSettings");
 qRegisterMetaType<ExNoSEGVersion>("ExNoSEGVersion");
 qRegisterMetaType<ExKeyFileWrongPassword>("ExKeyFileWrongPassword");
 qRegisterMetaType<ExNoListener>("ExNoListener");
}

/***************************************/
//SIGPIPE handling

/**Flag for a caught SIGPIPE
 */
volatile sig_atomic_t _SIGPIPE_RECEIVED = 0;

/**Signal handler for broken pipes. This may occur due to long waiting for a TAN or similar.
*/
void sigPipeHandler(int sig){
  cout << "sigPipeHandler has received SIGPIPE. Connection time out?\n";
  _SIGPIPE_RECEIVED = 1;
  signal(sig, sigPipeHandler); //set handler for the next time
}

/***************************************/

/** Returns a translated version of the jobType. Used to add a user readable
  * description of the jobs to the _BankMessages.
  */
QString JobManager::trJobDescription(int jobType)
{
    static const char* jobDescriptions[] =
    { 
       QT_TR_NOOP("NoType"),
       QT_TR_NOOP("GetBPD"),
       QT_TR_NOOP("FirstKeyRequest"),
       QT_TR_NOOP("KeyChange"),
       QT_TR_NOOP("FirstKeySend"),
       QT_TR_NOOP("GetNewSystemId"),
       QT_TR_NOOP("Resynchronize"),
       QT_TR_NOOP("GetBalance"),
       QT_TR_NOOP("GetTransactions"),
       QT_TR_NOOP("SingleTransfer"),
       QT_TR_NOOP("DatedTransfer"),
       QT_TR_NOOP("StandingOrder"),
       QT_TR_NOOP("DebitNote"),
       QT_TR_NOOP("BookedTransaction")
    };
    return tr(jobDescriptions[jobType]);
 } 



  /** The map with the translations for the kernel messages.
   */
 QMap<std::string, QString> JobManager::_KernelTranslations = QMap<std::string, QString>() ;

 /** Returns the translated kernel message.
*/
 QString JobManager::trKernelMessage(const std::string & msg)
{
  if(_KernelTranslations.isEmpty()) //fill the map, if not yet done
  {
/** Defines for message texts that are displayed to user. If you modify these,
  * a new translation run with Qt Linguist is required. See moneypenny/src/hobit/jobmanager.cpp, trKernelMessage() 
  *
#define MSG_DIALOGINIT "dialoginit"
#define MSG_DIALOGEND "dialogend"
#define MSG_SYNCH "synch"
#define MSG_CONNECTED "connected"
#define MSG_NOT_CONNECTED "not connected"

#define EX_HBCI_ERROR_TEXT "Unknown Banking Error"
#define EX_UNSUPPORTED_PASSPORT_TYPE_TEXT "Could not use passport. Type is not supported."
#define EX_NO_CHIPCARD_TEXT "Could not find a chipcard."
#define EX_INVALID_CHIPCARD_DATA_TEXT "Invalid data on chipcard."
#define EX_UNSUPPORTED_CHIPCARD_TEXT "Chipcard type is not supported."
#define EX_NO_READER_TEXT "No cardreader available!"
#define EX_WRONG_PIN_TEXT "Wrong PIN!"
#define EX_PIN_INPUT_INCOMPLETE_TEXT "PIN input was not completed."
#define EX_NO_BANK_KEYS "Invalid or no bank keys! New setup required."
#define EX_KEYFILE_WRONG_PASSWORD_TEXT "Wrong password for keyfile!"
*/
     //Messages sent via relay_message. Defines from kernel/global/HBCI_Cern_Message_defines
     // MUST be hard-coded, otherwise lupdate does not find the texts
    _KernelTranslations.insert(MSG_DIALOGINIT, tr(QT_TR_NOOP("dialoginit")));
    _KernelTranslations.insert(MSG_DIALOGEND, tr(QT_TR_NOOP("dialogend")));
    _KernelTranslations.insert(MSG_SYNCH, tr(QT_TR_NOOP("synch")));
    _KernelTranslations.insert(MSG_CONNECTED, tr(QT_TR_NOOP("connected")));
    _KernelTranslations.insert(MSG_NOT_CONNECTED, tr(QT_TR_NOOP("not connected")));

     //Exception texts from kernel/global/HBCI_Cern_Message_defines
     // MUST be hard-coded, otherwise lupdate does not find the texts
    _KernelTranslations.insert( EX_HBCI_ERROR_TEXT, tr(QT_TR_NOOP("Unknown Banking Error")));
    _KernelTranslations.insert( EX_UNSUPPORTED_PASSPORT_TYPE_TEXT, tr(QT_TR_NOOP("Could not use passport. Type is not supported.")));
    _KernelTranslations.insert( EX_NO_CHIPCARD_TEXT, tr(QT_TR_NOOP("Could not find a chipcard.")));
    _KernelTranslations.insert( EX_INVALID_CHIPCARD_DATA_TEXT, tr(QT_TR_NOOP("Invalid data on chipcard.")));
    _KernelTranslations.insert( EX_UNSUPPORTED_CHIPCARD_TEXT, tr(QT_TR_NOOP("Chipcard type is not supported.")));
    _KernelTranslations.insert( EX_NO_READER_TEXT, tr(QT_TR_NOOP("No cardreader available!")));
    _KernelTranslations.insert( EX_WRONG_PIN_TEXT, tr(QT_TR_NOOP("Wrong PIN!")));
    _KernelTranslations.insert( EX_PIN_INPUT_INCOMPLETE_TEXT, tr(QT_TR_NOOP("PIN input was not completed.")));
    _KernelTranslations.insert( EX_NO_BANK_KEYS, tr(QT_TR_NOOP("Invalid or no bank keys! New setup required.")));
    _KernelTranslations.insert( EX_KEYFILE_WRONG_PASSWORD_TEXT, tr(QT_TR_NOOP("Wrong password for keyfile!")));
  }

  QString translation = _KernelTranslations[msg];
  if( ! translation.isEmpty())
    return translation;
  else return QString(msg.c_str());
}

/******************************************************************************************/
/** Constructor
*/
JobManager::JobManager()
:C_Listener()
{
  registerExceptionMetatypes(); //register all exceptions.Needed for signalling exceptions to GUI thread. 
  _GV_Api = NULL;
  _TaskToPerform = NO_TASK;
 // renewApi();
  _Bank = NULL;
  _PinTanResponse = NULL;
  _ErrorMessageReceived = false;
  _PasswordInvalid = false;
  _XmlDir = MP_DIR() + MONEYPENNY_DATA_DIR;
  _ReaderFile = _XmlDir + "/" + MONEYPENNY_READERS_FILE;
  _MultiThreaded = false;
  setWaitFlag(false);
  connect(this, SIGNAL(started()),
          this, SLOT(slotThreadStarted()));
  connect(this, SIGNAL(finished()),
          this, SLOT(slotThreadFinished()));
}

/**Creates a new _GV_Api and registers events.
* Calls deleteApi, if _GV_Api is not NULL
*/
void JobManager::renewApi(){
  int listeners = C_EventRegistry::get_number_of_listeners();

  if(_GV_Api)
    deleteApi();
  listeners = C_EventRegistry::get_number_of_listeners();
  _GV_Api = new C_GV_API();
  _GV_Api->register_for_event(RESPONSE_ERROR,  this, "Jobmanager");
  _GV_Api->register_for_event(RESPONSE_HIRMS_STRING,  this, "Jobmanager");
  _GV_Api->register_for_event(KEYFILE_PASS_FAILED,  this, "Jobmanager");
  _GV_Api->register_for_event(KEYFILE_PASSED,  this, "Jobmanager");
  _GV_Api->register_for_event(OPEN_KEYFILE_FAILED,  this, "Jobmanager");
  _GV_Api->register_for_event(MSG_CREATION_ERROR,  this, "Jobmanager");
  _GV_Api->register_for_event(NO_SUCH_ELEMENT_WARNING,  this, "Jobmanager");
  _GV_Api->register_for_event(XML_FILE_ERROR,  this, "Jobmanager");
  _GV_Api->register_for_event(SIGNATURE_CHECK_PASSED,  this, "Jobmanager");
  _GV_Api->register_for_event(SIGNATURE_CHECK_FAILED,  this, "Jobmanager");
  _GV_Api->register_for_event(INSERT_CARD,  this, "Jobmanager");
  _GV_Api->register_for_event(ENTER_PIN_VIA_KEYPAD,  this, "Jobmanager");
  _GV_Api->register_for_event(ENTER_PIN_VIA_KEYBOARD,  this, "Jobmanager");
  _GV_Api->register_for_event(ENTER_PIN_ERROR,  this, "Jobmanager");
  _GV_Api->register_for_event(ENTER_PIN_FINISHED,  this, "Jobmanager");
  _GV_Api->register_for_event(BPD_ANALYSED,  this, "Jobmanager");
  _GV_Api->register_for_event(SYNCH_NEW_SYSTEM_ID,  this, "Jobmanager");
  _GV_Api->register_for_event(PINTAN_REQUEST_PIN, 	this, "Jobmanager");
  _GV_Api->register_for_event(PINTAN_REQUEST_TAN, 	this, "Jobmanager");
  _GV_Api->register_for_event(PINTAN_CERT_PATH, 	this, "Jobmanager");	
  _GV_Api->register_for_event(SENDING_BANK_KEY_HASH, 	this, "Jobmanager");	
  _GV_Api->register_for_event(RESPONSE_HIRMS_CODE, 	this, "Jobmanager");	
  _GV_Api->register_for_event(PINTAN_SEND_RESPONSE_CONTAINER, 	this, "Jobmanager");	
  _GV_Api->register_for_event(SENDING_MESSAGE,  this, "Jobmanager");
  _GV_Api->register_for_event(RESPONSE_OK,  this, "Jobmanager");
  _GV_Api->register_for_event(XML_ELEMENT_NOT_FOUND,  this, "Jobmanager");
  _GV_Api->register_for_event(TCP_CONNECT_PASSED,  this, "Jobmanager");
  _GV_Api->register_for_event(TCP_CONNECT_FAILED,  this, "Jobmanager");
  listeners = C_EventRegistry::get_number_of_listeners();
  _ErrorMessageReceived = false;
  _BankMessages = "";
}

/**Resets all Job status to JobNew, if status is JobQueued
* and deletes _GV_Api
*/
void JobManager::deleteApi(){
    resetLists();
    if(!_GV_Api)
     return;
   _GV_Api->unregister_for_event(RESPONSE_ERROR,  this, "Jobmanager"); 
   _GV_Api->unregister_for_event(RESPONSE_HIRMS_STRING,  this, "Jobmanager"); 
   _GV_Api->unregister_for_event(KEYFILE_PASS_FAILED,  this, "Jobmanager");
   _GV_Api->unregister_for_event(KEYFILE_PASSED,  this, "Jobmanager");
   _GV_Api->unregister_for_event(OPEN_KEYFILE_FAILED,  this, "Jobmanager");
   _GV_Api->unregister_for_event(MSG_CREATION_ERROR,  this, "Jobmanager");
   _GV_Api->unregister_for_event(NO_SUCH_ELEMENT_WARNING,  this, "Jobmanager");
   _GV_Api->unregister_for_event(XML_FILE_ERROR,  this, "Jobmanager");
   _GV_Api->unregister_for_event(SIGNATURE_CHECK_PASSED,  this, "Jobmanager");
   _GV_Api->unregister_for_event(SIGNATURE_CHECK_FAILED,  this, "Jobmanager");
   _GV_Api->unregister_for_event(INSERT_CARD,  this, "Jobmanager");
   _GV_Api->unregister_for_event(ENTER_PIN_VIA_KEYPAD,  this, "Jobmanager");
   _GV_Api->unregister_for_event(ENTER_PIN_VIA_KEYBOARD,  this, "Jobmanager");
   _GV_Api->unregister_for_event(ENTER_PIN_ERROR,  this, "Jobmanager");
   _GV_Api->unregister_for_event(ENTER_PIN_FINISHED,  this, "Jobmanager");	
   _GV_Api->unregister_for_event(BPD_ANALYSED,  this, "Jobmanager");
   _GV_Api->unregister_for_event(SYNCH_NEW_SYSTEM_ID,  this, "Jobmanager");
   _GV_Api->unregister_for_event(PINTAN_REQUEST_PIN, 	this, "Jobmanager");
   _GV_Api->unregister_for_event(PINTAN_REQUEST_TAN, 	this, "Jobmanager");	
   _GV_Api->unregister_for_event(PINTAN_CERT_PATH, 	this, "Jobmanager");	
   _GV_Api->unregister_for_event(SENDING_BANK_KEY_HASH, 	this, "Jobmanager");	
   _GV_Api->unregister_for_event(RESPONSE_HIRMS_CODE, 	this, "Jobmanager");	
   _GV_Api->unregister_for_event(PINTAN_SEND_RESPONSE_CONTAINER, 	this, "Jobmanager");	
   _GV_Api->unregister_for_event(SENDING_MESSAGE,  this, "Jobmanager");
   _GV_Api->unregister_for_event(RESPONSE_OK,  this, "Jobmanager");
   _GV_Api->unregister_for_event(XML_ELEMENT_NOT_FOUND,  this, "Jobmanager");
   _GV_Api->unregister_for_event(TCP_CONNECT_PASSED,  this, "Jobmanager");
   _GV_Api->unregister_for_event(TCP_CONNECT_FAILED,  this, "Jobmanager");
   delete _GV_Api;
   C_EventRegistry::remove_all_listeners(); //Emergency clear. This should not be neccessary
   _GV_Api = NULL;
}

JobManager::~JobManager()
{
   deleteApi();
   clearLists();
 //DO NOT delete _Bank!! it is a copy.
   if(_PinTanResponse)
     delete _PinTanResponse;
}

/**Adds transfer at end of list.Returns number of transfers in list on success.
 */ 
int JobManager::addTransfer(banking::Transfer * transfer)
{
  if(!_Bank || !_Bank->currentUser())
    throw ExHbciError(qPrintable("JobManager::addTransfer: " + tr("Error: No active bank or no active user.")));

  string currentAccountId = _Bank->currentUser()->currentAccount()->accountId();
  if(transfer->ourBankCode() != _Bank->bankCode() || transfer->ourAccountId() != currentAccountId)
  { //error
    QString msg = 
       tr("Bank and/or account of owner in transfer do not match current settings.") + "\n" 
     + tr("Bankcode of transfer:") + " " + QString(transfer->ourBankCode().c_str()) + "\n" 
     + tr("Account Id of transfer:") + " " + QString(transfer->ourAccountId().c_str()) + "\n" 
     + tr("Bankcode of active account:") + QString(_Bank->bankCode().c_str()) + "\n" 
     + tr("Account Id of active account:") + QString(currentAccountId.c_str()) + "\n" 
     + tr("If you proceed, transfer will be executed from active account.") + "\n" ;
    bool proceedAnyway = false;
     emit overridableErrorDetected(&proceedAnyway, msg);
    if(proceedAnyway)
     _TransferList.push_back(transfer); //add to list anyway
  }
  else _TransferList.push_back(transfer); //everything ok
 return _TransferList.count();
}

/**Returns transfer at index and removes it form the list.
 * May return NULL.
 */
banking::Transfer * JobManager::takeTransferAt(int index){
 if(index < _TransferList.count())
   return _TransferList.takeAt(index);
 else return NULL;
}

/**Deletes transfer at index.
 */
 void JobManager::deleteTransfer(int index){
 if(index < _TransferList.count()){
   banking::Transfer * job = _TransferList[index]; //for debugging
   job->unregisterJob(_GV_Api, this);
   delete job;
   _TransferList.removeAt(index);
 }
}

/**Returns a pointer to the transfer at index. Returns NULL if index is out of range.
*/
 const banking::Transfer * JobManager::pendingTransfer(int index){
 if(index < _TransferList.count())
   return _TransferList[index];
 else return NULL;
}

/**Adds adminjob at end of list.
*/
bool JobManager::addAdminJob(banking::BankingJob * job){
  if(!_Bank){
    delete job;
    return false;
  }
  _AdminList.push_back(job);
  return true; 
}

/**Deletes admin job at index
*/
void JobManager::deleteAdminJob(int index){
 if(index < _AdminList.count()){
   banking::BankingJob * job = _AdminList[index]; //for debugging
   job->unregisterJob(_GV_Api, this);
   delete job;
   _AdminList.removeAt(index);
 }
}

/**Returns a pointer to the adminjob at index. Returns NULL if index is out of range.
*/
 const banking::BankingJob * JobManager::getAdminJob(int index){
 if(index < _AdminList.count())
   return _AdminList[index];
 else return NULL;
}

/**Deletes all jobs from both lists.
 */
void JobManager::clearLists(){
  QMutexLocker locker(&_KernelMutex); //Lock the mutex
  unregisterJobs();
  for(int i = 0; i < _TransferList.count(); i++){
    delete _TransferList[i];
   }
  _TransferList.clear();
  for(int i = 0; i < _AdminList.count(); i++){
    delete _AdminList[i];
  }
  _AdminList.clear();
}

/**Removes all jobs from GV_Api registration.
 */
void JobManager::unregisterJobs(){
  for(int i = 0; i < _TransferList.count(); i++){
    _TransferList[i]->unregisterJob(_GV_Api, this);
   }
  for(int i = 0; i < _AdminList.count(); i++){
    _AdminList[i]->unregisterJob(_GV_Api, this);
  }
}

/**Resets all job status values to JobNew, if status is JobQueued
 */
void JobManager::resetLists(){
  QMutexLocker locker(&_KernelMutex); //Lock the mutex
  unregisterJobs();
  for(int i = 0; i < _TransferList.count(); i++)
    if(_TransferList[i]->status() == BankingJob::JobQueued)
      _TransferList[i]->setStatus(BankingJob::JobNew);
  for(int i = 0; i < _AdminList.count(); i++)
    if(_AdminList[i]->status() == BankingJob::JobQueued)
      _AdminList[i]->setStatus(BankingJob::JobNew);
 }

 /**
 * Method to handle an Event. For description see 
 * class C_Listener and EventRegistry Documentation
 * Throws execptions, if unrecoverable errormessages are received.
 */
 void JobManager::handle_message(int message_code,
                       string message,
                       string class_name,
                       string method_name,
                       int level,
                       void * possible_data){
  #ifdef DEBUG_JOBMANAGER
  cerr << "JobManager::handle_message mc: " ;
  cerr << message_code << " msg: " << message << " cn: " << class_name << " mn: " << method_name << endl;
  #endif
  //Handle kernel error messages here
  string * hash; // for debugging SENDING_BANK_KEY_HASH
  BankingJob * job = NULL;
  string * kernelPin = NULL;
  string systemId = "";
  QString msg; //buffer

  switch (message_code) 
  {
     case SENDING_MESSAGE:
           cerr << "Jobmanager: received send notification: |" << message << "|\n";
           _BankMessages += trKernelMessage(message) + "\n";
           emit sendingMessage(trKernelMessage(message));
           //waitForGui("SENDING_MESSAGE");
       break; 
     case RESPONSE_OK: 
           cerr << "Jobmanager: received response notification: |" << message << "|\n";
            msg = trKernelMessage(message);
           _BankMessages += msg + "\n";
           emit responseOk(msg);
          // waitForGui("RESPONSE_OK");
       break; 
     case TCP_CONNECT_PASSED: 
           cerr << "Jobmanager: received connect notification: |" << message << "|\n";
           msg = tr("Internet:") + " " + trKernelMessage(message);
           _BankMessages += msg + "\n";
           emit responseOk(msg);
       //    waitForGui("TCP_CONNECT_PASSED");
       break; 
     case RESPONSE_ERROR: //no need to stop everything, other jobs may be ok
           cerr <<  " Jobmanager:  Error code detected while analyzing response.  Class: " << class_name << " Message: "<< message << " level: " << level << endl;
           if(possible_data)
             job = getJobById(*((int*)possible_data));
           if(job){ //we have no job for firstKeySend here, it is kept in the GV_API
             job->setStatus(BankingJob::JobFailed);
             job->copyResponseMessages(); //for GuiJobList
             _BankMessages += "\n\n" + tr("Sent") + ": " + trJobDescription(job->type()) + "\n";
           }
           msg = tr("Received bank errormessage") + ": " + QString(message.c_str());
           _BankMessages += msg + "\n";
           if((message.find ("9800") != string::npos)) //???suppress sync error message 9800, sync is fixed automatically???
             _ErrorMessageReceived = true;        //Eventcode is possible data
           emit responseError(msg);
           //waitForGui("RESPONSE_ERROR");
        break;
     case TCP_CONNECT_FAILED: 
           cerr << "Jobmanager: received connect error notification: |" << message << "|\n";
           msg = tr("Internet connection error:") + " " + trKernelMessage(message);
           _BankMessages += msg  + "\n";
           emit responseError(msg);
           //waitForGui("TCP_CONNECT_FAILED");
       break; 
     case RESPONSE_HIRMS_STRING: //collect all messages
          cerr << "Jobmanager: received HIRMS string: |" << message << "|\n";
          if(message[0] == '9' )
           {
             msg = tr("Received bank errormessage") + ": " + QString(message.c_str());
             _BankMessages += msg + "\n";
             emit responseError(msg);
            // waitForGui("RESPONSE_HIRMS_STRING");
           }
          else 
           {
             msg = tr("Received bank message") + ": " + QString(message.c_str());
             _BankMessages += msg + "\n";
             emit responseOk(msg);
           //  waitForGui("RESPONSE_HIRMS_STRING");
           }
        break;
 
     case XML_ELEMENT_NOT_FOUND: //complete stop required! 
        throw ExKernelError( tr(" Jobmanager: Kernel error: XML_ELEMENT_NOT_FOUND: ") + QString(message.c_str())); 
        break;

     case KEYFILE_PASS_FAILED: //complete stop required! 
        _PasswordInvalid = true; 
        cerr << "Jobmanager: Wrong password for keyfile!\n";
        throw ExKernelError( tr(" Jobmanager: Password for keyfile seems to be wrong!")); 
        break;

     case KEYFILE_PASSED:	
        cerr << "Jobmanager: Pass for Keyfile correct!" << endl;	
        break;

     case OPEN_KEYFILE_FAILED://complete stop required
        throw ExKernelError(tr("Jobmanager: Error opening keyfile!"));
        break;

     case MSG_CREATION_ERROR://complete stop required
        throw ExKernelError( tr("Jobmanager: Error creating message!"));
        break;
     case NO_SUCH_ELEMENT_WARNING://no complete stop required
        cerr << "JobManager::handle_message mc: " ;
        cerr << message_code << " msg: " << message << " cn: " << class_name << " mn: " << method_name << endl;
        //throw ExKernelError( tr("Jobmanager: Error creating message!"));
        break;

     case XML_FILE_ERROR://complete stop required
        throw ExKernelError(tr("Unsupported HBCI-Version. New setup required. Please try HBCI 2.2 or FinTS 3.0.") + message.c_str());
        break;

     case SIGNATURE_CHECK_PASSED:
        cerr << "Jobmanager: Signature check passed!" << endl;
        break;
	
     case SIGNATURE_CHECK_FAILED://no need to stop everything, this may be due to non synchronisation
        cerr <<"Jobmanager: Signature check failed! Message: "<< message << " Class: "<< class_name << endl;
        break;

     case INSERT_CARD:
        cerr << "Jobmanager: Insert card request" << endl;
        emit insertCardRequest(); //we have to wait for the card
        waitForGui("INSERT_CARD"); //wait for the Gui to unlock mutex by calling wakeKernelThread() 
        break;
		
     case ENTER_PIN_VIA_KEYPAD:
        cout << "Jobmanager: Input pin via reader's keypad request" << endl;
        emit inputPinViaKeypadRequest();//No waiting required, input runs completely in kernel thread!
        break;
	
     case ENTER_PIN_VIA_KEYBOARD:
        cerr << "Jobmanager:Input PIN via keyboard request" << endl;
        emit inputPinViaKeyboardRequest(& _Pin); //We have to wait for GUI input
        waitForGui("ENTER_PIN_VIA_KEYBOARD"); //wait for the Gui to unlock mutex by calling wakeKernelThread() 
        ((C_uarray *) possible_data)->set(qPrintable(_Pin));
        _Pin = ""; //reset it immediately
        break;

     case ENTER_PIN_ERROR:
        cerr << "\n\nJobmanager:!!!!!!!!!!!!!WRONG PIN ENTERED!!!!!!!!!!!!!! " << *((int*)possible_data) << " TRIES LEFT!\n\n" << endl;
        emit pinWasNotOk(*((int*)possible_data));
        break;

     case ENTER_PIN_FINISHED:
        cout << "Jobmanager:PIN was ok." << endl;
        emit pinWasOk();
        break;

     case BPD_ANALYSED:
        cout << "Jobmanager: BPD and UPD analyzed by kernel." << endl;
        _BankMessages += tr("BPD and UPD analyzed.") + "\n";//QString(message.c_str()) + "\n";
        break;

     case SYNCH_NEW_SYSTEM_ID:
        systemId =*((string *)possible_data);
        cout << "Jobmanager: New system-id received: " << systemId << endl;
         _BankMessages += tr("New system-id received") + ": " + systemId.c_str() + "\n";
         if(_Bank)
          _Bank->setSystemId(systemId);
        break;

     case PINTAN_REQUEST_PIN:
        kernelPin = (string *)(possible_data);
        cout << "Jobmanager: PIN request received." << endl;
        emit pinRequest(& _Pin);
        waitForGui("PINTAN_REQUEST_PIN"); //wait for the Gui to unlock mutex by calling wakeKernelThread() 
        (* kernelPin) = string(qPrintable(_Pin));
        break;

     case PINTAN_REQUEST_TAN:
        cout << "Jobmanager: TAN request received. Message:" << message  << endl;
        if(! message.empty())
          emit tanRequest(QString(message.c_str()),& _Tan);
        waitForGui("PINTAN_REQUEST_TAN"); //wait for the Gui to unlock mutex by calling wakeKernelThread() 
        (*((string *)(possible_data))) = qPrintable(_Tan);
        cout << "Jobmanager: TAN used: " << qPrintable(_Tan) << endl;
        break;

     case PINTAN_CERT_PATH:
        cout << "Jobmanager: Certificate path request received." << endl;
        emit certificatePathRequest(& _CertificatePath);
        waitForGui("PINTAN_CERT_PATH"); //wait for the Gui to unlock mutex by calling wakeKernelThread() 
        (*((string *)(possible_data))) = qPrintable(_CertificatePath);
        cout << "Jobmanager: Certificate path used: " << qPrintable(_CertificatePath) << endl;
        break;
     case PINTAN_SEND_RESPONSE_CONTAINER:
          if(_PinTanResponse)
           delete _PinTanResponse;
          _PinTanResponse = (C_RESPONSE*) possible_data;
        break;	

    case SENDING_BANK_KEY_HASH:
                  cout <<  "Hash of Bank's Key received:" << endl;
                  hash = ((string*)possible_data);
                  cout << *((string*)possible_data) << endl;
                  emit verifyBankKeyHash(hash);// connected to GuiHbciWizard::slotVerifyBankKeyHash
                  waitForGui("SENDING_BANK_KEY_HASH"); //wait for the Gui to unlock mutex by calling wakeKernelThread()
                  cout << "GUI returned string result:" << *hash  << endl;
                  *((string*)possible_data) = *hash;
                  _BankMessages += tr("Hash of bank key received: " ) + (*hash).c_str() + "\n";
                  break;

    case BANK_KEY_HASH_ERROR:
        cout << message << endl;
        break;

    case RESPONSE_HIRMS_CODE:
        job = getJobById(*((int*)possible_data));
        if(job)
          job->addMessage(message);
        break;

    default: break;
  }

  //keep the GUI responsive
  qApp->processEvents();

 //Now handle successful jobs 
 //First scan AdminList
  bool bpdAndUpdOk = false;
  QList<banking::BankingJob *>::iterator itAdminList;
  for (itAdminList = _AdminList.begin(); itAdminList!= _AdminList.end(); itAdminList++){
    BankingJob * job = (*itAdminList);
    if(message_code == job->id()){
      switch (job->type()){
        case BankingJob::GetTransactions:
           if(possible_data){
               int * records = (int*)possible_data;
               cout << "Job: "<< job->id()<<" GetTransactions: Number of Transactions:" << *records << endl;
               job->setMessage(banking::to8Bit(tr("Number of transactions:")  +  " "  + QString::number(* records)));
               job->setStatus(BankingJob::JobExecuted);
           }
          break;

        case BankingJob::GetBalance:
               job->setMessage(banking::to8Bit(tr("Balance received.")));// + " " +
                               //  +((C_SaldoabfrageRes *) job->response())->saldo[0].c_str()));
               job->setStatus(BankingJob::JobExecuted);
          break;

        case BankingJob::FirstKeyRequest:
               job->setMessage(banking::to8Bit(tr("Bank keys received.")));
               job->setStatus(BankingJob::JobExecuted);
               _BankMessages +=  tr("Bank keys received.") + "\n";
          break;

        case BankingJob::FirstKeySend:
               job->setMessage(message);
               if(((string*) possible_data)->at(0) == '0')//check error code
                 job->setStatus(BankingJob::JobExecuted);
               else job->setStatus(BankingJob::JobFailed);
               _BankMessages += QString(message.c_str()) +"\n";
          break;

        case BankingJob::KeyChange:
               job->setMessage(message);
               if(((string*) possible_data)->at(0) == '0')//check error code
                 job->setStatus(BankingJob::JobExecuted);
               else job->setStatus(BankingJob::JobFailed);
               _BankMessages += QString(message.c_str()) +"\n";
          break;

         case BankingJob::GetBPD:
               bpdAndUpdOk = updateUPDandBPD((C_BPD*)possible_data);
               if(bpdAndUpdOk){
                 job->setMessage(banking::to8Bit(tr("BPD and UPD updated in database.")));
                 job->setStatus(BankingJob::JobExecuted);
               }
               else {
               job->setStatus(BankingJob::JobFailed);
               cout << "Jobmanager: ERROR: BPD and UPD analyzed by kernel seem to be invalid." << endl;
               throw ExKernelError( tr("Error: BPD and UPD analyze by kernel seeem to be invalid."));
               }
           break;

         default:cout << "Job: "<< job->id()<<"Unknown type:" <<  job->typeToString(job->type()) << endl;
          break;
         } //End switch
         emit jobStatusChanged(job);
      }//end if message_code
   }//end for
 
  //Now scan TransferList
  QList<banking::Transfer *>::iterator itTransferList;
  for (itTransferList = _TransferList.begin(); itTransferList!= _TransferList.end(); itTransferList++){
    Transfer * job = (*itTransferList);
    if(message_code == job->id()){
      switch (job->type()){

        case BankingJob::SingleTransfer:
           if(possible_data){
               C_UeberweisungRes * res = (C_UeberweisungRes *) possible_data;
               list <string>::const_iterator itCode = res->get_bank_error_codes().begin(); 
               list <string>::const_iterator itMsg = res->get_bank_messages().begin(); 
               while (itCode != res->get_bank_error_codes().end()){ //put codes and messages into text lines
                  string code = (*itCode);
                  string existingMsg = job->message();
                  if(!existingMsg.empty()) //we have more than one HIRMS segments, add them in new lines
                    existingMsg += "\n";
                  string msg = (*itMsg);
                  job->setMessage( existingMsg + code + " : " + msg);
                  itCode++;
                  itMsg++;
                  if(code == "0020" || code == "0010")
                    job->setStatus(BankingJob::JobExecuted);
                  else if(code[0] == '9')
                    job->setStatus(BankingJob::JobFailed);
                  else //all other codes, especially 3XXX warnings
                   if((job->status() != BankingJob::JobExecuted ) && ( job->status() != BankingJob::JobFailed)) //Do not overwrite success or error status
                     job->setStatus(BankingJob::JobWarning);
               }
           }
          break;

         default:cout << "Job: "<< job->id()<<"Unknown type:" <<  job->typeToString(job->type()) << endl;
                 break;

       } //End switch
       emit jobStatusChanged(job);
      }//end if message_code
   }//end for

  //keep the GUI responsive
  qApp->processEvents();
}

/**Searches for job in both lists. Returns NULL, if not found.
*/
banking::BankingJob * JobManager::getJobById(int id){

  //scan admin list
  QList<banking::BankingJob *>::iterator itAdminList;
  for (itAdminList = _AdminList.begin(); itAdminList!= _AdminList.end(); itAdminList++){
    BankingJob * job = (*itAdminList);
    if(job->id() == id)
      return job;
  }

  //scan transfer list
  QList<banking::Transfer *>::iterator itTransferList;
  for (itTransferList = _TransferList.begin(); itTransferList!= _TransferList.end(); itTransferList++){
    Transfer * job = (*itTransferList);
    if(job->id() == id)
      return job;
  }

  //not found
  return NULL;
}

/**Clears job lists on bank change and sets the bank to be used for all jobs.
 */
void JobManager::setBank(banking::Bank * bank){
 if(!bank)
   return;
 if(_Bank != bank){ //new bank, lists must be cleared
   cerr << "JobManager::setBank: Clearing job lists due to bank change.\n";
   clearLists();
   _Bank = bank;
 }
}

/**Returns the number of new jobs in the lists
 */
int JobManager::pendingJobs(){
  int adminJobs = 0;
  int transferJobs = pendingTransfers();
  int debitNoteJobs = pendingDebitNotes();

  QList<banking::BankingJob *>::iterator itAdminList;
  for (itAdminList = _AdminList.begin(); itAdminList!= _AdminList.end(); itAdminList++){
    banking::BankingJob * job = (*itAdminList);
    if(job->status() == banking::BankingJob::JobNew)
      adminJobs++;
  }
  return adminJobs + transferJobs + debitNoteJobs;
}

/**Returns number of pending transfers. 
*/
int JobManager::pendingTransfers()
{
  int transferJobs = 0;
  QList<banking::Transfer *>::iterator itTransferList;
  for (itTransferList = _TransferList.begin(); itTransferList!= _TransferList.end(); itTransferList++){
    banking::Transfer * job = (*itTransferList);
    if((job->type() == BankingJob::SingleTransfer) && (job->status() == banking::BankingJob::JobNew))
      transferJobs++;
  }
  return transferJobs;
}

/**Returns number of pending debitnotes. 
*/
int JobManager::pendingDebitNotes()
{
  int debitNoteJobs = 0;
  QList<banking::Transfer *>::iterator itTransferList;
  for (itTransferList = _TransferList.begin(); itTransferList!= _TransferList.end(); itTransferList++){
    banking::Transfer * job = (*itTransferList);
    if((job->type() == BankingJob::DebitNote) && (job->status() == banking::BankingJob::JobNew))
      debitNoteJobs++;
  }
  return debitNoteJobs;
}

/** Redirect output of cout and cerr
*/
void JobManager::sendOutputToLogfile(const QString & logFileName){
   //redirect output to logfile
   _LogFile.open(qPrintable(logFileName), ios_base::out | ios_base::app ); //append data
   _OldCoutSbuf = std::cout.rdbuf(); // save original sbuf
   std::cout.rdbuf(_LogFile.rdbuf()); // redirect 'cout' to logfile
   _OldCerrSbuf = std::cerr.rdbuf(); // save original sbuf
   std::cerr.rdbuf(_LogFile.rdbuf()); // redirect 'cerr' to logfile
}

/** Redirect output of cout and cerr
*/
void JobManager::sendOutputToConsole(){
   //redirect output to stdout and stderr again
   _LogFile.close();
   std::cout.rdbuf(_OldCoutSbuf); // restore the original stream buffer
   std::cerr.rdbuf(_OldCerrSbuf); // restore the original stream buffer
}

/** Sends all jobs to the kernel and executes them.
 *  Returns true on no error.
 *  Throws various exeptions. MUST be called in a try block!!
 */
bool JobManager::executeJobLists(const char * password, const QString & logFileName, const QString & bankDir){
    int listeners = C_EventRegistry::get_number_of_listeners();
   //set a the handler for SIGPIPE
   _BankMessages = tr("MoneyPenny: Starting job queue.") + "\n";
   cerr << "JobManager::executeJobLists: Starting job queue and sending log output to: "\
        << qPrintable(logFileName) << endl;
   sendOutputToLogfile(logFileName);
   //output a separator sequence
   cout << endl <<endl <<"******************Start of next Dialog***********************" << endl <<endl;
   try{
        sigpipeHandlerOn();
        renewApi();
        listeners = C_EventRegistry::get_number_of_listeners();
        if( _SIGPIPE_RECEIVED ) //reset SIGPIPE because it may have been set due to an SSL_shutdown
           _SIGPIPE_RECEIVED = 0;
        if(!_Bank)
          throw ExNoBank();
        if(!_Bank->isValid()) //checks bank and associated user
          throw ExInvalidBank();
        if (! initGvApi(password, qPrintable(bankDir))) {
          throw ExKernelError(tr("Initialization of banking engine failed. Please check your setup."));
        }
        listeners = C_EventRegistry::get_number_of_listeners();
        //now we are ready to send the jobs. Let the kernel thread do this.
        _TaskToPerform = EXECUTE_JOB_LIST;
        start();  
/*        if(jobIsFirstKeyRequest()){ //there is a single FirstKeyRequest job in the list. MUST be sent uncrypted. 
           sendAdminJobs();
        }
        else{ //crypted dialogs need DialogInit and DialogEnd
             sendTransferJobs();
             sendAdminJobs();
         }//end crypted

       _GV_Api->engage_dialog();
       //unset the handler for SIGPIPE 
       signal(SIGPIPE, SIG_DFL);
       cout << "sigPipeHandler off.\n";
       if(_SIGPIPE_RECEIVED)
        throw ExKernelError(tr("Connection to server was broken during data exchange."));*/
    }//end try
    catch(exception & e){
         //listeners = C_EventRegistry::get_number_of_listeners();
         unregisterJobs(); // remove all jobs from the message engine
         //listeners = C_EventRegistry::get_number_of_listeners();
         cerr << "JobManager::executeJobLists: Exception during execution of joblists"<< endl;
         cerr << e.what();
         //redirect output to stdout and stderr again
         sendOutputToConsole();
         renewApi(); //kill the banking engine
         listeners = C_EventRegistry::get_number_of_listeners();
         C_EventRegistry::output_listeners(); //produce a debug output
         //unset the handler for SIGPIPE
         sigpipeHandlerOff();
         throw; //send the exception to the gui for user information
    }

/*   sendOutputToConsole();

  //set final state after a TAN has been sent
  if(_PinTanResponse){
  //Scan TransferList
  QList<banking::Transfer *>::iterator itTransferList;
  for (itTransferList = _TransferList.begin(); itTransferList!= _TransferList.end(); itTransferList++){
    Transfer * job = (*itTransferList);
    int jobId = job->id();
    int pinTanResponseId = _PinTanResponse->get_event_code();
    if( pinTanResponseId == jobId){
      string msg;
      string code;
      switch (job->type()){

        case BankingJob::SingleTransfer:
                  msg = _PinTanResponse->get_HIRMG() + "\n";
                  msg += _PinTanResponse->get_HIRMS_string();
                  code = msg.substr(0,4);
                  job->setMessage( msg );
                  if(code == "0020" || code == "0010")
                    job->setStatus(BankingJob::JobExecuted);
                  else if(code[0] == '9')
                    job->setStatus(BankingJob::JobFailed);
                  else //all other codes, especially 3XXX warnings
                   if((job->status() != BankingJob::JobExecuted ) && ( job->status() != BankingJob::JobFailed)) //Do not overwrite success or error status
                     job->setStatus(BankingJob::JobWarning);
                  break;

         default:cout << "Job: "<< job->id()<<"Unknown type:" <<  job->typeToString(job->type()) << endl;
                 break;

       } //End switch
       emit jobStatusChanged(job);
      }//end if message_code
   }//end for

   //cleanup
   delete _PinTanResponse;
   _PinTanResponse = NULL;
   unregisterJobs(); // remove all jobs from the message engine
  }
  

  if(_ErrorMessageReceived){
    qDebug("JobManager::executeJobLists: Queue executed with the following error messages: \n %s", qPrintable(_BankMessages));
    emit errorMessagesReceived(_BankMessages);
   return false;
  }
  else{
    qDebug("JobManager::executeJobLists: Queue executed without critical error messages");
    _GV_Api->save_keyfile(); //always call save keyfile. It does nothing, if no keyfile is active.
    emit messagesReceived(_BankMessages);
    return true;
  }*/
  return true;
}

/**Returns a list of admin jobs that have JobExecuted _Status.
 * To be used for evaluation of the response data.
 */
banking::BankingJobList JobManager::executedAdminJobs(){
  banking::BankingJobList jobList;
  QList<banking::BankingJob *>::iterator itAdminList;
  for (itAdminList = _AdminList.begin(); itAdminList!= _AdminList.end(); itAdminList++){
    banking::BankingJob * job = (*itAdminList);
    if(job->status() == banking::BankingJob::JobExecuted)
      jobList.push_back(job);
  }
  return jobList;
}

/**Creates a job for getting balances.Returns number of jobs created.
*/
int JobManager::createJobGetBalance(const QString & accountId){
  if(!_Bank) 
   return 0;

  if(!(_Bank->currentUser()))
   return 0;
  int jobs = 0;
  if(accountId == "*"){ //wildcard
    const AccountList  accounts = _Bank->currentUser()->accounts();
    AccountList::const_iterator it;
    for(it= accounts.begin(); it!= accounts.end(); it++){
      BankingJob * job = new BankingJob(BankingJob::GetBalance,
                                        (*it)->accountId(),
                                        (*it)->iban(),
                                         _Bank->bankCode(),
                                         _Bank->bankBic(),
                                         _Bank->bankType());
      addAdminJob(job);
      jobs ++;
    }
    return jobs;
  }
  //account specified
  banking::Account * account = _Bank->currentUser()->accountById(qPrintable(accountId));
  if(account == NULL){
    qDebug() << "JobManager::createJobGetBalance: Error: Unknown account: " << qPrintable(accountId) << endl;
    return 0;
  }

  BankingJob * job = new BankingJob(BankingJob::GetBalance,
                                    account->accountId(),
                                    account->iban(),
                                   _Bank->bankCode(),
                                   _Bank->bankBic(),
                                   _Bank->bankType());
  addAdminJob(job);
  jobs ++;
  return jobs;
}

/**Creates a job for getting transactions. Returns number of jobs created.
*/
int JobManager::createJobGetTransactions(const QDate & fromDate, const QDate & toDate, const QString & accountId){
  if(!_Bank) 
   return 0;
  if(!_Bank->currentUser())
   return 0;
  if(fromDate > toDate){
   cerr << "JobManager::createJobGetTransactions: Error: fromDate > toDate" << endl;
   return 0;
  }
  if(!(fromDate.isValid() && toDate.isValid())){
   cerr << "JobManager::createJobGetTransactions: Error: fromDate or toDate is invalid." << endl;
   return 0;
  }

  int jobs = 0;
  if(accountId == "*"){
    const AccountList  accounts = _Bank->currentUser()->accounts();
    AccountList::const_iterator it;
    for(it= accounts.begin(); it!= accounts.end(); it++){
      BankingJob * job = new BankingJob(BankingJob::GetTransactions,
                                        (*it)->accountId(),
                                        (*it)->iban(),
                                         _Bank->bankCode(),
                                         _Bank->bankBic(),
                                         _Bank->bankType());
      job->setStartDate(DateTime(fromDate.year(), fromDate.month(), fromDate.day()));
      job->setEndDate(DateTime(toDate.year(), toDate.month(), toDate.day()));
      addAdminJob(job);
      jobs ++;
    }
    return jobs;
  }
   banking::Account * account = _Bank->currentUser()->accountById(qPrintable(accountId));
   if(account == NULL){
    cerr << "JobManager::createJobGetTransactions: Error: Unknown account: " << qPrintable(accountId) << endl;
    return 0;
  }

  BankingJob * job = new BankingJob(BankingJob::GetTransactions,
                                      account->accountId(),
                                      account->iban(),
                                      _Bank->bankCode(),
                                      _Bank->bankBic(),
                                     _Bank->bankType());
  job->setStartDate(DateTime(fromDate.year(), fromDate.month(), fromDate.day()));
  job->setEndDate(DateTime(toDate.year(), toDate.month(), toDate.day()));
  addAdminJob(job);
  jobs ++;

  return jobs;
}

 /**Creates a job for getting pending dated transfers. Returns number of jobs created (1 or 0).
 */
 int JobManager::createJobGetDatedTransfers(const QDate & fromDate, const QDate & toDate, const QString & accountId)
{
  if(!_Bank) 
   return 0;
  if(!_Bank->currentUser())
   return 0;
  if(fromDate > toDate){
   cerr << "JobManager::createJobGetDatedTransfers: Error: fromDate > toDate" << endl;
   return 0;
  }
  if(!(fromDate.isValid() && toDate.isValid())){
   cerr << "JobManager::createJobGetDatedTransfers: Error: fromDate or toDate is invalid." << endl;
   return 0;
  }
  if(accountId == "*"){
   cerr << "JobManager::createJobGetDatedTransfers: Error: No wildcards (*) allowed for account id." << endl;
   return 0;
  }

  banking::Account * account = _Bank->currentUser()->accountById(qPrintable(accountId));
  if(account == NULL){
   cerr << "JobManager::createJobGetTransactions: Error: Unknown account: " << qPrintable(accountId) << endl;
   return 0;
  }

  BankingJob * job = new BankingJob(BankingJob::GetDatedTransfers,
                                      account->accountId(),
                                      account->iban(),
                                      _Bank->bankCode(),
                                      _Bank->bankBic(),
                                     _Bank->bankType());
  job->setStartDate(DateTime(fromDate.year(), fromDate.month(), fromDate.day()));
  job->setEndDate(DateTime(toDate.year(), toDate.month(), toDate.day()));
  addAdminJob(job);
  return 1;
}

/**Creates a job to replace public key. Returns number of jobs created.
*/
int JobManager::createJobKeyChange(){
  if(!_Bank)
   return 0;
  if(!(_Bank->currentUser()))
   return 0;
  clearLists(); //There must not be other jobs in the lists.

  BankingJob * job = new BankingJob(BankingJob::KeyChange,
                                       "", //no account
                                       "", //no IBAN
                                     _Bank->bankCode(),
                                     _Bank->bankBic(),
                                     _Bank->bankType());
  addAdminJob(job);

  return 1;
}
/**Creates a job to send public key for the first time. Returns number of jobs created.
*/
int JobManager::createJobFirstKeySend(){
  if(!_Bank)
   return 0;
  if(!(_Bank->currentUser()))
   return 0;
  clearLists(); //There must not be other jobs in the lists.

  BankingJob * job = new BankingJob(BankingJob::FirstKeySend,
                                       "", //no account
                                       "", //no IBAN
                                     _Bank->bankCode(),
                                     _Bank->bankBic(),
                                     _Bank->bankType());
  addAdminJob(job);

  return 1;
}

/**Creates a job to get the public key of the bank. Returns number of jobs created.
   Throws ExTooManyJobs, if AdminList is not empty. 
*/
int JobManager::createJobFirstKeyRequest(){
  if(!_Bank) 
   return 0;
  if(!(_Bank->currentUser()))
   return 0;

  BankingJob * job = new BankingJob(BankingJob::FirstKeyRequest,
                                       "", //no account
                                       "", //no IBAN
                                     _Bank->bankCode(),
                                     _Bank->bankBic(),
                                     _Bank->bankType());
  if(!_AdminList.empty())
    throw ExTooManyJobs();
  addAdminJob(job);

  return 1;
 }

/**Creates a job to get the BPD and UPD from the bank. Returns 1 if ok.
*/
int JobManager::createJobGetUPD(){
  if(!_Bank) 
   return 0;
  if(!(_Bank->currentUser()))
   return 0;
  clearLists();
  BankingJob * job = new BankingJob(BankingJob::GetBPD, //gets BPD and UPD
                                       "", //no account
                                       "", //no IBAN
                                     _Bank->bankCode(),
                                     _Bank->bankBic(),
                                     _Bank->bankType());
  addAdminJob(job);
  return 1;
}


/**Creates a job to get the SystemId from the bank. 
*  Use this function for a NEW key. Returns number of jobs created.
*/
int JobManager::createJobGetNewSystemId(){
  if(!_Bank) 
   return 0;
  if(!(_Bank->currentUser()))
   return 0;
  BankingJob * job = new BankingJob(BankingJob::GetNewSystemId,
                            "", //no account
                            "", //no IBAN
                            _Bank->bankCode(),
                            _Bank->bankBic(),
                            _Bank->bankType());
  addAdminJob(job);
  return 1;
}

/**Creates a job to get the SystemId and Sequence number (Signature Id)from the bank. 
*  Use this function for an ALREADY ACTIVE key. Returns number of jobs created.
*/
int JobManager::createJobResynchronize(){
  if(!_Bank) 
   return 0;
  if(!(_Bank->currentUser()))
   return 0;
  clearLists();
  BankingJob * job = new BankingJob(BankingJob::Resynchronize,
                            "", //no account
                            "", //no IBAN
                            _Bank->bankCode(),
                            _Bank->bankBic(),
                            _Bank->bankType());
  addAdminJob(job);
  return 1;
}

/** Create a single Transfer.  Returns number of transfers in list.
 */
int JobManager::createJobTransfer (banking::Transfer * transfer){
  if(!_Bank) 
   return 0;
  if(!_Bank->currentUser())
   return 0;
  return addTransfer(transfer);
}

/*!
    \fn JobManager::sendAdminJobs()
    Send all jobs in admin list.
 */
void JobManager::sendAdminJobs()
{
    int row = 0;
    QList<banking::BankingJob *>::iterator itAdminList;
    for (itAdminList = _AdminList.begin(); itAdminList!= _AdminList.end(); itAdminList++){
      banking::BankingJob * job = (*itAdminList);
      if(job->addToQueue(_GV_Api, this))
        emit sentAdminJob(row, job);
      row ++;
    } 
}


/*!
    \fn JobManager::sendTransferJobs()
    Send all jobs in transfer list.
 */
void JobManager::sendTransferJobs()
{
     int row = 0;
     QList<banking::Transfer *>::iterator itTransferList;
     for (itTransferList = _TransferList.begin(); itTransferList!= _TransferList.end(); itTransferList++){
       banking::Transfer * job = (*itTransferList);
       if(job->addToQueue(_GV_Api, this))
          emit sentTransferJob(row, job);
       row ++;
     }
}

/*!
    \fn JobManager::jobIsUncrypted()
    Scans Admin job list for FirstKeyRequest. Throws ExTooManyJobs, if a new FirstKeyRequest job is in list
    and it is not the only FirstKeyRequest job.
 */
bool JobManager::jobIsFirstKeyRequest()
{
    bool result = false;
    QList<banking::BankingJob *>::iterator itAdminList;
    for (itAdminList = _AdminList.begin(); itAdminList!= _AdminList.end(); itAdminList++){
      if(result)//more than one job in uncrypted state is illegal
        throw (ExTooManyJobs());
      banking::BankingJob * job = (*itAdminList);
      if((job->type() == banking::BankingJob::FirstKeyRequest) 
      && (job->status() == banking::BankingJob::JobNew))
        result = true;
    }
    return result;
}


/*!
    Returns the bank's fingerprint from the kernel.
    This is a RipeMD160 hash over exponent and modulus chained together and padded. 
 */
QString JobManager::bankFingerprint()
{
  string hash= _GV_Api->get_keydata("hash_bank");
  return QString(hash.c_str());
}

/**Saves the present key data. This is required after getting the bank's public keys.
 */
bool JobManager::saveKeyfile(const QString & filename){
 return _GV_Api->save_keyfile(qPrintable(filename));
}

/** Updates user, account and bank data from fresh BPD and UPD data received from the bank.
 */
bool JobManager::updateUPDandBPD(C_BPD * bpd){
  if(!bpd)
    return false;
  if(!_Bank) 
   return false;
  banking::User * user = _Bank->currentUser();
  if(!user)
   return false;
  if(user->userId() != bpd->get_user_id()){ //this may happen, when a new key is activated. The BPD userid is the correct one.
     if(bpd->get_user_id().empty()){
       cerr << "[ERROR] No Userid in BPD." << endl;
       return false;
     }
     cerr << "[INFO] UPD user-id does not match previous userid." << endl;
     cerr << "[INFO] Updating userid using new userid from BPD." << endl;
     user->setUserId(bpd->get_user_id()); //use the new id
     _Bank->setCurrentUserId(user->userId());
  }

  if(_Bank->bankName().empty()) //do not overwrite existing names automatically.
    _Bank->setBankName(bpd->get_bank_name());
  banking::AccountList newAccounts = bpd->get_accounts();
  banking::AccountList oldAccounts = user->accounts();
  banking::AccountList::const_iterator itNew;
  banking::AccountList::const_iterator itOld;
  bool accountExists = false;
  for (itNew = newAccounts.begin(); itNew != newAccounts.end(); itNew++){
     accountExists = false;
     banking::Account * newAccount = (*itNew);
     for (itOld = oldAccounts.begin(); itOld != oldAccounts.end(); itOld++){
        banking::Account * oldAccount = (*itOld);
        if(newAccount->accountId() == oldAccount->accountId())
          accountExists = true;
     } //end old accounts loop
     if(!accountExists){
       banking::Account * account = new banking::Account(*newAccount); //make a copy
       user->addAccount(account);
     }
  }//end newAccounts loop
  return true;
}


/** Calls GV_Api to create a new RDH2 keypair. Will connect to bank to 
 *  get the public keys. Sends signal confirmBankKeyHash(string * hash).
 *  hash must be set to "OK", if user confirms correctness of bank hash.
 *  May throw exceptions.
 */
bool JobManager::createNewKeyfile( const QString & rdhVersion,
                               int passportType,
                               C_uarray& entropy,
                               const QString & instCode, 
                               const QString & country, 
                               const QString & server,
                               const QString & userId, 
                               const QString & password, 
                               const QString & keyNumber, 
                               const QString & fileName,
                               const QString & logFileName){
    try{
            _LogFileName = logFileName;
            C_Settings * settings = &(C_Settings::getInstance()); //do not delete! Api takes ownership.
            settings->set_hbciversion( FINTS3 );
            settings->set_entropy(entropy);
            settings->set_bankcode(qPrintable(instCode));
            settings->set_bankcountry(qPrintable(country));
            settings->set_userid(qPrintable(userId));
            settings->set_server(qPrintable(server));
            settings->set_passporttype(passportType );
            settings->set_keyfilefilename(qPrintable(fileName));
            settings->set_keyfilepassword(qPrintable(password));
            string keyNumberString = qPrintable(keyNumber);
            settings->set_keynumber(keyNumberString);
            settings->set_secprofilemode(string("RDH"));
            settings->set_secprofileversion(qPrintable(rdhVersion));
            settings->set_xmldir(qPrintable(_XmlDir));
            settings->set_systemid( string("0") );

            _TaskToPerform = CREATE_KEYFILE;
            qDebug()<< "Starting HBCI_Cern thread...";
            //run();
            start();
    } 
    catch(...){
      renewApi(); //free all api resources
      return false;
    }
    return true;
}

  /** Start the HBCI-engine to create a new keyfile. createNewKeyfile MUST be run immediately
   * prior to this function. DO NOT call directly. Is called by run().
   */
 void JobManager::runCreateNewKeyfile()
{
   qDebug() << "JobManager::runCreateNewKeyfile() started.";
   sendOutputToLogfile(_LogFileName);
   renewApi();
   cout <<"****************Creating and sending a new key. (FirstKeySend)***********\n";
   sleep(1);
   _GV_Api->createRDHKeyfile();
   sendOutputToConsole();
}


/** This method generates the customers ini-letter
*/
QString JobManager::createIniLetter(const QString & password, const QString & customerName, const QString & bankDir){
  renewApi();
  if(!_Bank)
    throw ExNoBank();
  if(!_Bank->isValid()) //checks bank and associated user
    throw ExInvalidBank();
  banking::User * user = _Bank->currentUser();
  if (!user)
    throw ExNoUser();

  // now get the hashes
  initGvApi(qPrintable(password), qPrintable(bankDir));
  QString indent = "    ";
  QString paragraphIndent = "        ";
  string linePrefix = qPrintable(paragraphIndent);
  int blocklength = 10; //groups per block
  int blocksPerLine = 1; //blocks per line
  QString captionIndent = "    ";
  string bankHash = _GV_Api->get_keydata("hash_bank", linePrefix, blocklength, blocksPerLine);
  string userHash = _GV_Api->get_keydata("hash_user", linePrefix, blocklength, blocksPerLine);
  blocklength = 20;
  string userModulus = _GV_Api->get_keydata("modulus_user", linePrefix, blocklength, blocksPerLine);
  string keyNumber = _GV_Api->get_keydata("key_number", " ", blocklength, blocksPerLine);

  //create the letter text
  QString sysid = "MoneyPenny " + QString(VERSION);
  QDateTime date = QDateTime(QDateTime::currentDateTime());
  QString dateFormat = tr("yyyy/ddd/MMMM/d", "Date format for printer output");
  QString timeFormat = tr("hh:mm:ss ap", "Time format for printer output");
  string userid = _GV_Api->get_bankdata("userid");

  QString result;
  QString header = tr("INI-Letter generated by") + " " + sysid;
  QString underline;
  for(int pos = 0; pos < header.length(); pos ++)
    underline += "=";
  result=  header + "\n" + underline + "\n\n";
  result += tr("Customername         :")+ " ";
  if(user->userName().empty() && customerName.isEmpty())
     result += "..............................\n";
  else
  { 
    if(!customerName.isEmpty())
      result += customerName + "\n";
    else result += QString(user->userName().c_str()) + "\n";
  }
  result += tr("Bank                 :") + " " + _Bank->bankName().c_str() +"\n";
  result += tr("User Id              :") + " " + userid.c_str() + "\n";
  result += tr("Print Date           :") + " " + date.toString(dateFormat) + "\n";
  result += tr("Time                 :") + " " + date.toString(timeFormat) + "\n";
  result += tr("Key number           :") + " " + keyNumber.c_str() + "\n";
  //result+= tr("Key version          :") + " " + " 1\n";
  result += tr("Customer system      :") + " " + sysid + "\n";
  result += "\n";

  QString caption = tr("My public key for electronic signature");
  QString info = tr("Additional Information:");
  underline = "";
  for(int pos = 0; pos < caption.length(); pos ++)
    underline += "=";
//  result += "\n\n";
  result += captionIndent;
  result += caption;
  result += '\n';
  result += captionIndent;
  result += underline;
  result += '\n';
  QString starLine = linePrefix.c_str();
  int nStars = blocksPerLine * blocklength + 2 * captionIndent.length() + 3;
  for(int i = 0; i < nStars + 4; i++)
    starLine += '*';
  result += '\n';
  result += captionIndent;
  result += tr("Hash value:");
  result += '\n';
  result += starLine;
  result += '\n';
  if(userHash.empty())
    result += tr("Error: No user key available");
  else
  {
   QStringList lines = QString(userHash.c_str()).split('\n');
   for(int nLine = 0; nLine < lines.count(); nLine ++)
   {
	QString line = QString(linePrefix.c_str())+ "*  "+ lines[nLine].trimmed();
    for(int pos = line.length(); pos < starLine.length()-1; pos++) //add blanks to get proper length
    	line += " ";
    line+= "*";
    result += line;
    result +="\n";
   }
  }
  result += starLine;
  result += "\n\n";

  result += captionIndent;
  result += info;
  result += '\n';
  result += captionIndent;
  underline = "";
  for(int pos = 0; pos < info.length(); pos ++)
    underline += "=";
  result += underline;
  result += "\n\n";
  result += captionIndent;
  result += tr("Exponent: ");
  result += '\n';
  result += paragraphIndent;
  result += ("65537 = 01 00 00 00 01");

  result += "\n";
  result += captionIndent;
  result += tr("Modulus:");
  result += '\n';
  if(userModulus.empty())
    result += tr("Error: No user key available");
  else result+= userModulus.c_str();

  result += "\n";
  result += tr("I confirm that my computer uses the above key for my electronic \nsignature.");
  result += "\n";
  result += tr("I furthermore confirm that my computer received a public key from the bank\nwith the following hash value:");
  result += "\n\n";
  if(bankHash.empty())
     result += tr("Error: No bank key available");
  else result += bankHash.c_str();
  result += "\n\n\n";

  result += tr("_________________________  ____________________________\n"
             "Location, date                            Signature\n");




  return result;
}

/** Initializes the GV_Api with bank and user data.
 */
bool JobManager::initGvApi(const char * password, const char * bankDir){
        _PasswordInvalid = false; //reset any old error flag
        if(!_Bank)
          return false;
        banking::User * user = _Bank->currentUser();
        if (!user)
          return false;
       //we need non const pointers to the strings for the GV_Api
        char pw[64];
        strncpy (pw, password, 64);
        char fileName[256];
        if(user->passportType() & PASSPORT_CHIPCARD) //reader definition file
          strncpy (fileName, qPrintable(_ReaderFile), 256);
        else strncpy (fileName, user->passportFile().c_str(), 256); //keyfile
        char server[256];
        strncpy (server, _Bank->server().c_str(), 256);

         //For PIN-TAN systemId is "0" for a new bank, userId is an arbitrary account number of the user
        if(user->passportType() == PASSPORT_SECURITY_OTP)
           _GV_Api->setPinTanData(_Bank->bankCode(), user->userId(), _Bank->systemId()); 
        C_Settings * settings = &(C_Settings::getInstance());
        settings->set_bpd(_Bank->bpd());
        settings->set_bpddir(bankDir);
        settings->set_hbciversion(_Bank->hbciVersion());
        settings->set_secprofileversion(_Bank->rdhVersion());
        settings->set_passporttype( user->passportType()); 
        settings->set_banktype(_Bank->bankType());
        if(user->passportType() & PASSPORT_CHIPCARD)
         {
           settings->set_keyfilefilename("");
           settings->set_keyfilepassword("");
           settings->set_checkCard(true); //enable card checking, we have to have a valid card here
          //TODO Add RDH modes
           settings->set_secprofilemode("DDV");    //   SecProfileMode,
         }
        else 
        {
          settings->set_keyfilefilename(fileName);
          settings->set_keyfilepassword(password);
          settings->set_secprofilemode("RDH");    //   SecProfileMode,
        }
        settings->set_server(server);
        settings->set_xmldir(qPrintable(_XmlDir));
        settings->set_readersxmlfilename(fileName);
        settings->set_cardreadermessages(NULL); //TranslationMap *  CardReaderMessages, 
        if(user->currentAccount())
          settings->set_account(user->currentAccount()->accountId());
        else  //do not use empty strings here, dummies are needed for getBankKeys
          settings->set_account("0000000");
        settings->set_owner(user->userName());
        settings->set_bankcode(_Bank->bankCode());
        settings->set_bankcountry(_Bank->country());
        settings->set_bankbic(_Bank->bankBic());
        settings->set_userid(user->userId());
        settings->set_systemid(_Bank->systemId());
        if(!_SecProfileVersion.isEmpty())
          settings->set_secprofileversion(qPrintable(_SecProfileVersion));
        settings->set_certdir(qPrintable( MP_DIR() + MONEYPENNY_CERTIFICATE_DIR + "/"));
        if(user->passportType() & PASSPORT_SECURITY_OTP_MTAN)
          settings->set_pintanmode(PINTAN_MODE_MTAN);
        else if(user->passportType() & PASSPORT_SECURITY_OTP_ITAN)
          settings->set_pintanmode(PINTAN_MODE_ITAN);
        else settings->set_pintanmode(PINTAN_MODE_SINGLE_STEP);

        _GV_Api->init_api(settings);
        return true; 
}

/** Reads the data from the chipcard. There MAY be userId, bankid, server. Returns the number of valid user data sets.
*   If no valid set is found, strings in set 1 (the first record) are used. Else the data of the last valid set are used.
*/
int JobManager::readDDVCardData(int & country, QString & bankId, QString & userId, QString & serverUrl, QString & readerName){
  _MultiThreaded = false;
  renewApi();

  //Settings suitable for reading a card
  C_Settings * settings = &(C_Settings::getInstance());  
  settings->set_passporttype( PASSPORT_CHIPCARD | PASSPORT_SECURITY_DDV);
  settings->set_checkCard(true); //enable card checking, we have to have a valid card here
  settings->set_xmldir(qPrintable(_XmlDir));
  settings->set_hbciversion(HBCI220); //start with lowest version
  settings->set_readersxmlfilename(qPrintable(_ReaderFile));
  settings->set_cardreadermessages(NULL); //TranslationMap *  CardReaderMessages, 
  settings->set_bpdRequired(false); //switch off automatic BPD reading
  _GV_Api->init_api();
  country = 280; //TODO : get data from card
  bankId = _GV_Api->get_bankdata("bankid").c_str();
  userId = _GV_Api->get_bankdata("userid").c_str();
  serverUrl = _GV_Api->get_bankdata("url").c_str();
  readerName = _GV_Api->get_bankdata("readername").c_str();
  settings->set_bpdRequired(true); //switch it on again!
  return 1;
}
/** Writes the data to record recordNumber on the chipcard. 
  * Will ask for PIN via signal inputPinViaKeyboardRequest or inputPinViaKeypadRequest.
  * There MUST be userid, bankid, server.
  * Returns true on success.
  */
bool JobManager::writeDDVCardData(int recordNumber, const QString & bankName, const QString & bankId, 
const QString & userId, const QString & serverUrl)
{
  qDebug("JobManager::writeDDVCardData");
   _TaskToPerform = WRITE_DDV_CARD_DATA;
  try{
       renewApi();
       _BankMessages = tr("MoneyPenny: Starting data writing.") + "\n";
      //build an API with default values for a DDV passport
        C_Settings * settings = &(C_Settings::getInstance()); //do not delete!
        settings->set_checkCard(false); //disable card checking, we have no valid card here, we want to make one!
        settings->set_bpdRequired(false); //do not try to get BPD, we just want to write to the card
       _GV_Api->init_api();
     
       //Type casting to std::string
       string sBankName = qPrintable(bankName);
       string sBankId = qPrintable(bankId);
       string sServerUrl = qPrintable(serverUrl);
       string sUserId = qPrintable(userId);

       settings->set_bankname(sBankName);
       settings->set_bankcode(sBankId);
       settings->set_bankcountry("280");
       settings->set_userid(sUserId);
       settings->set_server(sServerUrl);
       settings->set_passporttype( PASSPORT_CHIPCARD | PASSPORT_SECURITY_DDV );
       settings->set_secprofilemode(string("DDV"));
       settings->set_recordnumber( 1 );

       qDebug("JobManager::writeDDVCardData: Api initialized. Calling GV_Api now.");
       _MultiThreaded = true;
    } 
    catch(...){
      renewApi(); //free all api resources
      return false;
    }
    start(); //start thread
    return true;
}

/** Excecutes the joblists. executeJobLists MUST be run immediately
  * prior to this function. DO NOT call directly. Is called by run().
  */
void JobManager::runExecuteJobLists()
{
   if(jobIsFirstKeyRequest()){ //there is a single FirstKeyRequest job in the list. MUST be sent uncrypted. 
      sendAdminJobs();
   }
   else{ //crypted dialogs need DialogInit and DialogEnd
        sendTransferJobs();
        sendAdminJobs();
    }//end crypted

  _GV_Api->engage_dialog();
  //unset the handler for SIGPIPE 
  sigpipeHandlerOff();

  //results should be available now
   sendOutputToConsole(); //stop file logging

  //set final state after a TAN has been sent
  if(_PinTanResponse){
  //Scan TransferList
  QList<banking::Transfer *>::iterator itTransferList;
  for (itTransferList = _TransferList.begin(); itTransferList!= _TransferList.end(); itTransferList++){
    Transfer * job = (*itTransferList);
    int jobId = job->id();
    int pinTanResponseId = _PinTanResponse->get_event_code();
    if( pinTanResponseId == jobId){
      string msg;
      string code;
      switch (job->type()){

        case BankingJob::SingleTransfer:
                  msg = _PinTanResponse->get_HIRMG() + "\n";
                  msg += _PinTanResponse->get_HIRMS_string();
                  code = msg.substr(0,4);
                  job->setMessage( msg );
                  if(code == "0020" || code == "0010")
                    job->setStatus(BankingJob::JobExecuted);
                  else if(code[0] == '9')
                    job->setStatus(BankingJob::JobFailed);
                  else //all other codes, especially 3XXX warnings
                   if((job->status() != BankingJob::JobExecuted ) && ( job->status() != BankingJob::JobFailed)) //Do not overwrite success or error status
                     job->setStatus(BankingJob::JobWarning);
                  break;

         default:cout << "Job: "<< job->id()<<"Unknown type:" <<  job->typeToString(job->type()) << endl;
                 break;

       } //End switch
       emit jobStatusChanged(job);
      }//end if message_code
   }//end for

   //cleanup
   delete _PinTanResponse;
   _PinTanResponse = NULL;
   unregisterJobs(); // remove all jobs from the message engine
  }
  
   emit adminJobsToAnalyze();
   //waitForGui("Analyze admin jobs"); //wait for the list to be analyzed

  if(_ErrorMessageReceived){
    qDebug("JobManager::executeJobLists: Queue executed with the following error messages: \n %s", qPrintable(_BankMessages));
    emit errorMessagesReceived(_BankMessages);
//   return false;
  }
  else{
    qDebug("JobManager::executeJobLists: Queue executed without critical error messages");
    _GV_Api->save_keyfile(); //always call save keyfile. It does nothing, if no keyfile is active.
    emit messagesReceived(_BankMessages);
   // return true;
  } 
}

/** Reads data from a DDV card. readDdvCardData MUST be run immediately
  * prior to this function. DO NOT call directly. Is called by run().
  */
void JobManager::runReadDdvCardData()
{
  C_Settings * settings = &(C_Settings::getInstance());  
  _GV_Api->init_api();
  int country = 280; //TODO : get data from card
  QString bankId = _GV_Api->get_bankdata("bankid").c_str();
  QString userId = _GV_Api->get_bankdata("userid").c_str();
  QString serverUrl = _GV_Api->get_bankdata("url").c_str();
  QString readerName = _GV_Api->get_bankdata("readername").c_str();
  settings->set_bpdRequired(true); //switch on bpd reading again!
  emit cardDataRead(country, bankId, userId, serverUrl, readerName);
}

/** Writes data to a DDV card. writeDdvCardData MUST be run immediately
  * prior to this function. DO NOT call directly. Is called by run().
  */
void JobManager::runWriteDdvCardData()
{
    emit sendingMessage(tr("Writing data to chipcard."));
   _GV_Api->writeBankdataToDDVCard();
    emit sendingMessage(tr("Data were written to card."));
}

 /**This function runs in the subthread. It runs the GV_API functions. When an exception is caught, 
   * the _ErrorMessagesReceived flag is set, the exception is sent to the GUI via signal caughtException 
   * and the kernel thread is terminated. The kernelThreadFinished signal will then send a "false" value 
   * to the GUI and hopefully stop further processing there.
   */
void JobManager::run()
{ 
 _MultiThreaded = true;
  _WaitFlagMutex.unlock();
  setWaitFlag(false);
  qDebug()<< "JobManager::run() started.";
  try{
    QMutexLocker locker(&_KernelMutex); //Lock the mutex
    switch(_TaskToPerform){
       case CREATE_KEYFILE: runCreateNewKeyfile();break;
       case EXECUTE_JOB_LIST: runExecuteJobLists(); break;
       case READ_DDV_CARD_DATA: runReadDdvCardData(); break;
       case WRITE_DDV_CARD_DATA: runWriteDdvCardData(); break;
       default: break;
    }
  }
  catch(ExChipcardError & e)
  {
   qDebug() << "JobManager::run() caught an ExChipcardError.  what(): " << e.what();
   emit caughtException(e);
   renewApi(); //kill the banking engine
   sigpipeHandlerOff();
  }

  catch(ExHbciError & e)
  {
   qDebug() << "JobManager::run() caught an ExHbciError.  what(): " << e.what();
   _ErrorMessageReceived = true;
   _BankMessages += tr("Exception:") + " " + e.what() + "\n";
   _BankMessages += tr("Banking job execution has been cancelled.");
   emit caughtException(e);
   //listeners = C_EventRegistry::get_number_of_listeners();
   unregisterJobs(); // remove all jobs from the message engine
   //listeners = C_EventRegistry::get_number_of_listeners();
   cerr << "JobManager::run: Exception during execution of jobs"<< endl;
   cerr << e.what();
   //redirect output to stdout and stderr again
   sendOutputToConsole();
   renewApi(); //kill the banking engine
   int listeners = C_EventRegistry::get_number_of_listeners();
   cerr << "JobManager::run: " << listeners <<" listeners registered after exception.\n";
   C_EventRegistry::output_listeners(); //produce a debug output
   //unset the handler for SIGPIPE
   sigpipeHandlerOff();
  }

  catch(...) //catch everything here, or the program will crash
  {
   qDebug() << "JobManager::run() caught an unknown exeption.";
   _ErrorMessageReceived = true;
   _BankMessages += tr("Exception:") + " " + 
     tr("Jobmanager::run() caught an unexpected exception. Terminating the program is recommended.");
   emit caughtException(  ExKernelError(qPrintable("JobManager::run() "+tr("Unknown Reason."))));
   sendOutputToConsole();
   renewApi(); //kill the banking engine
   int listeners = C_EventRegistry::get_number_of_listeners();
   cerr << "JobManager::run: " << listeners <<" listeners registered after exception.\n";
   C_EventRegistry::output_listeners(); //produce a debug output
   //unset the handler for SIGPIPE
   sigpipeHandlerOff();
  }
  _MultiThreaded = false;
}

/** Wake up thread. To be called when kernel is waiting for input.
*/
void JobManager::wakeKernelThread()
{
 #ifdef DEBUG_JOBMANAGER
   cout << "JobManager::Waking kernel thread\n";
 #endif
  _LogFile.flush();
/* while(!_WaitFlag) //wait until thread is really waiting
 { 
   cout << "Kernel is still busy. Waiting for kernel to wait."; 
  _LogFile.flush();
   msleep(10);
   setWaitFlag(true);
 }
 cout << "\n";
*/
 msleep(10); //wait 10 ms at least
 _WaitCondition.wakeAll();
 setWaitFlag(false);
}

/** Uses _WaitFlagMutex to protect the _WaitFlag flag.
  * @param value new value for _WaitFlag flag
  */
void JobManager::setWaitFlag(bool value)
{
   _WaitFlagMutex.lock();
   _WaitFlag = value;
   _WaitFlagMutex.unlock();
}

/** Let kernel thread sleep. To be called when user (Gui) input is required.
*/
void JobManager::waitForGui(string reason)
{
  if(! _MultiThreaded) //do not wait in single thread mode
    return;

  #ifdef DEBUG_JOBMANAGER
  cout << "JobManager:: Let kernel thread wait. Reason:" << reason << "\n";
  #endif

  _LogFile.flush();
  if(!_WaitFlag)
  {
    setWaitFlag (true);
    _WaitCondition.wait(&_KernelMutex);
  }
  #ifdef DEBUG_JOBMANAGER
  cout << "JobManager:: Kernel is waiting now.\n";
  #endif
}

/** Called when subthread is started. Emits kernelThreadStarted.
 */
 void JobManager::slotThreadStarted()
{
    qDebug("JobManager::slotThreadStarted called"); 
    emit kernelThreadStarted();
}
 /** Called when subthread is complete and run() returned.
 */
void JobManager::slotThreadFinished()
{
  if(_ErrorMessageReceived){
    qDebug("JobManager::slotThreadFinished: Queue executed with the following error messages: \n %s", qPrintable(_BankMessages));
    emit errorMessagesReceived(_BankMessages);
  }
  else{
    qDebug("JobManager:slotThreadFinished: Queue executed without error messages:\n%s", qPrintable(_BankMessages));
    emit messagesReceived(tr("Send public key: OK") + "\n\n" + _BankMessages);
  }
  emit kernelThreadFinished(!_ErrorMessageReceived);
}

/** Sets the cancel flag in GV_Api.
 */
void JobManager::cancelKernelAction()
{
 QMutexLocker locker(&_KernelMutex);
 if(_GV_Api)
  _GV_Api->cancel_dialog();
}

/** Sets a SIGPIPE handler. Must be set to avoid chrashing due to network failure.
*/
void JobManager::sigpipeHandlerOn()
{
   _SIGPIPE_RECEIVED = 0; //clear any old errors
   signal(SIGPIPE, sigPipeHandler);
   cout << "sigPipeHandler activated.\n";
}

/** Removes SIGPIPE handler. Throws an exception, if a SIGPIPE was caught since last
* handler activation.
*/
void JobManager::sigpipeHandlerOff()
{
  signal(SIGPIPE, SIG_DFL);
  cout << "sigPipeHandler off.\n";  //unset the handler for SIGPIPE 
  signal(SIGPIPE, SIG_DFL);

  if(_SIGPIPE_RECEIVED)
   throw ExKernelError(tr("Connection to server was broken during data exchange."));
}

