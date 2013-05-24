/***************************************************************************
                          hobit_interactor.cpp  -  description
                             -------------------
    begin                : Do Aug 19 2004
    copyright            : (C) 2004 by Marcel Mulch, 2005 Walter Roth
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

#include "guiinteractor.h"
#include "globals.h"

#include <qmessagebox.h>
#include <string.h>
#include <qinputdialog.h>
#include "dlgenterpinkeypad.h"
#include <qapplication.h>

//#define BUFSIZ 32

#define HOBIT_INTERACTOR_ABORT_CHAR 27
#define HOBIT_INTERACTOR_DELETE_CHAR 127
#define HOBIT_INTERACTOR_ENTER_CHAR 10


#include <termios.h>

/**                                                                
  * base constructor
  * creates a new interactor
  */
GuiInteractor::GuiInteractor( QWidget *parent, HBCI::API *api)
        : QObject()
{
   // _Api = api;
    _Parent = parent;
     setObjectName("GuiIteractor"); 
    _DlgEnterPinKeypad = new DlgEnterPinKeypad(parent,"Enter PIN:");
     _SecurityMode = HBCI_SECURITY_RDH;
    _Timer = new QTimer(this);
    _Timer->setObjectName("Timer_For_Interactor");
    connect(_Timer, SIGNAL(timeout()), this, SLOT(slotTimerDone()));
    _Wait = false;
    qDebug("Constructor GuiInteractor");
}


/**
 * base destructor
 */
GuiInteractor::~GuiInteractor(){
  //no need to delete timer, this is done by parent
  qDebug("Destructor GuiInteractor");
}


/** Tell the user to insert a chip card. The default
  * implementation does nothing and returns false.
  *
  * @param user user for whom the card is to be inserted.
  *  Please note that this pointer may be invalid.
  * @param t The MediumType of the medium that should be inserted.
  * @author Marcel Mulch (marcel@mulch-online.de)
  * @return true if ok, false if user wants to abort.
  */
bool GuiInteractor::msgInsertMediumOrAbort(HBCI::Pointer<HBCI::User> user, MediumType mediumtype){

    if(true){ //ToDo: Check for inserted medium 
    HBCI::Pointer<HBCI::Bank> bank;
    QString msg;

    msg="<qt>";
    switch(mediumtype){
      case MediumTypeCard: msg+=QWidget::tr("If you have not yet done so, please insert your chipcard for");break;
      case MediumTypeFile:{ 
        if(user.isValid()){
           HBCI::Pointer<HBCI::Medium> medium = user.ref().medium();
           QString fileName = medium.ref().mediumName().c_str();
           msg+=
           QWidget::tr("Can not find keyfile:") + fileName + "\n" +
           QWidget::tr("If you have not yet done so, please insert your floppy disc or USB-stick containing the key for");
        }
        else
         msg= tr("If you have not yet done so, please insert your floppy disc or USB-stick for");
        break;
        }
      default: msg+=QWidget::tr("Please insert the key medium for");break;
    }
    if (user.isValid()){
      bank = user.ref().bank();
      if (bank.isValid()){
        if (bank.ref().name().empty())
          msg = msg + " " + tr("your bank");
        else msg = msg + " " + bank.ref().name().c_str();
       }
       else msg = msg + " " + QWidget::tr("your bank");
    }
    else msg = msg + " " + QWidget::tr("your bank");
    msg+="<br>";
    if(QMessageBox::information(_Parent, tr("Information"), msg,
        tr("&Cancel"), tr("&Ok"), "", 1, 0) == 1)
     return true;
    else return false;
    }
    else return true;
}


/**
   * Tell the user that:<ul>
   * <li>the currently inserted chip card is not the right one and</li>
   * <li>he/she should now insert the correct card</li>
   * </ul> The default
   * implementation does nothing and returns false.
   *
   * @author Marcel Mulch (marcel@mulch-online.de)
   * @param t The MediumType of the medium that should be inserted.
   * @return true if ok, false if user wants to abort.
   * @param user user for whom the card is to be inserted.
   * Please note that this pointer may be invalid.
   */
bool GuiInteractor::msgInsertCorrectMediumOrAbort(HBCI::Pointer<HBCI::User> user, MediumType t){

    HBCI::Pointer<HBCI::Bank> bank;
    QString msg;

    msg="<qt>";
    msg+=QWidget::tr("Please insert correct medium for");

    if (user.isValid()){
      bank = user.ref().bank();
      if (bank.isValid()){
        if (bank.ref().name().empty())
          msg = msg + " " + tr("your bank");
          else msg = msg + " " + bank.ref().name().c_str();
        }
        else msg = msg + " " + QWidget::tr("<unknown Bank>");
      }
     else msg= msg + " " + QWidget::tr("<Invalid user>");

    msg+="<br>";

    QMessageBox mbox(QString("Information"),
                     QString(msg),
                     QMessageBox::Information,
                     QMessageBox::Ok | QMessageBox::Default,
                     QMessageBox::Abort,
                     QMessageBox::NoButton);

    return (mbox.exec() == QMessageBox::Ok);
}



  /**
   * Ask the user for the pin of a security medium. This method is
   * called by the default Authentifikator. 
   * If _NewPassord is set, the user dialog is skipped and _NewPassword is used.
   *
   * @author Martin Preuss<martin@libchipcard.de>
   * @return true if ok, false if the user aborted
   * @param user user for whom the pin is to be asked for.
   * Please note that this pointer may be invalid.
   * @param pin reference to a variable to receive the pin
   * @param minsize minimum size of the pin.
   * @param newPin if true then the user should be asked for a new pin.
   * Most implementations will ask the user for the pin and then ask him
   * to confirm it (thus making the user doubly enter the pin).  */
  bool GuiInteractor::msgInputPin(HBCI::Pointer<HBCI::User> user,
                                     string &pin,
                                     int minsize,
                                     bool newPin,
                                     int trials){

  cerr << "Opening PIN-Dialog\n";

  HBCI::Pointer<HBCI::Bank> bank;
  QString msg;
  // should be changed later
  newPin = false;

  pin.erase();

  msg += "<qt>";
   if(_SecurityMode == HBCI_SECURITY_RDH)
    msg += tr("Please enter the password for the private HBCI key for");
  else{ 
    cerr <<"GuiInteractor::msgInputPin: Trials:" <<trials << endl; 
    if(trials >1)
      warnUser();
    msg += tr("Please enter the PIN for the chipcard from");
  }  
  //get bankname and build the message 
  QString bankNameA =  tr("your bank.", " last part of: Please enter the password for the private HBCI key for your bank.");     
  QString bankNameD =  tr("your bank.", " last part of: Please enter the PIN for the chipcard from your bank.");     
  if (user.isValid()){
      bank = user.ref().bank();
      if (bank.isValid()){
        if (!bank.ref().name().empty())
          bankNameA = bank.ref().name().c_str();
          bankNameD = bankNameA;
      }
      else{ 
       bankNameD = QWidget::tr("<unknown Bankname>");
       bankNameD = bankNameA;
      }
   }
  
   if(_SecurityMode == HBCI_SECURITY_RDH)
        msg+=" " + bankNameA + "<br>";
   else   
        msg+=" " + bankNameD + "<br>";
        
   while(1){ //loop until a password with correct length has been input
    if (! newPin){
      bool ok = false;
      QString answer;
      if(_NewPassword.isEmpty()) //have we already got a password from elsewhere ?
       answer = QInputDialog::getText(_Parent,  //no, ask user
                                    QString("MoneyPenny"), //title
                                    QString(msg),          //label text
                                    QLineEdit::Password,   //mode
                                    "",                    //start text
                                    &ok);                  //pointer to error flag
      else {
        answer = _NewPassword;  //yes, use the supplied password
        ok = true;
      }
      if (ok && !answer.isEmpty())
        pin = string(answer.toLatin1().data());
      if (!ok)
        return false;
    }
    else ; //Changing password is done in GuiMain without using OpenHbci
      // else is still missing

    qDebug("PIN minsize= %i", minsize); 
    if ((int) pin.length()>= minsize)
      break;
    else {
        qDebug("Too short PIN: %s minsize= %i", pin.c_str(), minsize); //To be removed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        QString msg2;
        if(_SecurityMode == HBCI_SECURITY_RDH)
          msg2 = tr("The password is too short.");
        else
          msg2 = tr("The PIN is too short.");
        QMessageBox::critical( 0,
                               "MoneyPenny",
                               msg2,
                               QWidget::tr("Ok"),0,0,0);
    }
  }//end while
    return true;
}


/** Write property of QString m__NewPassword. 
*/
void GuiInteractor::setNewPassword( const QString& _newVal){
  _NewPassword = _newVal;
}


void GuiInteractor::msgStartInputPinViaKeypad(Pointer<HBCI::User> user,
                                               int trials){
  cerr <<"GuiInteractor::msgStartInputPinViaKeypad: Trials:" <<trials << endl; 
  if(trials >1)
      warnUser();
    _DlgEnterPinKeypad->show();    
    _Wait = true;
    _Timer->start(1000); //wait a second
    qDebug ("GuiInteractor::msgStartInputPinViaKeypad: Waiting for timer.");
    while(_Wait)  
      qApp->processEvents();
    _Timer->stop();
    qDebug ("GuiInteractor::msgStartInputPinViaKeypad: Continuing.");
}

void GuiInteractor::msgFinishedInputPinViaKeypad(Pointer<HBCI::User> user){
  _DlgEnterPinKeypad->hide();
}





bool GuiInteractor::_inputPin(string &pin, bool r) {
  HBCI::Pointer<HBCI::Bank> b;
  string msg;
  char buffer[BUFSIZ];
  struct termios OldInAttr, NewInAttr;
  struct termios OldOutAttr, NewOutAttr;
  int AttrInChanged = 0;
  int AttrOutChanged = 0;
  int chr;
  unsigned int pos;
  bool rv;
#ifdef HAVE_SIGNAL_H
  sigset_t snew, sold;
#endif

  if (r)
    fprintf(stderr," Repeat PIN: ");
  else
    fprintf(stderr," PIN: ");

  // if possible, disable echo from stdin to stderr during password
  // entry
#ifdef HAVE_SIGNAL_H
  sigemptyset(&snew);
  sigaddset(&snew, SIGINT);
  sigaddset(&snew, SIGSTOP);
  sigprocmask(SIG_BLOCK, &snew, &sold);
#endif

  if (0 == tcgetattr (fileno (stdin), &OldInAttr)){
    NewInAttr = OldInAttr;
    NewInAttr.c_lflag &= ~ECHO;
    NewInAttr.c_lflag &= ~ICANON;
    tcsetattr (fileno (stdin), TCSAFLUSH, &NewInAttr);
    AttrInChanged = !0;
  }
  if (0 == tcgetattr (fileno (stderr), &OldOutAttr)){
    NewOutAttr = OldOutAttr;
    NewOutAttr.c_lflag &= ~ICANON;
    tcsetattr (fileno (stderr), TCSAFLUSH, &NewOutAttr);
    AttrOutChanged = !0;
  }

  // only show asterisks, don't use gets !
  pos=0;
  rv=false;
  for (;;) {
    chr=getchar();
    if (chr==HOBIT_INTERACTOR_DELETE_CHAR) {
      if (pos) {
        pos--;
        fprintf(stderr, "%c %c", 8, 8);

      }
    }
    else if (chr==HOBIT_INTERACTOR_ENTER_CHAR) {
      rv=true;
      fprintf(stderr, "\n");
      buffer[pos]=0;
      break;
    }
    else {
      if (pos<sizeof(buffer)) {
        if (chr==HOBIT_INTERACTOR_ABORT_CHAR) {
          DBG_INFO(0, "User aborted");
          rv=false;
          break;
        }
        else {
          fprintf(stderr, "*");
          buffer[pos++]=chr;
          buffer[pos]=0;
        }
      }
      else {
        // buffer full
        fprintf(stderr, "\007");
      }
    }
  } // for

  // re-enable echo (if previously disabled)
  if (AttrOutChanged)
    tcsetattr (fileno (stderr), TCSADRAIN, &OldOutAttr);
  if (AttrInChanged)
    tcsetattr (fileno (stdin), TCSADRAIN, &OldInAttr);

#ifdef HAVE_SIGNAL_H
  sigprocmask(SIG_BLOCK, &sold, 0);
#endif

  for (unsigned int i=0; i<BUFSIZ; i++)
    if (buffer[i]<32) {
      buffer[i]=0;
      break;
    }
  pin=buffer;

  // aborted ?
  if (pin.length()==0)
    return false;

  // done
  return rv;
}

  /* read from stdin
  */
  char GuiInteractor::_readCharFromStdin() {
  int chr;
  struct termios OldAttr, NewAttr;
  int AttrChanged = 0;
#ifdef HAVE_SIGNAL_H
  sigset_t snew, sold;
#endif

  // disable canonical mode to receive a single character
#ifdef HAVE_SIGNAL_H
  sigemptyset(&snew);
  sigaddset(&snew, SIGINT);
  sigaddset(&snew, SIGSTOP);
  sigprocmask(SIG_BLOCK, &snew, &sold);
#endif
  if (0 == tcgetattr (fileno (stdin), &OldAttr)){
    NewAttr = OldAttr;
    NewAttr.c_lflag &= ~ICANON;
    NewAttr.c_lflag &= ~ECHO;
    tcsetattr (fileno (stdin), TCSAFLUSH, &NewAttr);
    AttrChanged = !0;
  }

  for (;;) {
    chr=getchar();
    if (chr==-1 ||
        chr==HOBIT_INTERACTOR_ABORT_CHAR ||
        chr== HOBIT_INTERACTOR_ENTER_CHAR)
      break;
  }

  // re-enable canonical mode (if previously disabled)
  if (AttrChanged)
    tcsetattr (fileno (stdin), TCSADRAIN, &OldAttr);

#ifdef HAVE_SIGNAL_H
  sigprocmask(SIG_BLOCK, &sold, 0);
#endif

  return chr;
}


/** Read property of int m__SecurityMode. 
*/
int  GuiInteractor::getSecurityMode()const{
	return _SecurityMode;
}

/** Write property of int m__SecurityMode. 
*/
void GuiInteractor::setSecurityMode( int  _newVal){
	_SecurityMode = _newVal;
}
/** Callback function for_Timer. Sets  _Wait to false.
*/
void GuiInteractor::slotTimerDone(){
 _Wait = false;
}

/**Warns the user before retrying to enter a PIN
 * Ultimate warning for bad PIN. Exits program, if requested.
 */
void GuiInteractor::warnUser(){
     QString msg ="<html><h2>"+ tr("WARNING") +"</h2>" +tr("The last PIN you input may have been wrong." )+"<br>";
     msg +=tr("Make shure that the right card is in your reader and check the PIN before you enter it again.") + "\n";
     msg +=tr("It is also possible, that the card and / or the reader are not connected properly or are defect.") +"\n";
     msg += "<b>" +tr("After the third wrong PIN the card will destroy itself.") + "</b><br>";
     msg += tr("It might be better to stop the program and have your card checked by your bank.")+ "</html>";
     if (QMessageBox::warning(NULL, 
                               tr("Warning"), 
                               msg,
                               tr("Abort program at once"), 
                               tr("Retry PIN input")) == 0){
        qDebug("*********Aborted due to wrong PIN*******");
        exit (EXITCODE_WRONG_PASSWORD);
      }

}

