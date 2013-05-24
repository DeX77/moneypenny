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
#ifndef DLGSTATUSMESSAGE_H
#define DLGSTATUSMESSAGE_H

#include <ui_dlgstatusmessagebase.h>
#include <QDialog>

/**
A status info dialog. Should be used non-modal. 

	@author walter <walter@HP2100-Roth>
*/
class DlgStatusMessage : public QDialog, public Ui::DlgStatusMessageBase
{
Q_OBJECT
public:
    DlgStatusMessage(QWidget *parent = 0);

    ~DlgStatusMessage();
    /**Slots to be connected to status signals.
     */
public slots:
    void setLabelTopLeftText(const QString & newVal);
    void setLabelTopRightText(const QString & newVal);
    void setLabelBottomLeftText(const QString & newVal);
    void setLabelBottomRightText(const QString & newVal);
    void setLabelBottomRightNumber(int newVal);
};

#endif
