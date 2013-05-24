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
#ifndef BANKINGBANKITEM_H
#define BANKINGBANKITEM_H

#include "kernel/helpers/bank.h"

#include <QString>

class QTableWidget;

namespace banking {

/**
A table item for a bank. To be constructed from banking::bank.

	@author walter <walter@HP2100-Roth>
*/
class BankItem{
public:
    BankItem(const Bank & bank);

    ~BankItem();

    /** Adds the BankItem as last row to table.
      * If addHbciData is true, Server and HbciVersion are added to table.
      */
    void addToTable(QTableWidget * table, int row, bool addHbciData = true);
private:
     QString _BankCode;
     QString _FutureBankCode;
     bool _ToBeDeleted;
     QString _BankBIC;
     QString _Server;
     QString _HbciVersion;
     QString _BankName;
     QString _City;
     QString _Country;
};

}

#endif
