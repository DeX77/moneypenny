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
#include "guitransferform.h"
#include "hobit_string.h"
#include "dlgconfirmtransfer.h"

#include "globals.h"
#include <QtGui>

using namespace std;



/**
*constructor of GuiTransferForm
*
*@param main a pointer to moneypenny´s main class
*@param parent a pointer on the main widgets widget stack
*@param name widgets title
*@param hobit the hobit object, needed for hbci functions
*/
GuiTransferForm::GuiTransferForm(QWidget *parent )
:QWidget(parent) {
  setupUi(this);
  setObjectName("GuiTransferForm");
 _OurBankType = HoBIT::hobit()->bankType(); 
 _TemplateCreated = false;
 refresh();
 _Banks = HoBIT::hobit()->listOfAllKnownBanks();
 QRegExp rx("[0-9]{1,9},{1,1}[0-9]{1,2}"); // one to seven digits, a comma and one or two digits
 lineEditValue->setValidator(new QRegExpValidator(rx, lineEditValue));
 
 //initialize purpose lines with empty strings
 _PurposeLines.resize(scrollBarLines->maximum() + 2);
 _IsScrolling = false;
 // Clear Form
 clear();
 setType(banking::BankingJob::SingleTransfer);
}

/**
*destructor of GuiTransferForm
*/
GuiTransferForm::~GuiTransferForm(){
  qDebug("Destructor GuiTransferForm finished");
}

/**Call nextLine, if cursor is in purpose edits.
*/
void GuiTransferForm::keyPressEvent(QKeyEvent* ke){
  // go to next line, if one of the purpose edits has focus
  if((ke->key() == Qt::Key_Return)
    && (lineEditPurpose1->hasFocus() | lineEditPurpose2->hasFocus())){
    slotNextLine();
    return;
  }
  else QWidget::keyPressEvent(ke);
}

/** 
 * This is the default button slot. It is also called, whenever return is pressed.
*called function after pressing button of formular,
*gets current inputs of formular for transferjob  
*/
void GuiTransferForm::on_buttonExecute_clicked(){

  if(!isOk())
   return;
  
  int command = userReconfirmation();
  if ( command == DlgConfirmTransfer::Reject) 
    return;

  //now collect the data for the transfer, create a transfer and add it to the list
  _OurAccountId = HoBIT::hobit()->currentAccountId();    //DO NOT strip any leading zeros here!
  _OurBankCode = HoBIT::hobit()->bankCode();
  _OurBankType = HoBIT::hobit()->bankType();
  _OtherName = lineEditOtherName->text();
  _OtherBankCode = lineEditOtherBankCode->text().replace(" ",""); //strip blanks
  _OtherAccountId = HoBIT_String::toIdFormat(lineEditOtherAccountId->text().replace(" ","")); //strip blanks
  QString value = lineEditValue->text();
  QChar decimalPoint = QLocale::system().decimalPoint();//replace all candidates with current decimal point
  value.replace( ',', decimalPoint );
  value.replace( '.', decimalPoint );
  _Value = value.toDouble(); //this should work now

  createJob();

  switch (command){
   case DlgConfirmTransfer::SendLater:
        {
          QMessageBox::information(this,tr("Information"),
           tr("The job was added to the job queue."));
          break;
         }
   case DlgConfirmTransfer::SendNow:
         {
           emit sendNow(NULL, tr("Sending jobs in the lists."), true);
           break;
         }
   case DlgConfirmTransfer::SendNowAndGetTransactions:
         {
            HoBIT::hobit()->createJobGetBalance("*");
            HoBIT::hobit()->createJobGetTransactions(HoBIT::hobit()->lastDateOfTransactions(), QDate::currentDate(), "*");
            emit sendNow(NULL, tr("Job get transactions added. Sending jobs in the lists."), true);
            break;
         }
   default: return; //emergency break
  }
  clear(); //finally clear form, to indicate that job is in queue now 
 }

void GuiTransferForm::on_buttonSaveAsTemplate_clicked(){
  banking::Transfer * newTemplate;

  QString ourAccountId = HoBIT::hobit()->currentAccountId();
  QString ourIban = HoBIT::hobit()->currentIban();
  QString ourBankCode = HoBIT::hobit()->bankCode();
  QString ourBic = HoBIT::hobit()->getBicForBankCode(qPrintable(ourBankCode));
  QString otherName = lineEditOtherName->text();
  QString otherBankCode = lineEditOtherBankCode->text();
  QString otherBic = HoBIT::hobit()->getBicForBankCode(otherBankCode);
  QString otherAccountId = lineEditOtherAccountId->text();
  QString otherIban = ""; //TODO Replace with valid value!

  updatePurposeList();
  newTemplate = 
     new banking::Transfer(banking::BankingJob::NoType,
                  0.0, //value
                  qPrintable(ourAccountId),
                  qPrintable(ourIban),
                  qPrintable(ourBankCode),
                  qPrintable(ourBic),
                  _OurBankType,
                  qPrintable(otherAccountId),
                  qPrintable(otherIban),
                  qPrintable(otherBankCode),
                  qPrintable(otherBic),
             //optional parameters
                  qPrintable(otherName.left(27)), 
                  qPrintable(otherName.mid(27, 27)), 
                  _PurposeList);
   if(newTemplate->isValidTemplate()) {
     HoBIT::hobit()->addTransferTemplate(newTemplate);
     _TemplateCreated = true;
     emit refreshRequest();
     QMessageBox::information(this, tr("Information"), tr("This transfer has been added to the list of transfer templates."));
  }
  else {
    delete newTemplate;
    QMessageBox::critical(this, tr("Error"), tr("Please fill in all fields. Value and purpose fields may be empty."));
  }
}

/** restores a template */
void GuiTransferForm::on_buttonFromTemplate_clicked(){
  emit templateRequest(_Type); //send this to adjust the radio buttons in template form
  emit raiseWidget(TRANSFER_TEMPLATES_WIDGET);
}


/**
*Set all data in edits
*/
void GuiTransferForm::refresh()
{
    if(_TemplateCreated){ //DO NOT update, if refresh request is due do template saving
        _TemplateCreated= false;
        return;
    }
    textLabelAccount->setText(HoBIT::hobit()->currentAccountId());
    textLabelName->setText(HoBIT::hobit()->userName());
    textLabelBankId->setText(HoBIT::hobit()->bankCode());
    QDate curdate; curdate = curdate.currentDate();
    dateEditFirst->setDate(curdate);

    dateEditLast->setDate(QDate(curdate.year(),12,31));

    spinBoxDay->setMinimum(1);
    spinBoxDay->setMaximum(30);
}

/** 
*validation of lineEdit name of receiver, 
*opens messagebox if validation is false
*/
void GuiTransferForm::on_lineEditOtherName_textChanged(const QString &)
{ 
 // set input of lineEdit to var
 QString otherName = HoBIT_String::replaceSpecialChars(lineEditOtherName->text());
 if((HoBIT::hobit()->bankType() == BANKTYPE_SPARKASSE)
 || (HoBIT::hobit()->bankType() == BANKTYPE_POSTBANK)) //Sparkasse and Postbank need UPPERCASE
   otherName = otherName.toUpper();
// QString otherName = lineEditOtherName->text();
 // regular expression allows only letters and blanks with a length of 0 to 35
 QString exp = "[a-z,A-Z,ä,ö,ü,Ä,ö,Ü,ß,0-9,\\-,\\+,\\s\\,\\.]{0,54}";  //regex seems to be buggy with respect to Umlaute
 QRegExp rx(exp);
 QRegExpValidator v( rx, 0 );
 int pos = 0;
 // if validation is true current input get saved to var. var is used for check next time the text get changed
 if(v.validate(otherName, pos)==QValidator::Acceptable  || otherName.isEmpty()) _LastInputOtherName = otherName;
 // if validation is false, show messagebox
 else{
   QMessageBox::warning( this,
                         tr("Error"),
                         tr("Input Error\n Only letters are allowed here. Not more than 54 letters"),
                         tr("OK"), 
                         QString::null, 0, 1 ) ;
                         }
  updateEditor(lineEditOtherName, _LastInputOtherName);
 }

/** 
*validation of lineEdit accountId, 
*opens messagebox if validation is false
*/
void GuiTransferForm::on_lineEditOtherAccountId_textChanged(const QString &)
{
   // set input of lineEdit to var
   QString otherAccountId = lineEditOtherAccountId->text().replace(" ",""); //strip blanks
  
   // if validation is true current input get saved to var. var is used for check next time the text get changed
   if(HoBIT::hobit()-> validateAccountIdString(otherAccountId)) 
     _LastInputOtherAccountId = lineEditOtherAccountId->text();
   // if validation is false messagebox get opened for show that an invalid input was made
   else { 
     lineEditOtherAccountId->setText(_LastInputOtherAccountId);
     QMessageBox::warning( this,
                         tr("Error"),
                         tr("Input Error\n Only digits are allowed here.\nMaximum is 15 digits."),
                         tr("OK"), 
                         QString::null, 0, 1 ) ;
   }
 }

/** 
*validation of lineEdit bankCode, 
*opens messagebox if validation is false 
*/
void GuiTransferForm::on_lineEditOtherBankCode_textChanged(const QString & )
{
  // set input of lineEdit to var
  QString otherBankCode = lineEditOtherBankCode->text();
  otherBankCode = otherBankCode.replace(" ", ""); //strip all blanks
   // regular expression allows only digits with a length of 0 to 15
  QRegExp rx( "[0-9]{0,8}" );
  QRegExpValidator v( rx, 0 );
  int pos = 0;
 
  // if validation is true current input is saved to _LastInputOtherBankCode.
  if(v.validate(otherBankCode, pos)==QValidator::Acceptable  || otherBankCode.isEmpty()) _LastInputOtherBankCode = lineEditOtherBankCode->text();
  // if validation is false messagebox get opened for show that an invalid input was made
  else { lineEditOtherBankCode->setText(_LastInputOtherBankCode); //restore last valid input
  QMessageBox::warning( this,
                       tr("Error"),
                       tr("Input Error\n Only digits are allowed here. Exactly 8 digits required."),
                       tr("OK"), 
                       QString::null, 0, 1 ) ;
                       }
 
  //we are ready to check for a bank
  if(otherBankCode.length() == 8){
  int nBank = 0;
    QString bankName = tr("Could not find a bank with the code you input.");
    if (!_Banks.empty()){
      bool found = false;
      QList<banking::Bank>::iterator bit = _Banks.begin();
      banking::Bank & bank =*bit;
      string otherBankCodeString = qPrintable(otherBankCode);
       for (bit = _Banks.begin(); (bit != _Banks.end()) && (! found); bit++)
      {
        nBank++;
        bank =*bit;
        if(bank.bankCode()== otherBankCodeString){
          bankName = bank.bankName().c_str();
          found = true; // bank found, stop it
        }
      }// end for
      if(found && bank.isToBeDeleted()){
        QString msg;
        if( bank.futureBankCode().empty() || (bank.futureBankCode() == "00000000") )
           msg = tr("No future bankcode given.");
        else msg = tr("The future bankcode will be:") + " " + bank.futureBankCode().c_str();
        QMessageBox::warning(this, tr("Warning"), tr("This bankcode is marked as to be deleted soon.")
                             + "\n"  + msg);
      }
    }// end if ! empty
    textLabelBankName->setText(bankName);
    
  }//end if length == 8
  else textLabelBankName->setText("");
}

/** 
*validation of lineEdit Purpose1, 
*opens messagebox if validation is false
*/
void GuiTransferForm::on_lineEditPurpose1_textChanged(const QString & )
{
 if(_IsScrolling) //No validation when scrolling, otherwise an endless loop is created
   return;
 // set input of lineEdit to var 
 QString purpose = HoBIT_String::replaceSpecialChars(lineEditPurpose1->text());
 if((HoBIT::hobit()->bankType() == BANKTYPE_SPARKASSE)
 || (HoBIT::hobit()->bankType() == BANKTYPE_POSTBANK)) //Sparkasse and Postbank need UPPERCASE
   purpose = purpose.toUpper();
 validatePurpose(purpose, _LastInputPurpose);
 updateEditor(lineEditPurpose1, _LastInputPurpose);
 _PurposeLines[scrollBarLines->value()] = _LastInputPurpose;
}

/** 
*validation of lineEdit Purpose2, 
*opens messagebox if validation is false
*/
void GuiTransferForm::on_lineEditPurpose2_textChanged(const QString & )
{
 if(_IsScrolling) //No validation when scrolling, otherwise an endless loop is created
   return;
 // set input of lineEdit to var 
 QString purpose = HoBIT_String::replaceSpecialChars(lineEditPurpose2->text());
 if((HoBIT::hobit()->bankType() == BANKTYPE_SPARKASSE)
 || (HoBIT::hobit()->bankType() == BANKTYPE_POSTBANK)) //Sparkasse and Postbank need UPPERCASE
   purpose = purpose.toUpper();
 validatePurpose(purpose, _LastInputPurpose2);
 updateEditor(lineEditPurpose2, _LastInputPurpose2);
 _PurposeLines[scrollBarLines->value() + 1 ] = _LastInputPurpose2;
 if(_LastInputPurpose2.length() >= 27){
    slotNextLine();
 }
 else if(lineEditPurpose2->text() == "")
   slotPrevLine();
}

/**
*called on reject, emit signal cancelled()
*/
void GuiTransferForm::reject(){
 // Clear Form
 clear();
 emit cancelled();
}

/** Clears the form.
*/
void GuiTransferForm::clear(){
 //clear gui
 lineEditOtherName->setText("");
 lineEditOtherBankCode->setText("");
 lineEditOtherAccountId->setText("");
 lineEditValue->setText("");
 dateEditFirst->setDate(QDate::currentDate());
 dateEditLast->setDate(QDate::currentDate());
 
 //Clearing purpose is tricky, because of triple buffering
 //first of all clear buffers
 _LastInputPurpose= "";
 _LastInputPurpose2= "";
 //clear _PurposeLines and _PurposeList
 for(uint nLine = 0; nLine < 14; nLine++)
   _PurposeLines[nLine] = "";
 _PurposeList.clear();
 scrollBarLines->hide();
 //finally clear edits. This causes textChanged signals!
 lineEditPurpose1->setText("");
 lineEditPurpose2->setText("");
}

/** Validates purpose string. Only DTAUS characters are allowed here.
*/
bool GuiTransferForm::validatePurpose(QString purpose, QString & lastInput){
  // regular expression allows letters, blanks and digits with a length of 0 to 20
 QRegExp rx(tr("[a-z,A-Z,0-9,/,\\.\\,\\-\\s]{0,28}", "0 to 27 characters legal for a purpose"));
 QRegExpValidator v( rx, 0 );
 int pos = 0;
 
 //illegal char, display message and keep old string
 if(v.validate(purpose, pos) == QValidator::Acceptable  || purpose.isEmpty()) 
   lastInput = purpose;
 else {
   QMessageBox::warning( this,
                         tr("Error"),
                         tr("Input Error\n This character is not allowed here."),
                         tr("OK"), 
                         QString::null, 0, 1 ) ;
   return false;
 }
 // too long, display message and truncate
 if(purpose.length() > 27)
 { 
   QMessageBox::warning( this,
                         tr("Error"),
                         tr("Input Error\n If you need more than 27 characters, please use the next line."),
                         tr("OK"), 
                         QString::null, 0, 1 ) ;
   lastInput.truncate(27);  
   return false;
 }
 else return true;
}

/** Asks the user for reconfirmation. Returns true, if user clicks yes.
*/
int GuiTransferForm::userReconfirmation(){
  QString text = "<table>\n" +  reconfirmationTableRows() + "</table>\n";  

  //check for pending jobs
  int jobs = HoBIT::hobit()->pendingJobs(); 
  if(jobs > 0)
    text += "<p><b>" + tr("Pending jobs in job queue:") + " " + QString::number(jobs) + "</b></p>";

  //ask user for action
  DlgConfirmTransfer dlg(this, text); 
  dlg.exec();
  return dlg.getUserCommand();
}

/** Builds the html table rows for the reconfirmation dialog. Returns a string that is to be included into <table> </table tags. 
*/
QString GuiTransferForm::reconfirmationTableRows(){
  QString result;
  QString ourAccount = HoBIT::hobit()->currentAccountId();   
  QString name = lineEditOtherName->text();
  QString bankCode = lineEditOtherBankCode->text();
  QString accountId = lineEditOtherAccountId->text();
  QString purpose1 =  lineEditPurpose1->text();
  QString purpose2 =  lineEditPurpose2->text();
  QString value = lineEditValue->text();
  // text for show all inputs of form, used in messagebox

  result = "<tr><td colspan =2>" +   _ReconfirmationHeader +"</td></tr>\n";

  result += "<tr>";
  result += "<td>" + tr("From Account:") + "</td>";
  result += "<td>" + ourAccount + " " +HoBIT::hobit()->currentAccountName() + "</td>";
  result += "</tr>\n";

  result += "<tr>";
  result += "<td>" + tr("To Account:")+ "</td>";
  result += "<td>" + accountId +  "</td>";
  result += "</tr>\n";

  result += "<tr>";
  result += "<td>" + tr("At Bank:")+  "</td>";
  result += "<td>" + bankCode + " " + textLabelBankName->text() + "</td>";
  result += "</tr>\n";

  result += "<tr>";
  result += "<td>" + tr("Amount:") +  "</td>";
  result += "<td>" + value +  tr(" EUR")+ "</td>";
  result += "</tr>\n";

  if(updatePurposeList() == 0){
    result += "<tr>";
    result += "<td>" + tr("No Purpose") +  "</td>";
    result += "<td>"  "</td>";
    result += "</tr>\n";
  }  
  else {
    list<string>::iterator it;
    it= _PurposeList.begin();
    QString line = (*it).c_str();
    result += "<tr>";
    result += "<td>" + tr("Purpose:") +  "</td>";
    result += "<td>" + line + "</td>";
    result += "</tr>\n";
    it++;
   while ( it!=_PurposeList.end()) {
      line = (*it).c_str();
      result += "<tr>";
      result += "<td> </td>"; //leave first column empty
      result += "<td>" + line +  "</td>";      
      result += "</tr>\n";
      it++;
   }
  }//end else 
  return result;
}

/** Checks for empty fields, BLZ for 8 digits and value for !=0.0
*/
bool GuiTransferForm::isOk(){     
 QString otherBankCode = lineEditOtherBankCode->text().replace(" ", "");//strip blanks
 if(otherBankCode.length() != 8){
   QMessageBox::critical(this, tr("Error"), 
                               tr("The bankcode is not ok. Please correct."));
   return false;
 }

 QString value = lineEditValue->text();
 //replace the decimal komma with a dot for call to toDouble
 for(int pos = 0; pos < value.length(); pos++)
   if(value[pos] == ',')
     value[pos] = '.';

 if(value.toDouble() == 0.0){
   QMessageBox::critical(this, tr("Error"), 
                               tr("The value to be transfered is zero. Please correct."));
   return false;
 }
  // check if all necessary fields are not empty, otherwise errormessage
 if(lineEditOtherName->text().isEmpty() || lineEditOtherBankCode->text().isEmpty() 
    || lineEditOtherAccountId->text().isEmpty()    || lineEditValue->text().isEmpty()){
    QMessageBox::warning( this,
                          tr("Error"),
                          tr("Please fill in all fields. Purpose fields are not mandatory."),
                          tr("&OK"),
                          QString::null, 0, 1 ) ;
   return false;
 }
 
 if(updatePurposeList() == 0){ 
   if( QMessageBox::question( this,
                          tr("Warning"),
                          tr("No purpose has been given. Do you want to send without purpose?"),
                          QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
     return true;
   else return false;
 }
 return true; 
}

/** Clears all edits.
*/
void GuiTransferForm::on_buttonClearForm_clicked(){
 clear();
}

/** Updates the editor's text and takes care of cursor position in
 * replaced text.
 */
void GuiTransferForm::updateEditor( QLineEdit * edit, const QString & newText){
  int cursorPos = edit->cursorPosition();
  cursorPos += newText.length() - edit->text().length(); //if text is now longer because of replaceSpecialChars, add difference
  edit->setText(newText);
  edit->setCursorPosition(cursorPos);
}

/** Scrolls the purpose lines upwards
*/
void GuiTransferForm::slotNextLine(){
 if(lineEditPurpose1->hasFocus()){
   lineEditPurpose2->setFocus();
   return;
 }
 else if(!scrollBarLines->isVisible())
   scrollBarLines->show();
 if(scrollBarLines->value() == scrollBarLines->maximum()){
   QMessageBox::critical(this, tr("Error"), tr("Sorry, there are no more purpose lines available.")); 
 }  
 else scrollBarLines->setValue(scrollBarLines->value() +1);  
}

/** Scrolls the purpose lines downwards
*/
void GuiTransferForm::slotPrevLine(){
  scrollBarLines->setValue(scrollBarLines->value() -1);   
  if((scrollBarLines->value() > 0) ||((scrollBarLines->value() == 0) && !lineEditPurpose2->text().isEmpty())){ 
    //put cursor at the end of the edit by sending Ctrl+e keydown, keyup
    QApplication::postEvent(lineEditPurpose2, new QKeyEvent( QEvent::KeyPress, Qt::Key_E,Qt::ControlModifier, "e")); 
    QApplication::postEvent(lineEditPurpose2, new QKeyEvent( QEvent::KeyRelease, Qt::Key_E,Qt::ControlModifier, "e"));
  }
  else if(lineEditPurpose2->text().isEmpty()){
   lineEditPurpose1->setFocus();
   lineEditPurpose1->setCursorPosition(lineEditPurpose1->text().length()); 
  }
}
/**Scrolls the text lines through the two edits
 */
void GuiTransferForm::on_scrollBarLines_valueChanged(int value){
 _IsScrolling = true;

  lineEditPurpose1->setText( _PurposeLines.at(value));
  _LastInputPurpose = lineEditPurpose1->text();
  lineEditPurpose2->setText( _PurposeLines.at(value + 1));
  _LastInputPurpose2 = lineEditPurpose2->text(); 

  if(isScrollBarRequired())    
    scrollBarLines->show();
  else scrollBarLines->hide();

 _IsScrolling = false;
}

/** Checks whether a scrollbar is neccessary. Returns 
    true, if scrollbar should be made visible.
  */
bool GuiTransferForm::isScrollBarRequired(){
bool scrollerRequired = true;
 if(scrollBarLines->value() ==0){ //hide scroller, if only two lines are needed
    scrollerRequired = false;  
    for(int nLine = 2; nLine < _PurposeLines.count(); nLine++){
      if(_PurposeLines.at(nLine) != "")
        scrollerRequired = true;
    }
 }
 return scrollerRequired;
}

/** Sets _PurposeLines and purpose edits
*/
void GuiTransferForm::setPurpose(const QStringList & purpose){
  QStringList::const_iterator it = purpose.begin();
  for(int nLine = 0; nLine < _PurposeLines.count(); nLine++){
    if(it != purpose.end()){
      QString line = (*it);
      _PurposeLines[nLine] = line;
      it++;
    }
    else _PurposeLines[nLine]= "";
  }//end for

  if(isScrollBarRequired())
    scrollBarLines->show();
  else scrollBarLines->hide();

  lineEditPurpose1->setText(_PurposeLines.at(0));
  lineEditPurpose2->setText(_PurposeLines.at(1));
}

/** Sets _PurposeLines and purpose edits
*/
void GuiTransferForm::setPurpose(const list <string> & purpose){
  list<string>::const_iterator it = purpose.begin();
  for(int nLine = 0; nLine < _PurposeLines.count(); nLine++){
    if(it != purpose.end()){
      QString line = (*it).c_str();
      _PurposeLines[nLine] = line;
      it++;
    }
    else _PurposeLines[nLine]= "";
  }//end for

  if(isScrollBarRequired())
    scrollBarLines->show();
  else scrollBarLines->hide();

  lineEditPurpose1->setText(_PurposeLines.at(0));
  lineEditPurpose2->setText(_PurposeLines.at(1));
}

/** updates the purpose list and returns number of purpose lines.
*/
int GuiTransferForm::updatePurposeList(){
 int lines = 0;
 _PurposeList.clear();
 for(int nLine = 0; nLine < _PurposeLines.count(); nLine ++){
  if(_PurposeLines.at(nLine) != ""){ 
    _PurposeList.push_back(_PurposeLines.at(nLine).toLatin1().data());
    lines ++;
  }
 }
 return lines;
}

/** Sets edits from a transfer used as a template.
 *  Transfer is deleted after use.
 */
void GuiTransferForm::slotShowTemplateData(banking::Transfer * transfer){
  if(!transfer)
   return;
  lineEditOtherName->setText((transfer->otherName1() + transfer->otherName2()).c_str());
  lineEditOtherAccountId->setText(transfer->otherAccountId().c_str());
  lineEditOtherBankCode->setText(transfer->otherBankCode().c_str());
  lineEditValue->setText(""); //no value
  setPurpose(transfer->purposeLines());
  setType(transfer->type());
  delete transfer;
}


/** Creates the job and reurns true on success. To be overwritten.
  */
bool GuiTransferForm::createJob(){
  banking::Transfer * transfer = createTransfer();
  if(HoBIT::hobit()->createJobTransfer(transfer) > 0) //job is accepted by jobmanager, clear form
   {
    clear();
    return true;
   }
   else return false;
}

/**Sets _type and hides  or shows the appropriate widgets
*/
void GuiTransferForm::setType(banking::Transfer::JobType type){
  _Type = type;
  switch (_Type){
   case banking::BankingJob::SingleTransfer:{
                                    _ReconfirmationHeader = tr("You are about to create the following transfer: ");
                                    textLabelCaption->setText(tr("Single Transfer"));
                                    textLabelFirst->hide();
                                    dateEditFirst->hide();
                                    textLabelLast->hide();
                                    dateEditLast->hide();
                                    textLabelTurnus->hide();
                                    comboBoxTurnus->hide();
                                    textLabelDay->hide();
                                    spinBoxDay->hide();
       } break;
   case banking::BankingJob::DatedTransfer:{
                                    _ReconfirmationHeader = tr("You are about to create the following dated transfer: ");
                                    textLabelCaption->setText(tr("Dated Transfer"));
                                    textLabelFirst->show();
                                    textLabelFirst->setText(tr("Date of Execution:"));
                                    dateEditFirst->show();
                                    textLabelLast->hide();
                                    dateEditLast->hide();
                                    textLabelTurnus->hide();
                                    comboBoxTurnus->hide();
                                    textLabelDay->hide();
                                    spinBoxDay->hide();
   }break; 
   case banking::BankingJob::StandingOrder:{
                                    _ReconfirmationHeader = tr("You are about to create the following standing order: ");
                                    textLabelCaption->setText(tr("Standing Order"));
                                    textLabelFirst->show();
                                    textLabelFirst->setText(tr("Date of First Execution:"));
                                    dateEditFirst->show();
                                    textLabelLast->show();
                                    dateEditLast->show();
                                    textLabelTurnus->show();
                                    comboBoxTurnus->show();
                                    textLabelDay->show();
                                    spinBoxDay->show();
   }break; 
   case banking::BankingJob::DebitNote:{
                                    _ReconfirmationHeader = tr("You are about to create the following debit note: ");
                                    textLabelCaption->setText(tr("Debit Note"));
                                    textLabelFirst->hide();
                                    dateEditFirst->hide();
                                    textLabelLast->hide();
                                    dateEditLast->hide();
                                    textLabelTurnus->hide();
                                    comboBoxTurnus->hide();
                                    textLabelDay->hide();
                                    spinBoxDay->hide();
   }break; 
   default: {
                                    textLabelCaption->setText(tr("Error: Unknown Type"));
                                    textLabelFirst->show();
                                    dateEditFirst->show();
                                    textLabelLast->show();
                                    dateEditLast->show();
                                    textLabelTurnus->show();
                                    comboBoxTurnus->show();
                                    textLabelDay->show();
                                    spinBoxDay->show();
       } break;
  }
}

/** Creates a transfer and adds it to the list in JobManager.
 */
banking::Transfer * GuiTransferForm::createTransfer(){

 banking::Transfer * transfer = new banking::Transfer( _Type,
                                     _Value,
                                     qPrintable(HoBIT::hobit()->currentAccountId()),
                                     qPrintable(HoBIT::hobit()->currentIban()),
                                     qPrintable(HoBIT::hobit()->currentBankCode()),
                                     qPrintable(HoBIT::hobit()->currentBic()),
                                     _OurBankType,
                                     qPrintable(_OtherAccountId),
                                     qPrintable(_OtherIban),
                                     qPrintable(_OtherBankCode),
                                     qPrintable(_OtherBic),
             //optional parameters
                                     qPrintable(_OtherName.left(27)),
                                     qPrintable(_OtherName.mid(27, 27)),
                                     _PurposeList);
           //  const string & custRef = "",
           //  const string & currency = "EUR");
  return transfer;
}
