/***************************************************************************
                          guiiniletter.cpp  -  description
                             -------------------
    begin                : So Nov 7 2004
    copyright            : (C) 2004 by Marcel Mulch, Walter Roth
    email                : marcel@mulch-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms ofthe GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "guiiniletter.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QPrinter>
#include <QPrintDialog>
#include <QFont>

#include <iostream>

/**
*constructor of GuiIniLetter
*
*@param parent a pointer on the main widgets widget stack
*@param name widgets title
*@param hobit the hobit object, needed for hbci functions
*/
GuiIniLetter::GuiIniLetter( QWidget *parent) 
 : QWidget(parent), Ui::GuiIniLetter() {
   setupUi(this);
   setObjectName("GuiIniLetter"); 
  _Printer = new QPrinter(QPrinter::HighResolution); //use 1200 DPI for postscript or driver resolution on Windows
  _ReadableScene = new QGraphicsScene(iniLetterFrame->geometry());
  _PreviewScene = new QGraphicsScene(iniLetterFrame->geometry());
  _PreviewTextItem = NULL;
  _PreviewPaperRect = NULL;
  graphicsView->setScene(_ReadableScene);
  _Font = new QFont ("Courier", qApp->font().pointSize()); 
  _SetupOk = false;
  _ShowPreview = false;
  _LeftBorder = 3.5; //cm
  _TopBorder = 3.5;
  _Text = "No ini letter available.";
 qDebug("Constructor GuiIniLetter finished");
}


/**
*destructor of GuiIniLetter
*/
GuiIniLetter::~GuiIniLetter(){
 if(_Printer)
   delete _Printer;
 if(_ReadableScene)  
   delete _ReadableScene;
 if(_PreviewScene)  
   delete _PreviewScene;
 if(_Font)  
   delete _Font;
 qDebug("Destructor GuiIniletter");
}


/**
*function for printing the IniLetter
*/
void GuiIniLetter::on_buttonPrintNow_clicked() {
    QMatrix m, mOld;
    if(!_Printer)
      initPrinter();
    QPrintDialog printDialog(_Printer, this);
    if(_SetupOk || printDialog.exec() == QDialog::Accepted)
    {
         _ShowPreview = true; 
         showPreviewText();
         QPainter painter(_Printer);
         int x = _Printer->paperRect().x() - _Printer->pageRect().x();
         int y = _Printer->paperRect().y() - _Printer->pageRect().y();
         painter.translate(x,y);
        _PreviewScene->render(&painter, _Printer->paperRect(), _PreviewPaperRect->boundingRect());
        QMessageBox::information(this, tr("Print Ini Letter"), tr("The Ini-Letter has been sent to the printer."));
    }
    emit terminated();
}
/** 
*Create a new ini letter and show it on the screen.
*/
void GuiIniLetter::setIniLetter(const QString & iniLetter){
  _Text= iniLetter;
  showReadableText();
}

/** 
*Calls the printer setup dialog. This may take about 1 minute!!!
*/
void GuiIniLetter::on_buttonPrinterSetup_clicked(){
   if(!_Printer)
     initPrinter();
  buttonPrinterSetup->setText(tr("Initializing printer... Please wait."));
   repaint();
   qApp->setOverrideCursor(Qt::WaitCursor);
   QTime time;
   time.start();
   while (time.elapsed() < 500) 
    qApp->processEvents();
   QPrintDialog printDialog(_Printer, this);
   if( printDialog.exec() == QDialog::Accepted)
     _SetupOk = true;
   qApp->restoreOverrideCursor();
   if(_SetupOk)
     on_buttonPrintNow_clicked(); //The accept-button is labeled "Print", so we do print
}

/** 
*Calls the hardware setup dialog.
*/
void GuiIniLetter::on_buttonAddPrinter_clicked(){
  emit (addPrinterRequest());
}

/** 
*Displays the text on the canvas, already scaled for printing.
*/
void GuiIniLetter::on_buttonPreview_clicked(){
  if(!_Printer)
   initPrinter();
    _ShowPreview = ! _ShowPreview; 
  if(_ShowPreview)
     showPreviewText();
   else showReadableText();
}


/** 
*Shows the iniletter text scaled 1:1.
*/
void GuiIniLetter::showReadableText()
{
  buttonPreview->setText(tr("&Preview Print"));
  //Clear scene
  QList <QGraphicsItem *> itemList = _ReadableScene->items();
   while (!itemList.isEmpty())
     delete itemList.takeLast();
 
  //build scene from scratch
  //make a frame rectangle with 30 pixels margins
  QGraphicsTextItem * textItem = new QGraphicsTextItem();
  textItem->setPlainText(_Text);
  _Font->setPointSize( qApp->font().pointSize()) ;
  textItem->setFont(*_Font);
  textItem->setZValue(2.0); //put it on top
  _ReadableScene->addItem(textItem);

  QRectF rectF = textItem->boundingRect();
  rectF.setWidth(rectF.width() + 60.0);
  rectF.setHeight(rectF.height() + 60.0);
  rectF.translate( -30.0, -30.0);

  QGraphicsRectItem * rect = new QGraphicsRectItem();
  rect->setRect(rectF);
  _ReadableScene->addItem(rect);
  rect->setZValue(1.0); //background

  //now make it another 30 pixels wider and use it as bounding rect for the scene
  rectF.setWidth(rectF.width() + 60.0);
  rectF.setHeight(rectF.height() + 60.0);
  rectF.translate( -30.0, -30.0);
  _ReadableScene->setSceneRect(rectF);

  graphicsView->setMatrix(QMatrix()); //loadIdentity
  graphicsView->setScene(_ReadableScene);
  graphicsView->show();
}

/** 
*Shows a scaled print preview.
*/
void GuiIniLetter::showPreviewText(){
     int leftMargin, topMargin; //margins in pixels
     if(!_Printer)
       initPrinter();
     leftMargin = (int)((_LeftBorder / 2.54) * _Printer->logicalDpiX());
     topMargin = (int)((_TopBorder / 2.54) * _Printer->logicalDpiY());

     //Clear scene
     QList <QGraphicsItem *> itemList = _PreviewScene->items();
     while (!itemList.isEmpty())
       delete itemList.takeLast();

     //(re)build scene from scratch
     double paperHeight = _Printer->paperRect().height(); //convert to double
     double paperWidth = _Printer->paperRect().width(); //convert to double
     double printableWidth = _Printer->width();
     double viewHeight = graphicsView->height();
     double scaleX = viewHeight / (paperHeight * 1.1);  //leave a margin
     //printer's Y resolution may be different, therefore use separate scales fo X and Y
     double scaleY = scaleX * (double)_Printer->logicalDpiX() / (double)_Printer->logicalDpiY(); 
     double offsetX = (0.5 / 2.54) * _Printer->logicalDpiX();
     double offsetY = (0.5 / 2.54) * _Printer->logicalDpiY();

     //first draw a black page shadow rect, with 0.5cm offset
     QGraphicsRectItem * shadowRect = new QGraphicsRectItem();
     shadowRect->setRect(offsetX, offsetY, paperWidth, paperHeight);
     shadowRect->setBrush(QColor(0, 0, 0, 255)); //opaque black
     shadowRect->setZValue(0.0); //lowest z
 
     //draw paper rectangle
     _PreviewPaperRect = new QGraphicsRectItem();
     _PreviewPaperRect->setRect(0.0,0.0, paperWidth, paperHeight);
     _PreviewPaperRect->setBrush(QColor(255, 255, 255, 255)); //opaque white
     _PreviewPaperRect->setZValue(1.0);

     _PreviewTextItem = new QGraphicsTextItem();
     _PreviewTextItem->setPlainText(_Text);
     _Font->setPointSize(12); //12 point font for Ini letter 
     QFontMetrics printerFontMetrics(*_Font, _Printer);
     double printerFontHeight = printerFontMetrics.height();
     QFontMetrics widgetFontMetrics(*_Font, this);
     double widgetFontHeight = widgetFontMetrics.height();
     _Font->setPointSize((int)(_Font->pointSize()*( printerFontHeight / widgetFontHeight))) ;
     _PreviewTextItem->setFont(*_Font);
     _PreviewTextItem->setPos(leftMargin,topMargin);
     _PreviewTextItem->setTextWidth(printableWidth - 2 * _LeftBorder); //printable width - margins
     _PreviewTextItem->setZValue(2.0);

     //add items to scene
     _PreviewScene->setItemIndexMethod(QGraphicsScene::NoIndex);
     _PreviewScene->addItem(shadowRect);
     _PreviewScene->addItem(_PreviewPaperRect);
     _PreviewScene->addItem(_PreviewTextItem);
     QRectF sceneRect = _PreviewScene->itemsBoundingRect();
     _PreviewScene->setSceneRect(sceneRect);

    graphicsView->setMatrix(QMatrix()); //loadIdentity
    graphicsView->setScene(_PreviewScene);
    graphicsView->scale ( scaleX, scaleY );
    buttonPreview->setText(tr("&End Preview"));
    graphicsView->show();
    //force redraw
    qApp->flush();
    qApp->processEvents();
}

/** 
*Renews the layout and sets up the graphicsView according to preview flag.
*@param re pointer on the QResizeEvent - see in QT-Api for more help
*/
void  GuiIniLetter::resizeEvent(QResizeEvent * ){
if(_ShowPreview)
        showPreviewText();
else showReadableText();
}

/** 
*Fires signal terminated.
*/
void  GuiIniLetter::on_buttonCancel_clicked(){
  emit terminated();
}

/** 
*Fires signal terminated.
*/
void  GuiIniLetter::on_buttonSkipPrinting_clicked(){
  emit terminated();
}

 
/** Deletes old _Printer and creates a new one.
 */
void GuiIniLetter::initPrinter(){
 if(_Printer)
  delete _Printer;
 _Printer = new QPrinter(QPrinter::HighResolution); //use 1200 DPI for postscript or driver resolution on Windows
}
