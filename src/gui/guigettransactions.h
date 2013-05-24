/***************************************************************************
                          guigettransactions.h  -  description
                             -------------------
    begin                : Sa Nov 6 2004
    copyright            : (C) 2004 by Marcel Mulch
    email                : marcel@mulch-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GUIGETTRANSACTIONS_H
#define GUIGETTRANSACTIONS_H

#include "hobit.h"

#include "ui_guigettransactionsbase.h"

/** 
* Dialog for account(s) and interval data for a GetTransactions job. 
*/
class GuiGetTransactions : public QWidget, public Ui::GuiGetTransactionsBase  {
   Q_OBJECT
public: 

    /**
    *Constructor
    *
    *@param parent the parent dialog of this widget
    *@param hobit
    */
    GuiGetTransactions( QWidget *parent);
    /**Destructor 
    */
    virtual ~GuiGetTransactions();
    /** Called on show. Sets current account.
    */
    void showEvent ( QShowEvent * );

signals:
   void sendNow(bool *, const QString &, bool showJobList);
   void jobsCreated();
   void raiseWidget(int);
   void canceled();

protected slots:
   /** 
   * Look which transactions the user chose for Get and get them!
   */
   void on_buttonGet_clicked();
   /** Called on Cancel clicked. Sends signal cancelled.
   */
   void on_buttonCancel_clicked();
   /**Enables /disables Dateedits
   */
   void on_radioButtonDated_toggled(bool checked);
};

#endif
