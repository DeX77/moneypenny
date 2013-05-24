/***************************************************************************
 *   Copyright (C) 2004, 2007 by Walter Roth                               *
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
#ifndef GUITRANSFERFORM_H
#define GUITRANSFERFORM_H

#include <ui_guitransferformbase.h>

#include <QWidget>
#include <QVector>
#include <QStringList>

#include "hobit.h"
#include "kernel/helpers/transfer.h"

/**
* Class for all types of transfer forms. When setType is called, appearance is adjusted to needs for
* the respective type of transfer. After data have been accepted by user,a new transfer
* is added to the jobmanager of the hobit banking engine. 
*/
class GuiTransferForm : public QWidget, public Ui::GuiTransferForm
{
  Q_OBJECT
public:

  /**
  *constructor of GuiSingleTransfer
  *
  *@param parent a pointer to the main widget's widget stack
  *@param hobit the hobit object, needed for hbci functions
  */
  GuiTransferForm(QWidget *parent);

  /**
  *destructor 
  */
  virtual ~GuiTransferForm();

  /**Sets _type and hides  or shows the appropriate widgets
  */
  void setType(banking::Transfer::JobType type);

  /**
  *Set Current used Account to Label in Formular 
  */
  void refresh();
  /**Call nextLine, if cursor is in purpose edits.
  */
  void keyPressEvent(QKeyEvent* ke);

signals:
 void cancelled();
 void refreshRequest();
 void templateRequest(banking::BankingJob::JobType type);
 void sendNow(bool *, const QString &, bool);
 void raiseWidget(int PageId);


public slots:

  /**
  *gets current inputs of form for transferjob. Emits sendNow
  */
  void on_buttonExecute_clicked();
   
  /**
  * Saves the current transfer as a transfer template
  */ 
  void on_buttonSaveAsTemplate_clicked();
  
  
  /**
  * Loads a template from the template list
  */ 
  void on_buttonFromTemplate_clicked();
  
  /**
  *called on reject, emit signal cancelled()
  */
  void reject();
  /** Clears all edits.
  */
  void on_buttonClearForm_clicked();

  /**
  *validation of lineEdit name of receiver, 
  *opens messagebox if validation is false
  */
  void on_lineEditOtherName_textChanged(const QString & text);

  /**
  *validation of lineEdit accountId, 
  *opens messagebox if validation is false
  */
  void on_lineEditOtherAccountId_textChanged(const QString & text);

  /**
  *validation of lineEdit bankCode, 
  *opens messagebox if validation is false 
  */
  void on_lineEditOtherBankCode_textChanged(const QString & text);

  /**
  *validation of lineEdit Purpose1, 
  *opens messagebox if validation is false
  */
  void on_lineEditPurpose1_textChanged(const QString & text);

  /**
  *validation of lineEdit Purpose1, 
  *opens messagebox if validation is false
  */
  void on_lineEditPurpose2_textChanged(const QString & text);
  
  /** Called on scrollbar valueChanged
  */
  void on_scrollBarLines_valueChanged(int value);
  
  /** Sets _PurposeLines and purpose edits
  */
  void setPurpose(const QStringList & purpose);
  /** Sets _PurposeLines and purpose edits
  */
  void setPurpose(const list <string> & purpose);
  /** Scrolls the purpose lines upwards
  */
  void slotNextLine();
  /** Scrolls the purpose lines downwards
  */
  void slotPrevLine();
  /** Sets edits from a transfer used as a template.
   *  Transfer is deleted after use.
   */
  void slotShowTemplateData(banking::Transfer * transfer);
protected:
 /** Checks for empty fields, BLZ for 8 digits and value for !=0.0
 */
 bool isOk();
 /** Checks whether a scrollbar is neccessary. Returns 
     true, if scrollbar should be made visible.
   */
 bool isScrollBarRequired();
 /** Creates the job and reurns true on success. To be overwritten.
  */
 virtual bool createJob();
 /** Creates a transfer and adds it to the list in JobManager.
  */
 virtual banking::Transfer * createTransfer();
  /** Validates purpose string. Only DTAUS characters are allowed here.
  */ 
 bool validatePurpose(QString purpose, QString & _LastInput);
 /** updates the purpose list and returns number of purpose lines.
 */
 int updatePurposeList();
 /** Asks the user for reconfirmation. Returns true, if user clicks yes.
 */
 int userReconfirmation();
 /** Builds the html table rows for the reconfirmation dialog. Returns a string that ist to be included into <table> </table tags. 
 */
 QString reconfirmationTableRows();
 /** Updates the editor's text and takes care of cursor position in
  *  replaced text.
  */
 void updateEditor( QLineEdit * edit, const QString & newText);
   
  /** Clears the form.
  */
  void clear();

protected:
  banking::BankingJob::JobType _Type;
  // help-vars for input of lineEdits referring to validation
  QString _LastInputOtherName;
  QString _LastInputOtherAccountId;
  QString _LastInputOtherBankCode;
  QString _LastInputValue;
  QString _LastInputPurpose;
  QString _LastInputPurpose2;
  QList<banking::Bank> _Banks;
  QDate   _FirstDate;
  QDate   _LastDate;
  
  /**Fields for createJob
   */
  QString _OurAccountId;    //DO NOT strip any leading zeros here!
  QString _OurBankCode;
  BankType _OurBankType;
  QString _OtherName;
  QString _OtherBankCode;
  QString _OtherBic;
  QString _OtherAccountId;
  QString _OtherIban;
  double   _Value;
 
 /** The header to be used for the reconfirmation message.
 */
  QString _ReconfirmationHeader;

 /** The type of widget, as specified by the Widget number.
 * Used for setting the radiobuttons  in template widget.
 */
 banking::Transfer::JobType _JobType; 
 /**The 14 lines with 27 chars legal for a HBCI purpose. Vector allows scrolling.
 */
 QVector<QString> _PurposeLines;
 
 /** The final purpose list. Contains no blank lines. Call updatePurposeList prior to using this list.
 */
 list <string> _PurposeList;
 /** Flag for blocking validation when scrolling
 */
 bool _IsScrolling;
 /**Flag for blocking data reset after saving a template.
  */
 bool _TemplateCreated; 
};

#endif
