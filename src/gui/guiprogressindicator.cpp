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
#include "guiprogressindicator.h"

#include <iostream>
using namespace std;

GuiProgressIndicator::GuiProgressIndicator(QWidget* parent)
 : QDialog(parent), Ui::GuiProgressIndicatorBase()
{
  setupUi(this);
  setStatus(Inactive);
  setMaximum(0);
  _LineNumber = 0;
  _CancelPressed = false;
  qDebug("Constructor GuiProgressIndicator finished.");
}


GuiProgressIndicator::~GuiProgressIndicator()
{

}

/** Set the maximum (100%) value for progress bar. 
 * A value of 0 causes progress bar to oscillate.
 */
void GuiProgressIndicator::setMaximum(int value)
{
  progressBar->setMaximum(value);
}

/** Sends signal cancel
*/
void GuiProgressIndicator::on_buttonCancel_clicked()
{
  if(_Status == Running)
    _CancelPressed = true; //Wait for the kernel thread to sleep, 
                         //emit cancel when it is sending the next message
  else reject();
}

/** Set the progress bar to value.
 */
void GuiProgressIndicator::setProgress(int value)
{
  progressBar->setValue(value);
}

/** Set HTML text of textEdit.
 */
void GuiProgressIndicator::setText(const QString & newText)
{
 textEdit->setHtml(newText);
}

/** Add a HTML-formatted paragraph with specified color to textEdit. 
  */
void GuiProgressIndicator::addTextParagraph(const QString & newText, const QColor & color)
{
 QString sColor = "\"#";
 sColor += QString::number(color.red(), 16).toUpper();
 sColor += QString::number(color.green(), 16).toUpper();
 sColor += QString::number(color.blue(), 16).toUpper();
 sColor+="\"";
 QString sHtml = "<p> <font color=" + sColor + ">" 
    + QString::number(_LineNumber) + "  " + newText + "</font></p>";
 textEdit->append(sHtml);
 _LineNumber ++;
 if(_CancelPressed)
  {
   qDebug("GuiProgressIndicator::addTextParagraph: Emitting signal cancel() due to _CancelPressed flag.");
   emit cancel(); //The kernel thread is sleeping, it may be cancelled now
  }
}

/** Set the status. 
  * @param status begin: moving progress bar. Finished: Progress bar 100%
  */
void GuiProgressIndicator::setStatus(Status status)
{
 _Status = status;
 switch (_Status)
 {
   case DisplayLastProtocol:
      {
        progressBar->setMaximum(100);
        progressBar->setValue(100);
        textEdit->setHtml(_LastText);
        buttonCancel->setText(tr("&Close"));
      } break;

   case Running:
      {
        textEdit->clear();
        _LineNumber = 0;
        _CancelPressed = false;
        buttonCancel->setText(tr("&Cancel Running Transmission"));
        progressBar->setValue(0);
        progressBar->setMaximum(0);
      }break;

   case Finished:
      {
        progressBar->setMaximum(100);
        progressBar->setValue(100);
        addTextParagraph(tr("Transmission finished"));
        buttonCancel->setText(tr("&Close"));
        cout << "*******Messages received by GuiProgressIndicator****** \n" << qPrintable(textEdit->toPlainText());
        cout << "*********End of messages received by GuiProgressIndicator******* \n"; 
        _LastText = textEdit->toHtml();
      }break;

    default: 
      {
        buttonCancel->setText(tr("&Close"));
        progressBar->setValue(0);
        progressBar->setMaximum(0);
      }break;

 }
}
