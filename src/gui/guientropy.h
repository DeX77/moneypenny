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
#ifndef GUIENTROPY_H
#define GUIENTROPY_H

#include <QDialog>
#include <QPoint>
#include <QVector>
#include <QByteArray>
#include <QTimer>
#include <ui_guientropybase.h>

#include "kernel/helpers/C_uarray.h"

/**
Let the user generate entropy by drawing in a widget. On Linux, entropy is automatically collected in /dev/random. On Windows you need to extend this class and collect the entropy yourself.

	@author walter <walter@HP2100-Roth>
*/
class GuiEntropy : public QDialog, public Ui::GuiEntropyBase
{
Q_OBJECT
public:
    GuiEntropy(QWidget *parent = 0);

    ~GuiEntropy();
  /** Returns a deep copy of the entropy generated.
   */
  C_uarray entropy(){return _Entropy;};
protected:
   /** Draw the picture, count the events and set progress.
     * Start close-timer, when enough (4096 bit) entropy was collected.
     */
   void mouseMoveEvent(QMouseEvent * me);
   /** Paint the picture
    */
   void paintEvent(QPaintEvent * pe);
  /** Clear the list of points and restart from scratch.
   */
   void showEvent(QShowEvent * se);
protected slots:
   /**Close the window one second after enough points have been drawn.
   */
   void slotTimerDone();
   /** Calls reject()
     */
   void on_buttonCancel_clicked();
  /** emit back and call inherited reject
   */
   void reject();

signals:
  void back();
 // void entropyCollected();
private: 
   QVector < QPoint > _Points;
   QTimer _Timer; //show success image for 1 second
   bool _IsDone;
   QByteArray _Buffer;
   C_uarray _Entropy;
};

#endif
