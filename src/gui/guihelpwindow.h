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
#ifndef GUIHELPWINDOW_H
#define GUIHELPWINDOW_H
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>

#include "ui_guihelpwindowbase.h"

class QComboBox;
class QPopupMenu;


/**
The help browser. Based on help window example by Trolltech.
*
*/
class GuiHelpWindow : public QWidget, public Ui::GuiHelpWindowBase
{
  Q_OBJECT

public:

  /**
  *constructor of GuiHelpWindow
  *
  *@param home_ QString
  *@param _path QStringList
  *@param parent pointer on current QWidget
  *@param name name of the Widget
  */
  GuiHelpWindow(QWidget* parent, const QString& home);

  /**
  *destructor of GuiHelpWindow
  */
  ~GuiHelpWindow();

  /**
  *Set the helpfile to be displayed
  *@param helfFile name of help file
  */
  void setHelpfile(const QString & helpFile);

signals:

  void cancelled();

private slots:

  /** Show the index page
  */
  void on_buttonHome_clicked();
  /** Go to the last page, if available
  */
  void on_buttonBack_clicked();
  /** Go to the next page, if available.
  */
  void on_buttonForward_clicked();
 
  /**
  *Fires signal cancelled
  */
  void on_buttonClose_clicked();
  /** Activate forward button
  */
  void on_textBrowser_forwardAvailable(bool available);
  /** Activate backward button
  */
  void on_textBrowser_backwardAvailable(bool available);
private:

  /**
  *function for reading history
  */
  void readHistory();

  /**
  *function for reading the bookmarks
  */
  void readBookmarks();

  int backwardId, forwardId;
  QStringList history, bookmarks;
  QMap<int, QString> mHistory, mBookmarks;
  QPopupMenu *hist, *bookm;
};





#endif
