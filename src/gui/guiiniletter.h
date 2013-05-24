/***************************************************************************
                          guiiniletter.h  -  description
                             -------------------
    begin                : So Nov 7 2004
    copyright            : (C) 2004 by Marcel Mulch
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

#ifndef GUIINILETTER_H
#define GUIINILETTER_H

#include <QStackedWidget>
#include <QPrinter>

#include "hobit.h"
#include "ui_guiiniletterbase.h"

class QStackedWidget;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsTextItem ;
class QGraphicsRectItem ;
class QFont;
class QPrinter;



/**
*This is the base dialog for showing and printing the ini-letter
*/
class GuiIniLetter :public QWidget, public Ui::GuiIniLetter
{
  Q_OBJECT

public:

  /**
  *constructor of GuiIniLetter
  *
  *@param parent a pointer on the main widgets widget stack
  *@param name widgets title
  *@param hobit the hobit object, needed for hbci functions
  */
  GuiIniLetter( QWidget *parent);

  /**
  *destructor of GuiIniLetter
  */
  ~GuiIniLetter();

  /**
  *Generates an new ini-Letter and shows it on the textedit.
  */
  void setIniLetter(const QString & iniLetter);

  /**
  *Renews the layout and sets up the _View according to preview flag.
  *@param re pointer on the QResizeEvent - see in QT-Api for more help
  */
  void resizeEvent(QResizeEvent * re);

signals:

  /**
  *To be connected to GuiMain::slotAccountLogin()
  *
  */
  void terminated();
  
  /** Emitted on Add Printer
  */
  void addPrinterRequest();

public slots:
  /** Deletes old _Printer and creates a new one.
   */
  void initPrinter();
private slots:

  /**
  * Opens the printer-dialog and starts printing
  * the ini-letter
  */
  void on_buttonPrintNow_clicked();

  /**
  *Calls the printer setup dialog.
  */
  void on_buttonPrinterSetup_clicked();

  /** 
  *Calls the hardware setup dialog.
  */
  void on_buttonAddPrinter_clicked();

  /**
  *Displays the text on the canvas, already scaled for printing.
  */
  void on_buttonPreview_clicked();

  /**
  *Fires signal terminated.
  */
  void on_buttonCancel_clicked();

  /**
  *Fires signal terminated.
  */
  void on_buttonSkipPrinting_clicked();

  /**
  *Shows the iniletter text scaled 1:1.
  */

  void showReadableText();
  /**
  *Shows a scaled print preview.
  */
  void showPreviewText();

private:

private:
  QString _Text;
  QStackedWidget * _Stack;
  QGraphicsScene * _ReadableScene;
  QGraphicsScene * _PreviewScene;
  QGraphicsTextItem * _PreviewTextItem;
  QGraphicsRectItem * _PreviewPaperRect;
  QFont * _Font;
  QPrinter * _Printer;

  /**
  *Indicates, that the user has setup the printer
  */
  bool _SetupOk;

  /**
  *Flag for viewing state
  */
  bool _ShowPreview;

  /**
  *left border on printed page in cm
  */
  double _LeftBorder;

  /**
  *Top border on printed page in cm
  */
  double _TopBorder;
};

#endif
