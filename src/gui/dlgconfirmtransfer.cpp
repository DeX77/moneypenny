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
 
#include "dlgconfirmtransfer.h"
#include "qtextedit.h" 
 
DlgConfirmTransfer:: DlgConfirmTransfer ( QWidget * parent, QString text)
      :QDialog(parent){
 setupUi(this);
 _UserCommand = Reject;
 textEditTransfer->setText( text );
}

DlgConfirmTransfer::~DlgConfirmTransfer(){
}
 
void DlgConfirmTransfer::on_buttonSendNow_clicked(){
   _UserCommand = SendNow;
   accept();
} 

void DlgConfirmTransfer::on_buttonSendLater_clicked(){
    _UserCommand = SendLater;
    accept();
} 

void DlgConfirmTransfer::on_buttonSendAndGet_clicked(){
    _UserCommand = SendNowAndGetTransactions;
    accept();
} 


void DlgConfirmTransfer::on_buttonCancel_clicked(){
  reject();
}

/** The command given by the user. May be
*  NoCommand, SendLater, SendNow, SendNowAndGetTransactions
*/
int DlgConfirmTransfer::getUserCommand(){
  return _UserCommand;
}  
