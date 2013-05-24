/***************************************************************************
                          dlgenterpin.h  -  description
                             -------------------
    begin                : Do Aug 19 2004
    copyright            : (C) 2007 by Walter Roth
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

#ifndef DLGENTERPIN_H
#define DLGENTERPIN_H

#include <QDialog>
#include <ui_dlgenterpinbase.h>

/**A simple pin dialog.
  *@author Walter Roth
  */

class DlgEnterPin :public QDialog, public Ui::DlgEnterPinBase  {
   Q_OBJECT
public: 
	DlgEnterPin(QWidget *parent=0, const QString & reason = "");
	~DlgEnterPin();
   /** Must be called prior to exec.on_buttonOk_clicked() will write to thius buffer.
    */ 
   void setPinBuffer(QString * newPin);
  /** Call prior to exec to show the user, what is required to do.
   */
   void setLabelReason(const QString & newVal);
  /** Call prior to exec to show the user the bank's message. e.g. Tan index
   */
   void setLabelMessage(const QString & newVal);
  /** Call prior to exec to show the user, what kind of input is required.
   */
   void setLabelKind(const QString & newVal);
  /** Call prior to exec. Default is 5
   */
   void setMinimumInputLength(const int newVal);
   /** Set focus to the Pin edit
    */
   void showEvent(QShowEvent * se);
protected slots:
   void on_buttonOk_clicked();
   void on_buttonCancel_clicked();
   void on_lineEditPin_returnPressed();
   void on_checkBoxClearText_stateChanged(int state);
private :
 /** External PIN buffer
  */
 QString * _PinBuffer; //this is a copy for single use! DO NOT delete!
 /** The minimum number of digits required. Default: 5
  */
 int _MinimumInputLength;
};

#endif
