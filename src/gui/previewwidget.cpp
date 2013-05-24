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
#include "previewwidget.h"
#include <QPainter>
#include <QTextDocument>
#include <QTextFrame>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>

PreviewWidget::PreviewWidget(QWidget *parent)
 : QAbstractScrollArea(parent)
{
  _Pages = NULL;
  _PageDistance = 0;
  _Scale = 1.0;
  _PaperSize.rwidth()= 0;
  _PaperSize.rheight()= 0;
  setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
  verticalScrollBar()->setSingleStep(20);
}


PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::setPrinterGeometry(const QSizeF & paperSize, const QPointF pageTopLeft){
 _PaperSize = paperSize; 
 _PageTopLeft = pageTopLeft;
 _ShadowSize = 0.02 * _PaperSize.rwidth();
 resizeEvent(NULL);
}

void PreviewWidget::setPages(QVector <QTextDocument *> * newPages){
  _Pages = newPages;
   for( int nPage = 0; nPage < _Pages->count(); nPage ++){
    (*_Pages)[nPage]->documentLayout()->setPaintDevice(viewport());
   }
}

void PreviewWidget::paintEvent(QPaintEvent * ){
  if(!_Pages)
    return;

  QPainter painter(viewport());
  //draw a dark background
  painter.setBrush(Qt::gray);
  painter.drawRect(0,0, width(), height());
  painter.translate(_ShadowSize, _ShadowSize - verticalScrollBar()->value());

  QRect paperRect;
  paperRect.setWidth(_PaperSize.rwidth());
  paperRect.setHeight(_PaperSize.rheight());
  QRect shadowRect = paperRect;
  shadowRect.moveTop(_ShadowSize);
  shadowRect.moveLeft(_ShadowSize);

  for(int nPage = 0; nPage <  _Pages->count(); nPage ++){
     //setup transformation
     painter.save(); 
     painter.scale (_Scale, _Scale); 
     painter.translate(0, nPage * _PageDistance);

     //draw shadow and paper rectangle first
     painter.setBrush(Qt::black); 
     painter.drawRect(shadowRect);
     painter.setBrush(Qt::white);
     painter.drawRect(paperRect);
  
     paintText(&painter, nPage);
  
     painter.restore();
  }//end for nPage
 
}

void PreviewWidget::resizeEvent(QResizeEvent *)
{
    const QSize viewportSize = viewport()->size();
    _Scale = 0.9 * (double)width() / _PaperSize.rwidth();

    QSize docSize;
    docSize.setWidth(qRound(_PaperSize.width() + 2 * _ShadowSize) * _Scale);
    _PageDistance = _PaperSize.rheight() + 3.0 * _ShadowSize;
    verticalScrollBar()->setRange(0, _Pages->count() * _PageDistance);
    int pageCount = 1;
    if(_Pages)
      pageCount = _Pages->count();
    docSize.setHeight((pageCount * _PageDistance) * _Scale);

    horizontalScrollBar()->setRange(0, docSize.width() - viewportSize.width());
    horizontalScrollBar()->setPageStep(viewportSize.width());

    verticalScrollBar()->setRange(0, docSize.height() - viewportSize.height());
    verticalScrollBar()->setPageStep(viewportSize.height());
}

void PreviewWidget::paintText(QPainter *painter, int page)
{
    QTextDocument * doc = (*_Pages)[page];
    QSizeF pgSize = doc->pageSize();
    QRectF docRect(QPointF(0, 0), pgSize);
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = docRect;
    // don't use the system palette text as default text color, on HP/UX
    // for example that's white, and white text on white paper doesn't
    // look that nice
    ctx.palette.setColor(QPalette::Text, Qt::black);

    painter->translate(_PageTopLeft);
    painter->setClipRect(docRect);//.translated(printPreview->pageTopLeft));
    doc->documentLayout()->draw(painter, ctx);
}
