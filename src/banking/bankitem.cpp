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
#include "bankitem.h"

#include <QTableWidgetItem>
#include <QTableWidget>
namespace banking {

BankItem::BankItem(const Bank & bank)
{
		_BankCode =bank.bankCode().c_str();
                _ToBeDeleted = bank.isToBeDeleted();
                _FutureBankCode = bank.futureBankCode().c_str();
		_BankBIC =bank.bankBic().c_str();
		_Server =  QString::fromUtf8(bank.server().c_str());
		_HbciVersion = bank.hbciVersionString().c_str();
		//optional
		_BankName =  QString::fromLatin1(bank.bankName().c_str());
		_City=  QString::fromLatin1(bank.city().c_str());
		_Country= bank.country().c_str();
}


BankItem::~BankItem()
{
}

/** Adds the BankItem as last row to table.
  * If addHbciData is true, Server and HbciVersion are added to table.
  */
void BankItem::addToTable(QTableWidget * table, int row, bool addHbciData){
  //the twi (=tablewidgetitem) prefix of the locals prevents name clashes
  QTableWidgetItem *twiBankCode;
  QTableWidgetItem *twiBankBIC;
  QTableWidgetItem *twiBankName;
  QTableWidgetItem *twiCity;
  QTableWidgetItem *twiServer;
  QTableWidgetItem *twiHbciVersion;

  if(table->rowCount() < row +1)
   table->setRowCount(row +1);

  int column = 0;
  QString bankCode = _BankCode;
  if(_ToBeDeleted)
  {
    bankCode = _BankCode + "\n" + QObject::tr("Future bankcode:") + "\n";
    if(_FutureBankCode == "00000000")
      bankCode += QObject::tr("Unknown");
    else bankCode += _FutureBankCode;
  }
  twiBankCode = new  QTableWidgetItem( bankCode );
  if(_ToBeDeleted)
    twiBankCode->setForeground(Qt::red);
  table->setItem( row, column, twiBankCode );
  column++;

  twiBankBIC = new  QTableWidgetItem( _BankBIC );
  table->setItem( row, column, twiBankBIC);
  column++;

  twiBankName = new  QTableWidgetItem( _BankName );
  table->setItem( row, column, twiBankName);
  column ++;

  twiCity = new  QTableWidgetItem( _City );
  table->setItem( row, column, twiCity);
  column ++;
  if( addHbciData ){
     twiServer = new  QTableWidgetItem( _Server );
     table->setItem( row, column, twiServer);
     column ++;
   
     twiHbciVersion = new  QTableWidgetItem( _HbciVersion );
     table->setItem( row, column, twiHbciVersion);
     column ++;
  }

  //adjust row heights for whole table later on
 }

}
