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
#ifndef GUITRANSACTIONPRINTER_H
#define GUITRANSACTIONPRINTER_H

#include <QTextEdit>
#include <QListView>
#include <QStackedWidget>
#include <QPaintDevice>
#include <QPrinter>

#include "ui_guitransactionprinterbase.h"


class TransactionItem;
class HoBIT;
class QTextDocument;
namespace banking{ 
  class TransactionList;
};

/**
* Prints the transaction list. The preview generation follows the "TextEdit" example by Qt.
* There is very poor documentation on QTextDocument and related classes. 
* Before printing, the printer setup dialog is called.
* The spinboxes to the left mirror the controls of the printer setup dialog. They may be set in reverse
* order, thus allowing
* the user to set the from value to the higher page number.
* The pages are built as follows: 
* A set of html formatted page texts, fitting on one page each, is generated form the transactions.
* A set of QTextDocuments is created from the page texts and passed to the preview view.
* When printing, a temporary QTextDocument with the printer's geometry settings is created and printed 
* for each printer-page.
*/
class GuiTransactionPrinter : public QWidget, public Ui::guiTransactionPrinterBase
{
  Q_OBJECT
public:

  /**
  *this is the transaction printer dialog constructor
  *
  *@param main a pointer on moneypenny´s main class
  *@param name widgets title
  */
  GuiTransactionPrinter(QWidget* parent);

  /**
  * this is the transaction printer dialog destructor
  */
  ~GuiTransactionPrinter();

  /**
  * Creates the html text for printing and preview.
  */
  int createPrintPages();

  /**
  * Read property of QString m_AccountName.
  */
  const QString& getAccountName()const;

  /**
  * Write property of QString m_AccountName. 
  * @param _newVal QString
  */
  void setAccountName( const QString& _newVal);

  /**
  * To be called when main data have changed
  */
  void refresh();
  /** 
  * Rebuilds list.
  *
  */
  void showEvent(QShowEvent * se);

  /** invalidates bitmapo
  */
  void resizeEvent(QResizeEvent * re);

signals:

  /**
  * emitted on buttonBack clicked
  */
  void back();

  /**
  * emitted on help button clicked
  */
  void helpRequest(const QString& filename);


private: //private methods

  /**
  * This is a function for creating the page headers.
  * @param pageNumber Number of the page
  * @param totalPages Amount of pages
  */
  QString createPageHeader(int pageNumber, int totalPages);

  /**
  * This is a function for creating the page footers.
  * @param pageNumber Number of the page
  * @param totalPages Amount of pages
  */
  QString createPageFooter(int pageNumber, int totalPages);

  /**
  * Creates a page separator for the print preview.
  */
  QString createPageSeparator();

  /**
  * Prints a single page.
  */
  int printPage(QPainter * painter, int nPage);

  /**
  * Creates the background bitmap for fast print preview.
  */
  void createPreviewPixmap();

  /**
  * Returns a html compatible RGB string.
  * @param color QColor
  */
  QString colorToHtml(const QColor & color);


public slots:
  /**
  * Calls QPrinter::setup and synchronizes spin boxes and printer setup dialog.
  */
  bool on_buttonPrinterSetup_clicked();

  /**
  * Initializes printer and printer font.
  */
  void initPrinter();

  /**
  * Select line color 1
  */
  virtual void on_buttonColor1_clicked();

  /**
  * Select line color 2
  */
  virtual void on_buttonColor2_clicked();

  /**
  * Hide or show Options
  */
  virtual void on_buttonShowPrintOptions_clicked();

  /**
  * Select a printer font
  */
  virtual void on_buttonFont_clicked();

  /**
  * Set new page margins
  */
  virtual void on_buttonMarginsOk_clicked();

  /** set new border width
  */
  virtual void on_comboBoxBorderWidth_currentIndexChanged(int);

  /**
  * Send signal help
  */
  virtual void on_buttonHelp_clicked();
   /** Set the list to be printed.
   */
  void setTransactionList(banking::TransactionList * newVal){_TransactionList = newVal;};
  /**
  * prints the text on printer
  */
  void on_buttonPrint_clicked();
  /**
  * emits back();
  */
  void on_buttonBack_clicked();

  /**
  * Activates or deactivates the spin boxes.
  */
  void on_radioButtonRange_toggled(bool checked);
  /** Sets printer range
  */
  void on_spinBoxTo_valueChanged(int);
  /** Sets printer range
  */
  void on_spinBoxFrom_valueChanged(int);


private:

  /** Sets margins and pageSize
  */
  void setupDocGeometry(QTextDocument * doc, QPaintDevice * paintDevice, QSizeF pageSize);
  /** Deletes preview documents and htm-pagetexts.
  */
  void clearPages();
  /** Creates a new set of pages.
  */
  void updatePageLayout();
 
  banking::TransactionList *  _TransactionList;

  /**
  * flag for valid print pages
  */
  bool _PreviewDocumentsValid;

  /**
  * flag for valid printer setup
  */
  bool _PrinterSetupOk;

  /**Page margin settings in cm*/
  double _LeftMarginCm;
  double _TopMarginCm;
  double _RightMarginCm;
  double _BottomMarginCm;

  /** Line style for the table
  */
  int _BorderWidth;

  /**
  * paper size from printer driver
  */
  QSizeF _PaperSize;
  /** top left corner of printable area, scaled by the resolution ratio
   */
  QPointF _PreviewPageTopLeft;

  /**
  * page size for the preview, scaled by the resolution ratio
  */
  QSizeF _PreviewPageSize;
  /**
  * paper size for the preview, scaled by the resolution ratio
  */
  QSizeF _PreviewPaperSize;

  /**
  * The printer. Initialized in showEvent or when required.
  */
  QPrinter * _Printer;
  
  /**
  * The printer font
  */
  QFont _PrinterFont;

  /**
  * The list of pages. Contains one string per page.
  */
 // QStringList _Pages;

  /**
  * the scale for the preview
  */
  double _ScaleX;
  /**
  * the scale for the preview
  */
  double _ScaleY;

  /**
  * Backgroundcolor for uneven line numbres
  */
  QColor _Color1;

  /**
  * Backgroundcolor for even line numbres
  */
  QColor _Color2;
  /** The html texts for the pages. To be used for preview- and print documents.
  */
  QStringList _PageTextList;
  /** The preview documents.
   */
  QVector <QTextDocument *> _PreviewDocuments;
};

#endif
