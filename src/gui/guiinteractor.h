/***************************************************************************
                          hobit_interactor.h  -  description
                             -------------------
    begin                : Do Aug 19 2004
    copyright            : (C) 2004 by Marcel Mulch, Walter Roth
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

#ifndef GUIINTERACTOR_H
#define GUIINTERACTOR_H

#include <openhbci2.h>
#include <openhbci2/pointer.h>
using namespace HBCI;

#include <QWidget>
#include <QTimer>

#include "dlgenterpinkeypad.h"
//#include <termios.h>


class DlgEnterPinKeypad;

/** This class is neccessary for every kind of user Interaction.
  * For example it asks the user for entering the pin file or mounting
  * the security medium. It is a designed to be a member of HoBIT_Interactor. 
  * This avoids dual inheritance which causes trouble with Qt fighting against HBCI.
  * Al functions must be called from HoBIT_Interactor.
  *
  * @author Walter Roth
  */
class GuiInteractor : public QObject {
 Q_OBJECT 
public: 
  /**
  * base constructor
  * creates a new interactor
  */
  GuiInteractor(QWidget *parent, HBCI::API *api );
  ~GuiInteractor();
  

/** Tell the user to insert a chip card. The default
  * implementation does nothing and returns false.
  *
  * @param user user for whom the card is to be inserted.
  *  Please note that this pointer may be invalid.
  * @param t The MediumType of the medium that should be inserted.
  * @author Marcel Mulch (marcel@mulch-online.de)
  * @return true if ok, false if user wants to abort.
  */
  virtual bool msgInsertMediumOrAbort(HBCI::Pointer<HBCI::User> user, MediumType t);


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
  virtual bool msgInsertCorrectMediumOrAbort(HBCI::Pointer<HBCI::User> user, MediumType t);



  /**
   * Ask the user for the pin of a security medium. This method is
   * called by the default Authentifikator. The default
   * implementation does nothing and returns false.
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
  virtual bool msgInputPin(HBCI::Pointer<HBCI::User> user,
                           string &pin,
                           int minsize,
                           bool newPin,
                           int trials);

   /** Write property of QString _NewPassword. If _NewPassword is set, password dialog is skipped
   */
   void setNewPassword( const QString& _newVal); 
   
   
  /**
   * This method is called directly before the user is supposed to enter the
   * pin into the keypad of a card reader. This is used to inform the user
   * that OpenHBCI started secure pin verification. A graphical application
   * might want to open a window that tells the user about this.
   * (new since 0.9.5)
   * @author Marcel Mulch 
   * @param user user for whom the pin is to be asked for.
   * Please note that this pointer may be invalid.
   */
  virtual void msgStartInputPinViaKeypad(Pointer<HBCI::User> user,int trials);


  /**
   * This method is called directly after the secure pin verification (via
   * the card readers keypad) is finished. A graphical applicatition most
   * likely closes the notification window opened at
   * @ref msgStartInputPinViaKeypad().
   * (new since 0.9.5)
   * @author Marcel Mulch
   * @param user user for whom the pin is to be asked for.
   * Please note that this pointer may be invalid.
   */
  virtual void msgFinishedInputPinViaKeypad(Pointer<HBCI::User> user);
    

  /** read from stdin
  */
  char _readCharFromStdin();


  /** Read property of int _SecurityMode. 
  */
  int  getSecurityMode()const;
  /** Write property of int _SecurityMode. 
  */
  void setSecurityMode( int _newVal);
  
public slots:
    /** Callback function for_Timer. Sets  _Wait to false.
   */
   void slotTimerDone();
   
private:
  /**Warns the user before retrying to enter a PIN
   * Ultimate warning for bad PIN. Exits program, if requested.
   */
   void warnUser();

  /** input the pin code */
     bool _inputPin(string &pin, bool r);
     char readFromStdin();
  
  /** Pointer on the hbci wrapper */
//  HBCI_API *_api;
  
  /** Application Main Widget */
  QWidget* _Parent;
  /** The "Enter PIN on the Keypad" widget to be shown
  */
  DlgEnterPinKeypad* _DlgEnterPinKeypad; 
  /** _NewPassword is needed for callback function msgInputPin
  */
  QString _NewPassword;
  
  /**_SecurityMode used for asking for a password instaed of a PIN
  */
  int _SecurityMode;
  
  /** Timer for drawing
  */
  QTimer * _Timer;
  /** flag for waiting for timer
  */
  bool _Wait;
};

#endif



