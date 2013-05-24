/***************************************************************************
                          guisplash.h  -  description
                             -------------------
    begin                : 24.03.2007
    copyright            : (C) 2007 by Walter Roth
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "guisplash.h"

GuiSplash::GuiSplash()
: QWidget()
{
	setupUi(this);
   setObjectName("GuiSplash");
   qDebug("Constructor GuiSplash");
}	
	
GuiSplash::~GuiSplash(){
   qDebug("Destructor GuiSplash");
}

void GuiSplash::resizeEvent(QResizeEvent * re){
  if(!isVisible())
   return;
  QPixmap pixmap(":/images/moneypenny2.png");
  int w = re->size().width() - 2 * labelPixmap->x();
  QPixmap scaledPixmap = pixmap.scaled(QSize(w, w), Qt::KeepAspectRatio);
  labelPixmap->resize(scaledPixmap.width(), scaledPixmap.height());
  labelPixmap->setPixmap(scaledPixmap);
}
