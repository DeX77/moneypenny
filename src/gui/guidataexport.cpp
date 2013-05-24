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
#include "guidataexport.h"
#include "globals.h"
#include "hobit.h"
#include "kernel/helpers/transaction.h"
 
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <fstream>
using namespace std;

GuiDataExport::GuiDataExport(QWidget *parent)
 : QWidget(parent), GuiDataExportBase()
{
 qDebug("Constructor GuiDataExport");
 setupUi(this);
 setObjectName("GuiDataExport");
 setMountPoint ("~");
 _Encoding = "ISO 8859-1";
 groupBoxSeparators->hide();
}

GuiDataExport::~GuiDataExport()
{
 qDebug("Destructor GuiDataExport");
}

void GuiDataExport::on_buttonSelectFile_clicked(){
  QString keyFiles = tr("CSV Export Files (*.csv)");
  QString path = QFileDialog::getSaveFileName(this, tr("Select filename"), _mountpoint, keyFiles);
  if (!path.isEmpty())
    lineEditFile->setText(path);
}

void GuiDataExport::on_buttonExport_clicked(){
 
  switch (comboBoxExportType->currentIndex()){
  
    case 0:       if (QMessageBox::question(this,
                      tr("Reconfirmation"), 
                      tr("Exporting data to a plain text format is a security risk. "
                         "Do you want to export the data?"),
                      QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes )
                    return;
                   else exportCSV();
                   break;
 
    case 1:       if (QMessageBox::question(this,
                      tr("Reconfirmation"),
                      tr("Exporting to SQL will export MoneyPenny's whole Database "
                          "in plain text form. This is a security risk. "
                          "Do you want to export the data?"),
                      QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes )
                    return;
                  else emit exportSQL();
                  break;
  } 
  QMessageBox::information(this,tr("Finished"),
                           tr("Data have been exported to: ") + lineEditFile->text());

}


void GuiDataExport::exportCSV(){
  QString toExport = "";
  QFile file(lineEditFile->text());
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this,tr("Failed"), 
                          tr("Can't open export file"));
  }
  else{
     char fieldSeparator = MONEYPENNY_CSV_SEPARATORS(comboBoxFieldSeparator->currentIndex());
     char textSeparator = MONEYPENNY_CSV_SEPARATORS(comboBoxTextSeparator->currentIndex() + 4);

     HoBIT::hobit()->exportCSV(&file, fieldSeparator, textSeparator);
  }//end else

  file.close();
}
void GuiDataExport::exportSQL(){
  QString toExport = "";
  QFile file(lineEditFile->text());
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this,tr("Failed"), 
                          tr("Can't open export file"));
  }
  else{
    QMessageBox::critical(this,tr("Not implemented"), 
                          tr("Sorry, this is not yet implemented"));
     HoBIT::hobit()->exportSQL(&file);
  }//end else

  file.close();
}

 
void GuiDataExport::on_buttonBack_clicked(){
  emit back();
}
/**Toggles groupBoxSeparators
*/
void GuiDataExport::on_buttonDetails_clicked(){
 if(groupBoxSeparators->isVisible())
     groupBoxSeparators->hide();
 else groupBoxSeparators->show();
}

void GuiDataExport::setMountPoint(const QString & mp){
	_mountpoint = mp;
	QString filepath = _mountpoint + "/" + MONEYPENNY_DEFAULT_EXPORT_FILE;
	lineEditFile->setText(filepath);
}


void GuiDataExport::on_comboBoxExportType_currentIndexChanged(int index){
  switch (index){

    case 0: lineEditFile->setText(_mountpoint + "/" + MONEYPENNY_DEFAULT_EXPORT_FILE);
            break;

    case 1: lineEditFile->setText(_mountpoint + "/" + MONEYPENNY_DEFAULT_SQL_EXPORT_FILE);
            break;
  }
}

