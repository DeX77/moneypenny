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
#ifndef GUIPROGRESSINDICATOR_H
#define GUIPROGRESSINDICATOR_H

#include <QDialog>

#include "ui_guiprogressindicatorbase.h"

/**
Aa application-modal dialog, that blocks the rest of the GUI while indicating progress of the HBCI
kernel actions. If Cancel is visible, cancel commands can be sent to the kernel. This stops execution of the message queue, before the next message is sent. Use setMaximum(0) to achieve an oscillating bar.


	@author walter <walter@HP2100-Roth>
*/
class GuiProgressIndicator : public QDialog, public Ui::GuiProgressIndicatorBase
{
  Q_OBJECT

public:
    /**
      * The satus constants to be used with setStatus()
      */
    typedef enum{
     Inactive,
     DisplayLastProtocol,
     Running,
     Finished
    } Status;
 
     GuiProgressIndicator(QWidget* parent);

    ~GuiProgressIndicator();
     /** Set the maximum (100%) value for progress bar. 
      * A value of 0 causes progress bar to oscillate.
      */
     void setMaximum(int value);
     /** Set the progress bar to value.
      */
     void setProgress(int value);
     /** Set HTML - formatted text of textEdit.
      */
     void setText(const QString & newText);
     /** Set the status. 
       * @param status begin: moving progress bar. Finished: Progress bar 100%
       */
     void setStatus(Status status);
     /** Add a line of text to textEdit. <p> and </p> are added automatically.
      */
     void addTextParagraph(const QString & newText, const QColor & color = Qt::black);
     /** Returns Cancel flag.
      */
     bool isCancelled()const{ return _CancelPressed;};
protected slots:
     /** Sends signal cancel
     */
     void on_buttonCancel_clicked();
signals:
    void cancel();
 private:
  /** Flag for a pressed cancel button.
    */
  bool _CancelPressed;
  int _LineNumber;
  Status _Status;
  /** The text as it was when status was set to Finished.
   */
  QString _LastText;
};

#endif
