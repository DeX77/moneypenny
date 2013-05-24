/***************************************************************************
                          guidatedsingletransfer.h  -  description
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

#ifndef GUIDATEDSINGLETRANSFER_H
#define GUIDATEDSINGLETRANSFER_H

#include <qwidget.h>
#include "hobit.h"
#include <guitransferform.h>



class GuiMain;

/**
* class for formular for dated SingleTransfers with Validation
*/
class GuiDatedSingleTransfer : public GuiTransferForm {
   Q_OBJECT
public: 
  
  /**
  *this is the constructor for the formular
  *
  *@param main
  *@param parent the parent dialog of this widget
  *@param name widgets title
  *@param hobit
  */
	GuiDatedSingleTransfer(QWidget *parent);
	~GuiDatedSingleTransfer();

 
public slots: // Public slots
  /**
  *gets current inputs of form for transferjob. Emits sendNow
  */
  void on_buttonExecute_clicked();

protected:
 /** Builds the html table rows for the reconfirmation dialog. Returns a string that ist to be included into <table> </table tags. 
 */
 QString reconfirmationTableRows(); 
 
/** Creates a dated single transfer job and returns true on success.
  */
 virtual bool createJob();
 /** Creates a dated transfer.
  */
 virtual banking::Transfer * createTransfer();

};

#endif
