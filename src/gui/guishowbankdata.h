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
#ifndef GUISHOWBANKDATA_H
#define GUISHOWBANKDATA_H

#include "ui_guishowbankdatabase.h"

class HoBIT;

class QString;
/** Show the present data base. Mainly for debugging.
 */
class GuiShowBankData : public QWidget, public Ui::GuiShowBankData
{
 Q_OBJECT
  public:
    /** Constructor
    */
    GuiShowBankData(QWidget *parent);
    /**Destructor
    */
  virtual  ~GuiShowBankData();
  
    /** Updates the data listview.
    */
    void refresh();
    
  public slots:
    /** Emits signal back.
    */
    void slotBack();
    /** Emits signal help request.
    */
    void slotHelp();

  signals:
    void back();
    void helpRequest(const QString& );
    
  private:
    /**
    * updates the list of users
    */
    void updateList();
       
   /** The HoBIT object to query
   */
   HoBIT * _Hobit;  
};

#endif
