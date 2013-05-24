/***************************************************************************
                          guitransactions.h  -  description
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

#ifndef GUITRANSACTIONLIST_H
#define GUITRANSACTIONLIST_H

#include <qwidget.h>
#include <ui_guitransactionlistbase.h>

#include "kernel/helpers/transaction.h"
#include "transactionitem.h"
#include "hobit.h"


class GuiMain;

/**
* this is the class for showing the transactions
*/
class GuiTransactionList : public QWidget, public Ui::GuiTransactionList
{
  Q_OBJECT

public:

  /**
  *this is the transaction dialog constructor
  *
  *@param main a pointer on moneypenny´s main class
  *@param parent a pointer on the main widgets widget stack
  *@param name widgets title
  *@param hobit the hobit object, needed for hbci functions
  */
  GuiTransactionList(QWidget *parent);

  /**
  *this is the transaction dialog destructor
  */
  virtual ~GuiTransactionList();

  /**
  *this is a function for a refresh
  */
  void refresh();

  /**
  * this is a function for reading property of QPtrList<TransactionItem> _TransactionList.
  */
  const banking::TransactionItemList & transactionList()const;
  /** Update table row and collum sizes
   */
  void showEvent (QShowEvent * se);
protected slots:

  /**
  *this is a function for updating the listview depending on the selected account
  *Add a year to all toDates for "Current..." lists. There may be dates in the future.
  */
  virtual void on_comboBoxDate_currentIndexChanged(int index);

  /**
  *this is the slot for raising up the transaction widget
  */
  void on_buttonUpdateData_clicked();

  /**
  *this is the function which executes when the user performs a click
  *on a list item
  *
  *@param row number of the row 
  *@param col number of the col
  */
  void on_tableWidget_cellClicked(int row, int col);

  /** 
  * this is a function for activating GuiTransactionPrinter
  * Copies Hobit's list of transactions and sorts the copy according to 
  * sort column. Then emits print signal.
  */
  void on_buttonPrintList_clicked();

   /** calls export Dialog */
   void on_buttonExportList_clicked();
   /** Emits helpRequest().
   */
   void on_buttonHelp_clicked();
   /** Creates a set of transaction templates from all outgoing transactions in the list.
   */
   void on_buttonCreateTransferTemplates_clicked();
   /** Sorts by column
   */
   void slotSortByColumn(int column);
   /** Update table, if user changes dates.
   */
   void on_dateEditFrom_dateChanged(const QDate & date);
   /** Update table, if user changes dates.
   */
   void on_dateEditTo_dateChanged(const QDate & date);
signals:

  void printTransactions(banking::TransactionList * transactionList, const QDate & from, const QDate & to);
  void getTransactions();
  void showDetails(const banking::TransactionItem &);
  void exportData();
  void helpRequest(const QString& helpFile);
  void refreshRequest();
  void raiseWidget(int iD);

private:

  /**Builds the QTableWidget from list.
    */
  virtual void buildTable(QTableWidget * table, const QString dateFormat, banking::TransactionList * transactions);
  /**Do not build a new table, if false.
   */
  bool _UserAction;
  banking::TransactionItemList _TransactionItemList;
  /** The sorting order of the transactionsTable changes on second click
   *  on a column.
   */
  int _LastColumn;
  /** Sorting direction
   */
  Qt::SortOrder _SortOrder;
  /**Format string for dates. See QDate for details
   */
  QString _DateFormat;
};

#endif
