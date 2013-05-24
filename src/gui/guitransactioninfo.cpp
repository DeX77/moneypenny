/***************************************************************************
 *   Copyright (C) 2004 by Marcel Mulch, Walter Roth 2005                                    *
 *   marcel@linux                                                          *
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
#include "guitransactioninfo.h"
#include <QDate>
#include <QLabel>

#include "hobit.h"
#include "transactionitem.h"


GuiTransactionInfo::GuiTransactionInfo(QWidget * parent)
 : QWidget(parent){
    setupUi(this);
   setObjectName("GuiTransactionInfo");
   qDebug("Constructor GuiTransactionInfo");
}


GuiTransactionInfo::~GuiTransactionInfo()
{
  qDebug("Destructor GuiTransactionInfo");
}

void GuiTransactionInfo::on_buttonBack_clicked(){
  emit canceled();
}

void GuiTransactionInfo::setTransactionItem(const banking::TransactionItem & t){
   clearAll();

   if(t.type() == banking::BankingJob::BookedTransaction)
    {
      labelPrimanota1->show();
      labelPrimanota2->setText(t.primanota());
      labelPrimanota2->show();
      labelFirstExecution1-> setText(tr("Booking Date"));
      labelFirstExecution1-> show();
      labelFirstExecution2->setText(toLocalDateFormat(t.bookingDate()));
      labelFirstExecution2-> show();
      labelLastExecution1->setText(tr("Valuta Date"));
      labelLastExecution1-> show();
      labelLastExecution2->setText(toLocalDateFormat(t.valutaDate()));
      labelLastExecution2-> show();
      labelThree1->setText(tr("Store Date"));
      labelThree1->show();
      labelThree2->setText(toLocalDateFormat(t.storeDate()));
      labelThree2->show();
      labelType->setText("<h3><p align=\"center\">" + tr("Single Transfer") +  "</p></h3>");
    }
   else if(t.type() == banking::BankingJob::SingleTransfer)
    {
      labelPrimanota1->show();
      labelPrimanota2->setText(t.primanota());
      labelPrimanota2->show();
      labelFirstExecution1-> setText(tr("Booking Date"));
      labelFirstExecution1-> show();
      labelFirstExecution2->setText(toLocalDateFormat(t.bookingDate()));
      labelFirstExecution2-> show();
      labelLastExecution1->setText(tr("Valuta Date"));
      labelLastExecution1-> show();
      labelLastExecution2->setText(toLocalDateFormat(t.valutaDate()));
      labelLastExecution2-> show();
      labelThree1->setText(tr("Store Date"));
      labelThree1->show();
      labelThree2->setText(toLocalDateFormat(t.storeDate()));
      labelThree2->show();
      labelType->setText("<h3><p align=\"center\">" + tr("Single Transfer") +  "</p></h3>");
    }
    else if(t.type() == banking::BankingJob::DatedTransfer)
    {
      labelPrimanota1->hide();
      labelPrimanota2->setText("");
      labelPrimanota2->hide();
      labelFirstExecution1->setText(tr("Execution Date"));
      labelFirstExecution1-> show();
      labelFirstExecution2->setText(toLocalDateFormat(t.firstExecutionDate()));
      labelFirstExecution2-> show();
      labelLastExecution1-> hide();
      labelLastExecution2->setText("");
      labelLastExecution2-> hide();
      labelThree1->setText("");
      labelThree1->hide();
      labelThree2->setText("");
      labelThree2->hide();
      labelType->setText("<h3><p align=\"center\">" + tr("Dated Transfer") +  "</p></h3>");
    }
    else if(t.type() == banking::BankingJob::StandingOrder)
    {
      labelPrimanota1->hide();
      labelPrimanota2->setText("");
      labelPrimanota2->hide();
      labelFirstExecution1->setText(tr("First Execution"));
      labelFirstExecution1-> show();
      labelFirstExecution2->setText(toLocalDateFormat(t.firstExecutionDate()));
      labelFirstExecution2-> show();
      labelLastExecution1->setText(tr("Last Execution"));
      labelLastExecution1-> show();
      labelLastExecution2->setText(toLocalDateFormat(t.lastExecutionDate()));
      labelLastExecution2-> show();
      labelThree1->setText(tr("Interval"));
      labelThree1->show();
      labelThree2->setText(t.interval());
      labelThree2->show();
      labelType->setText("<h3><p align=\"center\">" + tr("Standing Order") +  "</p></h3>");
    }


   // own account
   labelOurAccountId2->setText(t. ourAccountId());
   labelOurName2->setText(HoBIT::hobit()->currentAccount()->ownerName().c_str());
   labelOurBankCode2->setText(t.ourBankCode());
   labelOurCountry2->setText(QString::number(t.ourCountry()));

   // other account
   labelOtherAccountId2->setText(t.otherAccountId());
   labelOtherName2->setText(t.otherName1() + t.otherName2());
   labelOtherBankCode2->setText(t.otherBankCode());
   labelOtherCountry2->setText(t.otherCountry());

   labelValue2->setText(t.stringValue());
   labelPurpose->setText(t.purpose().join("\n"));
}




/**
*Set Current used Account to Label in Formular 
*/
void GuiTransactionInfo::refresh()
{
}

/**
* Clears all textfields
*/
void GuiTransactionInfo::clearAll()
{
// own account
   labelOurAccountId2->setText("");
   labelOurBankCode2->setText("");
   labelOurCountry2->setText("");

   // other account
   labelOtherAccountId2->setText("");
   labelOtherBankCode2->setText("");
   labelOtherCountry2->setText("");

   labelPurpose->setText("");

   labelFirstExecution2->setText("");
   labelLastExecution2->setText("");
   labelThree2->setText("");

   labelPrimanota2->setText("");
   labelValue2->setText("");
}

/** convenience date formatting
*/
QString GuiTransactionInfo::toLocalDateFormat(const QDate & d){
   return d.toString ( Qt::LocalDate);
}
