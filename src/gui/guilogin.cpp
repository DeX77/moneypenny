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
#include "guilogin.h"
#include "accountitem.h"
#include <QHeaderView>

/**
*constructor of GuiLogin
*
*@param parent a pointer on the main widgets widget stack
*@param stack main QWidget Stack in the 'main frame'
*@param name widgets title
*@param hobit the hobit object, needed for hbci functions
*/
GuiLogin::GuiLogin(QWidget* parent)
    :QWidget(parent)
{
  setupUi(this);
  setObjectName("GuiLogin"); 
  qDebug("Constructor GuiLogin");
}

/** destructor*/
GuiLogin::~GuiLogin()
{
  qDebug("Destructor GuiLogin finished");
}


/**
*slot for the login
*/
void GuiLogin::on_buttonLogin_clicked()
{
  QTableWidgetItem * item = tableWidget->item(tableWidget->currentRow(), 1); //Bankcode column
  QString bankCode;
  if(item)
     bankCode = item->text();
  item = tableWidget->item(tableWidget->currentRow(), 3); //AccountId column
  QString account;
  bool ok = true;
  if(item)
     account = item->text();
  for(int i = 0; i < account.length(); i++) //check for non-numeric data
    if(account[i] < '0' || account[i] > '9')
      ok = false;
  if(account.isEmpty() || !ok)
  {
    QMessageBox::warning(this, 
                           tr("Attention"),
                           tr("This account is not valid.") +
                          "\n"+ tr("Further setup is required."));
    emit setCurrentAccount(bankCode, account);
  }
  else {
    qDebug("Activating account: %s", qPrintable(account));
    emit setCurrentAccount(bankCode, account);
 }
}

/** slot for continuing a RDH setup when key has been activated.
 */
void slotContinueRDHSetup(){
}


/**
*function for refreshing
*/
void GuiLogin::refresh()
{
  tableWidget->setRowCount(0);
  
  const banking::AccountList * accounts = HoBIT::hobit()->accounts();
  banking::AccountList::const_iterator  accit;
  if ((!accounts) || accounts->empty())
  {
    tableWidget->setItem(0,0, new QTableWidgetItem(tr("No accounts found yet")));
    buttonLogin->hide();
    emit raiseWidget(HBCI_WIZARD);
  }
  else
  {
    int nAccounts = accounts->size();
    qDebug("Number of accounts: %i", nAccounts );
    tableWidget->setRowCount(nAccounts);

    int row = 0;
    for (accit=accounts->begin(); accit!=accounts->end(); accit++)
    {
      banking::Account * account =(* accit);
      banking::AccountItem item(*account);
      banking::Bank * bank = HoBIT::hobit()->database()->bankByCode(account->bankCode());
      if(bank)
        item.setBankName(QString::fromUtf8(bank->bankName().c_str()));
      item.addToTable(tableWidget, row);
      for (int column = 0; column < tableWidget->columnCount(); column ++){
        tableWidget->item(row, column)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); //switch off editing
      }
      row ++;
    }
    tableWidget->selectRow(0);
    buttonLogin->show();
   }

    int tableWidth= tableWidget->verticalHeader()->width()+10;
    for (int column = 0; column < tableWidget->columnCount(); column ++){
      tableWidget->resizeColumnToContents(column);
      tableWidth += tableWidget->columnWidth(column);
    }

    int tableHeight= tableWidget->horizontalHeader()->height() +10;
    for (int row = 0; row < tableWidget->rowCount(); row ++){
      tableHeight += tableWidget->rowHeight(row);
    }
    tableHeight += tableWidget->rowHeight(0);
    if(tableHeight > frameAccountList->height() - buttonLogin->height() - 3 * frameAccountList->layout()->spacing())
       tableHeight = frameAccountList->height() - buttonLogin->height() - 3 * frameAccountList->layout()->spacing();

    QSize size = QSize(tableWidth, tableHeight);
    tableWidget->setMinimumSize(size);
    //tableWidget->setMaximumSize(size);
    if(tableWidget->item(0,0))
      tableWidget->setCurrentCell(0,0);
    on_tableWidget_currentCellChanged(0,0,0,0); //call slot, even if cell has not been changed
}

 /** Select the text on the login button according to valid/invalid accounts
 */
  void GuiLogin::on_tableWidget_currentCellChanged( int, int , int, int )
{
  int row = tableWidget->currentRow();
  if(tableWidget->rowCount() == 0 )
    return;
  if(row < 0){
    tableWidget->setCurrentItem(0,0);
    return;
  }

  QTableWidgetItem * item = tableWidget->item(row, 3); //AccountId column
  QString accountId;
  if(item)
     accountId = item->text();

  bool ok;
  double accountNumber = accountId.toDouble(&ok); //check for non-numeric chars
  if(!ok || accountNumber < 0.1)
   buttonLogin->setText(tr("Continue &setup for this bank."));
  else buttonLogin->setText(tr("&Select this account and login"));
}
