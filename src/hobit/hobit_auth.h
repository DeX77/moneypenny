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
#ifndef AUTHHOBIT_AUTH_H
#define AUTHHOBIT_AUTH_H

#include <openhbci2/auth.h>
#include <openhbci2/api.h>
#include <openhbci2.h>
using namespace HBCI;

#include <gwenhywfar/db.h>

#include <qwidget.h>


/**
* This class is used for retrieving a pin from a chipcard terminal
* @author Marcel Mulch
*/
class HoBIT_Auth : public Auth{

private:
  GWEN_DB_NODE *_secrets;
  QWidget *_parent;
  
  public:
  /**
   * Constructor.
   * @author Martin Preuss<openhbci@aquamaniac.de>
   * @param hbci pointer to central HBCI object
   */
  HoBIT_Auth(API *hbci, QWidget* parent);

  virtual ~HoBIT_Auth();

  /**
   * This method retrieves a PIN and writes it into the string
   * reference argument 'secret'.
   *
   * The default implementation calls @ref
   * Interactor::msgInputPin().  If you want to retrieve the PIN
   * by any other means, you can provide your own derivation of
   * Auth and override this method.  You may use the given token
   * to identify the secret.
   *
   * @author Martin Preuss<openhbci@aquamaniac.de>
   *
   * @return returns a Error object to show the result. Call
   * "isOk()" on it, to see if any error occurred.
   *
   * @param user pointer to the user for whom the secret is to be retrieved
   * From this user object you can get all information you might need to
   * show an explanatory dialog.
   *
   * @param token A string providing additional information to
   * identify the PIN OpenHBCI is asking for. When a MediumDDV
   * (chip card) asks for a pin, it shows here the "cardnumber",
   * whereas a MediumRDH presents the "path/filename" of the
   * medium to be accessed.
   *
   * @param secret reference to a string to receive the secret
   * (e.g. the pin).
   *
   * @param createSecret if true then a new secret is to be created.
   * In this case the user should be asked to enter the pin twice.  */
  virtual Error getSecret(Pointer<User> user,
                          const string &token,
                          string &secret,
                          bool createSecret=false);

  /**
   * Sets the status of the secret. This can be used after verifying a PIN
   * retrieved via @ref getSecret to tell the authentificator about the
   * status of the PIN.
   * If the status is @ref HBCI_AUTH_STATUS_BAD then the authentificator
   * should not return the same secret for the given token again !
   */
  void setSecretStatus(Pointer<User> user,
                       const string &token,
                       string &secret,
                       HBCI_AUTH_STATUS st);

  /**
   * Returns the status of the specified secret.
   */
  HBCI_AUTH_STATUS getSecretStatus(Pointer<User> user,
                                   const string &token,
                                   string &secret);

  /**
   * Reads the pin list
   */
  Error readPinList(const string &fname);


};


#endif
