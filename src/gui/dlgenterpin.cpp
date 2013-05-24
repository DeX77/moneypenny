/***************************************************************************
                          dlgenterpin.cpp  -  description
                             -------------------
    begin                : Do Aug 19 2004
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

#include "dlgenterpin.h"

#include <QMessageBox>

DlgEnterPin::DlgEnterPin(QWidget *parent, const QString & reason) 
 : QDialog(parent), Ui::DlgEnterPinBase()
{
 setupUi(this);
 _PinBuffer = NULL;
 _MinimumInputLength
  = 5;
 labelReason->setText(reason);
 labelMessage->setText("");
 qDebug("Constructor DlgEnterPin");
}

DlgEnterPin::~DlgEnterPin(){
 qDebug("Destructor DlgEnterPin");
}

void DlgEnterPin::on_buttonOk_clicked(){
  if(! _PinBuffer){
    QMessageBox::critical(this, 
     "Error", "DlgEnterPin::on_buttonOk_clicked() called without a valid pin buffer");
    reject();
  }

  if(lineEditPin->text().length() >= _MinimumInputLength){
    * _PinBuffer = lineEditPin->text(); // copy the pin to external buffer
    _PinBuffer = NULL; // use the pointer copy only once
    lineEditPin->setText(""); //Delete the pin string
    accept();
   }
   else QMessageBox::critical(this, 
        tr("Error"), 
        tr("Input is too short. \nMinimum number of digits required: ")
         + QString::number(_MinimumInputLength));
}

void DlgEnterPin::on_buttonCancel_clicked(){
  if(_PinBuffer){
    * _PinBuffer = ""; // return an empty string
  _PinBuffer = NULL; // use the pointer copy only once
  }
  reject();
}

void DlgEnterPin::on_lineEditPin_returnPressed(){
 on_buttonOk_clicked();
}

void DlgEnterPin::on_checkBoxClearText_stateChanged(int state){
  if(state == Qt::Checked)
    lineEditPin->setEchoMode(QLineEdit::Normal);
  else lineEditPin->setEchoMode(QLineEdit::Password);
}


void DlgEnterPin::setPinBuffer(QString * newPin){
 _PinBuffer = newPin;
 lineEditPin->setText("");
} 

void DlgEnterPin::setLabelReason(const QString & newVal)
{
    labelReason->setText(newVal);
    update();
}

void DlgEnterPin::setLabelMessage(const QString & newVal)
{
    labelMessage->setText(newVal);
    if(newVal.isEmpty()){
      labelMessage->hide();
      lineEditPin-> setEchoMode(QLineEdit::Password);
    }
    else {
      labelMessage->show();
      lineEditPin-> setEchoMode(QLineEdit::Normal);
    }
    update();
}

void DlgEnterPin::setLabelKind(const QString & newVal)
{
    labelKind->setText(newVal);
    update();
}

/** Call prior to exec. Default is 5
 */
void DlgEnterPin::setMinimumInputLength(const int newVal){
 _MinimumInputLength = newVal;
}

/** Set focus to the Pin edit
 */
void  DlgEnterPin::showEvent(QShowEvent * ){
  checkBoxClearText->setCheckState(Qt::Unchecked);
  lineEditPin-> setFocus();
}
