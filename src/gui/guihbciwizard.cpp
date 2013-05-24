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
#include "guihbciwizard.h"
#include "guifindmedium.h"
#include "dlgenterpin.h"
#include "keyfileopenhbci.h"
#include "bankitem.h"
#include "guientropy.h"

#include "kernel/global/HBCI_Cern_Message_defines"
#include "kernel/global/HBCI_exceptions.h"
#include "kernel/libicc/chipcard.h"
#include "list"
#include "string"
#include "math.h"
using namespace std;

#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QFileDialog>
#include <QRadioButton>
#include <QMessageBox>
#include <QColor>
#include <QLabel>
#include <QString>
#include <QRegExp>
#include <QValidator>
#include <QProgressBar>
#include <QCheckBox>
#include <QProcess>
#include <QInputDialog>
#include <QTextStream>


#define DEBUG_HBCI_WIZARD 0 

class ExNoJob: public exception
{
  virtual const char* what() const throw()
  {
    QString text = QObject::tr("Creating job failed");
    cerr << "***************Exeption Ex_NoJob: " << qPrintable(text)<< endl;
    return qPrintable(text);
  }
} _ExNoJob;

class ExJobFailed: public exception
{
  virtual const char* what() const throw()
  {
    QString text = QObject::tr("Job execution failed");
    cerr << "***************Exeption Ex_JobFailed: " << qPrintable(text)<< endl;
    return qPrintable(text);
  }
} _ExJobFailed;

/**
*this is the constructor. Reads bank list files and sets lists.  
*
*@param main 
*@param parent the parent dialog of this widget
*@param name widgets title
*@param hobit
*/
GuiHbciWizard::GuiHbciWizard( QWidget * parent)
    :QWidget(parent), Ui::GuiHbciWizard()
{
   qDebug("Constructor GuiHbciWizard"); 

   setupUi(this);
   setObjectName("GuiHbciWizard"); 
  _MediumDir = "";
  _MediumChanged = false;
  _EntropyOk = false;
  _UpdOk = false;
  _CardDataOk = false;
  _NewKeyCreated = false;
  _KeyFound = false;   
  _ChangingPassword = false; 
  _HBCIVersion = 220;
  _PassportType = 0;
  _Country = MONEYPENNY_DEFAULT_COUNTRY; //to be replaced for international use
  _KernelTask = NO_TASK;
  //activate all pages
  _ActivePages.resize(stackedWidgetMain->count());
  enablePage( WELCOME_PAGE, true);
  enablePage( INTERNET_PAGE, true);
  enablePage( MEDIUM_PAGE, true);
  enablePage( BANK_PAGE, true);
  enablePage( USER_PAGE, true);
  enablePage( CHECK_PAGE, true);
  enablePage( PASSWORD_PAGE, true);
  enablePage( CONNECT_PAGE, true);
  enablePage( FINGERPRINT_PAGE, true);
  enablePage( SEND_KEY_PAGE, true);
  enablePage( ACCOUNTS_PAGE, true);
  enablePage( FINISH_PAGE, true);

  buttonAbort->show();
  buttonFinish->hide();
  lineEditPort->setValidator(new QIntValidator(0, 65535, lineEditPort));
  if(USE_LIVE_CD_FEATURES)
    buttonDialIn->show();
  else buttonDialIn->hide();  
  progressBarStrength->setPalette(PALETTE_RED());

  _ConnectionTestSuccessful = false;
//  _SendingKeysSuccessful = false;

  _Banks = HoBIT::hobit()->listOfKnownHbciBanks();//read bank list file
  int nBanks = _Banks.count();
  if(tableWidgetBanks->rowCount() < nBanks)
    tableWidgetBanks->setRowCount(nBanks);
  if (nBanks == 0){ 
    QTableWidgetItem * noBanksWarning = new QTableWidgetItem("List with bankdata not found or empty: " + 
       MP_DIR()  + QString(MONEYPENNY_DATA_DIR) + "/" + QString(MONEYPENNY_HBCI_BANKDATA_FILE));
    tableWidgetBanks->setItem(0,0, noBanksWarning);
  }
  else
  for (int row = 0; row < nBanks; row++)
  {
    banking::BankItem  bankItem(_Banks[row]);
    bankItem.addToTable(tableWidgetBanks, row);
  }

  for (int column = 0; column < tableWidgetBanks->columnCount(); column ++)
    tableWidgetBanks->resizeColumnToContents(column);
  tableWidgetBanks->setSortingEnabled(true);
  qDebug("HBCI-banklist contains %i banks.", nBanks);

  comboBoxHBCIVersion->addItem(banking::Bank::hbciVersionString(201).c_str());
  comboBoxHBCIVersion->addItem(banking::Bank::hbciVersionString(210).c_str());
  comboBoxHBCIVersion->addItem(banking::Bank::hbciVersionString(220).c_str());
  comboBoxHBCIVersion->addItem(banking::Bank::hbciVersionString(300).c_str());
  comboBoxHBCIVersion->addItem(banking::Bank::hbciVersionString(400).c_str());

  buttonSelectFile->hide();
  buttonWriteDataToChipcard->hide();

  stackedWidgetMain->setCurrentIndex( WELCOME_PAGE );
  //set yellow color to all not yet checked edits
  textLabelTestConnection->setPalette(PALETTE_YELLOW());
  lineEditBankCode->setPalette(PALETTE_YELLOW());
  lineEditBankBIC->setPalette(PALETTE_YELLOW());
  lineEditBankName->setPalette(PALETTE_YELLOW());
  lineEditBankServer->setPalette(PALETTE_YELLOW());
  comboBoxHBCIVersion->setPalette(PALETTE_YELLOW());
  labelInternetState->setPalette(PALETTE_YELLOW());

}


/** destructor */
  GuiHbciWizard::~GuiHbciWizard(){
  qDebug("Destructor GuiHbciWizard finished");
}


/** Sets edits with bank data 
*/
void GuiHbciWizard::on_tableWidgetBanks_currentCellChanged()
{
  int row = tableWidgetBanks->currentRow();
  banking::Bank bank = _Banks[row];

  lineEditBankCode->setText(bank.bankCode().c_str());
  lineEditBankBIC->setText(HoBIT::hobit()->getBicForBankCode(bank.bankCode().c_str())); //get BIC from list
  lineEditBankName->setText((bank.bankName() + ", " + bank.city()).c_str());
  lineEditBankServer->setText(bank.server().c_str());
  comboBoxHBCIVersion->setCurrentIndex
    (comboBoxHBCIVersion->findText(bank.hbciVersionString().c_str()));
  slotUpdateBankDataPage();
}


/** Copies text from UserId to CustId
*/
void GuiHbciWizard::on_lineEditUserId_textChanged(const QString & text){
  if ( text.isEmpty())
    buttonNext->setEnabled( false);
  else buttonNext->setEnabled(true);
  lineEditCustId->setText(lineEditUserId->text());
}

/**
* function for check if typed-in bank is existing
*/
void GuiHbciWizard::on_lineEditBankCode_textChanged(const QString & )
{
  if(lineEditBankCode->text().length() != 8){ //no need to search for a bank, all ids are 8 digits long 
   //set yellow color to all not yet checked edits
   lineEditBankCode->setPalette(PALETTE_YELLOW());
   lineEditBankBIC->setPalette(PALETTE_YELLOW());
   lineEditBankName->setPalette(PALETTE_YELLOW());
   lineEditBankServer->setPalette(PALETTE_YELLOW());
   comboBoxBankType->setPalette(PALETTE_YELLOW());
   comboBoxHBCIVersion->setPalette(PALETTE_YELLOW());
   return;
  }

  bool bankFound = false;
  QList<banking::Bank>::iterator bit;
  for (bit = _Banks.begin(); bit != _Banks.end(); bit++)
  {
    QString bankCode = (*bit).bankCode().c_str();

    if (bankCode == lineEditBankCode->text()) //bank is in list
    {
      lineEditBankBIC->setText(HoBIT::hobit()->getBicForBankCode(bankCode)); //get BIC from list
      lineEditBankName->setText(QString::fromUtf8((*bit).bankName().c_str())+ ", " + QString::fromUtf8((*bit).city().c_str()));
      lineEditBankServer->setText((*bit).server().c_str());
      int hbciVersion = (*bit).hbciVersion();
      setHbciComboBox( hbciVersion);
      lineEditBankCode->setPalette(PALETTE_GREEN());
      lineEditBankBIC->setPalette(PALETTE_GREEN());
      lineEditBankName->setPalette(PALETTE_GREEN());
      lineEditBankServer->setPalette(PALETTE_GREEN());
      if(comboBoxBankType->currentIndex() != BANKTYPE_UNKNOWN)
        comboBoxBankType->setPalette(PALETTE_GREEN());
      else comboBoxBankType->setPalette(PALETTE_ORANGE());
      comboBoxHBCIVersion->setPalette(PALETTE_GREEN());
      bankFound = true;
    }
  }
  if(!bankFound)
  {
    if (lineEditBankCode->text().length() >= 8){
      QColor orange(255, 164, 0);
      lineEditBankName->setText(tr("Could not find a bank with the code you input."));
      lineEditBankServer->setText(tr("May be, that your bank has no HBCI Server."));
      lineEditBankCode->setPalette(PALETTE_ORANGE());
      lineEditBankBIC->setPalette(PALETTE_ORANGE());
      lineEditBankName->setPalette(PALETTE_ORANGE());
      lineEditBankServer->setPalette(PALETTE_ORANGE());
      comboBoxBankType->setPalette(PALETTE_ORANGE());
      comboBoxHBCIVersion->setPalette(PALETTE_ORANGE());
    }  
    else {
     lineEditBankName->setText("");
     lineEditBankServer->setText("");
     lineEditBankCode->setPalette(PALETTE_YELLOW());
     lineEditBankBIC->setPalette(PALETTE_YELLOW());
     lineEditBankName->setPalette(PALETTE_YELLOW());
     lineEditBankServer->setPalette(PALETTE_YELLOW());
     comboBoxBankType->setPalette(PALETTE_YELLOW());
     comboBoxHBCIVersion->setPalette(PALETTE_YELLOW());
    } 
  }
  slotUpdateBankDataPage();
}

/**
* Evaluate Bank name and server to find out bank type.
* Set comboBoxBankType, if banktype is recognized. 
*/
void GuiHbciWizard::on_lineEditBankServer_textChanged(const QString & text){
  QString server = text;
  QString bankName = lineEditBankName->text();
  comboBoxBankType->setCurrentIndex((int)HoBIT::hobit()->findBankType(server, bankName, _PassportType));
}

/**
* function for slot new medium
*/
void GuiHbciWizard::on_radioButtonNewMedium_toggled(bool checked)
{
  if(!checked)
   return;
  buttonWriteDataToChipcard->hide();
  stackedWidgetMedia->setCurrentIndex(MEDIA_PAGE_FILE);
  buttonSelectFile->hide();
//  _SendingKeysSuccessful = false;
  _NewKeyCreated = false;
  lineEditMediumPath->setPalette(PALETTE_GREEN());
  lineEditMediumPath->setText(_MediumDir + "/" + "moneypenny.key");
  buttonNext-> setEnabled(true); 
  labelHint->setText(tr("See below for your new key's name. You may edit the filename here."));
/* + "<br>"
   + tr("Please proceed to the next page.")); */
}

/**
* function for slot existing medium
*/
void GuiHbciWizard::on_radioButtonExistingMedium_toggled(bool checked)
{
  if(!checked)
   return;
  buttonWriteDataToChipcard->hide();
  stackedWidgetMedia->setCurrentIndex(MEDIA_PAGE_FILE);
  buttonSelectFile->show();
//  _SendingKeysSuccessful = true; //give it a try, if not ok, getUpd will fail
  labelSendKeys->setText(tr("You need not send the key again, if it has already been sent earlier."));
  getKeyFileName();
}

/**
* function for slot PIN-TAN
*/
void GuiHbciWizard::on_radioButtonPinTan_toggled(bool checked)
{
  if(!checked)
   return;
  buttonWriteDataToChipcard->hide();
  stackedWidgetMedia->setCurrentIndex(MEDIA_PAGE_PINTAN);
  repaint();
//  _SendingKeysSuccessful = true;
  labelSendKeys->setText(tr("You need not send a key. HBCI-PIN-TAN works with ssh certificates."));
  buttonNext->setEnabled(false);
  lineEditAccountPinTan->setFocus();
}

/** Enables next button when more than 4 Digits are in edit.
*/
void GuiHbciWizard::on_lineEditAccountPinTan_textChanged(const QString & text){
  if(text.length() > 4)
      buttonNext->setEnabled(true);
  else buttonNext->setEnabled(false);
}

/**
* function for slot chip card  
*/
void GuiHbciWizard::on_radioButtonChipCard_toggled( bool checked)
{
  bool cardOk = false;
  if(! checked)
    return;
  // start the timer which searches card readers
  QString bankCode, userId, url;
  int country; //presently readcard Data returns 0 TODO: get country data from card
  try{
   setCursor(Qt::WaitCursor);
   HoBIT::hobit()->readDDVCardData(country, bankCode, userId, url, _CardReaderType);
   cardOk = true;
  }
  catch(ExNoReader & e){
    if(e.readerName() == "No known reader")
      _CardReaderType = tr("No known reader"); //translate message
    else _CardReaderType = e.readerName().c_str();

    QMessageBox::critical(this, tr("Error"), 
                          tr("No cardreader response.") + "\n" 
                        + tr("USB system reported:") + " " + _CardReaderType + " "+ tr("is plugged in."));
  }
  catch(ExUnsupportedChipCard & e){
    QString msg; 
    if(e.type() == CHIPCARD_TYPE_SECCOS_RDH)
     msg = tr("A VR Networld card has been recognized.") + "\n"
         + tr("VR Networld cards are not yet supported.") +"\n"
         + tr("Please setup the HBCI connection with a security keyfile instead."); //translate message
    else msg = tr("This card type is not supported.");
    msg += "\n" + tr("Can not use the card.");
    QMessageBox::critical(this, tr("Error"), msg);
    _CardReaderType = tr("A cardreader was recognized");
  }
  catch(ExInvalidChipCardData & e)
  {
       _CardReaderType = e.getCardReaderName().c_str();
       QMessageBox::critical(this, tr("Error"), 
                                   tr("Chipcard type is ok, however the card contains no vaild data.") +"\n" 
                                   + tr("First Dataset on Card:") +"\n"
                                   + e.getCardData().c_str() );
       textLabelCheck->setText(textLabelCheck->text() + "\n"
         + tr("The \"Next\" button will be enabled, after writing the data to your chipcard."));
       bankCode = e.getBankCode().c_str();
       cardOk = true;
  }
  catch(exception & e){
       setCursor(Qt::ArrowCursor);
       QMessageBox::critical(this, tr("Error"), 
                                   tr("Exception while trying to read card data.") 
                                   + " " 
                                   +  e.what() );
  }
  stackedWidgetMedia->setCurrentIndex(MEDIA_PAGE_CHIPCARD);
  if(_CardReaderType.isEmpty())
     _CardReaderType = tr("No reader was found.");
  if(cardOk)
    buttonWriteDataToChipcard->show();
  labelReaders->setText( _CardReaderType ); 
  if( !userId.isEmpty()){
      lineEditBankCode->setText(bankCode);
      lineEditUserId->setText(userId);
      QMessageBox::information(this,tr("Information"), 
      tr("A set of user data has been read from your card and filled in the edit fields on the next pages.")+ "<br>" +
      tr("Please check the data versus the data you got from your bank in written form."));
  //    _SendingKeysSuccessful = true;
      buttonNext->setEnabled( true); 
      labelStatus->setText(tr("Using a DDV-chipcard from your Bank"));
      labelSendKeys->setText(tr("Key is already known. Sending is not neccessary."));
      _CardDataOk = true;
    }
    else{ //no user id, but a valid card,
     if(cardOk){
       lineEditBankCode->setText(bankCode); //try the BLZ on the card, if any
       QMessageBox::information(this,tr("Information"),
       tr("Your card was recognized. However data are missing on your card.") + "<br>"+
       tr("Could not find user data on your chipcard.") + "<br>"+
       tr("Your bank should have given you a letter containing user and access data.") +
       tr("Please input required data into the edit fields on the next pages manually.")+
       tr("After that MoneyPenny must write the data to the chipcard."));
       _CardDataOk = false;
       buttonNext->setEnabled( true );
     }
     else{ //no valid card
      labelReaders->setText(_CardReaderType + "\n" + tr("No carddata found or card type is unknown. Can not continue.") ); 
      buttonNext->setEnabled( false);
     }
    }
   setCursor(Qt::ArrowCursor);
}

/**
* function for change the medium type, called twice, once for every button 
*/
void GuiHbciWizard::slotChangePassportType()
{
  if(isVisible()&& _MediumChanged)
   getKeyFileName();
  _MediumChanged = true;
}

/**
* function for choose keyfiles. Checks file format.
*/
void GuiHbciWizard::on_buttonSelectFile_clicked()
{
  QString fileName;
  QString keyFiles = tr("HBCI key files (*.key)");
  QString userId = lineEditUserId->text();
  fileName = QFileDialog::getOpenFileName(this,tr("Select a Keyfile"),lineEditMediumPath->text(),keyFiles);
  if (!fileName.isEmpty()){
    if (evaluateKeyFile(fileName)){
      buttonNext->setEnabled(true);
      _KeyFound = true;
    }
    else{ // File can not be parsed
      buttonNext->setEnabled(false);
      _KeyFound = false;
    } 
   } 
  else{ //No filename
     buttonNext->setEnabled(false);
     _KeyFound = false;
  }
  lineEditMediumPath->setText(fileName); //put in whatever the user selected
}

/**
* function for a test connection to the bank. Creates a new user, if required. 
*/
void GuiHbciWizard::on_buttonTestConnection_clicked()
{
  bool ok = true;
  connect(HoBIT::hobit(), SIGNAL(kernelThreadFinished(bool)), 
          this, SLOT(slotKernelThreadFinished(bool)),
           Qt::UniqueConnection);
  // get information from the input mask
//  QString userId = lineEditUserId->text();
/*  if(radioButtonPinTan->isChecked())
    userId = lineEditAccountPinTan->text(); //for PinTan, one of the user's account numbers is used as user ID*/
/*  QString bankCode = lineEditBankCode->text();
  QString bankBIC = lineEditBankBIC->text();
  QString customerId = lineEditCustId->text();
  if(customerId.isEmpty()) //user has cleared the customer id edit, simply copy userId 
   customerId = userId;
  QString server = lineEditBankServer->text();
  _HBCIVersion  = banking::Bank::convertHbciVersionString(qPrintable(comboBoxHBCIVersion->currentText()));
  QString mediumName;
  QString keyDir;
  int country = MONEYPENNY_DEFAULT_COUNTRY;*/
  QString sPassportType;
  labelUpd->setText("");

  readControls(); //Update all properties
  if (radioButtonChipCard->isChecked())
  {
//    _PassportType = PASSPORT_CHIPCARD | PASSPORT_SECURITY_DDV;
    sPassportType = "DDV";
  }
  else if (radioButtonExistingMedium->isChecked() || radioButtonNewMedium->isChecked())
  {
    sPassportType = "RDH";
  }
  else if (radioButtonPinTan->isChecked())
  {
    sPassportType = "OTP";
  }

//   if(_PassportType & PASSPORT_KEYFILE){    //Split the path into name and dir
//      QFile keyFile (lineEditMediumPath->text());
//      QFileInfo keyFileInfo(keyFile);
//      mediumName = keyFileInfo.fileName();
//      keyDir = keyFileInfo.absoluteDir().absolutePath();
//   }
  _KernelTask = TEST_CONNECTION;
  try{
     if(ok)
     { //create user
         QString msg = "Creating user: keyName= " + _MediumName
                                     + " passportType= " + sPassportType
                                     + " RDHVersion= " + _RdhVersion
                                     + " country= " + QString::number(_Country)
                                     + " bankCode= " + _BankCode
                                     + " HBCIVersion: " + QString::number(_HBCIVersion)
                                     + " userId= " + _UserId
                                     + " customerId= " + _CustomerId
                                     + " server= " + _Server;
         qDebug("GuiHBCIWizard::testConnection: %s", qPrintable(msg));
         if(ok){
           ok = HoBIT::hobit()->createUser( _PassportType,
                                  _MediumName, //== passport filename
                                  QString::number(_Country),
                                  _Bankname,
                                  _BankBIC,
                                  _BankCode,
                                  (BankType)comboBoxBankType->currentIndex(),
                                  _HBCIVersion,
                                  _RdhVersion,
                                  _UserId,
                                  _CustomerId,
                                  _Server);
            if(ok)
              HoBIT::hobit()->setUserDefinedBankName(lineEditBankName->text());
            if(_PassportType & PASSPORT_KEYFILE) //user must get the path here!
              HoBIT::hobit()->setPassportFileDir(_KeyDir);
         }
         if (!ok){
           QMessageBox::critical(this, tr("Error"), 
            tr("Could not create user dataset.")+ "\n"
            +tr("Please check cardreader, chipcard and input data."));
         }
     }//end create user
     
     //if in RDH mode, run jobs GetKey and GetSystemId and wait for user confirmation of keys
     //before trying to get accounts
     if (ok && (_PassportType & PASSPORT_SECURITY_RDH) && ! _NewKeyCreated)
     {   
           HoBIT::hobit()->setPassportFileDir(_KeyDir); // this needs a valid user!
         ok = getRdhKeys(); //this starts threads for getRDHSystemId and getUpd
/*         if (!ok)
           throw _ExJobFailed;
        //Keys should have been sent by createNewKeys, 
         labelSendKeys->setText(tr("If you did already send your key, you need not send it again."));
         QString hash = HoBIT::hobit()->bankFingerPrint();
         if(hash.isEmpty())
            qDebug("ERROR: No bank hash!");
         else qDebug( "Bank Hash: %s", qPrintable(hash));
         textLabelBankHash->setText(hash);*/
     }

     // if in DDV card mode, run JobGetUPD now. 
     else if (ok && ( _PassportType & PASSPORT_SECURITY_DDV) )
     {
        ok = getUpd();
/*        if(ok)
          updateAccountList();*/
     }

     //PIN-TAN (OTP Mode)
    else if (ok && ( _PassportType & PASSPORT_SECURITY_OTP) )
    {
      //system Id required for Pin-Tan
      ok = getPinTanSystemId();
    }        //HoBIT::hobit()->addAccount(bankCode, lineEditAccountPinTan->text());
/*          ok = getUpd();
          if(ok)  
          updateAccountList();*/
    // }

/*     if(ok && ! _NewKeyCreated)
       HoBIT::hobit()->updateSystemId();//copy system id from BPD now and use it for next job*/
   }//end try

   catch(exception & e){
     cerr << "GuiHbciWizard: Exception during connection test."<< endl;
     cerr << e.what();
/*     if(_PassportType & PASSPORT_CHIPCARD)
      QMessageBox::critical(this, tr("Error"), 
       tr("Job execution failed.") +" <br><b>" 
       +tr(" Does your chipcard contain HBCI-userId, bankcode and bankserver?") + "</b><br>" + "</b><br>" 
       +tr("If not, go back and let MoneyPenny write the data to your card."));
     else*/
       QMessageBox::critical(this, tr("Error"), tr("Exception during connection test:") + " " +  e.what() );
     ok = false;
   }

/* //finally set the controls. 
  if (ok){
    HoBIT::hobit()->showLogText(tr("Execution terminated successfully.")); 
    textLabelTestConnection->setText(tr("Connection test was successful."));
    textLabelTestConnection->setPalette(PALETTE_GREEN());
    _ConnectionTestSuccessful = true;
    buttonNext->setEnabled(true);
  }
  else{ //not ok
    textLabelTestConnection->setText(tr("failed"));
    textLabelTestConnection->setPalette(PALETTE_RED());
    _ConnectionTestSuccessful = false;
    buttonNext->setEnabled(false);
  }
  #ifdef DEBUG_HBCI_WIZARD 
        buttonNext->setEnabled(true); //enable next, although test must fail due to disabled sending
  #endif
  update();*/
}

/**
* Dialog for send the keys. User gets informations about the status of sending
*/
void GuiHbciWizard::on_buttonSendKeys_clicked()
{
  bool result = true;

  // go ahead and create the job
  HoBIT::hobit()->showLogText(tr("Creating job: Send your public key"));
  result = HoBIT::hobit()->createJobFirstKeySend();

  // now execute the job queue without showing the progress
  HoBIT::hobit()->showLogText(tr("Executing job queue."));
   emit sendJobsNow(&result, tr("Your public key must be read from the crypted keyfile."), false);

  if (! result)
  {
    HoBIT::hobit()->showLogText(tr("Execution terminated with errors."));
    labelSendKeys->setText(tr("Sending public key failed."));
    labelSendKeys->setPalette(PALETTE_RED());
   // _SendingKeysSuccessful = false;
  }
  else{
    HoBIT::hobit()->showLogText(tr("Execution terminated successfully."));
    labelSendKeys->setText(tr("Sending key terminated successfully."));
    labelSendKeys->setPalette(PALETTE_GREEN());;
  //  _SendingKeysSuccessful = true;
  }

  pageChanged(); // set the controls
}

/** Fingerprints have been checked, if ok, save keyfile and get Upd now
*/
void GuiHbciWizard::on_checkBoxFingerprintOk_stateChanged(int state){
   if(state == Qt::Checked)
   {
     checkBoxFingerprintOk->setPalette(PALETTE_GREEN());
     HoBIT::hobit()->saveKeyfile(_KeyDir + _MediumName); //we have got a valid key, save it

     if(! getUpd()) //success is evaluated in slotThreadFinished
        _UpdOk = false; //error before thread was started
/*     if (_UpdOk) // accounts were in UPD, have been saved in bank object, now get them from there
        updateAccountList();
     else { //Upd error
       HoBIT::hobit()->showLogText(tr("Execution terminated with errors."));
       if(_PassportType & PASSPORT_CHIPCARD)
        QMessageBox::critical(this, tr("Error"), 
         tr("Job execution failed.") +" <br><b>" 
         +tr(" Does your chipcard contain HBCI-userId, bankCode and bankserver?") + "</b><br>" 
         +tr(" If not, please go back and let MoneyPenny write the data to your card."));
       if(_PassportType & PASSPORT_SECURITY_RDH)
        QMessageBox::critical(this, 
         tr("Possible error reasons"),
         "<html>"
         + tr("Did you already send (or give) your ini-letter to the bank?") 
         + tr("If not, please continue with this setup, and let MoneyPenny generate and print your ini-letter.")+ "<br>"
         + tr("Has your key already been activated by the bank?") + "<br>"
         + "</html>");
     }// end Upd error

     if(_UpdOk) {// system-id is in HKSYN   
        HoBIT::hobit()->saveKeyfile(lineEditMediumPath->text()); //we have got a valid system Id, save it
        labelUpd->setPalette(PALETTE_GREEN());;
        labelUpd->setText(tr("A set of user data was transferred."));
     }
     else{ //no Upd
        labelUpd->setPalette(PALETTE_YELLOW());;
        labelUpd->setText(tr("No user data were received. This may be due to an inactive key.") +"\n"+
                          tr("You may proceed anyway."));
        buttonNext->setEnabled(true); //enable next anyway
     }*/
   }
   else{ // not checked 
    checkBoxFingerprintOk->setPalette(PALETTE_YELLOW());
    labelUpd->setText("");
    buttonNext->setEnabled(false);
   }
  #ifdef DEBUG_HBCI_WIZARD 
        buttonNext->setEnabled(true); //enable next anyway
  #endif
}

/**
* Executed when the page changes 
*/
void GuiHbciWizard::pageChanged()
{
  emit wizardPageChanged(stackedWidgetMain->currentIndex()); //update page number and smallHelp in guiMain
  //any page
  if(radioButtonNewMedium->isChecked()){
      enablePage( WELCOME_PAGE, true);
      enablePage( INTERNET_PAGE, true);
      enablePage( MEDIUM_PAGE, true);
      enablePage( BANK_PAGE, true);
      enablePage( USER_PAGE, true);
      enablePage( CHECK_PAGE, true);
      enablePage( PASSWORD_PAGE, true);
      enablePage( CONNECT_PAGE, false); //connection is tested by createNewKey
      enablePage( FINGERPRINT_PAGE, false);// fingerprint is checked via modal messagebox
      enablePage( SEND_KEY_PAGE, false);//key is sent automatically
      enablePage( ACCOUNTS_PAGE, false);//no accounts possible
      enablePage( FINISH_PAGE, true);
    }
  else if(radioButtonExistingMedium->isChecked()){
      enablePage( WELCOME_PAGE, true);
      enablePage( INTERNET_PAGE, true);
      enablePage( MEDIUM_PAGE, true);
      enablePage( BANK_PAGE, true);
      enablePage( USER_PAGE, true);
      enablePage( CHECK_PAGE, true);
      enablePage( PASSWORD_PAGE, false);
      enablePage( CONNECT_PAGE, true);
      enablePage( FINGERPRINT_PAGE, true);
      enablePage( SEND_KEY_PAGE, false);
      enablePage( ACCOUNTS_PAGE, true);
      enablePage( FINISH_PAGE, true);
  }
  else if(radioButtonChipCard->isChecked()){
      enablePage( WELCOME_PAGE, true);
      enablePage( INTERNET_PAGE, true);
      enablePage( MEDIUM_PAGE, true);
      enablePage( BANK_PAGE, true);
      enablePage( USER_PAGE, true);
      enablePage( CHECK_PAGE, true);
      enablePage( PASSWORD_PAGE, false);
      enablePage( CONNECT_PAGE, true);
      enablePage( FINGERPRINT_PAGE, false);
      enablePage( SEND_KEY_PAGE, false);
      enablePage( ACCOUNTS_PAGE, true);
      enablePage( FINISH_PAGE, true);
  }
  else if(radioButtonPinTan->isChecked()){
      enablePage( WELCOME_PAGE, true);
      enablePage( INTERNET_PAGE, true);
      enablePage( MEDIUM_PAGE, true);
      enablePage( BANK_PAGE, true);
      enablePage( USER_PAGE, false);
      enablePage( CHECK_PAGE, false);
      enablePage( PASSWORD_PAGE, false);
      enablePage( CONNECT_PAGE, true);
      enablePage( FINGERPRINT_PAGE, false);
      enablePage( SEND_KEY_PAGE, false);
      enablePage( ACCOUNTS_PAGE, true);
      enablePage( FINISH_PAGE, true);
  }
  
 //  Welcome page
  if (stackedWidgetMain->currentIndex() == WELCOME_PAGE ){
    enableFinish(false);
    _MediumChanged = false;
    _NewKeyCreated = false;
    _KeyFound = false;
    _ChangingPassword = false;
    buttonAbort->show();
  }
  
  //  internet setup page
  if (stackedWidgetMain->currentIndex() == INTERNET_PAGE ){
    buttonTestInternetSetup->setFocus();
    buttonNext->setEnabled(true);
    buttonAbort->hide();
  }
  
  //  bank data page
  if (stackedWidgetMain->currentIndex() == BANK_PAGE ){
    lineEditBankCode->setFocus();
    slotUpdateBankDataPage();
    buttonAbort->hide();
  }
  

  // medium data page
  if (stackedWidgetMain->currentIndex() == MEDIUM_PAGE )
  {
    if ( !radioButtonNewMedium->isChecked() &&
         !radioButtonExistingMedium->isChecked() &&
         !radioButtonChipCard->isChecked() &&
         !radioButtonPinTan->isChecked())
      buttonNext->setEnabled(false);
    else {
          getKeyFileName();
      buttonAbort->hide();
      buttonNext->setEnabled( _KeyFound || radioButtonNewMedium->isChecked() || radioButtonChipCard->isChecked()); 
    }

 }

  //  customer data page
  if (stackedWidgetMain->currentIndex() == USER_PAGE )
  {
    lineEditUserId->setFocus();
    on_lineEditUserId_textChanged(lineEditUserId->text()); //check for empty edit and set next button
    buttonAbort->hide();
  }

  // show data information page
  if (stackedWidgetMain->currentIndex() == CHECK_PAGE )
  {
    textLabelInformation->setText(getDataString());
    buttonAbort->hide();
    buttonNext->setEnabled( _CardDataOk || ( !radioButtonChipCard->isChecked() ) );
   }

  //password and key generation page
  if (stackedWidgetMain->currentIndex() == PASSWORD_PAGE ){ 
     if(_ChangingPassword){
       textLabelOldPassword->show();
       lineEditOldPassword->show();
       lineEditOldPassword->setFocus();
       buttonNext->setEnabled( false );
       buttonAbort->show();
     }
     else{
       textLabelOldPassword->hide();
       lineEditOldPassword->hide();
       lineEditPassword1->setFocus();
       buttonNext->setEnabled(_NewKeyCreated);
       buttonAbort->show();
     }
  }
  
  //connection test page
  if (stackedWidgetMain->currentIndex() == CONNECT_PAGE ){ 
    if (radioButtonChipCard->isChecked()){
      enablePage(PASSWORD_PAGE, false);
      enablePage(FINGERPRINT_PAGE, false);
      enablePage(SEND_KEY_PAGE, false);
      enablePage(FINISH_PAGE, true);
     }
     else if(radioButtonNewMedium->isChecked()){
          enablePage(PASSWORD_PAGE, true);
          enablePage(SEND_KEY_PAGE, false); //key is sent automatically
          enablePage(FINGERPRINT_PAGE, false); //fingerprint is checked by slotVeriFyBankKeyHash
     }

    if (_ConnectionTestSuccessful) {
      buttonNext->setEnabled( true);
      buttonAbort->hide();
    }
    else buttonNext->setEnabled( false);
  }

  // fingerprint page
  if (stackedWidgetMain->currentIndex() == FINGERPRINT_PAGE ){ 
      buttonNext->setEnabled(checkBoxFingerprintOk->isChecked());
      buttonAbort->hide();
 }

  //send key page
  if (stackedWidgetMain->currentIndex() == SEND_KEY_PAGE ){ 
      buttonNext->setEnabled( true); 
      labelSendKeys->setPalette(PALETTE_GREEN());
  }

  //accounts page
  if (stackedWidgetMain->currentIndex() == ACCOUNTS_PAGE ){ 
/*   if( HoBIT::hobit()->accounts() )
     buttonNext->setEnabled( ! HoBIT::hobit()->accounts()->empty() ); 
   else buttonNext->setEnabled( false );*/
    buttonNext->setEnabled( true ); //Allow user to proceed to ini-letter
  }

 //last page
  if (stackedWidgetMain->currentIndex() == FINISH_PAGE )
  {
    if(radioButtonChipCard->isChecked() )
     textLabelCongratulations->setText(tr("<h1> Congratulations </h1>\
<p>\
You did succeed in setting up the securest bank communication system \
known up to now. You may now select an account and start homebanking. \
We recommend that you get your balance and transaction data as the first step.\
Please do not forget to remove your chipcard and to unplug the cardreader after you shutdown MonyeyPenny.\
</p><p>\
The MoneyPenny team wishes you a lot of fun while working with the program. \
</b>"));
   else if(radioButtonNewMedium->isChecked()|| radioButtonExistingMedium->isChecked())
   {
      if(_NewKeyCreated)
        {
        textLabelCongratulations->setText(tr("<h1> Congratulations </h1>\
<p>\
You did succeed in setting up the first step of the securest bank communication system \
known up to now. MoneyPenny is now ready to print the results of the \
communication setup process as a so called Ini-Letter with informations about your public key. \
In order to activate your homebanking account, you have to sign this \
letter and send it to your bank. \
</p><p>\
HBCI/FinTS homebanking will work as soon as your bank will have activated your account. \
</p>"));
        }//end _NewKeyCreated
        else if(_UpdOk)
        {
          textLabelCongratulations->setText(tr("<h1> Congratulations </h1>\
<p>\
Setup of the HBCI/FinTS system is completed. \
MoneyPenny has received a set of user and account data from the bank. \
When you click finish, MoneyPenny will open the list with the accounts received. \
Select an account from the list. \
Then select Account Information / Get Transactions from the main menu to \
ask for balance and transaction history data available for this account. \
</p>"));
        }//end _UpdOk
        else //neither new key nor UpdOk -> Key was sent, but not yet activated
        {
          textLabelCongratulations->setText(tr("<h1>User data not yet available</h1>\
<p>\
Setup is not yet complete. It seems as if your key is not yet active. \
Did you already give your Ini-Letter to the bank? \
If so, ask them to activate your key. \
<b>Do not create a new key, unless the bank told you, that no key was sent for your \
user Id. </b>\
</p>"));
        }
   }
   else if(radioButtonPinTan->isChecked())
     textLabelCongratulations->setText(tr("h1> Congratulations </h1>\
<p>\
You did succeed in setting up the securest bank communication system \
available for HBCI-PIN-TAN. The MoneyPenny CD is secure against \
viruses, keyloggers or man-in-the-middle attacks. However, please remember that everybody \
who knows your PIN can read your account data with a standard browser. \
<b> Therefore you must keep your PIN absolutely secret. </b> \
</p><p>\
The MoneyPenny team wishes you a lot of fun while working with the program. \
</b>"));
    enableFinish(true);
  }//end FINISH_PAGE
}


/**
* Executed when the user presses cancel button
*/
void GuiHbciWizard::reject()
{
  if(QMessageBox::question( this,
                         tr("Confirmation"),
                         tr("Cancel setup?<br> HBCI setup is not yet complete. Homebanking may not work. \
                             <br>Do you really want to cancel?"),
                         tr("&Yes"), tr("&No"),QString::null, 0, 1 ) == 0)
  emit aborted(); //this will run the main bank loop and restore old settings, if available
}

/**
* Executed when the user presses the finish button
*/
void GuiHbciWizard::on_buttonFinish_clicked()
{
  disconnect(HoBIT::hobit(), SIGNAL(kernelThreadFinished(bool)), 
          this, SLOT(slotKernelThreadFinished(bool)));
  HoBIT::hobit()->setUserDefinedBankName(lineEditBankName->text());
  int nAccounts = HoBIT::hobit()->accounts()->size();
  if( nAccounts == 0)
     HoBIT::hobit()->addAccount(MONEYPENNY_DUMMY_ACCOUNT_ID,"", "");//tr("No account available")); //add a dummy
  HoBIT::hobit()->saveEnvironment();
  if (radioButtonChipCard->isChecked())
    emit setupSuccessful(HBCI_MODE_DDV);
  else if(radioButtonPinTan->isChecked())
      emit setupSuccessful(HBCI_MODE_PIN_TAN);
  else if(radioButtonNewMedium->isChecked())
      emit setupSuccessful(HBCI_MODE_RDH_STEP1);
  else if(radioButtonExistingMedium->isChecked())
      {
       if(_UpdOk)
          emit setupSuccessful(HBCI_MODE_RDH);
       else emit setupSuccessful(HBCI_MODE_RDH_STEP1);
      }

}

/**
* Call help widget passing the appropriate help file.
*/
void GuiHbciWizard::on_buttonHelp_clicked()
{
  switch(stackedWidgetMain->currentIndex())
  { 
    case WELCOME_PAGE  : emit helpRequest("hbci_welcome.html"); break;
    case INTERNET_PAGE : emit helpRequest("hbci_internet.html"); break;
    case BANK_PAGE     : emit helpRequest("hbci_bank.html"); break;
    case USER_PAGE     : emit helpRequest("hbci_user.html"); break;
    case MEDIUM_PAGE   : emit helpRequest("hbci_medium.html"); break;
    case CHECK_PAGE    : emit helpRequest("hbci_check.html"); break;
    case PASSWORD_PAGE : emit helpRequest("hbci_password.html"); break;
    case CONNECT_PAGE  : emit helpRequest("hbci_connect.html"); break;
    case FINGERPRINT_PAGE: emit helpRequest("hbci_fingerprint.html"); break;
    case SEND_KEY_PAGE : emit helpRequest("hbci_send_key.html"); break;
    case FINISH_PAGE   : emit helpRequest("hbci_finish.html"); break;
    default            : emit helpRequest("hbci_welcome.html"); break;
  }
}

/** Show the help page on password security
*/
void GuiHbciWizard::on_buttonHelpPassword_clicked(){
 emit helpRequest("security.html");
}

/** Sets password bar according to log of number of possible passwords.
*/
void GuiHbciWizard::on_lineEditPassword1_textChanged(const QString & text){
 QString password = text;
 double nChars = 0.0; // use double to prevent numerical overflow
 int pos;
 QRegExp rx0( "[0-9]{1,20}");
 QRegExp rx1( "[a-z]{1,20}");
 QRegExp rx2( "[a-z, A-Z]{1,20}" );
 QRegExp rx3( "[a-z, A-Z, 0-9]{1,20}");
 QRegExpValidator v0( rx0, NULL );
 QRegExpValidator v1( rx1, NULL );
 QRegExpValidator v2( rx2, NULL );
 QRegExpValidator v3( rx3, NULL );
 if(v0.validate(password, pos) == QValidator::Acceptable)
   nChars = 10;
 else if(v1.validate(password, pos) == QValidator::Acceptable)
   nChars = 26;
 else if(v2.validate(password, pos) == QValidator::Acceptable)
   nChars = 52;
 else if(v3.validate(password, pos) == QValidator::Acceptable)
   nChars = 62;
 else if(password.length() > 0) 
   nChars = 128;
 
 double nPasswords = exp( log(nChars) *double(password.length()));
 int progress = int(log(nPasswords) / log(10.0) * 70.0 / 16.0); //70% for 10E16
 if(password.length() == 0)
   progress =0;
 progressBarStrength->setValue(progress); 
 progressBarStrength->setPalette(PALETTE_GREEN());
 if(progress < 70)
  progressBarStrength->setPalette(PALETTE_YELLOW());
 if(progress < 40)
  progressBarStrength->setPalette(PALETTE_RED());
}


/** Checks password and creates a new key if ok
*/
void GuiHbciWizard::on_buttonCheckPassword_pressed(){
 buttonNext->setEnabled(false);
 if(lineEditPassword1->text() == lineEditPassword2->text() ){
   if(progressBarStrength->value() > 40){ 
       if(_ChangingPassword)
         buttonNext->setEnabled(changeKeyfilePassword());
       else if (radioButtonNewMedium->isChecked()){
          //check for file errors
//          QMessageBox::information(this, tr("Information"), tr("Random data for a new security medium will be collected now"));
          GuiEntropy guiEntropy(this);
          if(!guiEntropy.exec())
            QMessageBox::critical(this, tr("Error"), tr("Key generation has been cancelled."));
          else{
            _Entropy = guiEntropy.entropy();
            _EntropyOk = true;
          }
         if(_EntropyOk&&QMessageBox::information(this,tr("Create Keys"), 
            tr("MoneyPenny will now check the internet connection to the bank, create a new keyfile and send the public key to the bank."),
            QMessageBox::Ok|QMessageBox::Cancel )== QMessageBox::Ok)
         {
           if(createNewKeyfile()) 
             buttonNext->setEnabled(true); 
           else buttonNext->setEnabled(false);
         }
       }//end if radioButtonNewMedium
     }//end if progress > 40
     else  QMessageBox::critical(this, tr("Error"), tr("The password is too weak, please use a longer one."));
 }
 else QMessageBox::critical(this, tr("Input Error"), 
                     tr("The passwords in the two edits are not identical. <br> Please input again."));  
}

/** Creates a html formatted QString containing the data input.
*/
QString GuiHbciWizard::getDataString(){
    
    QString passportType;
    readControls(); 
   if (radioButtonChipCard->isChecked())
    {
      passportType = tr("DDV-Chip Card");
    }
    else if (radioButtonExistingMedium->isChecked())
    {
      passportType = tr("RDH file on ");
      if (radioButtonFloppy->isChecked())
        passportType += tr("Floppy Disc");
      else passportType += tr("Usb-Memory-Stick");
    }
    else if (radioButtonNewMedium->isChecked())
    {
      passportType = tr("RDH file on ");
      if (radioButtonFloppy->isChecked())
        passportType += tr("Floppy Disc");
      else passportType += tr("Usb-Memory-Stick");
      _MediumName = QString(lineEditMediumPath->text());
    }
    else if (radioButtonChipCard->isChecked())
    {
      passportType = tr("Chip card");
      _MediumName = tr("Card reader ");
    }
    
    QString data;
    data = "<html> <table>";
    data += "<tr><td>" + tr("Bank name: ");            data += "</td><td>" + _Bankname;     data += "</td></tr>";
    data += "<tr><td>" + tr("Bank code: ");            data += "</td><td>" + _BankCode;     data += "</td></tr>";
    data += "<tr><td>" + tr("Server address: ");       data += "</td><td>" + _Server;       data += "</td></tr>";
    data += "<tr></tr>";
    data += "<tr><td>" + tr("Customer ID: ");          data += "</td><td>" + _CustomerId;   data += "</td></tr>";
    data += "<tr><td>" + tr("User Id: ");              data += "</td><td>" + _UserId;       data += "</td></tr>";
    data += "<tr></tr>";
    data += "<tr><td>" + tr("Medium used for keys: "); data += "</td><td>" + passportType; data += "</td></tr>";
    if (radioButtonChipCard->isChecked()){
      data += "<tr><td>" + tr("Card reader: ") + "</td><td>" +  _CardReaderType + "</td></tr>";
    }
    else {
      data += "<tr><td>" + tr("File path to medium: ")  + "</td><td>" + _MediumName + "</td></tr>";
    }
    data += "</table> </html> ";
    return data;
}

/** Updates the state of the controls on bank data page.
*/
void GuiHbciWizard::slotUpdateBankDataPage(){
    if ( lineEditBankCode->text().isEmpty() ||
         lineEditBankName->text().isEmpty() ||
         lineEditBankServer->text().isEmpty())
    {
      buttonNext->setEnabled(false);
    }
    else buttonNext->setEnabled(true);
    update();
}

/** Sets the corresponding radiobutton.(Floppy or Usb)
*/
void GuiHbciWizard::setStorageType(StorageType storage){
 switch(storage){
   case STORAGE_USB: radioButtonUsb->setChecked(true);break; //calls slotChangeMediumtype via connect
   case STORAGE_FLOPPY: radioButtonFloppy->setChecked(true); break; //calls slotChangeMediumtype
   default: radioButtonFloppy->setChecked(true); break; //calls slotChangeMediumtype
 }
 _MediumChanged = false; //reset flag. it has been set by slotChangeMediumtype
}

/** Write property for _MediumDir
*/
void GuiHbciWizard::setMediumDir(const QString & dir){
  _MediumDir = dir;
  lineEditMediumPath->setText(_MediumDir);
}

/** Searches for an existing key file or asks the user for a name.
*/
void GuiHbciWizard::getKeyFileName(){
    QString absFileName;
    QString fileName;
    _KeyFound = false;

    //medium changed
    if(_MediumChanged){
      GuiFindMedium guifindmedium(this );
       if(guifindmedium.exec() == QDialog::Accepted){
        _MediumDir = guifindmedium.getCryptDirName();
      }
    }

    //new file
    if( radioButtonNewMedium->isChecked()){
      absFileName = _MediumDir + "/moneypenny.key";
      buttonNext->setEnabled(true);
    }
    else absFileName = lineEditMediumPath->text();

    //existing keyfile
    if(radioButtonExistingMedium->isChecked()){ //search for existing key
       QFile keyFile(absFileName);
       QDir keyDir(_MediumDir);
       QStringList keyFiles = keyDir.entryList(QStringList("*.key"));
       int nKeyFiles = keyFiles.count();
       if(absFileName.endsWith(".key") && keyFile.exists() && (nKeyFiles == 1) ) //key file with default name found 
         _KeyFound= true;

       if(_KeyFound){
          if(evaluateKeyFile(absFileName)){
            buttonNext->setEnabled(true);
            lineEditMediumPath->setPalette(PALETTE_GREEN());
            labelHint->setText("<html>"+ tr("Your key has been found. Please go to the next page. ") + "</html>");
          }
          else buttonNext->setEnabled(false);
       }
       else{ 
         on_buttonSelectFile_clicked(); //No keyfile, try to get one
         absFileName = lineEditMediumPath->text(); //get the new filename
       }
     }

     if(radioButtonExistingMedium->isChecked() && !_KeyFound) // still no key
      {
        QMessageBox::information(this, tr("Information"), tr("Could not find a *.key file in directory: ") + _MediumDir + tr("\n Please search manually."));
        labelHint->setText("<html>"+tr(
"Hint:<br>\
MoneyPenny is running on Linux. There are no drive letters.\
/ is used instead of \\ in the path.  Please try the following paths:<br>\
For floppy disk medium try /mnt/floppy or /media/floppy, for USB stick medium try /mnt/sda1 or /mnt/usb or /media/sda1 or/media/usb") +"</html>");
       absFileName = _MediumDir;
       } 
  lineEditMediumPath->setText(absFileName);
}

/** Checks the Internet setup. Code is copied from guiConnections
*/
void GuiHbciWizard::on_buttonTestInternetSetup_clicked(){
 HoBIT_Connector::ConnectionState state;
 emit testInternet(true, //show success message
                   lineEditUrl->text(), lineEditPort->text().toInt(), &state); //connected to guiMain::slotTestInternet
 switch (state){
  case HoBIT_Connector::ConnectionWasOk:{
         labelInternetState->setText(tr("Status: Internet setup ok."));
         labelInternetState->setPalette(PALETTE_GREEN());
         break;
        }
  case HoBIT_Connector::ConnectionFailed:{
        labelInternetState->setText(tr("Status: Internet setup is not ok. Cables connected?"));
        labelInternetState->setPalette(PALETTE_RED());
        break;
       }
  default:{ 
       labelInternetState->setText(tr("Status: Internet setup not yet tested."));
       labelInternetState->setPalette(PALETTE_YELLOW());
       break;
     }
 }
}

/** Calls Internet setup program.
*/
void GuiHbciWizard::slotInternetSetup(){
 emit setupInternet(); //connected to guiMain::slotHardwareConfig
}


/** show log widget
 */
void GuiHbciWizard::on_buttonDetails_clicked(){
  emit showLog(LOG_WIDGET);
}


/** reads the user's previous configuration from the OpenHbci database */  
void GuiHbciWizard::evaluateStoredData(){

  QString bankCode = HoBIT::hobit()->bankCode();
  const banking ::Bank * bank = HoBIT::hobit()->bank();
  banking::User * user  = HoBIT::hobit()->user();

  if(user){
    if(user->passportType() & PASSPORT_KEYFILE){
      QString passportFile = user->passportFile().c_str();
      qDebug("Using keyfile: %s", qPrintable(passportFile));
      lineEditMediumPath->setText(passportFile);   
      enablePage(PASSWORD_PAGE, false);
      radioButtonExistingMedium->setChecked(true);
    }
    else
      radioButtonChipCard->setChecked(true);  
  }
  else //no valid medium available
    radioButtonNewMedium->setChecked(true);

  if (bank){
    lineEditBankCode->setText(bank->bankCode().c_str());
    lineEditBankBIC->setText(bank->bankBic().c_str());
    lineEditBankName->setText(bank->bankName().c_str());
    lineEditBankServer->setText(bank->server().c_str());

    int hbciVersion = bank->hbciVersion();
    setHbciComboBox(hbciVersion);
    QList<banking::Bank>::iterator bit;
    //get the official name from the list
    bool found = false;
    for (bit = _Banks.begin(); (bit != _Banks.end()) && (!found); bit++)
    {
      QString bankCode = (*bit).bankCode().c_str();
  
      if (bankCode == lineEditBankCode->text()){
        lineEditBankName->setText((*bit).bankName().c_str());
         found = true; // stop it 
        }
     }

    if (user){
      lineEditUserId->setText(user->userId().c_str()); //this copies text to lineEditCustId via slotCopyUserId
      lineEditCustId->setText(user->customerId().c_str());  
      if(HoBIT::hobit()->accounts()->empty()) //show the account error message and the skip button
        textLabelWelcome->setText(tr(
//Do not indent this multiline string          
"<font size=\"+1\"><h2>Welcome to MoneyPenny. </h2>\
Your configuration data have been found and read.\
However, there are no accounts in your configuration data.\
If you use a keyfile security medium (RDH,)then\
most probably your key was not yet activated by the bank,\
when you used MoneyPenny for the last time.<br>\
To obtain the account list from your bank,\
send the Ini-Letter to your bank and ask them to activate your key.\
Then run this wizard again.<br>\
Most banks send the account list during the connection test.\
However, some banks do not send account data.\
In this case, you will have to fill in the account form of this wizard yourself.\
</font>" ));
        }//end if user
        else textLabelWelcome->setText(tr(
//Do not indent this multiline string          
"<font size=\"+1\"><h2>Welcome to MoneyPenny. </h2>\
 Your configuration data have been found and read.\
 However, there seems to be a problem with the customer data.\
 We suggest, that you run this wizard again.\
 </font>"
                                         ));
  }//end if bank is valid

}


/** Reads /etc/chipcard.conf and lists all card readers found*/

/** Opens card reader configuration tool */
void GuiHbciWizard::configureCardReader(){
  QProcess process(this);
  QStringList args;
  args <<"kcardsetup";
  process.start("kdesu", args );
}




/**
  *raises the add account widget 
  */
  void GuiHbciWizard::on_buttonAddAccount_clicked(){
   emit addAccount();	
  }

  
void GuiHbciWizard::updateAccountList(){
  
  bool found = false;
  listWidgetAccounts->clear();
  
  banking::AccountList::const_iterator accit;
  banking::AccountList accounts;
  int nAccounts = 0;
  banking::User * user = HoBIT::hobit()->user();
  if(user)
     accounts = user->accounts();
  else return;
  if ( accounts.empty()){
          listWidgetAccounts->addItem(tr("No accounts found"));
	 }
  else{ 
  	for (accit=accounts.begin(); accit!=accounts.end(); accit++)
             nAccounts ++;
    if(nAccounts > 1){ //not only a dummy account
      HoBIT::hobit()->deleteAccount(MONEYPENNY_DUMMY_ACCOUNT_ID);// delete dummy, if it exists
      accounts = user->accounts(); //reload list
   }
  	for (accit=accounts.begin(); accit!=accounts.end(); accit++){
             string accountId = (*accit)->accountId();
             listWidgetAccounts->addItem( QString(accountId.c_str()) );
   }
   found = true;
  }
 
  if (found){
      textLabelAccounts->setText(tr("MoneyPenny has retrieved a list of your accounts from the bank server: "));
      buttonAddAccount->hide();
  }
}



/** Changes the password for the hbci communication. Only valid for RDH files.
*/
void GuiHbciWizard::changePassword(){
  banking::User * user = HoBIT::hobit()->user();
  int passportType = 0;
  if(user)
   passportType = user->passportType();
  if(passportType & PASSPORT_SECURITY_DDV)
     {
       QMessageBox::information(this, tr("Sorry"), tr("Can not change the PIN of your chipcard." ));
     }
  else  if(passportType & PASSPORT_SECURITY_RDH)
     {
        _ChangingPassword = true;
        enablePage(PASSWORD_PAGE, true);
        enablePage(CHECK_PAGE, false);
        stackedWidgetMain->setCurrentIndex(PASSWORD_PAGE);
        pageChanged();
      }
  else //default
     {
       QMessageBox::critical(this, tr("Error"), tr("No valid security mode has been set. Please run HBCI-setup first." ));
     }
}

/** Create a new keyfile.
*/
bool GuiHbciWizard::createNewKeyfile(){
  buttonNext->setEnabled(false); //block next page
  _KernelTask = CREATE_NEW_KEYFILE;
  bool ok = true;
  readControls(); //Update all properties
  
  _HBCIVersion  = banking::Bank::convertHbciVersionString(qPrintable(comboBoxHBCIVersion->currentText()));

 //we are ready to go, check network first 
  HoBIT_Connector::ConnectionState state;
  emit testInternet(false, // no success message
                    _Server, 3000, &state);
  ok = state == HoBIT_Connector::ConnectionWasOk;

  if(ok)
  try{
       connect(HoBIT::hobit(), SIGNAL(verifyBankKeyHash(string *)), 
               this, SLOT(slotVerifyBankKeyHash(string *)),
                Qt::UniqueConnection);
       connect(HoBIT::hobit(), SIGNAL(kernelThreadFinished(bool)), 
               this, SLOT(slotKernelThreadFinished(bool)),
                Qt::UniqueConnection);
       ok =  HoBIT::hobit()->createNewKeyfile ( _Entropy,
                                        _RdhVersion, 
                                        _PassportType,
                                        _KeyDir + _MediumName,
                                        lineEditPassword1->text(),
                                        QString::number(_Country),
                                        _BankCode,
                                        _HBCIVersion,
                                        _Server,
                                        _UserId,
                                        _CustomerId);
/*      _Entropy.clear(); //delete 
      _EntropyOk = false;
      if(ok)
        ok = HoBIT::hobit()->createUser( _PassportType,
                                _MediumName,
                                QString::number(_Country),
                                _BankBIC,
                                _BankCode,
                                (BankType)comboBoxBankType->currentIndex(),
                                _HBCIVersion,
                                _RdhVersion,
                                _UserId,
                                _CustomerId,
                                _Server);
       if(_PassportType & PASSPORT_KEYFILE) //user must get the path here!
         HoBIT::hobit()->setPassportFileDir(dirName);*/
  }
  catch (exception & e){ //catch everything here and display a message
   QMessageBox::critical(this, tr("Error while creating keyfile"), e.what());
   ok = false;
  }

/*  if (ok) {
    HoBIT::hobit()->saveEnvironment();
    _NewKeyCreated = true;
    buttonNext->setEnabled(true); 
    if(DEBUG_TEST_KEYCREATION)
      QMessageBox::information(this, "DEBUG_TEST_KEYCREATION", "Keys were generated but not sent due to debug mode!");
    else QMessageBox::information(this, tr("Information"), tr("The security file was created and sent successfully"));
     emit renewHoBIT();
    }
  else QMessageBox::critical(this, tr("Error"), tr("Error while creating and sending keys."));*/
  return ok;
}

/** Changes password of current keyfile.
*/
bool GuiHbciWizard::changeKeyfilePassword(){
  if(QMessageBox::question(this, tr("Confirmation"), tr("You are about to change the password of the current key file.") +
                             tr("\nDo you really want to change the password?"), 
                             QMessageBox::No| QMessageBox::Escape , QMessageBox::Yes, QMessageBox::NoButton )
                             == QMessageBox::Yes) 
  {
     QString oldPassword = lineEditOldPassword->text();
     QString newPassword = lineEditPassword1->text();
     QString filename = lineEditMediumPath->text();
     KeyFileOpenHbci keyfile(filename, oldPassword);
     if(!keyfile.readFile()){
       QMessageBox::critical(this, tr("Error"), tr("Could not read keyfile: ") + filename + tr("\n Wrong password or wrong file format?"));
       return false;
     }
     else{
       if(keyfile.changePassword(newPassword)){
         QMessageBox::information(this, tr("Information"),tr("Your HBCI password has been changed."));
         emit(renewHoBIT()); //make shure, that any old key data are deleted
         return true;
       }
       else{
         QMessageBox::critical(this, tr("Error"), tr("Could not change password. No write access to keyfile?"));
         return false;
       }// end final error
     }// end if OK 
  }//end confirmed
  else return false;
}

/** Fires signal dialIn.
*/
void GuiHbciWizard::on_buttonDialIn_clicked(){
 emit dialIn();
}

/** Sets hbci combo.
*/
void GuiHbciWizard::setHbciComboBox(int hbciVersion){
      if(hbciVersion < 220){
        QMessageBox::warning(this,
            tr("Warning"),
            tr("The HBCI-Version documented in the bank list is too low. Trying HBCI 2.2 instead."));
        hbciVersion = 220;
      }

      switch(hbciVersion){
       case 201: comboBoxHBCIVersion->setCurrentIndex(1); break;
       case 210: comboBoxHBCIVersion->setCurrentIndex(2); break;
       case 220: comboBoxHBCIVersion->setCurrentIndex(3); break; 
       case 300: comboBoxHBCIVersion->setCurrentIndex(4); break; 
       case 400: comboBoxHBCIVersion->setCurrentIndex(5); break; 
       default:  comboBoxHBCIVersion->setCurrentIndex(0); break;    
      }
}

/**Reads the keyfile an puts data into edits.
*/
bool GuiHbciWizard::evaluateKeyFile(const QString & fileName){
    QString password = QInputDialog::getText
       (this, tr("Import keyfile"), tr("Please input the password for your existing keyfile."), QLineEdit::Password);
    KeyFileOpenHbci keyFile(fileName, password);
    if(keyFile.readFile()){
       QFileInfo info (fileName);
       QString newFileName = info.fileName(); //strip dir
       newFileName = _MediumDir + "/" + newFileName;
       qDebug("Using Keyfile: %s",qPrintable(newFileName));
 //      keyFile.invalidateInstKeys(); //delete any old bank keys, otherwise a new key will be rejected by Openhbci
 //      keyFile.writeFile(newFileName);
       lineEditMediumPath->setPalette(PALETTE_GREEN());
       lineEditMediumPath->setText(newFileName);
       buttonNext->setEnabled(true);
       lineEditBankCode->setText(keyFile.getBankCode());
       QString bankCode = keyFile.getBankCode();
       lineEditBankBIC->setText(HoBIT::hobit()->getBicForBankCode(bankCode)); //get BIC from list
       lineEditUserId->setText(keyFile.getUserId());
       if(keyFile.getRdhSecurityMode() == PASSPORT_SECURITY_RDH1_MODE)
         radioButtonRDH1->setChecked(true);
       else if(keyFile.getRdhSecurityMode() == PASSPORT_SECURITY_RDH2_MODE)
         radioButtonRDH2->setChecked(true);
       else if(keyFile.getRdhSecurityMode() == PASSPORT_SECURITY_RDH10_MODE)
         radioButtonRDH10->setChecked(true);
       else radioButtonRDH2->setChecked(true);
       return true;
   }
   else{
     QMessageBox::critical(this, tr("Error"),tr("Could not read keyfile. Wrong password or unsupported file format."));
     return false;
   }
}


/** switch to next page
*/
void GuiHbciWizard::on_buttonNext_clicked()
{
  int page = stackedWidgetMain->currentIndex();
  page++;
  while(! _ActivePages[page] && page < _ActivePages.count()) //skip deactivated pages
    page++;
  stackedWidgetMain->setCurrentIndex(page);
  pageChanged();
}

/** switch to page before
*/
void GuiHbciWizard::on_buttonBack_clicked()
{
  int page = stackedWidgetMain->currentIndex();
  page--;
  while(! _ActivePages[page] && page > 0) //skip deactivated pages
    page--;
  stackedWidgetMain->setCurrentIndex(page);
  enableFinish(false);
  pageChanged();
}

/** Activate or deactivate a page
 */
void GuiHbciWizard::enablePage( int pageNumber, bool enable){
  if((pageNumber > 0) && (pageNumber < _ActivePages.count()))
    _ActivePages[pageNumber] = enable;
}

/** Activate or deactivate finish button
 */
void GuiHbciWizard::enableFinish(bool enable){
   if(enable){
    buttonFinish->setEnabled(true);
    if(_UpdOk)
      buttonFinish->setText(tr("&Finish HBCI Setup"));
    else buttonFinish->setText(tr("&Generate Ini-letter"));
    buttonFinish->show();
    buttonNext->setEnabled(false);
    buttonNext->hide();
    buttonAbort->hide();
   }
   else{
    buttonFinish->setEnabled(false);
    buttonFinish->hide();
    buttonNext->setEnabled(true);
    buttonNext->show();
    buttonAbort->show();
   }
}

/**Shows the page at  given pageNumber
  */
void GuiHbciWizard::showPage(int pageNumber){
  stackedWidgetMain->setCurrentIndex(pageNumber);
  pageChanged();
}

/** Returns index of current page
*/
int GuiHbciWizard::currentPageIndex(){
 return stackedWidgetMain->currentIndex();
}

/*!
    \fn GuiHbciWizard::getRdhKeys()
    Runs job GetPublicBankKeys.
 */
bool GuiHbciWizard::getRdhKeys()
{
   bool ok = false; 
   HoBIT::hobit()->showLogText(tr("Adding job: Get bank's public key"));
    _KernelTask = GET_RDH_KEYS;
    ok = HoBIT::hobit()->createJobFirstKeyRequest() > 0;
    if (!ok)
       throw _ExNoJob;

    HoBIT::hobit()->showLogText(tr("Executing job queue."));
    emit sendJobsNow(&ok, tr("Asking for the bank's keys."), false); //ask guiMain to send the joblist
 //   HoBIT::hobit()->clearJobLists();
    return ok;
}

/*!
    \fn GuiHbciWizard::getPinTanSystemId()
    Runs job GetSystemId for a PIN-TAN account. 
 */
bool GuiHbciWizard::getPinTanSystemId()
{
   bool ok = false; 
   HoBIT::hobit()->showLogText(tr("Adding job: System ID Renewal."));
   ok = HoBIT::hobit()->createJobGetNewSystemId() == 1;
   HoBIT::hobit()->showLogText(tr("Executing job queue."));
   emit sendJobsNow(&ok, tr("Your computer system must be registered at the bank."), false); //ask guiMain to send the job

  if (!ok ){
       HoBIT::hobit()->showLogText(tr("Executing JobGetSystemId failed. Wrong PIN?"));
       qDebug("JobGetSystemId for PinTan failed.");
       QMessageBox::critical(this,tr("Error"), tr("Could not synchronize with the bank server.")+ "<br>"
        + tr("Did you enter a wrong PIN?"));
    } 
   return ok;
}


/*!
    \fn GuiHbciWizard::getRdhSystemId()
    Runs job GetSystemId. An activated user key is required for this to work
 */
bool GuiHbciWizard::getRdhSystemId()
{
   bool ok = false; 
   HoBIT::hobit()->showLogText(tr("Adding job: System ID Renewal."));
   _KernelTask = GET_RDH_SYSTEM_ID;
   ok = HoBIT::hobit()->createJobGetNewSystemId() == 1;
   HoBIT::hobit()->showLogText(tr("Executing job queue."));
   emit sendJobsNow(&ok, tr("Your computer system must be registered at the bank."), false); //ask guiMain to send the joblist//this may fail due to an unsent key

   if (!ok ){ //this fails for an inactive key
       HoBIT::hobit()->showLogText(tr("Executing JobGetSystemId failed. Key not yet activated?"));
       qDebug("JobGetSystemId failed. Key not yet activated?");
       QMessageBox::critical(this,tr("Error"), tr("Could not synchronize with the bank server.")+ "<br>"
        + tr("Most probably your public key was not yet sent or not activated by the bank."));
       labelSendKeys->setText(tr("Your key seems to be inactive.") + "<br>"
                             +tr("Sending key is required, if you did not yet do so."));
      // _SendingKeysSuccessful = false;
    } 
    else{ // systemId is ok
    // _SendingKeysSuccessful = true;
     labelSendKeys->setText(tr("Key is already known. Sending is not neccessary."));
    }
   return ok;
}


/*!
    \fn GuiHbciWizard::getUPD()
    Gets BPD and UPD from the bank. This does not work with an not activated key.
 */
bool GuiHbciWizard::getUpd()
{
   bool ok = true;
   HoBIT::hobit()->showLogText(tr("Adding job: Get username and accounts (UPD)"));
//    if(ok && (_PassportType & PASSPORT_SECURITY_RDH)){ //system Id required for RDH
//       ok = getRdhSystemId();
//      if(!ok) // most probably not yet an active key, we will add a dummy account automatically on finish
//       {
//         enablePage( ACCOUNTS_PAGE, false);
//       }
//     } 
//     else if(ok && (_PassportType & PASSPORT_SECURITY_OTP)){ //system Id required for Pin-Tan
//       ok = getPinTanSystemId();
//     }

   //RDH or PinTan synchronizing ok. Now try to get the accounts 
   if(ok)
     ok = (HoBIT::hobit()->createJobGetUPD() == 1);
   // now execute the job queue
   _KernelTask = GET_UPD;
   if(ok){
     HoBIT::hobit()->showLogText(tr("Executing job queue."));
     emit sendJobsNow(&ok, tr("Your account data are requested from the bank."), false); //ask guiMain to send the joblist. starts kernelthread
   }
//  HoBIT::hobit()->clearJobLists(); 
  return ok;
}

/** Write modified data to the chipcard.
 */
 void GuiHbciWizard::on_buttonWriteDataToChipcard_clicked()
{
    QString userId = QString(lineEditUserId->text());
    QString bankCode = QString(lineEditBankCode->text());
    QString bankName = QString(lineEditBankName->text());
    QString serverUrl = QString(lineEditBankServer->text());

    if(QMessageBox::question(this, tr("Write data to Chipcard?"),
    tr("Writing data to the chipcard is not neccessary, if you have not changed the data that were read from the card.")
    + "\n"
    +tr("Do you want to write the data shown on the screen to the card?"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
     return;

    bool ok = true;
    setCursor(Qt::WaitCursor);
    while(ok && bankName.length() > 20)
    {
       bankName = bankName.left(20);
       bankName = QInputDialog::getText(this,
         tr("Input Shortened Bankname"),
         tr("The bankname for the card must not be longer than 20 letters. Please enter a shortened version."), 
         QLineEdit::Normal,
         bankName,
         &ok);
     }

    if(ok)
     connect(HoBIT::hobit(), SIGNAL(kernelThreadFinished(bool)), 
             this, SLOT(slotKernelThreadFinished(bool)),
              Qt::UniqueConnection);
     try{ //many things can happen...
      _KernelTask = WRITE_DDV_CARD_DATA;
      _CardDataOk = HoBIT::hobit()->writeDDVCardData(1, //always use first record!
                             bankName,
                             bankCode,
                             userId,
                             serverUrl);
      }
      catch(exception e){
        cout << "GuiHbciWizard::on_buttonWriteDataToChipcard_clicked: Exception! what = "<< e.what() <<endl;
        _CardDataOk = false;
      }
      setCursor(Qt::ArrowCursor);//return to normal cursor anyway
}

/** Called when api asks for hash verification. If hash is ok,
  * hash string is modified to "OK".
  */
void GuiHbciWizard::slotVerifyBankKeyHash(string * hash)
{
 QString msg;
 QMessageBox::StandardButtons buttons;
 if(hash->empty()){
  msg = tr("Error. No keys received. Can not continue.");
  buttons = QMessageBox::No;
 }
 else 
 {
  msg = hash->c_str();
  buttons = QMessageBox::Yes | QMessageBox::No;
 }
   
 if(QMessageBox::question(this, tr("Check Hash Value"), 
    tr("MoneyPenny received a key from the bank with the following hash value:")
  + "\n" + msg
  + "\n" + tr("If this hash is identical with the one on the bank's letter, please click Yes.")
  + "\n" + tr("If it is not identical, click No and contact your bank immediately."),
  buttons)
  == QMessageBox::Yes)
  {
   enablePage( FINGERPRINT_PAGE, false ); //switch off fingerprint page
   buttonNext->setEnabled(true);
   *hash ="OK";
  }
  HoBIT::hobit()->wakeKernelThread();//we have copied valid input data, kernel thread may continue 
}

/**Show accounts page and enable Next button, if accountlist is not empty.
  */
void GuiHbciWizard::slotAccountAdded()
{
 stackedWidgetMain->setCurrentIndex( ACCOUNTS_PAGE );
}


/** Transfer settings of buttons and edits to class attributes for easy access.
 */
bool GuiHbciWizard::readControls()
{
    if (radioButtonChipCard->isChecked()) //evaluate medium button states
    {
      _PassportType = PASSPORT_CHIPCARD | PASSPORT_SECURITY_DDV;
      _MediumName = _CardNumber;
    }
    else if (radioButtonExistingMedium->isChecked() || radioButtonNewMedium->isChecked())
    {
      if(radioButtonRDH1->isChecked())
       {
         _RdhVersion = PASSPORT_SECURITY_RDH1_MODE;
        _PassportType = PASSPORT_KEYFILE | PASSPORT_SECURITY_RDH | KF_OPENHBCI;
       }
      else if(radioButtonRDH2->isChecked())
       {
         _RdhVersion = PASSPORT_SECURITY_RDH2_MODE;
        _PassportType = PASSPORT_KEYFILE | PASSPORT_SECURITY_RDH | KF_RDH2;
       }
      else if(radioButtonRDH10->isChecked())
       {
         _RdhVersion = PASSPORT_SECURITY_RDH10_MODE;
        _PassportType = PASSPORT_KEYFILE | PASSPORT_SECURITY_RDH | KF_RDH10;
       }
      HoBIT::hobit()->setSecProfileVersion(_RdhVersion);
    }
    else if (radioButtonPinTan->isChecked())
    {
      if(radioButtonITAN->isChecked())
       _PassportType = PASSPORT_SECURITY_OTP | PASSPORT_SECURITY_OTP_ITAN;
      else if(radioButtonMTAN->isChecked())
       _PassportType += PASSPORT_SECURITY_OTP | PASSPORT_SECURITY_OTP_MTAN;
    }
    else
    {
      QMessageBox::critical(this, tr("Data missing"), tr("A medium that contains the key must be specified."));
      return false;
    }

    // get information from the edits
     if(radioButtonPinTan->isChecked())
       _UserId = lineEditAccountPinTan->text(); //for PinTan, one of the user's account numbers is used as user ID
    else _UserId = QString(lineEditUserId->text());
    _BankCode = QString(lineEditBankCode->text());
    _BankBIC = QString(lineEditBankBIC->text());
    _Bankname = QString(lineEditBankName->text());
    _CustomerId = QString(lineEditCustId->text());
    _Server = QString(lineEditBankServer->text());
    _HBCIVersion  = banking::Bank::convertHbciVersionString(qPrintable(comboBoxHBCIVersion->currentText()));

    if(_PassportType & PASSPORT_KEYFILE)
    {    //Split the path into name and dir
      QString fileName = lineEditMediumPath->text();
      QFileInfo fileInfo(fileName);
      _KeyDir = fileInfo.absoluteDir().absolutePath();
      QFileInfo dirInfo(_KeyDir);
      if(!dirInfo.exists()){
       QMessageBox::critical(this, tr("Error"), tr("Directory: ") + _KeyDir +
       tr(" Does not exist.<br>Please go to page before, select an existing directory and try again."));
       return false;
      }
      if(fileInfo.isDir()) //repair it
        fileName += _KeyDir + "/" + lineEditUserId->text() + ".key";
      fileInfo.setFile(fileName);
      if(fileInfo.exists() && radioButtonNewMedium->isChecked())
      {
       if(QMessageBox::warning(this, tr("Warning"), tr("Key file exists.<br>\
          If you overwrite your key, you must send a new ini-letter to your bank, in order to activate the new key.<br>\
          <b>Homebanking with the new key does not work before the bank has activated it.</b>"),
          tr("Keep existing key and old password"), // button0 text
          tr("Overwrite existing key")) //button1 text
         == 0){ // keep button was clicked
             _NewKeyCreated = false;
             buttonNext->setEnabled( true);
             QMessageBox::information(this, tr("Information"), 
             tr("Using existing key file. Please proceed to next page.")); 
             return true;
         }
         else if(!fileInfo.isWritable()){
          QMessageBox::critical(this, tr("Error"), tr("No write permission to file: ") + lineEditMediumPath->text() +
          tr("<br>Please make medium writable and try again."));
          return false;
         }
      }//end if exists
      else if(!dirInfo.isWritable()){
        QMessageBox::critical(this, tr("Error"), tr("No write permission to directory: ") + _KeyDir +
        tr("<br>Please make medium writable and try again."));
        return false;
      }
      _MediumName = fileInfo.fileName(); //everything checked, use this filename
     } else _MediumName = "Chipcard";
     return true;
}


void GuiHbciWizard::slotKernelThreadFinished(bool ok)
{
  setCursor(Qt::ArrowCursor);
  HoBIT::hobit()->clearJobLists(); //lists must be cleared anyway

  switch(_KernelTask)
  {
    case CREATE_NEW_KEYFILE:
     {
      _KernelTask = NO_TASK; //clear _KernelTask
      _Entropy.clear(); //delete 
      _EntropyOk = false;
      if(ok)
        ok = HoBIT::hobit()->createUser( _PassportType,
                                _MediumName,
                                QString::number(_Country),
                                _Bankname,
                                _BankBIC,
                                _BankCode,
                                (BankType)comboBoxBankType->currentIndex(),
                                _HBCIVersion,
                                _RdhVersion,
                                _UserId,
                                _CustomerId,
                                _Server);
         HoBIT::hobit()->setPassportFileDir(_KeyDir); //give it to the new user
         if (ok) {
           HoBIT::hobit()->saveEnvironment();
           _NewKeyCreated = true;
           buttonNext->setEnabled(true); 
           if(DEBUG_TEST_KEYCREATION)
             QMessageBox::information(this, "DEBUG_TEST_KEYCREATION", "Keys were generated but not sent due to debug mode!");
           else QMessageBox::information(this, tr("Information"), tr("The security file was created and sent successfully"));
            emit renewHoBIT();
           }
         else QMessageBox::critical(this, tr("Error"), tr("Error while creating and sending keys.")); 

         buttonNext->setEnabled(ok); 
     }break;

    case GET_RDH_KEYS:{
     _KernelTask = NO_TASK; //clear _KernelTask
     HoBIT::hobit()->clearJobLists();
      QString hash = HoBIT::hobit()->bankFingerPrint();
      if(hash.isEmpty())
      qDebug("ERROR: No bank hash!");
      else qDebug( "Bank Hash: %s", qPrintable(hash));
      textLabelBankHash->setText(hash);
       if(ok){
         getRdhSystemId();
       }
    }break;

    case GET_RDH_SYSTEM_ID:{
      _KernelTask = NO_TASK; //clear _KernelTask
      if (!ok ){ //job fails for an inactive key
           HoBIT::hobit()->showLogText(tr("Executing JobGetSystemId failed. Key not yet activated?"));
           qDebug("JobGetSystemId failed. Key not yet activated?");
           QMessageBox::critical(this,tr("Error"), tr("Could not synchronize with the bank server.")+ "<br>"
            + tr("Most probably your public key was not yet sent or not activated by the bank."));
           labelSendKeys->setText(tr("Your key seems to be inactive.") + "<br>"
                                 +tr("Sending key is required, if you did not yet do so."));
          // _SendingKeysSuccessful = false;
           buttonNext->setEnabled(true); //enable next anyway
        } 
        else{ // systemId is ok
        // _SendingKeysSuccessful = true;
         labelSendKeys->setText(tr("Key is already known. Sending is not neccessary."));
         getUpd(); //try to get accounts
        }
    }break;

    case GET_PIN_TAN_SYSTEM_ID:{
       _KernelTask = NO_TASK; //clear _KernelTask
       if (!ok ){
            HoBIT::hobit()->showLogText(tr("Executing JobGetSystemId failed. Wrong PIN?"));
            qDebug("JobGetSystemId for PinTan failed.");
            QMessageBox::critical(this,tr("Error"), tr("Could not synchronize with the bank server.")+ "<br>"
             + tr("Did you enter a wrong PIN?"));
         } 
        else getUpd();
    }break;

    case GET_UPD:{
        _KernelTask = NO_TASK; //clear _KernelTask
        if(ok){
          _UpdOk = true;
          updateAccountList();
           HoBIT::hobit()->saveKeyfile(lineEditMediumPath->text()); //we have got a valid system Id, save it
           labelUpd->setPalette(PALETTE_GREEN());;
           labelUpd->setText(tr("A set of user data was transferred."));
        }
        else{ //no Upd
           labelUpd->setPalette(PALETTE_YELLOW());;
           labelUpd->setText(tr("No user data were received. This may be due to an inactive key.") +"\n"+
                             tr("You may proceed anyway."));
           buttonNext->setEnabled(true); //enable next anyway
         }

       if(ok && ! _NewKeyCreated)
         HoBIT::hobit()->updateSystemId();//copy system id from BPD now and use it for next job

       if (ok && (_PassportType & PASSPORT_SECURITY_RDH)){   
          //Keys should have been sent by createNewKeys, 
           if(! _NewKeyCreated)
             labelSendKeys->setText(tr("If you did already send your key, you need not send it again."));
/*           QString hash = HoBIT::hobit()->bankFingerPrint();
           if(hash.isEmpty())
              qDebug("ERROR: No bank hash!");
           else qDebug( "Bank Hash: %s", qPrintable(hash));
           textLabelBankHash->setText(hash);*/
       }

       if(! ok && (_PassportType & PASSPORT_CHIPCARD))
        QMessageBox::critical(this, tr("Error"), 
         tr("Job execution failed.") +" <br><b>" 
         +tr(" Does your chipcard contain HBCI-userId, bankcode and bankserver?") + "</b><br>" + "</b><br>" 
         +tr("If not, go back and let MoneyPenny write the data to your card."));

       if(!ok && _PassportType & PASSPORT_SECURITY_RDH)
        QMessageBox::critical(this, 
         tr("Possible error reasons"),
         "<html>"
         + tr("Did you already send (or give) your ini-letter to the bank?") 
         + tr("If not, please continue with this setup, and let MoneyPenny generate and print your ini-letter.")+ "<br>"
         + tr("Has your key already been activated by the bank?") + "<br>"
         + "</html>");

       //finally set the controls. 
        if (ok){
          HoBIT::hobit()->showLogText(tr("Execution terminated successfully.")); 
          textLabelTestConnection->setText(tr("Connection test was successful."));
          textLabelTestConnection->setPalette(PALETTE_GREEN());
          _ConnectionTestSuccessful = true;
          buttonNext->setEnabled(true);
        }
        else{ //not ok
          textLabelTestConnection->setText(tr("failed"));
          textLabelTestConnection->setPalette(PALETTE_RED());
          _ConnectionTestSuccessful = false;
          buttonNext->setEnabled(true); //allow user to proceed to ini-letter
         }
        #ifdef DEBUG_HBCI_WIZARD 
              buttonNext->setEnabled(true); //enable next, although test must fail due to disabled sending
        #endif
        update();
     }break;

    case WRITE_DDV_CARD_DATA:{
        _KernelTask = NO_TASK; //clear _KernelTask
        _CardDataOk = ok;
        if(_CardDataOk)
         QMessageBox::information(this, tr("Information"), tr("Data were written to the card."));
        else
         QMessageBox::warning(this, tr("Warning"), tr("No data were written to the card."));
        buttonNext->setEnabled(_CardDataOk);
     }break;

    default: { 
       QString msg;
       if(ok)
        msg = "OK";
       else msg = "Error";
       QMessageBox::information(this, "Thread terminated", msg);
       } break;
  }
}
