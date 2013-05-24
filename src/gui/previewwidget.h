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
#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QAbstractScrollArea>

class QTextDocument;

/**
The preview for the printed transactionlist.

	@author walter <walter@HP2100-Roth>
*/
class PreviewWidget : public QAbstractScrollArea
{
Q_OBJECT
public:
    PreviewWidget(QWidget *parent = 0);

    ~PreviewWidget();
 void paintEvent(QPaintEvent * se);

 void setPrinterGeometry(const QSizeF & paperSize, const QPointF pageTopLeft);

 void setPages(QVector <QTextDocument *> * newPages);

 void resizeEvent(QResizeEvent *);

 void paintText(QPainter *painter, int page);

 private:
  QSizeF _PaperSize;
  QPointF _PageTopLeft;
  int _ShadowSize;
  int _PageDistance;
  double _Scale;
  QVector <QTextDocument * > * _Pages;
};

#endif
