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

#ifndef GUISPLASH_H
#define GUISPLASH_H

#include <QWidget>
#include "ui_guisplashbase.h"

class GuiSplash: public QWidget, public Ui::GuiSplash
{
  public:	
	GuiSplash();
	~GuiSplash();
void resizeEvent(QResizeEvent * re);
};

#endif
