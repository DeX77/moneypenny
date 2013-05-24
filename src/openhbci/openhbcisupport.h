/***************************************************************************
 *   Copyright (C) 2007 by Walter Roth                                     *
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

/**Functions to access the gwenhywfar database and to convert old data to Hbci_Cern
* types.
* @author Walter Roth
*/
#ifndef OPENHBCISUPPORT_H
#define OPENHBCISUPPORT_H

#include <string>
using namespace std;

#include <QString>

// AqMoney includes
#include "aqmoney2/aqmapi.h"
#include "aqmoney2/accountjobs.h"
#include "aqmoney2/adminjobs.h"

namespace banking{
  class Bank;
  class TransferList;
  class TransactionList;
}

class OpenHbciSupport{
  public:
    OpenHbciSupport(){};

    virtual ~OpenHbciSupport(){};

    void setApi(Pointer<AqMoneyAPI> api){_api = api;};
  
  /**Reads all transactions form the gwen database for account and returns a
   * new list of Transaction objects. List and Objects in this list must be deleted after use.
  */
  banking::TransactionList * gwenTransactionList(const string & accountId);
 
  /** Reads OppenHbci configuration data
  */
  banking::Bank * readData(const string & dataDir, const string & keyFileDir);

  /** reads stored templates from file */
  banking::TransferList * readGwenTransferTemplates(const string& templateFile);

private:
 /** Constructs a new list containing all transactions of an account which were made during
 *  a specific time periode. List must be deleted by caller. DEPRECATED
 */

 banking::TransactionList * listGwenTransactions(int country,
                                             const string &bankCode,
                                             const string &accountId,
                                             const string &accountSubId,
                                             const Date &fromDate,
                                             const Date &toDate);
/** Deletes bank messages from config file. Bank messages may cause gwen to crash. 
*/
void deleteBankMessages(const QString & dataDir);
/** Returns the current bank */
Pointer<Bank> getBank();

 /**For OpenHBCI compatibility
 */ 
 Pointer<AqMoneyAPI> _api;
};
#endif
