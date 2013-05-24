/***************************************************************************
                          guitransactions.cpp  -  description
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

#include "hobit_transfertemplate.h"
#include "hobit_string.h"
#include "guitransactionlist.h"
#include "dateitem.h"

#include <QLabel>
#include <QDateTime>
#include <QScrollBar>

/**
* constructor
*
*@param parent a pointer on the main widgets widget stack
*@param hobit the hobit object, needed for hbci functions
*/
GuiTransactionList::GuiTransactionList(QWidget *parent)
: QWidget(parent) {
   setupUi(this);
   setObjectName("GuiTransactionList");
  _LastColumn = -1;
  _SortOrder = Qt::AscendingOrder;
  _UserAction = false;
  frameFromTo->hide(); 
  _DateFormat = tr("ddd.MMM.d.yyyy", "Date format to be used for transaction list");
  connect ((QObject*)tableWidget->horizontalHeader(), SIGNAL (sectionClicked(int)), this, SLOT(slotSortByColumn(int)));
  qDebug("Constructor GuiTransactionList");
}


/**
*this is the transaction dialog destructor
*/
GuiTransactionList::~GuiTransactionList(){
  qDebug("Destructor GuiTransactionList");
}


/**Builds the QTable from list in ascending ord descending date order
*/
void GuiTransactionList::buildTable(QTableWidget * table, const QString dateFormat, 
                                    banking::TransactionList * transactions){
  banking::Value income;
  banking::Value outgo;
  int rowCount = 0;
  int row = 0;
  DateItem::setDateFormat(dateFormat);

  tableWidget->setRowCount(0); //ToDo: Check for memory leak
  tableWidget->setSortingEnabled(false);

  _TransactionItemList .clear();

  banking::TransactionList::iterator transit;

  for (transit = transactions->begin(); transit != transactions->end(); transit++){ //set rowCount first
    rowCount ++;
  }
  tableWidget->setRowCount(rowCount);

  for (transit = transactions->begin(); transit != transactions->end(); transit++){
    banking::TransactionItem item = banking::TransactionItem( *(*transit) );
    item.setId(row);
    banking::Value val = item.value();
    if (val.isCredit()) 
       income += val;
    else outgo += val;
    item.addToTable(table, row);
     _TransactionItemList .push_back(item);
     row++;
  }

  QString num = income.readableString().c_str();
  lineEditIncome->setText(num + " EUR");
  num = outgo.readableString().c_str();
  lineEditOutGo->setText(num + " EUR");

  tableWidget->resizeColumnsToContents();
  tableWidget->resizeRowsToContents();

  delete transactions; //free memory for the list
}



/**
*this is a function for updating the table depending on the selected account
*Add a year to all toDates for "Current..." lists. There may be dates in the future.
*/
void GuiTransactionList::on_comboBoxDate_currentIndexChanged(int ){

    QDate fromDate;
    QDate toDate;

    QDate currentDate;
    currentDate = currentDate.currentDate();
    int dateselection = comboBoxDate->currentIndex();
    if(dateselection != 7)
      frameFromTo->hide();
    switch (dateselection){

      case 0:  fromDate = QDate(1980, 1,1); //Show all transactions
               toDate = currentDate.addYears(1);
               qDebug("Show all transactions");
               break;
      case 1:  fromDate = QDate(currentDate.year(), 1,1);     //current year
               toDate = currentDate.addYears(1); //One year later
               qDebug("Show transactions of current year");
               break;
      case 2:  fromDate = QDate(currentDate.year() - 1, 1 ,1); //last year
               toDate = QDate(currentDate.year()-1, 12 , 31);
               qDebug("Show transactions of last year");
               break;
      case 3:  fromDate = QDate(currentDate.year(), currentDate.month(), 1); //current month
               toDate = currentDate.addYears(1);
               qDebug("Show transactions of current month");
               break;
      case 4:  if(currentDate.month() == 1){ //we are in january, last month is december last year
                fromDate = QDate(currentDate.year()-1, 12 ,1);
                toDate = QDate(currentDate.year()-1, 12 ,fromDate.daysInMonth());
                qDebug("Show transactions of last month, december last year");
              }
              else{
                fromDate = QDate(currentDate.year(), currentDate.month()-1, 1);
                toDate = QDate(currentDate.year(), currentDate.month()-1, fromDate.daysInMonth());
                qDebug("Show transactions of last month");
              }
              break;
      case 5: if (currentDate.month() < 4){                          //current quarter
                        fromDate = QDate(currentDate.year(), 1, 1);
                        toDate = QDate(currentDate.year() +1, 3 , 31);
              }
              else if (currentDate.month() < 7){
                        fromDate = QDate(currentDate.year(), 4 ,1);
                        toDate = QDate(currentDate.year() + 1, 6 , 30);
              }
              else if (currentDate.month() < 10){
                        fromDate = QDate(currentDate.year(), 7 ,1);
                        toDate = QDate(currentDate.year() + 1, 9 , 31);
              }
              else if (currentDate.month() <= 12){
                        fromDate = QDate(currentDate.year(), 10 ,1);
                        toDate = QDate(currentDate.year() + 1, 12, 31);
              }
              qDebug("Show transactions of current quarter");
              break;
	case 6: if (currentDate.month() < 4){ //last quarter of last year
                          fromDate = QDate(currentDate.year() -1, 10 ,1);
                          toDate = QDate(currentDate.year()-1, 12 ,31);
                }
                else if (currentDate.month() < 7){
                          fromDate = QDate(currentDate.year(), 1, 1);
                          toDate = QDate(currentDate.year(), 3, 31);
                }
                else if (currentDate.month() < 10){
                          fromDate = QDate(currentDate.year(), 4, 1);
                          toDate = QDate(currentDate.year(), 6, 30);
                }
                else if (currentDate.month() <= 12){
                          fromDate = QDate(currentDate.year(), 7, 1);
                          toDate = QDate(currentDate.year(), 9, 30);
                }
                qDebug("last quarter");
                break;
     case 7: { //show the date edits
               frameFromTo->show(); 
               fromDate = QDate(1980, 1,1); //Show all transactions
               toDate = currentDate.addYears(1);
             }break;
     default:  fromDate = QDate(1980, 1,1); //Show all transactions
               toDate = currentDate.addYears(1);
               qDebug("Show all transactions");
               break;
       }; //end switch

       _UserAction = false;
       dateEditFrom->setDate(fromDate);
       dateEditTo->setDate(toDate);
       _UserAction = true;

       qDebug("Transactions from %s to %s", qPrintable(fromDate.toString()), qPrintable(toDate.toString()));

       HoBIT::hobit()->setFromTo(dateEditFrom->date(), dateEditTo->date());
        buildTable(tableWidget, 
                   _DateFormat,
                   HoBIT::hobit()->listTransactions());
        update();
}


/**
*this is the slot for raising up the transaction widget
*/
void GuiTransactionList::on_buttonUpdateData_clicked(){
      emit getTransactions();
}


/**
*this is the function which executes when the user performs a double click
*on a list item
*
*@param row number of the row 
*@param col number of the col
*/
void GuiTransactionList::on_tableWidget_cellClicked(int row, int ){
   int lastColumn = tableWidget->columnCount() -1;
   int index = tableWidget->item(row, lastColumn)->text().toInt(); //get the id from the last column
   emit showDetails( _TransactionItemList .at(index));
}


/**
*reloads account data
*/
void GuiTransactionList::refresh()
{
  textLabelAccount->setText(HoBIT::hobit()->currentAccountId());
  if (HoBIT::hobit()->currentAccount() != NULL){
       on_comboBoxDate_currentIndexChanged(comboBoxDate->currentIndex()); //keep setting, but refresh dates
  }
}

/** 
* this is a function for activating GuiTransactionPrinter
* Copies Hobit's list of transactions and sorts the copy according to 
* sort column. Then emits print signal.
*/
void GuiTransactionList::on_buttonPrintList_clicked(){
 // A sorted copy of Hobit' stransaction list. Used for printing. To be deleted by caller
 banking::TransactionList * printerTransactionList = new banking::TransactionList();
 banking::TransactionList * hobitTransactions = HoBIT::hobit()->listTransactions();
 for (int row = 0; row < tableWidget->rowCount(); row ++)
   {
     int id = tableWidget->item(row, tableWidget->columnCount() -1)->text().toInt();
     //search for the next transaction in the sorted tableWidget
     banking::TransactionList::iterator it = hobitTransactions->begin();
     for(int nTransaction = 0; nTransaction < id; nTransaction ++) 
       if(it != hobitTransactions->end())
        it++;
    //now copy it and add it to the printer list
    banking::Transaction * copy = new banking::Transaction( *(*it));
    printerTransactionList->push_back(copy);
   }
  emit printTransactions(printerTransactionList, dateEditFrom->date(), dateEditTo->date());//printerTransactionList must be deleted by receiver
}

/** 
* this is a function for reading property of QPtrList<TransactionItem> _TransactionItemList . 
*/
const banking::TransactionItemList &  GuiTransactionList::transactionList()const{
	return _TransactionItemList ;
}

/** calls export Dialog */
void GuiTransactionList::on_buttonExportList_clicked(){
    emit exportData();
}

/** Emits helpRequest().
*/
void GuiTransactionList::on_buttonHelp_clicked(){
 emit helpRequest("transactionlist.html");
}

/** Creates a set of transaction templates from all outgoing transactions in the list.
*/
void GuiTransactionList::on_buttonCreateTransferTemplates_clicked(){
 QString otherName; 
 QString ourBankCode; 
 QString ourAccountId;   
 QString otherBankCode; 
 QString otherAccountId;
 QStringList purpose; 
 QStringList CreatedTemplates;
 QString otherId;
 bool isDone;
 banking::Transfer * newTemplate;
   
 if(QMessageBox::question(this, tr("Question"), 
   tr("MoneyPenny will create a transfer template from every outgoing transaction in the list now.") + "<br>"
   + tr("Continue?"), QMessageBox::Yes, QMessageBox::No)
   == QMessageBox::No)
 return;
 
  //first get the existing templates
 banking::TransferList oldTemplates = HoBIT::hobit()->listTransferTemplates();
 banking::TransferList::iterator it;
 int nTemplate = 0;
 for(it = oldTemplates.begin(); it != oldTemplates.end(); it++){
    nTemplate ++;
    otherAccountId = HoBIT_String::toIdFormat((*it)->otherAccountId().c_str());
    otherBankCode = HoBIT_String::toIdFormat((*it)->otherBankCode().c_str());
    otherId= otherBankCode + otherAccountId;
    CreatedTemplates.append(otherId);
 }
 qDebug("GuiTransactionList::slotCreateTransferTemplates: Number of existing transfer templates: %i",nTemplate);
 nTemplate = 0;
 //now check the Transactionlist for transactions, that are not yet in the template list
 for (int index = 0; index < _TransactionItemList .count(); index++){
      banking::TransactionItem item = _TransactionItemList [index];
      banking::Value val = item.value();
      otherBankCode = HoBIT_String::toIdFormat(item.otherBankCode());
      otherAccountId = HoBIT_String::toIdFormat(item.otherAccountId());
      purpose = item.purpose();
      if((val.isDebit()) && (!otherBankCode.isEmpty()) && (!otherAccountId.isEmpty())){ //this is an outgoing transaction
        ourBankCode = HoBIT_String::toIdFormat(item.ourBankCode());
        ourAccountId = HoBIT_String::toIdFormat(item.ourAccountId());
        otherId= otherBankCode + otherAccountId;
        isDone = false;
        QStringList::iterator it;
        for(it = CreatedTemplates.begin(); it != CreatedTemplates.end(); it++){
         if((*it) == otherId)
           isDone = true;
        } 
        if(!isDone  && (!ourBankCode.isEmpty()) && (!ourAccountId.isEmpty())){
          newTemplate = (banking::Transfer *)item.createJob();
          newTemplate->setType(banking::BankingJob::NoType);
          newTemplate->setValue(0.0);
          HoBIT::hobit()->addTransferTemplate(newTemplate);
          nTemplate++;
          CreatedTemplates.append(QString((otherId)));
        }
      }//end if val.isDebit
 }//end for 
 qDebug("GuiTransactionList::slotCreateTransferTemplates: Number of tranfer templates added: %i", nTemplate);
 emit refreshRequest();
 QMessageBox::information(this,tr("Information"), tr("The new transfer templates have been added to the list of templates.") );
 emit raiseWidget(TRANSFER_TEMPLATES_WIDGET);
}

  /** Sorts by column. Does not (yet) work for value.
  */
  void GuiTransactionList::slotSortByColumn(int column){

    if(column == _LastColumn){
      if(_SortOrder == Qt::AscendingOrder)
         _SortOrder = Qt::DescendingOrder; //toggle sorting direction
      else  _SortOrder = Qt::AscendingOrder;
    }
    else _SortOrder = Qt::AscendingOrder;     //always start in ascending order 

    //Sort using class specific operator < and set the sort arrow
    tableWidget->setSortingEnabled(true);
    tableWidget->sortByColumn(column, _SortOrder); 
    _LastColumn = column; 
    //finally adjust all row heights
  //  int hPos = tableWidget->horizontalScrollBar()->value();

    //Due to a bug (or is it a feature??)  in QTableWidget::resizeRowToContents,
    //the highest field (normally the purpose field) must be in the visibilty range.
    //Otherwise the rows are too narrow.
    int oldScrollerValue = tableWidget->horizontalScrollBar()->value();
    tableWidget->horizontalScrollBar()->setValue(0); //scroll purpose into visibility range
    //repaint(); //force repainting
    tableWidget->resizeRowsToContents();
    tableWidget->horizontalScrollBar()->setValue(oldScrollerValue); //scroll back again
 }

/** Update table row and collum sizes
*/
void GuiTransactionList::showEvent (QShowEvent * ){
  tableWidget->horizontalScrollBar()->setValue(0);
  tableWidget->resizeColumnsToContents();
  tableWidget->resizeRowsToContents();
}

/** Update table, if user changes dates.
*/
void GuiTransactionList::on_dateEditFrom_dateChanged(const QDate & ){
  if(! _UserAction)
    return;
  HoBIT::hobit()->setFromTo(dateEditFrom->date(), dateEditTo->date());
  buildTable(tableWidget, 
             _DateFormat,
             HoBIT::hobit()->listTransactions());
  update();
}

/** Update table, if user changes dates.
*/
void GuiTransactionList::on_dateEditTo_dateChanged(const QDate & date){
  on_dateEditFrom_dateChanged(date);
}
