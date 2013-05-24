/***************************************************************************
 *   Copyright (C) 2007 by Marcel Mulch, Walter Roth                                    *
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
#ifndef GUITRANSACTIONINFO_H
#define GUITRANSACTIONINFO_H

#include <ui_guitransactioninfobase.h>

class HoBIT;
class DateTime;

namespace banking {
  class TransactionItem;
}

/**
* This is the form for showing all sorts of transactions. GUI elements, that are not required are simply hidden.
* @author Walter Roth
*/
class GuiTransactionInfo : public QWidget, public Ui::GuiTransactionInfoBase
{
   Q_OBJECT
public:
    GuiTransactionInfo(QWidget * parent);
    ~GuiTransactionInfo();
    /** Sets all labels from transaction data
     */
    void setTransactionItem(const banking::TransactionItem & t);

    /**
      *Set Current used Account to Label in Formular 
      */
	virtual void refresh();
	
	 /**
      *Clears all textfields
      */
	virtual void clearAll();

public slots:
   void on_buttonBack_clicked();

signals:
  void canceled();
 
private:
  /** convenience date formatting
  */
  QString toLocalDateFormat(const QDate & d);
};

#endif
