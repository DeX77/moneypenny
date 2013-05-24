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
#ifndef JOBMANAGER_H
#define JOBMANAGER_H

#include <QList>
#include <QObject>
#include <QMap>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>


#include <fstream>
#include <string>
class QDate;
class QString;

#include "kernel/helpers/transfer.h"
#include "kernel/helpers/bank.h"
class C_GV_API;
class C_uarray;

#include "kernel/registry/C_Listener.h"

/**
This class owns the lists for pending transfer- and adminjobs. 
Banking communication is carried out with the HBCI-kernel object _GV_Api.
Presently, all pending jobs are deleted on shutdown. Use GuiTransferForm for editing and GuiJobs for listing.
Prior to adding jobs, the Bank property MUST be set, otherwise no jobs will be accepted.
CreateJobXXX methods accept a * as wildcard for all accounts.
Translates the kernel callbacks and exceptions into Qt-Signals.
Runs the banking engine _GV_Api in a thread of its own. The run() method and everything it calls directly,
runs in the sub-thread (kernel-thread). This includes the callback function handleMessage(), 
which emits most of the signals for the GUI.run() locks _KernelMutex. 
All other methods run in the main (GUI) thread.
The _MultiThreaded flag is used to indicate single or multiple threading.
While the kernel thread is running, it emits various signals from the handleMessage and runXXX functions.
These signals are connected to the hobit engine (see hobit.cpp).
The signal kernelThreadFinished is emitted, when thread is finished. 
Do NOT call clearLists() before a kernelThreadFinished signal was received.
Use _Waitcondition.wait(&_KernelMutex) from inside this class (Jobmanager) to wait for GUI input.
When input is complete, call wakeKernelThread() from a GUI class to wake up the thread waiting for this input.
If the GUI is just notified and no input or feedback is required, waiting is not neccessary.
	@author Walter Roth <walter@amd2600-roth>
*/
class JobManager: public QThread, public C_Listener{
 Q_OBJECT
private:
  /** Registers all types of exceptions that may occur and 
    *shall be sent through signal/slot mechanism to the GUI thread. Call only once.
    */
  static void registerExceptionMetatypes();
public:
    JobManager();
    ~JobManager();

    /** Returns a translated version of the jobType. Used to add a user readable
      * description of the jobs to the _BankMessages.
      */
    QString trJobDescription(int jobType);

    /** Returns the translated kernel message.
      */
    QString trKernelMessage(const std::string & msg);

    /**Adds transfer at end of list. Returns numberof transfers in list on success.
     */ 
    int addTransfer(banking::Transfer * transfer);
   /**Returns transfer at index and removes it form the list.
    * May return NULL.
    */
   banking::Transfer * takeTransferAt(int index);
   /**Deletes transfer at index.
    */
    void deleteTransfer(int index);
   /**Returns a pointer to the pending transfer at index. Returns NULL if index is out of range.
   */
    const banking::Transfer * pendingTransfer(int index);
   /**Adds adminjob at end of list.Returns true on success.
   */
   bool addAdminJob(banking::BankingJob * job);
   /**Deletes admin job at index
   */
   void deleteAdminJob(int index);
   /**Returns a pointer to the adminjob at index. Returns NULL if index is out of range.
   */
   const banking::BankingJob * getAdminJob(int index);
   /**Deletes all jobs from both lists.
    */
   void clearLists();
   /**Resets all job status values to JobNew, if status is JobQueued
    */
   void resetLists();
   /** Sends all jobs to the kernel and executes them.
    *  Sends kernel output to logFileName.
    *  Returns true on no error.
    *  Throws various exeptions. MUST be called in a try block!!
    */
   bool executeJobLists(const char * password, const QString & logFileName, const QString & bankDir);
   /** Sets the cancel flag in GV_Api.
    */
   void cancelKernelAction();
   /**Returns a list of admin jobs that have JobExecuted _Status.
    * To be used for evaluation of the response data.
    */
   banking::BankingJobList executedAdminJobs();
   /**
   * Method to handle an Event. For description see 
   * class C_Listener and EventRegistry Documentation
   */
   void handle_message(int message_code,
                         string message,
                         string class_name,
                         string method_name,
                         int level,
                         void * possible_data = 0);
   /**Clears job lists on bank change and sets the bank to be used for all jobs.
    */
    void setBank(banking::Bank * bank);
   /** Set the security profile version for new bank connections. 
     * DO NOT call this function for existing connections. 
    */
    void setSecProfileVersion(const QString & newVal){_SecProfileVersion = newVal;};
   /**Returns the number of new jobs in the lists
    */
    int pendingJobs();

   /**Returns number of pending transfers. 
   */
   int pendingTransfers();

   /**Returns number of pending debitnotes. 
   */
   int pendingDebitNotes();

   /**Creates a job for getting balances. Returns number of jobs created.
   */
   int createJobGetBalance(const QString & accountId);
   /**Creates a job for getting transactions. Returns number of jobs created.
   */
   int createJobGetTransactions(const QDate & fromDate, const QDate & toDate, const QString & accountId);
   /**Creates a job for getting pending dated transfers. Returns number of jobs created (1 or 0).
   */
   int createJobGetDatedTransfers(const QDate & fromDate, const QDate & toDate, const QString & accountId);
   /**Creates a job to send public key for the first time. Returns number of jobs created.
   */
   int createJobFirstKeySend();
   /**Creates a job to replace public key. Returns number of jobs created.
   */
   int createJobKeyChange();
   /**Creates a job to get the public key of the bank. Returns number of jobs created.
   */
   int createJobFirstKeyRequest();
   /**Creates a job to get the SystemId from the bank. 
   *  Use this function for a NEW key. Returns number of jobs created.
   */
   int createJobGetNewSystemId();
   /**Creates a job to get the SystemId and Sequence number (Signature Id)from the bank. 
   *  Use this function for an ALREADY ACTIVE key. Returns number of jobs created.
   */
   int createJobResynchronize();
   /**Creates a job to get the BPD and UPD from the bank. Returns 1 if ok.
   */
   int createJobGetUPD();
   /** Create a single Transfer.  Returns number of transfers in list.
    */
   int createJobTransfer (banking::Transfer * transfer);
   /**Returns error messages. May return an empty string
   */
   const QString & bankMessages(){return _BankMessages;};
   /** Returns the bank's fingerprint from the kernel.
    *  This is a RipeMD160 hash over exponent and modulus chained together and padded. 
    */
   QString bankFingerprint();

   /**Saves the present key data. This is required for a new key and after getting the bank's public keys.
    */
   bool saveKeyfile(const QString & filename);
   /** Calls passport to create a new keypair.
    */
   bool createNewKeyfile( const QString & rdhVersion,
                      int passportType,
                      C_uarray& entropy, 
                      const QString & instCode, 
                      const QString & country, 
                      const QString & server,
                      const QString & userId, 
                      const QString & password, 
                      const QString & keyNumber, 
                      const QString & fileName,
                      const QString & logFileName);

   /** This method generates the customer's ini-letter
   */
   QString createIniLetter(const QString & password, const QString & customerName, const QString & bankDir);

   /** Reads the data from the chipcard. There MAY be userId, bankid, server. 
     * Returns the number of valid user data sets.
     * If no valid set is found, strings in set 1 (the first record) are used. 
     * Else the data of the last valid set are used.
     * Throws various exeptions. MUST be called in a try block!!
     */
   int readDDVCardData(int & country, QString & bankId, QString & userId, QString & serverUrl, QString & readerName);

   /** Writes the data to record recordNumber on the chipcard. 
    * Will ask for PIN via signal inputPinViaKeyboardRequest or inputPinViaKeypadRequest.
    * There MUST be userid, bankid, server.
    * Returns true on success.
    */
   bool writeDDVCardData(int recordNumber, const QString & bankName, const QString & bankId, 
                     const QString & userId, const QString & serverUrl);

   /** Wake up kernel thread. To be called when kernel is waiting for input.
   */
   void wakeKernelThread();

protected:
    bool jobIsFirstKeyRequest();
    void sendTransferJobs();
    void sendAdminJobs();
   /** Let kernel thread sleep. To be called when user (Gui) input is required.
   *   @param reason debugging text
   */
   void waitForGui(string reason = "not specified");
    /** Updates user, account and bank data from fresh BPD and UPD data received from the bank.
     */
    bool updateUPDandBPD(C_BPD * bpd);
    /** Initialises the GV_Api with bank and user data.
     */
    bool initGvApi(const char * password, const char * bankDir);
    /**Searches for job in both lists. Returns NULL, if not found.
    */
    banking::BankingJob * getJobById(int id);
    /**This function runs in the subthread. It runs the GV_API functions. When an exception is caught, 
      * the _ErrorMessagesReceived flag is set, the exception is sent to the GUI via signal caughtException 
      * and the kernel thread is terminated. The kernelThreadFinished signal will then send a "false" value 
      * to the GUI and hopefully stop further processing there.
      */
    void run();
    /** Sets a SIGPIPE handler. Must be set to avoid chrashing due to network failure.
      */
    void sigpipeHandlerOn();
    /** Removes SIGPIPE handler. Throws an exception, if a SIGPIPE was caught since last
      * handler activation.
      */
    void sigpipeHandlerOff();
private slots:
   /** Called when subthread is started. Emits kernelThreadStarted.
    */
    void slotThreadStarted();
   /** Called when subthread is complete and run() returned.
    */
    void slotThreadFinished();
signals:
  /** Emitted by all catchers instead of throwing the exception. 
    *This signal transfers (copies of) exceptions to the main thread.
   */
    void caughtException(exception e);
  /**Emitted when a callback for job execution was received.
  */
  void jobStatusChanged(banking::BankingJob * job);
  /**Emitted after job has been passed to GV_Api and job id has become valid.
  */
  void sentAdminJob(int row, banking::BankingJob * job);
  /** Sent when admin jobs have been executed successfully.
   */
  void adminJobsToAnalyze();
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
  void tanRequest( QString bankmessage, QString * tan );
  /**Emitted when a certificate path is required for a PIN-TAN ssl connection.
   */
  void certificatePathRequest( QString * certificatePath );
  /** Emitted when api asks for hash verification. If hash is ok,
    * hash string must be modified (and returned) as "OK".
    */
  void verifyBankKeyHash(string * hash);
  /** Sent, when error messages (9XXX) have been received
  */
  void errorMessagesReceived(const QString & messages);
  /** Sent, when messages have been received and no 9XXX message was detected
  */
  void messagesReceived(const QString & messages);
  /**Sent when thread is starting. 
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
   * Sent when a user- overrideable error warning was detected
   */
  void overridableErrorDetected(bool * override, const QString & msg);
private:
   /**Removes all jobs from GV_Api registration.
    */
   void unregisterJobs();
  /**Creates a new _GV_Api and registers events.
  * Calls deleteApi, if _GV_Api is not NULL
  */
  void renewApi();

  /**Resets all Job status to JobNew, if status is JobQueued
   * and deletes _GV_Api
   */
   void deleteApi();

   /**Redirect cout and cerr
   */
   void sendOutputToLogfile(const QString & logFileName);
   /** Back to console again
   */
   void sendOutputToConsole();
   /** Start the HBCI-engine to create a new keyfile. createNewKeyfile MUST be run immediately
     * prior to this function. DO NOT call directly. Is called by run().
     */
   void runCreateNewKeyfile();
   /** Excecutes the joblists. executeJobLists MUST be run immediately
      * prior to this function. DO NOT call directly. Is called by run().
      */
   void runExecuteJobLists();
   /** Reads data from a DDV card. readDdvCardData MUST be run immediately
      * prior to this function. DO NOT call directly. Is called by run().
      */
   void runReadDdvCardData();
   
   /** Writes data to a DDV card. writeDdvCardData MUST be run immediately
      * prior to this function. DO NOT call directly. Is called by run().
      */
   void runWriteDdvCardData();

private:
    bool _MultiThreaded;
    /** The tasks to be performed by run().
      */
    typedef enum{
       NO_TASK,
       CREATE_KEYFILE,
       EXECUTE_JOB_LIST,
       READ_DDV_CARD_DATA,
       WRITE_DDV_CARD_DATA
    } TaskType;
   /** The task to be performed
    */
   TaskType _TaskToPerform;

   /**The mutex for protecting attributes. 
    * All setters and getters have to lock it prior to accessing data.
    */
   QMutex _KernelMutex;
   /**The mutex that protects the KernelThreadIsWaiting flag.
     */
   QMutex _WaitFlagMutex;
  /** The wait condition for synchronizing input. 
    * Call wait in run(), call wakeAll when input is complete.
    */
   QWaitCondition _WaitCondition;
  /** Flag for the waiting kernel thread. Do always use setWaitFlag function to modify flag!!
   */
   bool _WaitFlag;
   /** Uses _WaitFlagMutex to protect the _WaitFlag flag.
    * @param value new value for _WaitFlag flag
    */
   void setWaitFlag(bool value);

   /**The job lists
   */
   QList <banking::Transfer *> _TransferList;
   QList <banking::BankingJob *> _AdminList;
   /** The kernel engine
    */
   C_GV_API * _GV_Api;
   /**The Bank to use
    */
   banking::Bank * _Bank;
   /**All error messages that have been received.
    */
   QString _BankMessages;
   /** Flag for errors
    */
   bool _ErrorMessageReceived;
  /** Flag for a detected invalid password.
   */
   bool _PasswordInvalid;
   /** A buffer for the PIN entered on the computer's keyboard
   */
   QString _Pin;
  /** A buffer for the TAN entered on the computer's keyboard
   */
   QString _Tan;
  /** A buffer for the certificate path to be used for ssl PIN-TAN
    * connections.
    */
   QString _CertificatePath;
  /** Response container for PinTan success response
   */
   C_RESPONSE * _PinTanResponse;
  /** The security profile version 1, 2 or 10 for RDH keyfiles
   */
   QString _SecProfileVersion;
  /**Full path to the cardreader definition file readers.xml
   */
   QString _ReaderFile;
  /** Directory with the xml hbci-definition files
   */
   QString _XmlDir;
  /** Name of the logfile tor redirect output to.
   */
   QString _LogFileName;

  /** The map with the translations for the kernel messages.
   */
   static QMap<std::string, QString> _KernelTranslations;

   std::streambuf* _OldCoutSbuf;
   std::streambuf* _OldCerrSbuf;
   std::ofstream _LogFile;
};

#endif
