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
#include "guifindmedium.h"

#include <stdlib.h>

#include <QListWidget>
#include <QPushButton>
#include <QFile>
#include <QStringList>
#include <QRadioButton>
#include <QRegExp>
#include <QLabel>
#include <QMessageBox>
#include <QDir>
#include <QCursor>
#include <QProcess>
#include <QApplication>
#include <QFileDialog>
#include <QFrame>
#include <QGroupBox>
#include <QLayout>
#include <QTextStream>


GuiFindMedium::GuiFindMedium(QWidget * parent)
 : QDialog(parent), Ui::GuiFindMediumBase()
{ 
 qDebug("Constructor GuiFindMedium");
 setupUi(this);
 setObjectName("GuiFindMedium"); 
 connect(&_Timer, SIGNAL(timeout()), this, SLOT(slotTimerDone()));
 reset(); //reset
 frameBankList->hide();

 //search for a floppy and hide controls if not available
 FILE * pipe = popen("cat /etc/fstab | grep fd0", "r");
 if(pipe){
   char buf [256];
   _HasFloppy =fread(buf, 1, sizeof(buf), pipe) != 0; //no line with fd0 in fstab => no floppy
    
   pclose(pipe);
 }
 _MountBackupMedium = false;
}



GuiFindMedium::~GuiFindMedium()
{
 qDebug("Destructor GuiFindMedium finished");
}

/** Read property of StorageType m_Storage. 
*/
const StorageType& GuiFindMedium::getStorage()const{
	return _Storage;
}

/** Write property of StorageType m_Storage. 
*/
void GuiFindMedium::setStorage( const StorageType& _newVal){
	_Storage = _newVal;
}

/** Runs the whole self-finding process for a storage device
*/
void GuiFindMedium::slotTimerDone(){  
   //do not interfere with running processes
   if(_SkipTimer)
     return;

   //count down after Medium has been found
   if( _BankDataAvailable ){ 
         buttonNext->setEnabled(true);
        if( listWidgetBanks->count() <= 1 || (_BankDataAvailable && !frameBankList->isVisible()))
          _TimeToShow --; //only one bank, or new medium, or mp2 data, proceed automatically
        if(_TimeToShow <= 0) 
          on_buttonNext_clicked(); // proceed after _TimeToShow
   }

   if(_MediumFound && _IsWritable) //return after Medium has been found
     return; 

   //Start searching for writable medium
   system("sync"); //flush all buffers

   QString writeProtectMsg = 
     tr("The storage device seems to be write protected.")+ "<br>"
    + tr("Will retry writing. Please wait a little longer.") + "<br>"
    + tr("If this does not succeed, remove device, make it writable and try again.");

   //Search for Medium
   if(_Storage == STORAGE_UNKNOWN){ // search for device
     if (BUILD_FOR_MAC_OSX)
       searchForVolumeOSX();
     else if(USE_UDRW_FEATURES && !(radioButtonUsb->isChecked() || radioButtonFloppy->isChecked()))
       searchForUdrwDevice();
     else if(radioButtonFloppy->isChecked())
       searchForFloppy();
     else if(radioButtonUsb->isChecked()){
       searchForUsbDevice();
     }
     return;
   }//end if STORAGE_UNKNOWN
   else 
   { // device found or selected, wait for /proc/mounts to be updated
     if((_Storage == STORAGE_USB) ){
        if(_MountPoint.isEmpty())
          _MountPoint = findMountPoint(); // this will not work immediately, 
        if(USE_LIVE_CD_FEATURES && _MountPoint.isEmpty())
           runProcess(sudo("mount " +_DeviceName, this)); //try to mount manually

        setStatus(
            tr("New USB device found. Checking for writability. This will take a few seconds.<br> Please wait."),
            PALETTE_YELLOW());
        if(_MountPoint.length() > 0){ 
          unsetCursor();
          qDebug("USB device found: %s", _MountPoint.toLatin1().data());
          _IsWritable = canWriteTestFile();
          if(_IsWritable){
            setStatus(tr("USB device is writable. Please proceed."), PALETTE_GREEN());
            _MediumFound = true;
         }//end mountpoint found
       }
      }//end USB

      else if(_Storage == STORAGE_FLOPPY){ 
 //       _MountPoint = findMountPoint("/proc/mounts", _DeviceName);
        if(_MountPoint.length() > 0){ 
           _IsWritable = canWriteTestFile();
           if(_IsWritable){
             qDebug("Floppy found: %s", _MountPoint.toLatin1().data());
             setStatus(tr("A floppy disc has been found. Please proceed."), PALETTE_GREEN());
             _MediumFound = true;
           }
        }//end length > 0
      }//end floppy  

      else if(_Storage == STORAGE_OSX){
         setStatus(
        tr("New storage device found. Checking for writability. This will take a few seconds.<br> Please wait."),
          PALETTE_YELLOW());
	 _IsWritable = canWriteTestFile();
         if(_IsWritable){
           qDebug("Storage device found: %s", _MountPoint.toLatin1().data());
           setStatus(tr("Storage device is writable. Please proceed."), PALETTE_GREEN());
           _MediumFound = true;
         }
      }//end OSX

      else if(_Storage == STORAGE_MANUALLY){ 
           _IsWritable= canWriteTestFile();
           if(_IsWritable)
             setStatus(tr("A writable storage device has been found. Please proceed."), PALETTE_GREEN());
           else
             setStatus(writeProtectMsg, PALETTE_YELLOW());
      }//end storage manual  

      if((_MountPoint.length() > 0)&&! (_IsWritable )) {//device is not writable
         _IsWritable = canWriteTestFile(); //retry
         _WriteCycles ++;
         if(_WriteCycles > 10){
           qDebug("Error: Device is write protected.");
           setStatus(writeProtectMsg, PALETTE_RED());
         }
      }
   }//end else  device found
   
    //mountpoint found for a writable device, build the bank list
   if((_MountPoint.length() > 0) && _IsWritable){
      if(! _BankDataAvailable )
         _BankDataAvailable = buildBankList() > 0; 
/*
      if(! _BankDataAvailable && !_MountBackupMedium)
        { //no bank, error
          setStatus(tr("No bank data file could be found.") + "<br>"
                         + tr("Please check your medium!"),
                        PALETTE_RED());
            buttonNext->setText(tr("Continue anyway!"));
            buttonNext->setEnabled(true); //allow to continue  
        }
*/
      if( _BankDataAvailable && _MountBackupMedium)
        { //medium is not empty, error
          setStatus(tr("A directory containing MoneyPenny data has been found.") + "<br>"
                        + tr("Please rename or remove the directory moneypenny_data.") + "<br>" 
                        + tr("Backup requires an empty medium"),
                        PALETTE_RED());
            buttonNext->setText(tr("Rename automatically and continue!"));
            buttonNext->setEnabled(true); //allow to continue  
        }
  }//end mountpoint found 
}

/** Starts the timer loop
*/
void GuiFindMedium::showEvent(QShowEvent *){
   if(! _HasFloppy)
    {
      labelSelect->setText("");
      labelFloppy->hide();
      radioButtonFloppy->hide();
      radioButtonUsb->setChecked(true);
      radioButtonUsb->setText(tr("Please plug in your USB-stick now."));
    }

    radioButtonUsb->setChecked(true);
    reset();
    _Timer.start(1000);
}


/** Stops the timer loop
*/
void GuiFindMedium::hideEvent(QHideEvent *){
   _Timer.stop();
}

/** Calls mediumTypeChanged()
*/
void GuiFindMedium::on_radioButtonFloppy_toggled(bool){
  mediumtypeChanged();
}

/** Calls mediumTypeChanged()
*/
void GuiFindMedium::on_radioButtonUsb_toggled(bool){
  mediumtypeChanged();
}

/** Tries to read from floppy or USB device
*/
void GuiFindMedium::mediumtypeChanged(){
 if(radioButtonFloppy->isChecked()){
   setStatus(tr("Waiting for a floppy disk to be inserted now."), PALETTE_YELLOW());
 }
 else if(radioButtonUsb->isChecked()){
   setStatus(tr("Waiting for a USB device to be plugged in now."), PALETTE_YELLOW());
 }
 setCursor(Qt::WaitCursor);
 _Storage = STORAGE_UNKNOWN;
 _MountPoint = "";
 _BankDataAvailable = false;
 frameBankList->hide();
}

/** Searches for a mounted floppy or tries to mount it if required by OS
*/
bool GuiFindMedium::searchForFloppy(){
 _DeviceName = "/dev/fd0";
 bool doMount = true;//USE_LIVE_CD_FEATURES; // mount required for Knoppix 501
 QFile suseVersionFile("/etc/SuSE-release");
 if(suseVersionFile.exists()){
   suseVersionFile.open(QIODevice::ReadOnly);
   QString contents= suseVersionFile.readAll();
   suseVersionFile.close();
   if(contents.indexOf("10.2")>=0) //mount required for SuSE 10.2
    doMount=true;
 }
 if(doMount){ //auto mount the floppy
   runProcess("mount " + _DeviceName); 
 }
 findMountPoint(); //first try the devicename /dev/fd0
 if(_MountPoint.isEmpty()){
   _DeviceName = "floppy";
   findMountPoint(); //there may be other floppy drives, should be mounted on .../floppy/...
 }
 if(!_MountPoint.isEmpty()){//give up searching, look what we have found
    QDir dir(_MountPoint); 
    QStringList files = dir.entryList();
    if(files.count() > 0){
      _Storage = STORAGE_FLOPPY; //do not set _MountPoint here, it is set in slotTimerDone
      return true;
    } 
    else return false;
 }   
 else return false;
}

/** Searches for a new plugged in volume, 
  *  when moneypenny is running on mac osx
  * Changed: 19.01.06  We are using 'df' instead 'ls' here, so we can scan for '/Volumes/'   (AG)
  */
bool GuiFindMedium::searchForVolumeOSX(){

 bool result = false;
 QStringList newList;
 QStringList sections;
 QString newLine, oldLine; //for easy debugging

  // get moneypenny-tmp-dir
  // this is on the ramdisk, when running from CD
  QString homeDir = getenv("HOME"); 
  QString mpHomeDirName = homeDir + "/" + MONEYPENNY_MAIN_DIR;

  QString checkfile = mpHomeDirName + "/volumes_check";

  system( QString("df > " + checkfile).toLatin1().data() );

 QFile volumesFile(checkfile);
 if(volumesFile.open(QIODevice::ReadOnly)){
   QTextStream s(&volumesFile);
   while(! s.atEnd()){ //read present partion list and save it in newList
     newLine = s.readLine();
     	newList.push_back(newLine);
   }//end while
   volumesFile.close();
 }//end if 
 
 // remove temporary file
 system(QString("rm " + checkfile).toLatin1().data());

 //check for changes in list
 if(_PartitionList.count() > 0){
   if(_PartitionList.count() < newList.count()){

	for (int i = 0; i < newList.count(); i++){
		
		bool isNewDirectory = true;
		QString newDir;
		for (int j = 0; j <_PartitionList.count(); j++){
			//if (newList[i].startsWith("d")){  // only get directories
				if (_PartitionList[j] == newList[i]) {
					isNewDirectory = false;
				}
				else {
					 newDir = newList[i];
					}
		       //} else isNewDirectory = false;
		}
		if (isNewDirectory){
  			int pos = newDir.lastIndexOf("/Volumes/", newDir.length() -2);
			QString gotDir = newDir.remove(0, pos);
    			_MountPoint = gotDir;
                        _MountFlags = "rw";
			qDebug("New Directory found: %s", _OsxVolume.toLatin1().data());	
 			_Storage = STORAGE_OSX;
			result = true;
		}
	}	
   }//end for nNewLine
   else if (_PartitionList.count() == newList.count()){
   //Nothing to do
   }
   else if (_PartitionList.count() > newList.count()){
     setStatus(tr("A storage medium has been removed. Please insert your storage medium now."),
        PALETTE_YELLOW());
     reset();
   }//end if >

}
 _PartitionList = newList; //save present list
 return result;

}


/** Searches for a USB-device. Processes entries in /proc/partitions
*/
bool GuiFindMedium::searchForUsbDevice(){
 bool result = false;
 bool isNew = false;
 QStringList newList;
 QStringList sections;
 QRegExp rxDevice("[a-z]{3,3}[0-9]{0,1}"); //3 letters and no or one digits e.g. sdb or sda1
 QRegExp rxSplit("[\\s]"); //any whitespace
 QString newLine, oldLine; //for easy debugging
 QFile partitionsFile("/proc/partitions");
 if(partitionsFile.open(QIODevice::ReadOnly)){
   QString fileContents = partitionsFile.readAll();//try to read. atEnd() will always return true for /proc files
   partitionsFile.close();
   if(fileContents.isEmpty())
     qDebug("Error: Can not read /proc/partitions");
   newList = fileContents.split("\n");
 }//end if 
 //check for changes in list
  if(_PartitionList.count() > 0){
   if(_PartitionList.count() < newList.count()){
/*      if(USE_LIVE_CD_FEATURES){
        _DeviceName = "/dev/usbstick"; //this name is used in udev and fstab of the CD
	_Storage = STORAGE_USB;
	result = true;
      }
      else*/ 
     //USB device will be mounted under partition name on standard Linux systems
       for(int nNewLine = 0; nNewLine < newList.count(); nNewLine ++){ // run through the whole list and check all entries
       isNew = true;
       newLine = newList[nNewLine];
       for(int nOldLine = 0; nOldLine < _PartitionList.count(); nOldLine ++){//compare to all old lines
         oldLine = _PartitionList[nOldLine];
         if(newLine == oldLine) 
           isNew = false;
       }//end for nOLdLine
       if(isNew){  
         sections= newLine.split(rxSplit);
         if(rxDevice.exactMatch(sections[sections.count() -1])){ //last column contains device name
           _DeviceName = "/dev/" + sections[sections.count() -1]; // this is only sda1 (uba1) at this point
           _Storage = STORAGE_USB;
           result = true;
         }// end if match 
       }//end isNew
     }//end for nNewLine
   }// end if <
   else if (_PartitionList.count() == newList.count()){
   //Nothing to do
   }
   else if (_PartitionList.count() > newList.count()){
     setStatus(tr("A storage medium has been removed. Please insert your storage medium now."), PALETTE_YELLOW());
     _MountPoint = "";
     _IsWritable = false;
  }//end if >
 }//end if > 0
 _PartitionList = newList; //save present list
 return result;
}

/** Searches the mountpoint in mtab or /proc/mounts.
  * Sets _DeviceName 
  */
QString GuiFindMedium::findMountPoint(){
 QString result, line;
 QStringList entries;
 bool found = false;
 uint n =0;
 QString configFile = "/proc/mounts";
 QFile file(configFile);
  
 if(file.open(QIODevice::ReadOnly)){
   QString fileContents = file.readAll();
    if(fileContents.isEmpty())
      qDebug("Error: Can not read /proc/mounts");
   QStringList lines = fileContents.split("\n");
   qDebug("GuiFindMedium::findMountPoint: Searching for: %s", qPrintable(_DeviceName)); 
   int lineNumber = 0;
   while((lineNumber < lines.count()) && (!found)){
     line = lines[lineNumber];
     if(line.indexOf(_DeviceName, 0, Qt::CaseInsensitive)>=0){ //search for the name, whereever it is 
       found = true;
       qDebug("GuiFindMedium::findMountPoint: Found mountpoint: %s   Device line: %s", _DeviceName.toLatin1().data(), line.toLatin1().data());
       entries= line.split(" ");
       n = entries.count();
       _MountPoint = entries[1];
       result= _MountPoint;
       _MountFlags = entries[3];
      }
     lineNumber++;
   }//end while
   file.close();
 }//end if 
 return result;
}

/** Read property of QString _CryptDirName. 
*/
const QString& GuiFindMedium::getCryptDirName()const{
	return _CryptDirName;
}

/** Read property of QString _MountPoint. 
*/
const QString& GuiFindMedium::getMountPoint()const{
	return _MountPoint;
}

/**
*shows a short help message in a mesagebox.
*/
void GuiFindMedium::on_buttonHelp_clicked(){
   QString msg =
   tr("MoneyPenny needs the removable storage medium in order to save data and key files.<br>\
   Finding a USB device is somewhat tricky, because there may be many USB-devices. \
   The easiest way is to look for a freshly plugged in device.<br>\
   Therefore it is important to switch on the computer <b>without</b> the USB device and plug it in, when MoneyPenny asks you to do so.\
   If you have already plugged in your USB device, remove it and plug it in again, after the removal has been detected.<br>\
   A floppy disk will be found, if it has been inserted, regardless of the insertion time. \
   <p>The program will proceed automatically a few seconds after a medium has been found.</p>");
   QMessageBox::information(this, "MoneyPenny Help", msg);
}

/** Read property of bool _IsWritable. 
*/
const bool& GuiFindMedium::isWritable()const{
	return _IsWritable;
}

/** Closes dialog and creates crypt-directory, if _IsWriteable is true. Else repeats search.
*/
void GuiFindMedium::on_buttonNext_clicked(){
 if((_Storage != STORAGE_UNKNOWN) && _IsWritable){
  _CryptDirName = _MountPoint + "/" + QString(MONEYPENNY_MAIN_DIR);
   QDir dir(_CryptDirName);

  //Try to rename existing data directory, if creating a backup
  if(dir.exists() && _MountBackupMedium){
   int nBackup = 1;
   QString bakDirName = _CryptDirName + "_bak" + QString::number(nBackup);
   while(dir.exists(bakDirName) && nBackup < 10){
     nBackup++;
     bakDirName = _CryptDirName + "_bak" + QString::number(nBackup);
   }
   if(!dir.rename(_CryptDirName, bakDirName)){
    QMessageBox::critical(this, tr("Critical Error"), tr("Could not rename old data directory: ") + _CryptDirName);  
     _Timer.stop();
    return;
   }
  }

  //create a new directory, if none is available
  if(!dir.exists())
     dir.mkpath(_CryptDirName);
  if(!dir.exists()){
    QMessageBox::critical(this, tr("Critical Error"), tr("Could not create data directory: ") + _CryptDirName);  
     _Timer.stop();
    return;  
  }
  QDialog::accept();
 }
}

/** Closes dialog by calling reject.
*/
void GuiFindMedium::on_buttonCancel_clicked(){
  reject();
}


 /** Resets the flags
 */
 void GuiFindMedium::reset(){
    _Storage = STORAGE_UNKNOWN;
    buttonNext->setEnabled(false);
    buttonNext->setText(tr("&Next"));
    _TimeToShow = 2;
    _BankDataAvailable = false;
     _IsWritable = false;
    _MkDirStarted = false;
    _MediumFound = false;
    _DeviceName = "";
    _MountPoint = "";
    _MountFlags = "";
    _CryptDirName = "";
    _SkipTimer = false;
    _PartitionList.clear();
     frameMore->hide();
    _WriteCycles = 0;
    mediumtypeChanged();
 }
 
 /**Opens an Xterm and a file dialog
 */
 void GuiFindMedium::on_buttonSearchManually_clicked(){
  _SkipTimer = true;
  unsetCursor();
  QMessageBox::information(this, tr("How To Search"), "<html>" +
  tr("Sorry for not finding your medium. MoneyPenny will open a shell for experts next.\
   <br>Search the name of your usb device in /proc/partitions, mount it to /mnt/usb-moneypenny and exit the shell.\
   <br>Then a file dialog pops up. Select your mountpoint in this dialog and click Ok.") + "</html>");
  if(USE_LIVE_CD_FEATURES)
    system(c_sudo( "xterm", this));
  else 
    system("xterm");
  QString s = QFileDialog::getExistingDirectory(
                 this,
                 "Select your mountpoint",
                 "/mnt");
  if(s[s.length()]== QChar('/'))
    s = s.remove(s.length()-1, 1); // strip trailing slash /               
  _Storage = STORAGE_MANUALLY;
  qDebug("Mountpoint manually selected: %s", s.toLatin1().data());
  _MountPoint= s; //take the mountpoint for devicename 
  _MountFlags= "rw"; //believe in writeabilty
  _MediumFound = true;
 // _IsWritable = true;
  _SkipTimer = false;
 }

 /**Shows or hides the extra buttons
 */
 void GuiFindMedium::on_buttonMore_clicked(){
    if(frameMore->isVisible()){
      frameMore->hide(); 
      frameBankList->hide();
      labelPixmap->show();
      buttonMore->setText(tr("&More"));
    }
    else{
      frameMore->show(); 
      frameBankList->show();
      labelPixmap->hide();
      buttonMore->setText(tr("&Less"));
    }
 }
 
/** Runs the specified process, spins the event loop and sets the _MediumFound flag, so that timer events are not processed.
*/
void GuiFindMedium::runProcess(const QString & command){
   QCursor oldCursor = cursor();    
   _MediumFound = true; //do not process pending timer events
   setCursor(Qt::WaitCursor);
   qDebug("Starting process: %s", command.toLatin1().data());
   system(command.toLatin1().data());
   _MediumFound = false; 
   setCursor(oldCursor);
}

/** If more than one bank from mp1 is available, shows the bank List.
*  Returns number of banks. Returns -1, if medium is new and no moneypenny_data dir exists.
*  Returns 1 if a moneypenny2.bf file is found.
*/
int GuiFindMedium::buildBankList(){
  bool bankdataFound = false;

  if(_MountPoint.isEmpty())
   return 0;
  QDir banksDir = QDir(_MountPoint + "/"+ MONEYPENNY_MAIN_DIR);
  if(!banksDir.exists()) //this is a completely new medium
   return -1; 
  QStringList bankFiles = banksDir.entryList(QStringList("*" + QString(MONEYPENNY_ENCRYPTED_HBCI_CONF_FILE)));
  QStringList mp2Files = banksDir.entryList(QStringList(QString(MONEYPENNY2_ENCRYPTED_HBCI_CONF_FILE)));
  if(mp2Files .count() == 0 && bankFiles.count() > 0)
    checkBoxConvertMp1Data->setChecked(true); 
  if(mp2Files .count() > 0 ) //hide the conversion list, if converted data exist
    frameBankList->hide(); 
  if( bankFiles.count() > 0){ // fill list, if no valid mp2 data are available
     //list all the banknames = prefixes to moneypenny.bf
     listWidgetBanks->clear();
     QStringList::iterator itBank = bankFiles.begin();
     while( itBank != bankFiles.end()){
       QString bankName = (*itBank).remove("_" + QString(MONEYPENNY_ENCRYPTED_HBCI_CONF_FILE));
       if(!bankName.isEmpty()){
         if((bankName != MONEYPENNY_ENCRYPTED_HBCI_CONF_FILE)||(bankFiles.count() ==1) ) //show old file only, if no others are available
           listWidgetBanks->addItem(bankName);
       }
       itBank ++;
     }
  }//list of mp1 banks complete
  listWidgetBanks->selectAll(); //select them all for conversion

  if(listWidgetBanks->count() <= 1 || mp2Files .count() > 0){
    frameBankList->hide();
  }
  else {
    frameBankList->show();
    qApp->flush();
    qApp->processEvents();
    unsetCursor();
  }
 
  bankdataFound = listWidgetBanks->count() > 0 || mp2Files .count() > 0;

  if( bankdataFound )
   return 1;
  else return 0;
}

 /** Returns list of banks selected for conversion to MP2
 */
 QStringList GuiFindMedium::bankNames(){
   QStringList bankList;

   for(int nBank = 0; nBank < listWidgetBanks->selectedItems().count(); nBank++)
     bankList.push_back(listWidgetBanks->selectedItems()[nBank]->text());

   return bankList;
 }
 
/** Sets the new status text and resizes and repaints the dialog. Enables buttonNext, 
  * if palette is green.
  */  
void GuiFindMedium::setStatus(const QString & newText, const QPalette & newPalette){
     if(labelStatus->text()== newText)
      return;

     //change text. This may change the size of the label. 
     labelStatus->setText(newText);
     labelStatus->setPalette(newPalette);
     update();
     qApp->processEvents(); //recalculate the dialog's layout for the new label

     if(newPalette == PALETTE_RED())
       setCursor(Qt::WaitCursor);
     else unsetCursor();

     buttonNext->setEnabled(newPalette == PALETTE_GREEN());

     repaint(); //now force resizing and repainting
}

/** Tries to write a testfile into dir. Returns true on success.
 */
bool GuiFindMedium::canWriteTestFile(){
       QString testFileName;
       bool result = false;
       if(_MountFlags.indexOf(QString("rw"), 0) < 0){ //not mounted rw
         _IsWritable = false;
         qDebug("Device  %s is not mounted rw. Mountflags= %s",
         _MountPoint.toLatin1().data(), _MountFlags.toLatin1().data());
         return false;
       }
       testFileName = _MountPoint + "/moneypenny.tst";
       qDebug("Testfile: %s", testFileName.toLatin1().data());
       QFile testFile(testFileName);
       result = testFile.open(QIODevice::WriteOnly); //do not rely on rw in /proc/mounts, try to write!
       system("sync"); //wait for all files to be written
       if(result) {      //an empty file has been written
         testFile.remove(); //remove it
         qDebug ("Wrote testfile: %s", testFileName.toLatin1().data()); 
       }  
       else qDebug("Device is mounted rw. Could not yet write testfile: %s", testFileName.toLatin1().data());
       return result;
 }

/** Searches for a Udrw-device. Uses a fixed path
*/
bool GuiFindMedium::searchForUdrwDevice(){
   bool result = false;
   qDebug("Using UD-RW features");
   _DeviceName = UDRW_DEVICE; // this is only sda1 (uba1) at this point
   qDebug("Device name: %s", _DeviceName.toLatin1().data());
   _Storage = STORAGE_USB;
   result = true;
   return result;
}

/** Returns true, if checkbox Laod Legacy Data is checked
*/
bool GuiFindMedium::isDataConvertingSelected(){
 return checkBoxConvertMp1Data->isChecked();
}
