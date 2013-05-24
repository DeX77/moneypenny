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
#ifndef GUIFINDMEDIUM_H
#define GUIFINDMEDIUM_H

#include "ui_guifindmediumbase.h"
#include "globals.h"

#include <qtimer.h>
/**
Searches for a freshly inserted removable storage medium by reading /proc/partitions
If it finds a suitable medium, it creates the moneypenny main directory, when accept is called. The absolute
path of this directory can be retrieved with getDirName. 
@author Walter Roth
*/
class GuiFindMedium : public QDialog, public Ui::GuiFindMediumBase
{
    Q_OBJECT
public:
    /**Constructor connects timer
    */
    GuiFindMedium(QWidget * parent);

    /**Destructor
    */
    virtual ~GuiFindMedium();
    
    /** Read property of StorageType m_Storage. 
    */
    const StorageType& getStorage()const;
    
    /** Write property of StorageType _Storage. 
    */
    void setStorage( const StorageType& _newVal);

    /** Read property of QString _CryptDirName. 
    */
    const QString& getCryptDirName()const;
    
    /** Read property of QString _MountPoint. 
    */
    const QString& getMountPoint()const;

    /** Returns list of banks selected for conversion to MP2
    */
    QStringList bankNames();

    /** Set the backup medium mode. Renaming old data dir is
     *  activated in backup mode.
     */
    void setMountBackupMedium(bool newVal){_MountBackupMedium = newVal;};
    /** Returns true, if checkbox Laod Legacy Data is checked
    */
    bool isDataConvertingSelected();
protected slots:    
    /** Searches for new partitions
    */
    void slotTimerDone();
    
    /** Calls mediumTypeChanged()
    */
    void on_radioButtonFloppy_toggled(bool);

    /** Calls mediumTypeChanged()
    */
    void on_radioButtonUsb_toggled(bool);

    /**
    *shows a short help message in a mesagebox.
    */
    void on_buttonHelp_clicked();
    
    /** Closes dialog if _IsWriteable is true. Else repeats search.
    */
    void on_buttonNext_clicked();

    /** Closes dialog by calling reject.
    */
    void on_buttonCancel_clicked();
    
    /**Opens an Xterm and a file dialog
    */
    void on_buttonSearchManually_clicked();
    
    /**Shows or hides the extra buttons
    */
    void on_buttonMore_clicked();

 private:
     
    
    /** Resets the flags and restarts timer
    */
    void reset();
    /** Tries to read from floppy or USB device
    */
    void mediumtypeChanged();
     /** Starts the timer loop
     */
     void showEvent(QShowEvent * se);
     
     /** Stops the timer loop
     */
     void hideEvent(QHideEvent * se);

     /** Searches for a mounted floppy
     */
     bool searchForFloppy();
     
     /** Searches for a USB-device
     */
     bool searchForUsbDevice();
     /** Searches for a UDRW-device
     */
     bool searchForUdrwDevice();

     /** Searches for a new plugged in volume, 
      *  when moneypenny is running on mac osx
      */
     bool searchForVolumeOSX();

     /** Searches the mountpoint in mtab
     */
     QString findMountPoint();
    
     /** Read property of bool _IsWritable. 
     */
     const bool& isWritable()const;
     
     /** Runs the specified process, spins the event loop and sete the _SkipTimer flag, so that timer events are not processed.
     */
     void runProcess(const QString & command);
     /** If more than one bank is available, shows the bank List.
     */
     int buildBankList();
     /** Sets the new status text and resizes and repaints the dialog. Enables buttonNext, 
       * if palette is green.
      */  
     void setStatus(const QString & newText, const QPalette & newPalette);
     /** Tries to write a testfile into _MountPoint dir. Returns true on success
     */
     bool canWriteTestFile();
private:

    /** Storage is the type of removable medium detected
    */
    StorageType _Storage;
    
    /** The timer for the check loop
    */
    QTimer _Timer;
    
    /**List of entries in /proc/partitions
    */
    QStringList _PartitionList;
    
    /** _CryptDirName is the directory name of the mounted medium
    */
    QString _CryptDirName;
    
    /** _CryptDirName is the directory name of the mounted medium
    */
    QString _MountPoint;

    /** _MountFlags are the flags as take from /proc/mounts
    */
    QString _MountFlags;
    /** _Device is the device name of the mounted medium (fd0, sda1)
    */
    QString _DeviceName;
 
     /**
     * Volume Mount path from a new found volume on osx
     */
    QString _OsxVolume;
    
    /**time until the dialog will close automatically
    */
    int _TimeToShow;  
    /** IsWritable is set according to the mount flags of the medium in /proc/mounts
    */
    bool _IsWritable;
    /**mkdir started, do not start again
    */
    bool _MkDirStarted;

   /**Flag to keep dialog open while searching manually
   */
   bool _SkipTimer;
 
   /** Flag to stop searching after medium has been found.
   */
   bool _MediumFound;  
   /** Bank file(s) found 
   */
   bool _BankDataAvailable;
   /**Flag for mounting a backup medium
    */
   bool _MountBackupMedium;
   /**Number of tries to write testfile
   */
   int _WriteCycles;
  /** System has a floppy drive
   */
   bool _HasFloppy;
 };

#endif
