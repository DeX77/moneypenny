/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Do Nov  4 11:52:24 CET 2004
    copyright            : (C) 2004 by Marcel Mulch, Walter Roth
    email                : marcel@mulch-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <QString>
#include <QTextCodec>
#include <QTranslator>
#include <QDir>
#include <QMessageBox>


//Styles
//#include <qplatinumstyle.h>
//#include <qwindowsstyle.h>
//#include <qmotifplusstyle.h>
#include <QStyleFactory>

#include "guimain.h"
#include "globals.h"

int main(int argc, char *argv[])
{
//  Check Card before asking for PIN

  ExitCode exitcode = EXITCODE_RUN_BANK_LOOP;
  QApplication app(argc, argv);
  setMP_DIR(argv[0]);
  qDebug("Application constructed.");
  parseCommandLine();
  if(USE_LIVE_CD_FEATURES){
    qDebug("USE_LIVE_CD_FEATURES: true");
      // starting chipcard daemon
      // system("sudo run_chipcard2 &");
    }
  else qDebug("USE_LIVE_CD_FEATURES: false");

  app.setFont(calculateScreenFont());//application wide font setting for all widgets  ;

  initPalettes(); //Now that the application Palette is complete, init the other ones
  // Inintialize the translation engines
  // translation engine for Qt
  QTranslator qtTranslator( 0 );
  QString qmFileName = QM_FILE_NAME("qt_");
    if(qmFileName.isEmpty())
    qDebug("Will use the untranslated source code strings for Qt.");
  else{
    qDebug("Loading Qt translations from file: %s", qmFileName.toLatin1().data());
    qtTranslator.load( qmFileName );
  }
  app.installTranslator( &qtTranslator );

  // translation engine for application strings
  QTranslator appTranslator( 0 );
  qmFileName = QM_FILE_NAME("moneypenny_");
  //qmFileName = QString("/home/uwe/Messe2/moneypenny21/translations/moneypenny_de.qm"); 
  if(qmFileName.isEmpty())
    qDebug("Will use the untranslated source code strings for MoneyPenny.");
  else{
    qDebug("Loading MoneyPenny translations from file: %s", qmFileName.toLatin1().data());
    appTranslator.load( qmFileName );
  }
  app.installTranslator( &appTranslator );


 // Create some styles for choice
  //QStyle *style0=QStyleFactory::create("windows");
  //QStyle *style0=QStyleFactory::create("motif");
/*  QStyle *style0=QStyleFactory::create("Plastique");
  if(style0)
    app.setStyle(style0); // style will be deleted by app on shutdown*/
   // create Main Gui
  GuiMain *guiMain = new GuiMain();
  qDebug("Main widget constructed.");

    //Show the splash
  guiMain->showSplashScreen();

  guiMain->buildMainApplication(); // Build the main application. This may take some time.
  qDebug("guiMain child widgets constructed.");

   //run bank loop
   while (exitcode == EXITCODE_RUN_BANK_LOOP || exitcode == EXITCODE_RESTART_SESSION){
      guiMain->showSplashScreen();
      if(guiMain->findMedium(false)){ //read medium, no backup medium
       guiMain->login();

       //hide the splash screen
       guiMain->hideSplashScreen();

       //Select size of the window here
        //guiMain->setGeometry((QApplication::desktop())->screenGeometry());
        //guiMain->setGeometry(0,0,1024,768);
    
        //guiMain->show();  //use default window size
       guiMain->showMaximized(); //use full screen, but do not cover system tray
       app.processEvents();
       guiMain->loadEnvironment(); //load decrypted data
       guiMain->slotAccountLogin(); //log in or start new setup

       exitcode = (ExitCode)app.exec();
       qDebug("Event loop terminated");
     }
     else exitcode = EXITCODE_FAILURE; //no useable medium
  }
  //delete guiMain; //deletes all child widgets 
  freePalettes();
  if(USE_LIVE_CD_FEATURES && exitcode == EXITCODE_AUTOSHUTDOWN)
    system("sudo shutdown -h now");
  return exitcode;

  //exit codes:
   // 0: Everything was ok
   // 1: Wrong login password
   // 2: No write access to medium
   // 3: Loading environment failed
   // 4: Installation error
   // 5: No automatic shutdown, call to "wait $moneypenny_pid" in script /etc/X11/xsession.d/45xsession will return 5 = nonzero
   // 6: Change bank. Shut down and restart GuiMain
     
}


