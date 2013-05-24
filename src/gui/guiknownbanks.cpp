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
#include "guiknownbanks.h"

#include <QTableWidget>

#include "hobit.h"
#include "bankitem.h"
using namespace std;


/**
*constructor of GuiKnownBanks
*
*@param parent a pointer on the main widgets widget stack
*@param name widgets title
*@param hobit the hobit object, needed for hbci functions
*/
GuiKnownBanks::GuiKnownBanks(QWidget *parent)
 :QWidget(parent), GuiKnownBanksBase()
{
    setupUi(this);
    setObjectName("GuiKnownBanks");
   _Banks = HoBIT::hobit()->listOfAllKnownBanks();

   //preset number of rows, this is a very slow operation
   int nBanks = _Banks.count();
   tableWidgetBanks->setRowCount(nBanks); 
   
    for (int row = 0; row < nBanks; row++)
    {
      banking::BankItem  bankItem(_Banks[row]);
      bankItem.addToTable(tableWidgetBanks, row, false); //do not add Hbci-Data
    }

    tableWidgetBanks->setSortingEnabled(true);
    tableWidgetBanks->sortByColumn(0); //sort by BankCode
    tableWidgetBanks->resizeColumnsToContents();
    tableWidgetBanks->resizeRowsToContents();
   qDebug("Banklist contains %i banks.", nBanks);
}

/**
*destructor of GuiKnownBanks
*/
GuiKnownBanks::~GuiKnownBanks()
{
  qDebug("Destructor GuiKnownBanks finished");
}

/** Emits signal back()
*/
void GuiKnownBanks::on_buttonBack_clicked(){
  emit back();
}

