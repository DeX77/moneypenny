/***************************************************************************
 *   Copyright (C) 2004 by Marcel Mulch                                    *
 *   marcel@mulch-online.de                                                *
 *                                                                         *
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
#ifndef GUIJOBS_H
#define GUIJOBS_H

#include <QWidget>

#include "ui_guijobsbase.h"

class HoBIT;
namespace banking{
  class Transfer;
  class BankingJobItemList;
  class BankingJob;
}
/**
* Class for the job list. Owns the list of pending jobs.
* Has functions for editing or deleting jobs.
* Use sendJobs() to make the HBCI engine send the jobs.
* Transfer type jobs are sent before admin type jobs.
* <h3>Synchronisation with JobManager</h3>
* New JobItems in _TransferJobList and _AdminJobList are associated by row number
* to jobs in Jobmanagers's lists. When jobs have been queued, Jobmanager updates
* the Id numbers by emitting signals sentXXXJob.
* Further status changes are tracked by signal jobStatusChanged. The associated
* JobItem is then identified by its Id, because replies of the bank may be in a different order.
* Connections are made in GuiMain
*/
class GuiJobs: public QWidget, public Ui::GuiJobs
{

  Q_OBJECT

public:

  /**
  *constructor of GuiJobs
  *
  *@param main a pointer on moneypenny´s main class
  *@param parent a pointer on the main widgets widget stack
  *@param name widgets title
  *@param hobit the hobit object, needed for hbci functions
  */
  GuiJobs(QWidget *parent);

  /** destructor */
  ~GuiJobs();

  /**
  *refreshes the job queues 
  */
  void refresh();

 /**
   *
   */
  void setMountPoint(const QString & newMountPoint){mountPoint = newMountPoint;};

/** set hobit object*/
  void setHoBIT(HoBIT* hobit);

public slots:
  /** Slot for editing a transfer job
  */
  virtual void on_buttonEditJob_clicked();

  /**
  *slot for deleting jobs
  */
  void on_buttonDeleteJob_clicked();

  /** 
  *Deletes all jobs
  */
  void on_buttonDeleteAllJobs_clicked();
  
  /**
  *sends the jobs 
  */
  void on_buttonSendJobs_clicked();
  /**
    * Create DTA-file(s)
    */
  void on_buttonSaveAsDTAFile_clicked();

  /** Disables /enables the control buttons.
  */
   void setButtonsEnabled(bool enable);

   /**Searches the JobItem associated to the job by id and calls it's
    *updateStatus function.Finally resizes status columns of tableWidgets.
    */
   void slotUpdateJobStatus(banking::BankingJob * job);
   /**Sets Id of TransferItem in row to Id of job. Must be called
    * after queuing the job into kernel queue.
    */
    void slotSentTransferJob(int row, banking::BankingJob * job);
   /**Sets Id of BankingJobItem in row to Id of job. Must be called
    * after queuing the job into kernel queue.
    */
    void slotSentAdminJob(int row, banking::BankingJob * job);

signals:
  void sendNow(bool *, const QString &, bool);
  void jobsCreated();
  void editTransferJob(banking::Transfer * job);

private:
   /** 
     * Write contents to fileName. Return true on success. Show a messagebox on error.
     */
   bool writeTransferFile(const QString & fileName, const QString & contents);

  bool _adminSelected;
  bool _accountSelected;
  /** Contains all transfer jobs.
  */
  banking::BankingJobItemList * _TransferJobItems;
  /** Contains all admin jobs.
  */
  banking::BankingJobItemList * _AdminJobItems;
  /**
    * Mount point of the USB device.
    */
  QString mountPoint;
};

#endif
