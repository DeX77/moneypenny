/***************************************************************************
 *   Copyright (C) 2004 by Marcel Mulch                                *
 *   marcel@mulch-online.de                                          *
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
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include "guijobs.h"

#include "hobit.h"
#include "transferitem.h"

/**
*constructor of GuiJobs
*
*@param main a pointer on moneypenny´s main class
*@param parent a pointer on the main widgets widget stack
*@param name widgets title
*@param hobit the hobit object, needed for hbci functions
*/
GuiJobs::GuiJobs(QWidget *parent)
    :QWidget(parent)
{
   setupUi(this);
   setObjectName("GuiJobs");

   _TransferJobItems = new banking::BankingJobItemList();
   _AdminJobItems = new banking::BankingJobItemList();
   refresh();

   _adminSelected = false;
   _accountSelected = false;

}


 /** destructor */
 GuiJobs::~GuiJobs(){
   delete _TransferJobItems;
   delete _AdminJobItems;
   qDebug("Destructor GuiJobs finished");
 }

/** 
*refreshes the job queues 
*/
void GuiJobs::refresh(){
  _TransferJobItems->clear();
  _AdminJobItems->clear();
  tableWidgetTransfers->clearContents();
  tableWidgetTransfers->setRowCount(0);
  tableWidgetAdminJobs->clearContents();
  tableWidgetAdminJobs->setRowCount(0);

  //transfers first
 int row = 0;
 const banking::Transfer * transfer;
  while((transfer = HoBIT::hobit()->pendingTransfer(row)) != NULL){
    tableWidgetTransfers->insertRow(row);
    banking::TransferItem item = banking::TransferItem(*transfer);
    item.addToTable(tableWidgetTransfers, row);
    _TransferJobItems->append(item);
    row++;
  }
  tableWidgetTransfers->resizeRowsToContents();
  tableWidgetTransfers->resizeColumnsToContents();

  //Admin jobs
  row = 0;
  const banking::BankingJob * job;
  while((job = HoBIT::hobit()->jobManager()->getAdminJob(row)) != NULL){
    tableWidgetAdminJobs->insertRow(row);
    banking::BankingJobItem item = banking::BankingJobItem(*job);
    item.addToTable(tableWidgetAdminJobs, row);
    _AdminJobItems->append(item);
    row ++;
  }
  tableWidgetAdminJobs->resizeColumnsToContents();
}




/**
*slot for deleting jobs
*/
void GuiJobs::on_buttonDeleteJob_clicked()
{
   bool keep = true;

   if ((tableWidgetTransfers->selectedItems().count() == 0)  //nothing selected?
       && (tableWidgetAdminJobs->selectedItems().count() == 0))
   return;

   keep = QMessageBox::question(this, 
                             tr("Delete?"), 
                             tr("Do you really want to delete the selected job ?"),
                             tr("&Yes"), tr("&No"), QString::null, 0, 1);
   if (tableWidgetTransfers->selectedItems().count() > 0){
       if (!keep){
          int row = tableWidgetTransfers->currentRow();
         HoBIT::hobit()->jobManager()->deleteTransfer(row);
         _TransferJobItems->removeAt(row);
         tableWidgetTransfers->removeRow(row);
        }
   }
   else if (tableWidgetAdminJobs->selectedItems().count() > 0){
        if (!keep){
          int row = tableWidgetAdminJobs->currentRow();
          HoBIT::hobit()->jobManager()->deleteAdminJob(row);
         _AdminJobItems->removeAt(row);
          tableWidgetAdminJobs->removeRow(row);
        }
   }
   refresh();
}

/** Slot for editing a transfer job
*/
void GuiJobs::on_buttonEditJob_clicked(){
 if((tableWidgetTransfers->rowCount() == 0))
  return;

 int row = tableWidgetTransfers->currentRow();
 if(row < 0)
  return;

 banking::Transfer * transferJob= HoBIT::hobit()->jobManager()->takeTransferAt(row);
 tableWidgetTransfers->removeRow(row);
 if(transferJob != NULL)
   emit editTransferJob(transferJob);
}

/**
  * Create DTA-file(s)
  */
void GuiJobs::on_buttonSaveAsDTAFile_clicked()
{
  QDateTime dateTime = QDateTime::currentDateTime(); 
  QString fileName;
  QString dirName = QFileDialog::getExistingDirectory
                      (this, 
                       tr("Directory for DTA Files"), 
                       mountPoint + MONEYPENNY_DTA_DIR
                       );
  if(HoBIT::hobit()->pendingTransfers() > 0)
  {
    QString DTATransfers;
    try{
      DTATransfers = HoBIT::hobit()->generateDTAFile(banking::BankingJob::SingleTransfer);
      if(!DTATransfers.isEmpty())
      {
       fileName  = dirName + "/"
                   + tr("DTA_transfers_", "This is a part of the filename for DTA files. No blanks etc. please!")
                   + dateTime.toString("yyyy_MM_dd_hh_mm") + ".dta";
       writeTransferFile(fileName, DTATransfers);
      }
    }
    catch(exception e)
    {
      QMessageBox::critical(this, tr("Error when generating DTA transfer file."), e.what());
    }
  }

  if(HoBIT::hobit()->pendingDebitNotes() > 0)
  {
    try{
       QString DTADebitNotes = HoBIT::hobit()->generateDTAFile(banking::BankingJob::DebitNote);
       if(!DTADebitNotes.isEmpty())
       {
        fileName  = dirName + "/"
                    + tr("DTA_debitnotes_", "This is a part of the filename for DTA files. No blanks etc. please!")
                    + dateTime.toString("yyyy_MM_dd_hh_mm") + ".dta";
        writeTransferFile(fileName, DTADebitNotes);
       }
    }
    catch(exception e)
    {
      QMessageBox::critical(this, tr("Error when generating DTA debit note file."), e.what());
    }
  }
}

/** 
  * Write contents to fileName. Return true on success. Show a messagebox on error.
  */
bool GuiJobs::writeTransferFile(const QString & fileName, const QString & contents)
{
     QFile dtaFile(fileName);
     if(dtaFile.open(QIODevice::WriteOnly))
     {
        QTextStream s(&dtaFile);
        s << contents;
        return true;
     }
     QMessageBox::critical(this, tr("Error"), tr("Could not write DTA file:") + "  " + fileName );
     return false;
}


/**
*sends the jobs when executes
*/
void GuiJobs::on_buttonSendJobs_clicked()
{
  emit sendNow(NULL, tr("Sending all jobs in the list."), false);
}

/** Deletes all jobs
*/
void GuiJobs::on_buttonDeleteAllJobs_clicked(){
 
   if((tableWidgetTransfers->rowCount() == 0) //nothing to delete?
     && (tableWidgetAdminJobs->rowCount()==0))
   return;

   bool keep  = QMessageBox::question(this, 
   				       tr("Delete All?"), 
   		 	   	       tr("Do you really want to delete all jobs? <br> \
                                           There is no way to restore them. \""), 
			               tr("&Yes"), tr("&No"), QString::null, 0, 1);
  if(!keep){
     HoBIT::hobit()->jobManager()->clearLists();
     tableWidgetTransfers->setRowCount(0);
     tableWidgetAdminJobs->setRowCount(0);
      refresh();
  }
}

/** Disables /enables the control buttons.
*/
 void GuiJobs::setButtonsEnabled(bool enable){
  buttonCancel->setEnabled(enable);
  buttonEditJob->setEnabled(enable);
  buttonDeleteJob->setEnabled(enable);
  buttonDeleteAllJobs->setEnabled(enable);
  buttonSendJobs->setEnabled(enable);
}

/**Searches the JobItem associated to the job by id and calls it's
 *updateStatus function.Finally resizes status columns of tableWidgets.
 */
void GuiJobs::slotUpdateJobStatus(banking::BankingJob * job){
  int jobId = job->id();
  for(int row = 0; row < _TransferJobItems->count(); row ++)
    if(_TransferJobItems->at(row).id() == jobId){
      (*_TransferJobItems)[row].setStatus(job->status());
      (*_TransferJobItems)[row].setMessage(job->message().c_str());
    }
  for(int row = 0; row < _AdminJobItems->count(); row ++)
    if(_AdminJobItems->at(row).id() == jobId){
      (*_AdminJobItems)[row].setStatus(job->status());
      (*_AdminJobItems)[row].setMessage(job->message().c_str());
    }
   //resize Status columns
   tableWidgetTransfers->resizeColumnToContents( 0 );
   tableWidgetAdminJobs->resizeColumnToContents( 0 );
   //resize message columns
   tableWidgetTransfers->resizeColumnToContents( 1 );
   tableWidgetAdminJobs->resizeColumnToContents( 1 );
   //resize row heights
   tableWidgetTransfers->resizeRowsToContents();
   tableWidgetAdminJobs->resizeRowsToContents();
   repaint();
   qApp->processEvents();
   qApp->flush();
}

/**Sets Id of TransferItem in row to Id of job. Must be called
 * after queuing the job into kernel queue.
 */
void GuiJobs::slotSentTransferJob(int row, banking::BankingJob * job){
  if((row >=_TransferJobItems->count()) || (row < 0))
    return;
  banking::BankingJobItem * item = &(*_TransferJobItems)[row];
  item->setId(job->id());
  item->setStatus(job->status());
  tableWidgetTransfers->resizeColumnToContents(0);
  repaint();
}

/**Sets Id of BankingJobItem in row to Id of job. Must be called
 * after queuing the job into kernel queue.
 */
void GuiJobs::slotSentAdminJob(int row, banking::BankingJob * job){ 
 if((row >=_AdminJobItems->count()) || (row < 0))
    return;
 banking::BankingJobItem * item = &(*_AdminJobItems)[row];
 item->setId(job->id());
 item->setStatus(job->status());
 tableWidgetAdminJobs->resizeColumnToContents(0);
 repaint();
}

