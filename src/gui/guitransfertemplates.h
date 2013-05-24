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

#ifndef GUITRANSFERTEMPLATES_H
#define GUITRANSFERTEMPLATES_H

#include "ui_guitransfertemplatesbase.h"
//#include "guimain.h"
#include "hobit.h"

class GuiMain;

class GuiTransferTemplates : public QWidget, public Ui::GuiTransferTemplates
{
  Q_OBJECT
public:
  GuiTransferTemplates(QWidget* parent = 0);
  ~GuiTransferTemplates();
  /*$PUBLIC_FUNCTIONS$*/

  /** Update table row and column sizes
  */
  void showEvent (QShowEvent * se);

signals:
  void raiseWidget(int iD);
  void back();
    /**
  *Raises a transfer template
  */
  void raiseTemplate( banking::Transfer * t);
  
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void on_buttonUse_clicked();
  virtual void on_buttonDelete_clicked();
  virtual void on_buttonClearPurpose_clicked();
  /**Enables delete button
  */
  void on_tableWidget_clicked();
  virtual void cancel();
  /** rebuilds the list
  */
  virtual void refresh();
  /** Activates the corresponding radiobutton.
  */
  void slotSetTypeRadioButton(banking::BankingJob::JobType);

private:
   banking::TransferList _Templates;

protected slots:
  /*$PROTECTED_SLOTS$*/

};

#endif

