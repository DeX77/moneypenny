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
/****************************************************************************
** $Id: guihelpwindow.cpp,v 1.2 2010-09-28 16:50:27 hbci Exp $
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "guihelpwindow.h"

#include <ctype.h>


/**
*constructor of GuiHelpWindow
*
*@param indexFile QString
*@param _path QStringList
*@param parent pointer on current QWidget
*@param name name of the Widget
*/
GuiHelpWindow::GuiHelpWindow(QWidget* parent, const QString& indexFile )
    : QWidget(parent), Ui::GuiHelpWindowBase()
{
    setupUi(this);
    setObjectName("GuiHelpWindow");

    if ( !indexFile.isEmpty() )
        textBrowser->setSource( indexFile );
    qDebug("Help index file: %s", qPrintable(indexFile));    

   textBrowser->setFocus();
}


/**
*destructor of GuiHelpWindow
*/
GuiHelpWindow::~GuiHelpWindow()
{
    qDebug("Destructor GuiHelpWindow finished");
}

/** 
*Fires signal cancelled
*/
void GuiHelpWindow::on_buttonClose_clicked(){
  emit cancelled();
}

/** 
*Set the helpfile to be displayed
*@param helfFile name of help file
*/
void GuiHelpWindow::setHelpfile(const QString & helpFile) {
 textBrowser->setSource( helpFile );
}

/** Show the index page
*/
void GuiHelpWindow::on_buttonHome_clicked(){
 textBrowser->home();
}
/** Go to the last page, if available
*/
void GuiHelpWindow::on_buttonBack_clicked(){
  textBrowser->backward();
}
/** Go to the next page, if available.
*/
void GuiHelpWindow::on_buttonForward_clicked(){
 textBrowser->forward();
}
/** Activate forward button
*/
void GuiHelpWindow::on_textBrowser_forwardAvailable(bool available){
  buttonForward->setEnabled(available);
}
/** Activate backward button
*/
void GuiHelpWindow::on_textBrowser_backwardAvailable(bool available){
  buttonBack->setEnabled(available);
}
