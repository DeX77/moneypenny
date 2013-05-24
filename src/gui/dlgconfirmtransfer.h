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
#ifndef DLGCONFIRMTRANSFER_H
#define DLGCONFIRMTRANSFER_H

#include "ui_dlgconfirmtransferbase.h"

#include <QDialog>
/** This is a simple dialog that shows the data of a transaction and
 * asks the user what to do with it.
 */
 
class DlgConfirmTransfer: public QDialog, public Ui::DlgConfirmTransfer {
 Q_OBJECT
public:
 DlgConfirmTransfer ( QWidget * parent, QString text = "No Text");
 ~DlgConfirmTransfer();
 
 /** Returns the user command id.
  */ 
 int getUserCommand();

  static const int Reject = 0;
  static const int SendLater = 1;
  static const int SendNow = 2;
  static const int SendNowAndGetTransactions = 3; 

 protected slots:  
    void on_buttonSendNow_clicked(); 
    void on_buttonSendLater_clicked(); 
    void on_buttonSendAndGet_clicked();
    void on_buttonCancel_clicked();
     
 private: 
   /** The command given by the user. May be
    *  NoCommand, SendLater, SendNow, SendNowAndGetTransactions
    * */
   int _UserCommand;  
};    
#endif /*DLGCONFIRMTRANSACTION_H_*/
