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


#include <QStringList>
#include <QScrollBar>

#include "hobit_string.h"

#include "guitransfertemplates.h"
#include "globals.h"
#include "transferitem.h"

GuiTransferTemplates::GuiTransferTemplates(QWidget* parent)
: QWidget(parent), Ui::GuiTransferTemplates()
{
  setupUi(this);
  setObjectName("GuiTransferTemplates");
   refresh();
}

GuiTransferTemplates::~GuiTransferTemplates()
{
  qDebug("Destructor GuiTransferTemplates finished");
}

/**Enables delete button
*/
void GuiTransferTemplates::on_tableWidget_clicked()
{
  bool enable = tableWidget->currentItem() != NULL;
  buttonDelete->setEnabled(enable);
  buttonUse->setEnabled(enable);
}

void GuiTransferTemplates::on_buttonUse_clicked()
{
   int row = tableWidget->currentRow();

   banking::TransferList templateList = HoBIT::hobit()->listTransferTemplates();
   banking::TransferList::iterator it = templateList.begin();
   int i = 0;
   while( it != templateList.end() && i < row){
     it++;
     i++;
   }
   banking::Transfer * transfer = new banking::Transfer(*(*it));
   if (radioButtonSingleTransfer->isChecked()){
     transfer->setType(banking::Transfer::SingleTransfer);
   }
   else if (radioButtonDatedTransfer->isChecked()){
     transfer->setType(banking::Transfer::DatedTransfer);
   }
   else if (radioButtonStandingOrder->isChecked()){
     transfer->setType(banking::Transfer::StandingOrder);
   }
   else if (radioButtonDebitNote->isChecked()){
     transfer->setType(banking::Transfer::DebitNote);
   }
   transfer->setOurAccountId(qPrintable(HoBIT::hobit()->currentAccountId()));
   transfer->setOurBankCode(qPrintable(HoBIT::hobit()->bankCode()));

  emit raiseTemplate(transfer);
  emit raiseWidget(TRANSFER_WIDGET);
}

void GuiTransferTemplates::on_buttonDelete_clicked()
{
   int row = tableWidget->currentRow();
   HoBIT::hobit()->deleteTransferTemplate(row);
   refresh();
}

void GuiTransferTemplates::cancel()
{
  emit back();
}


void GuiTransferTemplates::refresh()
{  
  _Templates = HoBIT::hobit()->listTransferTemplates();
  banking::TransferList::iterator it;
 
  int rows = 0;
  tableWidget->setRowCount(rows); //delete old contents
  for(it = _Templates.begin(); it != _Templates.end(); it++)
     rows++;
  tableWidget->setRowCount(rows); //resize to new number of templates

  int row = 0;
  for(it = _Templates.begin(); it != _Templates.end(); it++){
    banking::TransferItem item = banking::TransferItem(*(*it));
    item.addToTemplateTable(tableWidget, row);
    row ++;
  }//end for
  tableWidget->resizeColumnsToContents();
  tableWidget->resizeRowsToContents();
 // tableWidget->sortItems(0);
  on_tableWidget_clicked();
}

/** Activates the corresponding radiobutton.
*/
void GuiTransferTemplates::slotSetTypeRadioButton(banking::BankingJob::JobType type){
 switch (type){
  case banking::BankingJob::SingleTransfer:{
                                    radioButtonSingleTransfer->setChecked(true); 
                                    break;
                                  } 
  case banking::BankingJob::DatedTransfer: {
                                    radioButtonDatedTransfer->setChecked(true); 
                                    break;
                                  } 
  case banking::BankingJob::StandingOrder: {
                                    radioButtonStandingOrder->setChecked(true); 
                                    break;
                                  }  
  case banking::BankingJob::DebitNote:  {
                                 radioButtonDebitNote->setChecked(true); 
                                 break;
                               }  
  default: {
             radioButtonSingleTransfer->setChecked(true); 
             break;
           }  
 }//end switch
}

/**Clears the purpose of the selected template
*/
void GuiTransferTemplates::on_buttonClearPurpose_clicked(){
   int row = tableWidget->currentRow();
   if(!tableWidget->currentItem()->isSelected())
     return;
   banking::TransferList::iterator it = HoBIT::hobit()->listTransferTemplates().begin();
   for (int i = 0; i < row; i++){
    if(it != HoBIT::hobit()->listTransferTemplates().end()) 
      it++;
   }
   if(it != HoBIT::hobit()->listTransferTemplates().end()) 
     (*it)->clearPurposeLines();
   refresh();
}

/** Update table row and column sizes
*/
void GuiTransferTemplates::showEvent (QShowEvent * ){
  tableWidget->horizontalScrollBar()->setValue(0);
  tableWidget->resizeColumnsToContents();
  tableWidget->resizeRowsToContents();
}
