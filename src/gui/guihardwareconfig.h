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
#ifndef GUIHARDWARECONFIG_H
#define GUIHARDWARECONFIG_H

#include "ui_guihardwareconfigbase.h"
#include <qprocess.h>
#include "guimain.h"


/**
* class for show hardware-configuration of modem, isdn, dsl, lan, printer, home and save
* starts the Knoppix hardware configuration tools.
*/
class GuiHardwareConfig: public QWidget, public Ui::GuiHardwareConfigBase
{
  Q_OBJECT
public:
  /**
  *this is the constructor of class for hardware configuration 
  *
  *@param main 
  *@param parent the parent dialog of this widget
  *@param hobit
  */
  GuiHardwareConfig(QWidget * parent);

/** destructor*/
~GuiHardwareConfig();

 /** Creates any missing device nodes. To be called before starting the isdn system.
 */
 static bool makeIsdnNodes(QWidget* parent);


public slots:
  /** These functions call external KNOPPIX configuration programs
  */
  void on_buttonModem_clicked();
  void on_buttonIsdn_clicked();
  void on_buttonDsl_clicked();
  void on_buttonLan_clicked();
  void on_buttonWlan_clicked();
  void on_buttonGprs_clicked();
  void on_buttonSave_clicked();
  /** Emits signal printerAutoSetupRequest
  */
  void on_buttonPrinterAuto_clicked();
  /** Emits signal printerManualSetupRequest
  */
  void on_buttonPrinterManual_clicked();
  /** Emits signal finished
  */
  void on_buttonNext_clicked();
  /** Emits signal back
  */
  void on_buttonBack_clicked();
  /** Copy the predefined kppprc file to ~/.kde/share/config
  */
  void on_buttonPredefinedModem_clicked();
  /** Copy the predefined isdn file to /etc/sysconfig/isdn
  */
  void on_buttonPredefinedISDN_clicked();

signals:

  void finished();
  void back();
  void raiseWidget(int);

private:
 /** Copies file from src to dest using root privileges. For use with KNOPPIX only.
 * Uses sudoe if asRoot is true.
 */
 bool copyConfigFile(QString srcPath, QString destPath, QString fileName, bool asRoot);

 /** Run command. Run it as root using sudo, if runAsroot is true
  */
 void runProcess(const QString & command, bool runAsRoot = true);

 // process to be executed
  //QProcess* _Process;    //QProcess does not work for gui-processes like kppp
};

#endif
