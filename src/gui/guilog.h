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

#ifndef GUILOG_H
#define GUILOG_H

#include "ui_guilogbase.h"


class HoBIT;

/**A dialog that shows the progress and / or the results of job execution.
 @author Walter Roth
*/

class GuiLog : public QWidget, private Ui::GuiLog
{
  Q_OBJECT

public:
  GuiLog(QWidget* parent);
  ~GuiLog();
  /*$PUBLIC_FUNCTIONS$*/

  void setCursor ( const QCursor & cursor);
    
  /** Set the text on job number label.
  */
  void setJobText(const QString & jobText);
  /** Adds the text in edit, and increments job number on label present job.
  */
  void addLogText(QString & logText);
  /** Sets the text in edit.
  */
  void setLogText(QString & logText);
  /** Shows the results of the execution
  */
  void showResults();


signals:
  void closeRequest(); 
public slots:
  /*$PUBLIC_SLOTS$*/
  /** called on closeButton clicked
  */
  void slotCloseRequest();
protected slots:
  /*$PROTECTED_SLOTS$*/
  /** Show the log messages.
  */
  void slotShowDetails();
  /** Hide the log messages.
  */
  void slotHideDetails();
private:
int _Jobs;  
int _JobsOk;
int _JobsFailed;  
int _JobsWarned;  
};

#endif

