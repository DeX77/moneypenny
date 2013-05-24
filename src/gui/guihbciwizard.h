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
 *   Free Software Foundation, nc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef GUIHBCIWIZARD_H
#define GUIHBCIWIZARD_H

//#include "guimain.h"
#include <QStackedWidget>
#include <QList>
#include "hobit.h"
#include "hobit_connector.h"
#include "ui_guihbciwizardbase.h"
class GuiMain;


/**
* class for the wizard to set up the hbci (account-, bank- and user-details and internet connection)
*/

class GuiHbciWizard: public QWidget, public Ui::GuiHbciWizard
{
  Q_OBJECT
public:
   /**The numbers for the wizard pages, must be in the same order as the pages in the ui file
    */
   typedef enum{
      WELCOME_PAGE,
      INTERNET_PAGE,
      MEDIUM_PAGE,
      BANK_PAGE,
      USER_PAGE,
      CHECK_PAGE,
      PASSWORD_PAGE,
      CONNECT_PAGE,
      FINGERPRINT_PAGE,
      SEND_KEY_PAGE,
      ACCOUNTS_PAGE,
      FINISH_PAGE
   }PAGE;

  /**
  *this is the constructor of dialog for setting up wizard  
  *
  *@param main 
  *@param parent the parent dialog of this widget
  *@param name widgets title
  *@param hobit
  */
  GuiHbciWizard( QWidget * parent);

  /** destructor */
  ~GuiHbciWizard();

   /** Creates a html formatted QString containing the data input.
   */
   QString getDataString();
   /** Sets the corresponding radiobutton.(Floppy or Usb) default is floppy.
   */
   void setStorageType(StorageType storage);
   /** Write property for _MediumDir
   */
   void setMediumDir(const QString & dir);
  /** reads the user's previous input */
  virtual void evaluateStoredData();

  /** update account list */
  void updateAccountList();
  
  /** Changes the password for the hbci communication. Only valid for RDH files.
  */
  void changePassword();

  /**Shows the page at  given pageNumber
    */
  void showPage(int pageNumber);
  /** Returns index of current page
  */
  int currentPageIndex();
public slots:

  /**
  * Select bank from list, and fill in data, if available
  */
  virtual void on_lineEditBankCode_textChanged(const QString & text);
  /**
  * Evaluate Bank name and server to find out bank type.
  * Set comboBoxBankType, if banktype is recognized. 
  */
  virtual void on_lineEditBankServer_textChanged(const QString & text);

  /** Copies text from UserId to CustId
  */
  virtual void on_lineEditUserId_textChanged(const QString & text);
  
  /**
   *function for slot new medium
   */
  virtual void on_radioButtonNewMedium_toggled(bool checked);
  
  /**
   *function for slot existing medium
   */
  virtual void on_radioButtonExistingMedium_toggled(bool checked);
  /**
   *function for slot PIN-TAN
   */
  virtual void on_radioButtonPinTan_toggled(bool checked);
  /** Enables next button when more than 4 Digits are in edit.
   */
  virtual void on_lineEditAccountPinTan_textChanged(const QString & text);
  /**
   *function for slot chip card  
   */
  virtual void on_radioButtonChipCard_toggled( bool checked);
  
  /**
   *function for changing the medium type 
   */
  virtual void slotChangePassportType();
  
  /**
   *function for selecting a keyfile 
   */
  virtual void on_buttonSelectFile_clicked();
  
  /**
   *function for a test connection to the bank, the user gets informations about the status 
   */
  virtual void on_buttonTestConnection_clicked();

  /**
   * Dialog for send the keys. User gets informations about the status of sending
   */
  virtual void on_buttonSendKeys_clicked();

  /** Fingerprints have been checked
  */
  void on_checkBoxFingerprintOk_stateChanged(int state);

  /**
   * Executed when the page changes 
   */
  virtual void pageChanged();

  /** Show the help page on password security
  */
  void on_buttonHelpPassword_clicked();

  /** Checks the Internet setup.
  */
  void on_buttonTestInternetSetup_clicked();
  
  /** Calls Internet setup program.
  */
  void slotInternetSetup();
    
  /** Show next valid page
   */
  void on_buttonNext_clicked();
 /** Show page before
  */
  void on_buttonBack_clicked();

 /** Write modified data to the chipcard.
  */
  void on_buttonWriteDataToChipcard_clicked();

  /** Called when api asks for hash verification. If hash is ok,
    * hash string is modified to "OK".
    */
  void slotVerifyBankKeyHash(string * hash);
 /** Called when kernel thread is finished. If no error occurred, ok is true  
  */
  void slotKernelThreadFinished(bool ok);
  /**Show accounts page and enable Next button, if accountlist is not empty.
    */
  void slotAccountAdded();

protected slots:
  /**
   * Executed when the user presses cancel button
   */
  virtual void reject();

  /**
   * Executed when the user presses the finish button
   */
  virtual void on_buttonFinish_clicked();

  /**
  * Call help widget passing the appropriate help file.
  */
  void on_buttonHelp_clicked();
  
  /** Sets password bar according to log of number of possible passwords.
  */
  void on_lineEditPassword1_textChanged(const QString & text);
  
  /** Checks password and creates a new key if ok
  */
  void on_buttonCheckPassword_pressed();
  /** Updates the state of the controls on bank data page.
  */
  void slotUpdateBankDataPage();
  
  /** show log widget
  */
  void on_buttonDetails_clicked();
  
  /** configure card reader 
  */
  void configureCardReader();

  /**
  *raises the add account widget 
  */
  void on_buttonAddAccount_clicked();
  
  /** Fires signal dialIn.
  */
  void on_buttonDialIn_clicked();

 
signals:
  void testInternet(bool showSuccessMessage, const QString & url, int port, HoBIT_Connector::ConnectionState * state);
  void testConnection(bool * ok); //check connection to bank server
  void helpRequest(const QString & helpfile);
  void setupInternet();
  void setupSuccessful(HBCI_MODE RDHMode);
  void showLog(int Page);
  void renewHoBIT();
  void addAccount();
  void dialIn();
  void wizardPageChanged(int Page);
  void aborted();
  void sendJobsNow(bool *, const QString & reason, bool showJobList);
  void entropyRequest();
  
protected:
  /** Transfer settings of buttons and edits to class attributes for easy access.
   */
  bool readControls();
  /** Searches for an existing key file or asks the user for a name.
  */
  void getKeyFileName();
  
  /**Reads the keyfile an puts data into edits.
  */
  bool evaluateKeyFile(const QString & fileName);
  
  /** Create a new keyfile.
  */
  bool createNewKeyfile();
  
  /** Changes password of current keyfile.
  */
  bool changeKeyfilePassword();  
  
  /** Sets hbci combo.
  */
  void setHbciComboBox(int hbciVersion);
  /** Sets edits with bank data 
  */
  void on_tableWidgetBanks_currentCellChanged();
  /** Activate or deactivate a page
   */
  void enablePage(int pageNumber, bool enable);
  /** Activate or deactivate finish button
   */
  void enableFinish(bool enable);
  /**Get BPD and public keys for RDH security
   */ 
  bool getRdhKeys();
  /**
    * Runs job GetSystemId for a PIN-TAN account. 
   */
  bool getPinTanSystemId();
  /**
    *  Runs job GetSystemId for an RDH account. An activated user key is required for this to work
    */
  bool getRdhSystemId();
  /**Get BPD and UPD 
   */ 
  bool getUpd();

private:  
  QStackedWidget* _Stack;
  QList<banking::Bank> _Banks;

  //Flags
  bool _ConnectionTestSuccessful;
  bool _KeyFound;
  bool _MediumChanged;
//  bool _SendingKeysSuccessful;
  bool _EntropyOk;
  /** The entropy for a new keyfile
  */
  C_uarray _Entropy;
  bool _NewKeyCreated; // medium has been created
  bool _UpdOk; //a corrrect set of UPD's was received 
  bool _CardDataOk; //Chipcard contains valid data
  bool _ChangingPassword;
  /** The initial directory for key and config
  */
  QString _MediumDir;
  /**The name of the cardreader as detected by chipcard-tool list
  */
  QString _CardReaderType;
  /** The number of the chipcard as a string
  */
  QString _CardNumber;
  /** The HBCIVersion of the bank server.
  */
  int _HBCIVersion;
  /** The RDH version of the bank. Only used for RDH-Systems. Empty for DDV.
   */
  QString _RdhVersion;
  /** The passport type as specified in kernel/hbci_MSG_defines
   */
  int _PassportType; 

  //Convenience copies of the edit texts, set by getDataString
  QString _UserId;
  QString _CustomerId;
  QString _BankCode;
  QString _BankBIC;
  QString _Bankname;
  QString _Server;
  QString _MediumName;
  QString _KeyDir;

  /** Country code for the bank = 280
  */
  int _Country;

  /** The page numbers for the media stack
   */
  enum {
    MEDIA_PAGE_FIRST,
    MEDIA_PAGE_FILE,
    MEDIA_PAGE_CHIPCARD,
    MEDIA_PAGE_PINTAN
  }MediaPage;
  QVector <bool> _ActivePages;

 /** The tasks for the HBCI engine
  */
  typedef enum{
   NO_TASK,
   CREATE_NEW_KEYFILE,
   GET_RDH_KEYS,
   GET_RDH_SYSTEM_ID,
   GET_PIN_TAN_SYSTEM_ID,
   GET_UPD,
   TEST_CONNECTION,
   GET_ACCOUNTS,
   WRITE_DDV_CARD_DATA
   }KernelTask;
 /** The task for the HBCI engine
  */
  KernelTask _KernelTask;
};

#endif
