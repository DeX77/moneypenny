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
#include <QBrush>
#include <QPainter>
#include <QMatrix>
#include <QPaintDevice>
#include <QBitmap>
#include <QProgressDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QGraphicsPixmapItem>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextFrame>
#include <QAbstractTextDocumentLayout>
#include <QPrintEngine>

#include "guitransactionprinter.h"
#include "../hobit/hobit.h"
#include "../banking/transactionitem.h"
#include "../../kernel/helpers/transaction.h"

#include "globals.h"
static int cmToPixelsY(double cm, QPaintDevice *device)
{
    double inches = cm * 0.39370147;
    return (int)(inches * device->logicalDpiY());
}

static int cmToPixelsX(double cm, QPaintDevice *device)
{
    double inches = cm * 0.39370147;
    return (int)(inches * device->logicalDpiX());
}


/**
*this is the transaction printer dialog constructor
*
*@param main a pointer on moneypenny´s main class
*@param name widgets title
*/
GuiTransactionPrinter::GuiTransactionPrinter(QWidget* parent)
:QWidget(parent)
{
  setupUi(this);
  setObjectName("GuiTransactionPrinter");
  _TransactionList = NULL;
  _PreviewDocumentsValid = false;
  _PrinterSetupOk = false;
  framePreview->setPages(&_PreviewDocuments);
      //Page settings in cm
  _LeftMarginCm =   1.5;
  editLeft->setText(QString::number(_LeftMarginCm, 'f',1));
  _TopMarginCm =    2.5;
  editTop->setText(QString::number(_TopMarginCm, 'f',1));
  _RightMarginCm =  1.5;
  editRight->setText(QString::number(_RightMarginCm, 'f',1));
  _BottomMarginCm = 0.5;
  editBottom->setText(QString::number(_BottomMarginCm, 'f',1));
  comboBoxBorderWidth->setCurrentIndex(1); //narrow border
  _Printer = NULL; //initialize in showEvent or when required
  _PrinterFont = QFont( MONEYPENNY_PRINTER_FONT, 10); //this works well
  _Color1 = Qt::white;
  _Color2 = QColor(240, 240, 240); // very light gray
  groupBoxOptions->hide();
}

/**
*this is the transaction printer dialog destructor
*/
GuiTransactionPrinter::~GuiTransactionPrinter()
{
qDebug("Destructor GuiTransactionPrinter called");
 if(_Printer != NULL)
   delete _Printer;
 if(_TransactionList)
  delete _TransactionList;
 clearPages();
qDebug("Destructor GuiTransactionPrinter finished");

}


/** 
* This is a function for creating the html text for printing and preview. Find out text heights by querying
* QSimpleRichText::height after adding 
* respective paragraph of text. Then create a set of preformatted pages with headers and footers. Every page
* is formatted
* as a complete html table. The table rows may be multiline in height, therefore the page height has to be
* tested after 
* every new table row. 
* To find out the total number of pages, pages are preformatted. When all pages are complete, the final text
* is
* created with the headers and footers showing the correct page and total page numbers. 
* Returns number of pages.
* Formatting works as follows: A first guess on the number of lines per page is made according to page and
* font size.
* A test text is built and formatted with that number of lines. If it fits on the page, the length of the
* test text is increased.
* If it is to long, a line is removed and the removing flag is set. Once removing lines has started, the
* page is complete, when
* the text fits into the print rectangle.
*/
int GuiTransactionPrinter::createPrintPages(){
 if(!_TransactionList)
   return 0;
  QCursor oldCursor = cursor();
  setCursor(Qt::WaitCursor);
  //show a progress dialog
  QProgressDialog progDlg(
        tr("Formatting printer output..."), 
        tr("&Cancel"), 
        0,
        ((_TransactionList->size()) / 12) + 1, //guessed number of pages
        this);
  progDlg.setValue(0);  //start the progress dialog 
  qApp->processEvents(); //allow progress dialog to repaint 

  QString testText; // the test string for testing the page length
  QString line;
  QString header;
  QString footer;
  QStringList allLines; 

  QTime time;
  time.start();
  int nPage = 0;
  int nLine = 0; //for debugging
  int nPages = 0; //number of all pages
  clearPages(); //delete old pages
  QStringList pageBodies;
  //get the font data for the current printer
  if(_Printer == NULL)
    initPrinter();//initialize printer and font

  //Create test header and footer for obtaining exact header and footer heights
  header = createPageHeader(0, 0); //use dummies for page numbers, they are not yet known, no margins
  footer = createPageFooter(0, 0);

  //get html formatted table rows from all transactions and put them into allLines
  if(_TransactionList->end() != _TransactionList->begin()){
    //create the lines for all pages and put them into allLines
    QString sColor1 = colorToHtml(_Color1);
    QString sColor2 = colorToHtml(_Color2);
    banking::TransactionList::iterator it;
    for(it = _TransactionList->begin(); it != _TransactionList->end(); it++){
      nLine ++;
      banking::TransactionItem transactionItem = banking::TransactionItem(*(*it));
      if(checkBoxLineColor->isChecked() && ((nLine %2) != 0))
      line = transactionItem.htmlTableRow(sColor2); //default: lightgray
      else  line = transactionItem.htmlTableRow(sColor1);
      allLines += line;
    }
    qDebug("GuiTransactionPrinter::createPrintPages: Transactionlist evaluated.");
    //create the pages from allLines
    int nStartLine= 0;
    int nStopLine = 0;
    bool bDocumentFinished = false;

    //group the lines in pages that fit on the paper
    QTextDocument testDoc;
    do{//document loop

       //create a page that is one line too long
       nLine = nStartLine;
       testText = "";
       do{ //page loop
         testText += allLines[nLine];
         testDoc.setHtml(header + testText + footer);
         setupDocGeometry( & testDoc, framePreview->viewport(), _PreviewPageSize);
         nLine ++;
         nStopLine = nLine;
         bDocumentFinished = nLine >= allLines.count() && (testDoc.pageCount() < 2 ); //make shure that the last line is printed
       }while ((testDoc.pageCount() < 2 ) && (! bDocumentFinished));

      //remove the last line that does not fit on the page, if document is not finished
      if(!bDocumentFinished){
        nStopLine --;
        testText = "";
        for (nLine = nStartLine; nLine < nStopLine; nLine ++)
          testText += allLines[nLine];
      }
      nPage ++;
      nStartLine = nStopLine;
     // do {}while(((time.elapsed() /1000)% 10)!= 0); //time loop for debugging progress dialog
      progDlg.setValue(nPage); //set the new value for progress
      qApp->processEvents(); //keep the application responsive, allow progress dialog to repaint progress bar

       _PageTextList += testText; //save the page
       nPages = _PageTextList.count(); // copy to local variable for debugging
    }while (!bDocumentFinished);//end document loop 

  }
  else //no Transactions available
    _PageTextList += tr("No transactions available"); 
  qDebug("GuiTransactionPrinter::createPrintPages: Pages formatted.");


  // add correct headers and footers and put page texts into preformatted TextDocuments
  for (int nPage = 0; nPage < _PageTextList.count(); nPage ++) {

    QString page; //for debugging
    page = createPageHeader(nPage +1, nPages); //page numbers start with 1
    page += _PageTextList[nPage];
    page += createPageFooter(nPage +1, nPages);
    _PageTextList[nPage] = page;

    QTextDocument * pageDoc = new QTextDocument();
    pageDoc->setHtml(page); //overwrites FrameFormat of root frame
    setupDocGeometry( pageDoc, framePreview->viewport(), _PreviewPageSize );
    _PreviewDocuments.append(pageDoc);
//    qDebug("\n\nPage: %i\n%s", nPage, qPrintable(page));
  } //end for  

  framePreview->resizeEvent(NULL);
  textLabelPages->setText(QString::number(nPages));
  spinBoxFrom->setMaximum(nPages);
  spinBoxFrom->setValue(1);
  spinBoxTo->setMaximum(nPages);
  spinBoxTo->setValue(nPages);
  _Printer->setFromTo(1, nPages);
  _PreviewDocumentsValid = true;

  setCursor(oldCursor);
  qDebug("GuiTransactionPrinter::createPrintPages: finished.");

  return nPages;
}


/** 
* This is a function for printing the text on printer. Uses one simpleRichText object per page.
*/
void GuiTransactionPrinter::on_buttonPrint_clicked()
{
    if(_Printer == NULL)
     initPrinter();
    if(!_PrinterSetupOk) 
      on_buttonPrinterSetup_clicked();

    if(_PrinterSetupOk){ 
         QString fileName = _Printer->printEngine()->property(QPrintEngine::PPK_OutputFileName).toString();
         if(!fileName.isEmpty())
         { //We want to print to a file
           if(fileName.indexOf("media") >= 0)
             QMessageBox::information(this, tr("Information"),
              tr("If you do not want to write the pdf file to the USB-Stick containing the banking data, please remove this stick now and put in another one.")
              +tr("Wait about 20s after plugging in the new stick. Then press OK."));
         }
        // Check that there is a valid device to print to.
        QPainter printPainter;
        printPainter.begin(_Printer);
        if ( !printPainter.device() ) {
          QMessageBox::critical(this, tr("Error"), tr("Printer can not print."));
          return;
        }
        QProgressDialog  progDlg(tr("Sending pages to printer..."), 
                                tr("&Cancel"), 
                                 0,
                                _Printer->toPage() - _Printer->fromPage(), // number of pages
                                this); 
        progDlg.setMinimumDuration(0);  // do always show the dialog
        qApp->processEvents();
        bool newPageRequired = false; //first page is supplied automatically
        if(_Printer->pageOrder()== QPrinter::FirstPageFirst)
          for (int nPage = _Printer->fromPage() -1; nPage < _Printer->toPage(); nPage++){ //page loop forwards
            progDlg.setValue(nPage); //set the new value for progress
            qApp->processEvents();
            qDebug("GuiTransactionPrinter::slotPrint: Printing page %i", nPage);
            if(newPageRequired)
              _Printer->newPage(); 
            printPage (&printPainter, nPage);
            newPageRequired = true;
          }//end page loop
         else //LastPageFirst 
          for (int nPage = _Printer->fromPage(); nPage >= _Printer->toPage(); nPage--){ //page loop backwards
            if(newPageRequired)
              _Printer->newPage(); 
            printPage (&printPainter, nPage);
            newPageRequired = true;
          }//end page loop
        //Keep the dialog for another 2 seconds
        QTime stopwatch;
        stopwatch.start();
        while (stopwatch.elapsed() < 2000)  
          qApp->processEvents();
        printPainter.end();
        QMessageBox::information(this, tr("Information"), tr("Data have been printed."));

    }//end if setup ok
}


/** 
* This is a function for creating the page headers.
* @param pageNumber Number of the page
* @param totalPages Amount of pages
*/
QString GuiTransactionPrinter::createPageHeader(int pageNumber, int totalPages){
  QString result;
  QDateTime date = QDateTime(QDate::currentDate());
  QString dateFormat = tr("yyyy/MMM/dd", "Date format for printer output");
 
 result += "<p align =\"center\"><table width = 100% border =0>\n"; //first line is a separate table 
 result += "<tr>";
   result += "<th align = \"left\">";
    result += tr("Bank: ") + HoBIT::hobit()->bankName();
    result += "   ";
    result += tr("Account: ") + HoBIT::hobit()->currentAccountId();
   result += "</th>";
   result += "<th align = \"left\">";
    result += tr("Printed: ") + date.toString(dateFormat);
   result += "</th>";
   result += "<th align =\"right\">";
    result+=tr("Page") + " "  + QString::number(pageNumber) + " " + tr("of") + " " + QString::number(totalPages);
   result += "</th>";
 result += "</tr>";
 result += "</table>";
 
 QString borderWidth = QString::number(comboBoxBorderWidth->currentIndex());
 result += "<table width = 100% border ="+ borderWidth + ">\n"; // this defines the central table, 100% width will cause the outer border not to be drawn
 result += "<tr>";
   
   result += "<th width = 10%>";
    result += tr("Date");
   result += "</th>";
   
   result += "<th width = 15%>";
    result += tr("Name");
   result += "</th>";
   
   result += "<th width = 25%>";
    result += tr("Purpose");
   result += "</th>";
   
   result += "<th width = 15%>";
    result += tr("Text");
   result += "</th>";
    
   result += "<th width = 10%>";
    result += tr("Bank Code");
   result += "</th>";
   
   result += "<th width = 10%>";
    result += tr("Account");
   result += "</th>";
   
   result += "<th width = 10% align = \"right\">";
    result += tr("Value");
   result += "</th>";
   
 result += "</tr>\n";
// result += "</table>\n"; 
 
 Qt::escape(result); 
 return result;
}

/** 
* This is a function for creating the page footers.
* @param pageNumber Number of the page
* @param totalPages Amount of pages
*/
QString GuiTransactionPrinter::createPageFooter(int pageNumber, int totalPages){
  QString result;
  QDateTime date = QDateTime(QDate::currentDate());
  QString dateFormat = tr("yyyy/MMM/dd", "Date format for printer output");
  
 result += "</table>\n";  //end of table started by header
 result += "<p align = \"right\">";
 result += tr("Page")+ " "+ QString::number(pageNumber)
        + " "+ tr("of", "this the *of* in: Page 1 of 20" )+ " "+ QString::number(totalPages);
 result += "</p>\n";
 Qt::escape(result); 
 return result;
}

/** 
* Creates a page separator for the print preview.
*/
QString GuiTransactionPrinter::createPageSeparator(){
  return "";
//  return QString("<br><br><hr><br><br>\n");
}


/**  
* Initializes printer and printer font.
*/
void GuiTransactionPrinter::initPrinter(){
  if(_Printer == NULL)
  {
    qDebug("GuiTransactionPrinter::initPrinter: Initializing transaction printer.");
    _Printer = new QPrinter( QPrinter::HighResolution );
    qDebug("GuiTransactionPrinter::initPrinter: Printer constructed.");
  }
  _Printer->setFullPage(TRUE);
  _Printer->setOrientation(QPrinter::Landscape);
  _Printer->setOutputFileName("/media/usbstick/moneypenny_printdata.pdf");

  _ScaleX = (double)logicalDpiX() / (double)_Printer->logicalDpiX();
  _ScaleY = (double)logicalDpiY() / (double)_Printer->logicalDpiY();

  _PaperSize =_Printer->paperRect().size();
  _PreviewPaperSize.rwidth() = _PaperSize.rwidth() * _ScaleX;
  _PreviewPaperSize.rheight() = _PaperSize.rheight() * _ScaleY;
  
  QSizeF pageSize = _Printer->pageRect().size();
  _PreviewPageSize.rwidth() = pageSize.rwidth() * _ScaleX;
  _PreviewPageSize.rheight() = pageSize.rheight() * _ScaleY;
  _PreviewPageTopLeft.rx() = _Printer->pageRect().x() * _ScaleX;
  _PreviewPageTopLeft.ry() = _Printer->pageRect().y() * _ScaleY;
  framePreview->setPrinterGeometry(_PreviewPaperSize, _PreviewPageTopLeft);
}

/** Sets margins and pageSize
*/
void GuiTransactionPrinter::setupDocGeometry(QTextDocument * doc, QPaintDevice * paintDevice, QSizeF pageSize){
  doc->setUseDesignMetrics(true);
  doc->setDefaultFont(_PrinterFont);
  doc->documentLayout()->setPaintDevice(paintDevice);
  int leftMargin = cmToPixelsX(_LeftMarginCm, paintDevice); 
  int rightMargin = cmToPixelsX(_RightMarginCm, paintDevice); 
  int topMargin = cmToPixelsY(_TopMarginCm, paintDevice); 
  int bottomMargin = cmToPixelsY(_BottomMarginCm, paintDevice); 

  //set the document's geometry data to fit the preview viewport

  QTextFrameFormat docFormat = doc->rootFrame()->frameFormat();
  docFormat.setLeftMargin(leftMargin);
  docFormat.setRightMargin(rightMargin);
  docFormat.setTopMargin(topMargin);
  docFormat.setBottomMargin(bottomMargin);
  doc->rootFrame()->setFrameFormat(docFormat);
  doc->setPageSize(pageSize);
}

/** 
* Activates or deactivates the spin boxes.
*/
void GuiTransactionPrinter::on_radioButtonRange_toggled(bool checked){
 if(checked)
   _Printer->setPrintRange(QPrinter::PageRange);
 else  _Printer->setPrintRange(QPrinter::AllPages);
 spinBoxFrom->setEnabled(checked);
 spinBoxTo->setEnabled(checked);
 if(radioButtonAll->isChecked()){
  spinBoxFrom->setValue(1);
  spinBoxTo->setValue(_PreviewDocuments.count());
 }
}

/** Sets printer range
*/
void GuiTransactionPrinter::on_spinBoxTo_valueChanged(int){
  _Printer->setFromTo(_Printer->fromPage(), spinBoxTo->value());
}

/** Sets printer range
*/
void GuiTransactionPrinter::on_spinBoxFrom_valueChanged(int){
  _Printer->setFromTo(spinBoxFrom->value(), _Printer->toPage());
}

/** 
* Prints a single page.
*/
int GuiTransactionPrinter::printPage(QPainter * painter, int nPage){
    QTextDocument  doc; 
    doc.setHtml(_PageTextList[nPage]);
    setupDocGeometry(&doc, _Printer, _PaperSize);
    QSizeF pgSize = doc.pageSize();
    QRectF docRect(QPointF(0, 0), pgSize);
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = docRect;
    // don't use the system palette text as default text color, on HP/UX
    // for example that's white, and white text on white paper doesn't
    // look that nice
    ctx.palette.setColor(QPalette::Text, Qt::black);

//    painter->translate(_PageTopLeft);
//    painter->setClipRect(docRect);//.translated(printPreview->pageTopLeft));
    doc.documentLayout()->draw(painter, ctx);

// _PreviewDocuments[nPage]->print(_Printer);
/*  int height;
  QTextDocument richText;
  richText.setHtml(text); 
  richText.setDefaultFont(font);
  richText.setTextWidth(printRect.width() );
  richText.drawContents( painter, printRect );
  height = richText.size().height();
  return height;
 */
 return nPage;
}
/** 
* Calls QPrinter::setup and synchronizes spin boxes and printer setup dialog.
*/
bool GuiTransactionPrinter::on_buttonPrinterSetup_clicked(){
    bool result;
    if(_Printer == NULL)
      initPrinter();

    if(spinBoxTo->value() < spinBoxFrom->value()){ //user wants reverse order
      _Printer->setPageOrder(QPrinter::LastPageFirst);
      _Printer->setFromTo(spinBoxTo->value(), spinBoxFrom->value()); 
    }
    else _Printer->setFromTo(spinBoxFrom->value(), spinBoxTo->value()); 
    QPrintDialog dlg(_Printer, this);
    dlg.setMinMax(1, _PreviewDocuments.count());
    dlg.setFromTo(spinBoxFrom->value(), spinBoxTo->value());
    result = dlg.exec();
    int from = 1;
    int to = 1;
    if(result == QDialog::Accepted){ 
      if(_Printer->printRange() == QPrinter::AllPages){
         radioButtonAll->setChecked(true); //this will also enable/disable the spinboxes via slotRangeChange
         from = 1;
         to = _PreviewDocuments.count();
      }
      else{ 
        radioButtonRange->setChecked(true);
        from = _Printer->fromPage();
        to = _Printer->toPage();
      }

      if(_Printer->pageOrder() == QPrinter::FirstPageFirst){//LastPage First
        spinBoxFrom->setValue(from);
        spinBoxTo->setValue(to);
      }
      else{
        spinBoxFrom->setValue(to);
        spinBoxTo->setValue(from);
      }
    }
    _Printer->setFromTo(from, to);
//    createPrintPages();
    _PrinterSetupOk = result;
    return result;
}


/** 
* Select line color 1
*/
void GuiTransactionPrinter::on_buttonColor1_clicked(){
 QColor newColor = QColorDialog::getColor(_Color1, this);
 if(newColor.isValid()){
   _Color1 = newColor;
   createPrintPages();
   framePreview->viewport()->update();
 }
}
/** 
* Select line color 2
*/
void GuiTransactionPrinter::on_buttonColor2_clicked(){
 QColor newColor = QColorDialog::getColor(_Color2, this);
 if(newColor.isValid()){
   _Color2 = newColor; 
   createPrintPages();
   framePreview->viewport()->update();
 }
}
/** 
* Select printer font
*/
void GuiTransactionPrinter::on_buttonFont_clicked(){
  bool modified = false;
  QFont newFont  = QFontDialog::getFont(&modified, _PrinterFont, this);
  if(modified){
    _PrinterFont = newFont;
    initPrinter();
    createPrintPages();
    framePreview->viewport()->update();
  }
}
/** 
* Set new page margins
*/
void GuiTransactionPrinter::on_buttonMarginsOk_clicked(){
  updatePageLayout();
}

/** set new border width
*/
void GuiTransactionPrinter::on_comboBoxBorderWidth_currentIndexChanged(int){
  updatePageLayout();
}

/** Creates a new set of pages.
*/
void GuiTransactionPrinter::updatePageLayout(){
   _LeftMarginCm = editLeft->text().toDouble();
   _RightMarginCm = editRight->text().toDouble();
   _TopMarginCm = editTop->text().toDouble();
   _BottomMarginCm = editBottom->text().toDouble();
   createPrintPages();
   framePreview->viewport()->update();
}

/** 
* Send signal help
*/
void GuiTransactionPrinter::on_buttonHelp_clicked(){
 emit helpRequest ("printer.html");
}
/** 
* Hide or show Options
*/
void GuiTransactionPrinter::on_buttonShowPrintOptions_clicked(){
 if(groupBoxOptions->isVisible()){
  groupBoxOptions->hide();
  buttonShowPrintOptions->setText(tr("Show Print &Options"));
 }
 else{
  groupBoxOptions->show();
  buttonShowPrintOptions->setText(tr("Hide Print &Options"));
 }
 framePreview->viewport()->update();
}

/** 
* Returns a html compatible RGB string.
* @param color QColor
*/
QString  GuiTransactionPrinter::colorToHtml(const QColor & color){
  QString sR, sG, sB;
  sR.setNum(color.red(), 16);
  if(sR =="0") sR= "00";
  sG.setNum(color.green(), 16);
  if (sG=="0") sG= "00"; 
  sB.setNum(color.blue(), 16);
  if(sB == "0") sB = "00";
  return QString("#" + sR+ sG + sB);
}

/** 
* To be called when main data have changed
*/
void GuiTransactionPrinter::refresh(){
 _PreviewDocumentsValid = false;
 framePreview->viewport()->update();
}

/** 
* Rebuilds list.
*/
void GuiTransactionPrinter::showEvent(QShowEvent * ){
  if(_TransactionList == NULL)
    _TransactionList = HoBIT::hobit()->listTransactions();
  _PreviewDocumentsValid = false;
  createPrintPages();
}

 /** invalidates bitmap
 */
 void GuiTransactionPrinter::resizeEvent(QResizeEvent *){
   initPrinter();
 }

/** Deletes preview documents and htm-pagetexts.
*/
void GuiTransactionPrinter::clearPages(){
 for (int page = 0; page < _PreviewDocuments.count(); page++){
  delete _PreviewDocuments[page];
 }
 _PreviewDocuments.clear(); 
 _PageTextList.clear();
}

/**
* emits back();
*/
void GuiTransactionPrinter::on_buttonBack_clicked(){
  emit back();
}
