/***************************************************************************
                          guidatedtransfers.h  -  description
                             -------------------
    begin                : Do Nov 11 2004
    copyright            : (C) 2004 by Marcel Mulch, 2011 Walter Roth
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GUIDATEDTRANSFERS_H
#define GUIDATEDTRANSFERS_H

#include <QWidget>
#include "ui_guidatedtransfersbase.h"

#include "kernel/helpers/transfer.h"
#include "transferitem.h"

class HoBIT;


/**
* class for a dialog, which shows a list of all dated transactions
*/

class GuiDatedTransfers : public QWidget, private Ui::GuiDatedTransfersBase  {
   Q_OBJECT
public: 
  
  /**
  *this is the constructor for the dialog
  *
  *@param main
  *@param parent the parent dialog of this widget
  *@param name widgets title
  *@param hobit
  */
	GuiDatedTransfers(QWidget *parent);
	~GuiDatedTransfers();
   
   /** 
   * updates the listview depending on the selected account 
   */
   virtual  void slotChangeAccount();
   /** 
   * executed when user performs a double click on a list item 
   */
   void slotSelectItem();
   /** 
   * deletes an existing dated transfer job
   */
   void slotDeleteDatedTransfer();
   /** 
   * set accountlabel to current account
   */
   void refresh();

signals:
  void showInformation( const banking::TransferItem & dt);
  void sendJobsNow(bool *, const QString & reason, bool showJobList);
  void newDatedTransferRequired();

public slots: // Public slots
  /** 
  *raises widget of dated single transfer formular 
  */
  void on_buttonNewDatedTransfer_clicked();
  /** 
  *executed when user performs a double click on a list item.
  *shows details of the item
  */
  void on_tableWidget_cellDoubleClicked ( int row, int column );
   /** 
   * creates job of dated transfers and send 
   */
   void on_buttonGetList_clicked();

private:
	banking::TransferItemList datedTransferList;

   /** 
   * creates list for dated transfers
   */	
   void buildTable(banking::TransferList * datedtransfers);
};



#endif

