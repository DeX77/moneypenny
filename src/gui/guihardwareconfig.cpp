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
#include "guihardwareconfig.h"
#include <QString>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QDir>
#include <QInputDialog>

/**
*this is the constructor of the hardware configuration class
*
*@param main 
*@param parent the parent dialog of this widget
*@param name widgets title
*@param hobit
*/
GuiHardwareConfig::GuiHardwareConfig(QWidget * parent)
   :QWidget(parent), Ui::GuiHardwareConfigBase()
{
   setupUi(this);
   if ( ! SHOW_PREDEFINED_CONFIGS )
    groupBoxPredefinedConfigs->hide();
   setObjectName("GuiHardwareConfig");
}

/** destructor*/
GuiHardwareConfig::~GuiHardwareConfig(){
  qDebug("Destructor GuiHardwareConfig finished");
}

void GuiHardwareConfig::runProcess(const QString & command, bool runAsRoot)
{
  if(! USE_LIVE_CD_FEATURES ){
   QMessageBox::information(this, tr("Information"), 
    tr("Please use the buttons on the command tray for hardware configuration!"));
    return;
  }

 QString cmd;
 if(runAsRoot)
 {
     cmd = sudo(command + " &", this);
  system(qPrintable(cmd));
  qDebug("Running process: %s", qPrintable( cmd));
 }
 else {
  cmd += " &";
  system(qPrintable(cmd));
  qDebug("Running process: %s", qPrintable(cmd));
 }
}

void GuiHardwareConfig::on_buttonModem_clicked()
{ 
  runProcess("gnome-ppp", true);
}


void GuiHardwareConfig::on_buttonIsdn_clicked()
{
  runProcess("nm-connection-editor");
}

void GuiHardwareConfig::on_buttonDsl_clicked()
{
  runProcess("nm-connection-editor", true);
}

void GuiHardwareConfig::on_buttonLan_clicked()
{
 QMessageBox::information(this, tr("LAN Configuration"),
 tr("Normally a local wired network has a router supplying DHCP.")+ "\n"
 +tr("In this case, no configuration is neccessary.") +"\n" 
 +tr("For advanced users only:" ) + "\n"
 +tr("If you need a special configuration like a static IP-address, please edit the config file /etc/network/interfaces using sudo gedit.")
 );
}

void GuiHardwareConfig::on_buttonPrinterAuto_clicked()
{
  emit raiseWidget(PRINTER_AUTO_SETUP_WIDGET);
}

void GuiHardwareConfig::on_buttonPrinterManual_clicked()
{
  emit raiseWidget(PRINTER_MANUAL_SETUP_WIDGET);
}

void GuiHardwareConfig::on_buttonWlan_clicked()
{
  runProcess("nm-connection-editor");
}

void GuiHardwareConfig::on_buttonGprs_clicked()
{
  runProcess("nm-connection-editor");
}

void GuiHardwareConfig::on_buttonSave_clicked() //presently disabled, no button
{
  runProcess("/usr/sbin/saveconfig_moneypenny", true);
}

void GuiHardwareConfig::on_buttonNext_clicked()
{
  emit finished();
}

void GuiHardwareConfig::on_buttonBack_clicked()
{
  emit back();
}

/** Copy the predefined kppprc file to ~/.kde/share/config
*/
void GuiHardwareConfig::on_buttonPredefinedModem_clicked(){
  QString destPath= "~/.kde/share/config";
  QString srcPath = MP_DIR() + MONEYPENNY_INTERNET_CONFIG_DIR;
  QString fileName ( "kppprc");
  bool ok = true;
  qDebug ("GuiHardwareConfig::slotPredefinedModem started");
  
  QDir dir(destPath);
  if(!dir.exists()){
    QString mkdirCommand = QString("mkdir -p " + destPath);    
    system(qPrintable(mkdirCommand));
  }  
  ok = copyConfigFile(srcPath, destPath, fileName, false); //copy as user knoppix
  
  if(!ok)
   QMessageBox::critical(this, tr("Error"), tr("Could not copy modem configuration file."));
  else
   QMessageBox::information(this, tr("Internet Configuration"), 
   tr("The predefined modem configuration file has been activated.\n You may now try to connect to the net with your modem attached to COM1 (= /dev/tty0).")); 
  on_buttonModem_clicked();
}
/** Copy the predefined isdn file to /etc/sysconfig/isdn
*/
void GuiHardwareConfig::on_buttonPredefinedISDN_clicked(){
  QString destPath= "/etc/sysconfig";
  QString srcPath= MP_DIR() + MONEYPENNY_INTERNET_CONFIG_DIR;
  QString fileName ("isdn");
  bool ok = true;
  qDebug ("GuiHardwareConfig::slotPredefinedISDN started");
 
  ok = copyConfigFile(srcPath, destPath, fileName, true); //copy isdn config files as root
  if (ok){
   fileName = "device.ippp0";
   destPath = "/etc/isdn"; //copy the files for /etc/isdn
   ok = copyConfigFile(srcPath, destPath, fileName, true);   
  }
  if (ok){
   fileName = "device.ippp0+1";
   ok = copyConfigFile(srcPath, destPath, fileName, true);  
  }
  if (ok){
   fileName = "device.ippp0+1";
   ok = copyConfigFile(srcPath, destPath, fileName, true);  
  }
  if (ok){
   fileName = "ipppd.ippp0";
   ok = copyConfigFile(srcPath, destPath, fileName, true);  
  }
  if (ok){
   fileName = "ipppd.ippp0+1";
   ok = copyConfigFile(srcPath, destPath, fileName, true);  
  }   

  if(!ok){
   QMessageBox::critical(this, tr("Error"), tr("Could not copy isdn configuration files. Please configure manually."));
   on_buttonIsdn_clicked();
  } 
  else{ // try to start the ISDN system with the copied configuration
   system("sync"); //make shure that everything has been written

  //Search for a Fritz! card and initialize CAPI if found
  if(ok){
    qDebug("GuiHardwareConfig::slotPredefinedISDN: Searching for a Fritz! card.");
    bool fcOk = system(c_sudo("/usr/sbin/detect-fc", this) ) == 0;
    if (fcOk)
     fcOk = system(c_sudo("modprobe capidrv", this)) == 0;
    else qDebug("No Fritz! card detected.");
    if (fcOk)
     fcOk = system(c_sudo("modprobe capi", this)) == 0;
    if(fcOk)
     qDebug("GuiHardwareConfig::slotPredefinedISDN: Fritz! card and CAPI initialization was successful." );
  }
  if(ok)
    ok= makeIsdnNodes(this);
  if(ok)
   ok = system(c_sudo("/etc/init.d/isdnutils start", this))==0; 
  if(ok){
     QMessageBox::information(this, tr("Internet Configuration"), 
     tr("The predefined ISDN configuration has been copied and the ISDN system has been started.") + "<br>" +
     tr("You may now try to connect to the internet via ISDN.")); 
     qDebug ("GuiHardwareConfig::slotPredefinedISDN: startup OK");
   }
   else{
      qDebug ("GuiHardwareConfig::slotPredefinedISDN: Starting ISDN system failed.");
      QMessageBox::warning(this, tr("Error"), 
      tr("Starting the ISDN system with the predefined configuration failed.") + "<br>" +
      tr("Most probably your hardware could not be detected automatically.") + "<br>" +
      tr("Please configure the ISDN system manually.")); 
      qDebug ("GuiHardwareConfig::slotPredefinedISDN: startup failed");
      on_buttonIsdn_clicked();
   }
  } 
}

/** Creates any missing device nodes
*/
bool GuiHardwareConfig::makeIsdnNodes(QWidget* parent){
// Create missing device nodes in Knoppix 5 from isdntool by Klaus Knopper
/*[ -e /dev/isdnctrl0 ] || mknod -m 666 /dev/isdnctrl0 c 45 64
[ -e /dev/isdnctrl ] || ln -s isdnctrl0 /dev/isdnctrl
[ -e /dev/isdninfo ] || mknod -m 444 /dev/isdninfo c 45 255
*/
 QFile node;
 QString isdnctrl= "/dev/isdnctrl";
 QString isdnctrl0= "/dev/isdnctrl0";
 QString isdninfo= "/dev/isdninfo";
 bool result = true;
 node.setFileName(isdnctrl0);
 if(!node.exists())
   result = system(c_sudo("mknod -m 666 " + isdnctrl0 + " c 45 64", parent))==0;
 if(result){
   node.setFileName(isdnctrl);
   if(!node.exists())
     result = system(c_sudo("ln -s " + isdnctrl + " " + isdnctrl0, parent))==0;
 }
 if(result){
   node.setFileName(isdninfo);
   if(!node.exists())
     result = system(c_sudo("mknod -m 444 " + isdninfo + " c 45 255", parent))==0;
 }
 if(! result)
   (qDebug("GuiHardwareConfig::makeISDNNodes: Error when creating nodes."));
 return result;
}

/** Copies file from src to dest using root privileges. For use with KNOPPIX only.
 * Uses sudoe if asRoot is true.
 */
bool GuiHardwareConfig::copyConfigFile(QString srcPath, QString destPath, QString fileName, bool asRoot)
{ 
 QString fileSrcPath = srcPath + "/" + fileName;
 QFileInfo fileInfo (fileSrcPath);
 
 bool ok = fileInfo.exists();
 if (ok){
   QString cpCommand = ("cp " + fileSrcPath + " " + destPath);
   if( asRoot )
     cpCommand = sudo(cpCommand, this);
     
   ok = system(qPrintable(cpCommand)) == 0;
   
   if(ok)    
     qDebug ("GuiHardwareConfig::CopyConfigFile: copy command ok: %s", qPrintable(cpCommand));
   else 
     qDebug ("GuiHardwareConfig::CopyConfigFile: copy command failed: %s", qPrintable(cpCommand));
 }
 else qDebug("GuiHardwareConfig::CopyConfigFile: File not found: %s", qPrintable(fileSrcPath));
 return ok;
}

