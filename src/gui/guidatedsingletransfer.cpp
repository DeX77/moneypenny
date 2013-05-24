/***************************************************************************
                          guidatedsingletransfer.cpp  -  description
                             -------------------
    begin                : Sa Nov 13 2004
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


#include "globals.h"
#include "guidatedsingletransfer.h"
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlineedit.h>

/**
*constructor of GuiSingleTransfer
*
*@param main a pointer on moneypenny´s main class
*@param parent a pointer on the main widgets widget stack
*@param name widgets title
*@param hobit the hobit object, needed for hbci functions
*/
GuiDatedSingleTransfer::GuiDatedSingleTransfer(QWidget *parent)
:GuiTransferForm(parent) {
   textLabelCaption->setText(tr("Dated Transfer"));
   setType (banking::BankingJob::DatedTransfer); 
}

/**
*destructor of GuiSingleTransfer
*/
GuiDatedSingleTransfer::~GuiDatedSingleTransfer(){
	qDebug("Destructor GuiDatedSingleTransfer finished");
}

/** 
* called function after pressing button of formular,
* gets current inputs of formular for transferjob  
*/
void GuiDatedSingleTransfer::on_buttonExecute_clicked(){
  _FirstDate = dateEditFirst->date();
  GuiTransferForm::on_buttonExecute_clicked();
}

/** Builds the html table rows for the reconfirmation dialog. Returns a string that ist to be included into <table> </table tags. 
*/
QString GuiDatedSingleTransfer::reconfirmationTableRows(){
  QString result = GuiTransferForm::reconfirmationTableRows();
  QDate d = dateEditFirst->date();
  result += "<tr>";
  result += "</tr>\n";

  result += "<tr>";
  result += "<td> <b>" +tr("Date of execution:") + "</b></td>";
  result += "<td><b>" + d.toString(Qt::LocalDate) +  "</td></b>";
  result += "</tr>\n";
  return result;
}

/** Creates the dated single transfer job and returns true on success.
  */
 bool GuiDatedSingleTransfer::createJob(){
    if(! GuiTransferForm::createJob())
      return false;
    QDate currentDate = QDate::currentDate();
    QDate nextYear = QDate(currentDate.year() + 1, currentDate.month(), currentDate.day());
   HoBIT::hobit()->createJobGetDatedTransfers(currentDate,
                                      nextYear,
                                      HoBIT::hobit()->currentAccountId());
   return true; 
 }

 /** Creates a dated transfer.
  */
banking::Transfer * GuiDatedSingleTransfer::createTransfer()
{
 banking::Transfer * transfer = GuiTransferForm::createTransfer();
 DateTime executionDate ( _FirstDate.year(), _FirstDate.month(), _FirstDate.day() );
 transfer->setFirstExcecutionDate(executionDate);
 return transfer;
}
