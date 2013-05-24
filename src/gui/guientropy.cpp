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
#include "guientropy.h"
#include <QPainter>
#include <QTime>
#include <QCryptographicHash>

//Approximately 8 bit of entropy per mouse event
#define BYTES_REQUIRED 4096 / 8

GuiEntropy::GuiEntropy(QWidget *parent)
 : QDialog(parent), GuiEntropyBase()
{
  setupUi(this);
  setObjectName("GuiEntropy");
  qDebug("%s","Constructor GuiEntropy");
  _Timer.setInterval(1000);
  connect(&_Timer, SIGNAL(timeout()), this, SLOT(slotTimerDone()));
  progressBar->setMaximum(BYTES_REQUIRED);
  _IsDone = false;
}


GuiEntropy::~GuiEntropy()
{
}

/**Close the window one second after enough points have been drawn.
*/
void GuiEntropy::slotTimerDone()
{
  // emit entropyCollected();
   accept();
}

/** Draw the picture, count the events and set progress.
* Calls accept() when enough (4096 bit) entropy was collected.
*/
void GuiEntropy::mouseMoveEvent(QMouseEvent * me)
{
  QTime time = QTime::currentTime();
  int msecs = time.msec();
  unsigned char rand1 = msecs % 256;
  int x = me->pos().x();
  int y = me->pos().y();
  unsigned char rand2 = (x * y) % 256; //mix x and y
  rand2 = (rand1 ^ rand2); //mix position and time
   _Buffer.append(rand2);
  if(_Buffer.length() >= 16)//we have got enough bytes for a reasonable hashing
  {
    _Buffer = QCryptographicHash::hash(_Buffer, QCryptographicHash::Sha1); //hash the stuff
    _Buffer = QCryptographicHash::hash(_Buffer, QCryptographicHash::Md5); //hash the stuff
//     int size = _Buffer.size();
    _Entropy.append((unsigned char *)_Buffer.data(), _Buffer.length());
    _Buffer.clear();
    if(_Entropy.length() > BYTES_REQUIRED)
    {
      _IsDone = true;
      _Timer.start();
    }
  }
  _Points.append(me->pos()); 
  progressBar->setValue(_Points.count());
  update();
}

/** Paint the picture
*/
void GuiEntropy::paintEvent(QPaintEvent *){
  QPainter painter(this);
  painter.setClipRect(frame->frameGeometry());
  if(_IsDone){
   painter.setBrush(Qt::green);
   painter.drawRect(frame->frameGeometry());
   painter.setFont (QFont("Helvetica", 30, QFont::Bold));
   painter.drawText(frame->x() + frame->width() / 2,
                    frame->y() + frame->height() / 2, 
                    tr("OK"));
  }
  else
  {
    painter.setBrush(Qt::white);
    painter.drawRect(frame->frameGeometry());
  }
  if(_Points.count() > 2)
    painter.drawPolyline(_Points);
}

/** Calls reject()
*/
void GuiEntropy::on_buttonCancel_clicked(){
  reject();
}

/** Clear the list of points and restart from scratch.
*/
void GuiEntropy::showEvent(QShowEvent *){
 _Points.clear();
 progressBar->setValue(0);
 update();
}

/** emit back and call inherited reject
*/
void GuiEntropy::reject(){
  emit back();
  QDialog::reject();
}

