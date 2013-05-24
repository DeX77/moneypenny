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
  
  #include "guishowbankdata.h"
  #include "hobit.h"
  
  #include <qlistview.h>
  #include <qstring.h>


  
/** Constructor
*/
GuiShowBankData::GuiShowBankData(QWidget *parent)
 : QWidget(parent)
{
 setupUi(this);
 setObjectName("GuiShowBankData");
 qDebug("Constructor GuiShowBankData");
}

/**Destructor
*/
GuiShowBankData::~GuiShowBankData()
{
 qDebug("Destructor GuiShowBankData");
}

/** Emits signal back.
*/
void GuiShowBankData::slotBack(){
 emit back();
}

/** Updates the data listview.
*/
void GuiShowBankData::refresh(){
 updateList();
}

/**
* updates the list of users
*/
void GuiShowBankData::updateList()
{
  tableWidget->clear();

  banking::Bank * bank = HoBIT::hobit()->currentBank();
  if(bank){
    tableWidget->setItem(0, 0,new QTableWidgetItem(tr("Bank:\t")));
    tableWidget->setItem(0, 1,new QTableWidgetItem(bank->bankName().c_str()));
    tableWidget->setItem(1, 0,new QTableWidgetItem(tr("Bankcode:\t")));
    tableWidget->setItem(1, 1,new QTableWidgetItem(bank->bankCode().c_str()));
    tableWidget->setItem(6, 0,new QTableWidgetItem(tr("Used HBCI-Version:\t")));
    tableWidget->setItem(6, 1,new QTableWidgetItem(bank->hbciVersion()));

    banking::User * user = bank->currentUser();
    if(user){
       tableWidget->setItem(2, 0,new QTableWidgetItem(tr("UserId:\t")));
       tableWidget->setItem(2, 1,new QTableWidgetItem(user->userId().c_str()));
       tableWidget->setItem(3, 0,new QTableWidgetItem(tr("Medium type for key:\t")));
       tableWidget->setItem(3, 1,new QTableWidgetItem(QString::number(user->passportType())));
       tableWidget->setItem(5, 0,new QTableWidgetItem(tr("Medium path:\t")));
       tableWidget->setItem(5, 1,new QTableWidgetItem(user->passportFile().c_str()));
    }
  } // end if

  else
    tableWidget->setItem(0, 0,new QTableWidgetItem(tr("No HBCI-Configuration found")));

  for (int column = 0; column < tableWidget->columnCount(); column ++)
    tableWidget->resizeColumnToContents(column);

}
/** Emits signal help request.
*/
void GuiShowBankData::slotHelp(){
 emit helpRequest("showData.html");
}

