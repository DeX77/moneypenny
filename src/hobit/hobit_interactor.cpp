/***************************************************************************
                          hobit_interactor.cpp  -  description
                             -------------------
    begin                : Do Aug 19 2004
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

#include "hobit_interactor.h"
#include <string.h>
#include "dlgenterpinkeypad.h"
#include "guiinteractor.h"

#define HOBIT_INTERACTOR_ABORT_CHAR 27
#define HOBIT_INTERACTOR_DELETE_CHAR 127
#define HOBIT_INTERACTOR_ENTER_CHAR 10


#include <termios.h>

/**                                                                
  * base constructor
  * creates a new interactor
  */
HoBIT_Interactor::HoBIT_Interactor( QWidget *parent, HBCI::API *api)
        : HBCI_Interactor()
{
    _api = api;
    _parent = parent;  
    _Gui = new GuiInteractor(_parent, _api);
    _PinInputTrials=0;    
    qDebug("Constructor HoBIT_Interactor");   
}


/**
 * base destructor
 */
HoBIT_Interactor::~HoBIT_Interactor(){
  //no need to delete timer, this is done by parent
  qDebug("Destructor HoBIT_Interactor");
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
bool HoBIT_Interactor::msgInsertMediumOrAbort(HBCI::Pointer<HBCI::User> user, MediumType mediumtype){

    return _Gui->msgInsertMediumOrAbort(user, mediumtype);

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
bool HoBIT_Interactor::msgInsertCorrectMediumOrAbort(HBCI::Pointer<HBCI::User> user, MediumType type){
   
    return _Gui->msgInsertCorrectMediumOrAbort(user, type);
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
  bool HoBIT_Interactor::msgInputPin(HBCI::Pointer<HBCI::User> user,
                                     string &pin,
                                     int minsize,
                                     bool newPin){
    
    return _Gui->  msgInputPin( user, pin, minsize, newPin, _PinInputTrials);  
    _PinInputTrials++;                               
  }


/** Write property of QString m__NewPassword. 
*/
void HoBIT_Interactor::setNewPassword( const QString& _newVal){
  _Gui->setNewPassword (_newVal);
}


void HoBIT_Interactor::msgStartInputPinViaKeypad(Pointer<HBCI::User> user){
    _Gui->msgStartInputPinViaKeypad(user, _PinInputTrials);
}

void HoBIT_Interactor::msgFinishedInputPinViaKeypad(Pointer<HBCI::User> user){
  _Gui->msgFinishedInputPinViaKeypad(user);
  _PinInputTrials++;
}

/** Read property of int m__SecurityMode. 
*/
int  HoBIT_Interactor::getSecurityMode()const{
	return _Gui->getSecurityMode();
}

/** Write property of int m__SecurityMode. 
*/
void HoBIT_Interactor::setSecurityMode( int  _newVal){
	_Gui->setSecurityMode (_newVal);
}


/** Sets the counter to zero.
*/ 
void HoBIT_Interactor::resetPinInputTrials(){
 _PinInputTrials = 0;
}
