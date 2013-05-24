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
#ifndef GUIDATAEXPORT_H
#define GUIDATAEXPORT_H

#include <QString>
#include <ui_guidataexportbase.h>

class HoBIT;

/**
This is MoneyPenny's data export dialog, which allows exporting data to csv-format. 

@author Marcel Mulch
*/
class GuiDataExport : public QWidget, public Ui::GuiDataExportBase
{
Q_OBJECT
public:
    GuiDataExport(QWidget *parent);
    ~GuiDataExport();

    virtual void setMountPoint(const QString & mp);

public slots:
    void on_buttonDetails_clicked();
    void on_buttonSelectFile_clicked();
    void on_buttonBack_clicked();
    void on_buttonExport_clicked();
    void on_comboBoxExportType_currentIndexChanged(int index);

signals:
    void back();

private:
 /** Exports the data to CSV as specified by GUI elements.
 */
 void exportCSV();
 void exportSQL();

 QString _mountpoint;
 QString _Encoding; //ISO 8859-1 to ISO 8859-15, UTF-8, 
};

#endif
