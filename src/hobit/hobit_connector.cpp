/***************************************************************************
                          hobit_validator.cpp  -  description
                             -------------------
    begin                : So Okt 24 2004
    copyright            : (C) 2004 by Marcel Mulch, Walter Roth
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

#include "hobit_connector.h"

#include <QMessageBox>
#include <QLabel>
#include <QProgressDialog>
#include <QApplication>
#include <QPixmap>
#include <QtGui>

HoBIT_Connector::HoBIT_Connector(QWidget* parent, QString testUrl, int testPort){
  _Parent = parent;
  setUrl(testUrl);
  setPort(testPort);
  _TimeElapsed = 0;
  _ConnectionState = ConnectionUntested;
  _TimeOut = 20;
              
  connect(&_Socket, SIGNAL(error( QAbstractSocket::SocketError )),
  	  this, SLOT(slotTCPError( QAbstractSocket::SocketError))); // error output via cerr
  connect(&_Socket, SIGNAL(connected()),
  	  this, SLOT(slotTCPConnectionEstablished())); //set _tcpConnectionEstablished flag
  connect(&_Socket, SIGNAL(disconnected()),
  	  this, SLOT(slotCloseConnection())); //reset _tcpConnectionEstablished flag
  connect(&_Timer, SIGNAL(timeout()),
          this, SLOT(slotNextSecond()));     //count the seconds 
}

HoBIT_Connector::~HoBIT_Connector(){
}


void HoBIT_Connector::connectTCP(){
   int time;
   QProgressDialog dlg( "", tr("&Cancel"), 0, _TimeOut, _Parent);
   dlg.setWindowTitle(tr("Internet Connector")); 
   dlg.setMinimumDuration(2);
   _TimeElapsed = 0;
   dlg.setValue(_TimeElapsed);
   
   QLabel * labelConnect = new QLabel(&dlg);
   dlg.setLabel(labelConnect);
   QString sConnect(tr("Trying to connect to: ") +_Url);
   QFontMetrics metrics(dlg.font());
   int stringWidth = metrics.width(sConnect);
   QPixmap pixmapConnect(50 + stringWidth, 32);
   pixmapConnect.fill(dlg.palette().color(QPalette::Window));
   QPainter painter(&pixmapConnect);

   painter.drawPixmap(0, 0, QPixmap(MP_DIR() + QString(MONEYPENNY_ICON_DIR) + "/connect_creating.png"));
   painter.drawText(40, 15, sConnect);
   labelConnect->setPixmap(pixmapConnect);
   
   _Timer.start(1000); 
   _Socket.connectToHost(_Url, _Port); //_ConnectionState = ConnetcionConnected is set via _Socket::Signal connected
   _ConnectionState = ConnectionConnecting;
   
  while((_TimeElapsed < _TimeOut)&& (_ConnectionState != ConnectionConnected) && !dlg.wasCanceled()){
   qApp->processEvents(); // let the timer increment _TimeElapsed
   dlg.setValue(_TimeElapsed);
  }
  qDebug("Time to connect: %i", _TimeElapsed);
  if(_ConnectionState == ConnectionConnected){ // show success for 2 seconds
     time = _TimeElapsed;    
     pixmapConnect.fill(dlg.palette().color(QPalette::Window));
     painter.drawPixmap(0, 0, QPixmap(MP_DIR()  + QString(MONEYPENNY_ICON_DIR) + "/connect_creating.png"));
     sConnect = tr("Connected to: ") +_Url;     
     painter.drawText(40, 15, sConnect);
     labelConnect->setPixmap(pixmapConnect);//redraw
     dlg.setValue(_TimeOut - 1);
     while(_TimeElapsed < time + 2){ // let the timer increment _TimeElapsed
      qApp->processEvents();
     }
  }   
  else {
     pixmapConnect.fill(dlg.palette().color(QPalette::Window));
     painter.drawPixmap(0, 0, QPixmap(MP_DIR()  + QString(MONEYPENNY_ICON_DIR) + "/connect_no.png"));
     sConnect =tr("Failed to connect to: ") +_Url;     
     labelConnect->setPixmap(pixmapConnect);//redraw
     painter.drawText(40, 15, sConnect);
      _ConnectionState = ConnectionFailed;
     QMessageBox::warning(_Parent, tr("Connect failed"), tr("Please connect to the internet manually prior to sending jobs."));
  }   
  _Timer.stop();
  _TimeElapsed = 0;
}


void HoBIT_Connector::slotTCPError( QAbstractSocket::SocketError socketError){
     _ConnectionState = ConnectionFailed;
    switch(socketError){
    	case QTcpSocket::ConnectionRefusedError:
		qDebug("TCP Connection Refused Error");
		break;
	case QTcpSocket::HostNotFoundError:
		qDebug("TCP Host Not Found Error");
		break;
	case QTcpSocket::SocketAccessError:
		qDebug("TCP Socket Access Error");
		break;
	default:
	       qDebug( "Establishing TCP Connection failed. Reason unknown.");
               break;
    } 
    closeConnection();    
}


void HoBIT_Connector::slotCloseConnection(){
	 _ConnectionState = ConnectionFailed;
	qDebug( "Connection closed by foreign host");
}

void HoBIT_Connector::closeConnection(){
       _Socket.close();
       _ConnectionState = ConnectionClosed;
       qDebug("TCP connection closed by command.");
}


HoBIT_Connector::ConnectionState HoBIT_Connector::connectionState(){
	return _ConnectionState;
}

void HoBIT_Connector::reconnectTCP(){
	closeConnection();
	connectTCP();
}

void HoBIT_Connector::slotTCPConnectionEstablished(){
        qDebug("TCP connection established.");
	_ConnectionState = ConnectionConnected;
}

void HoBIT_Connector::slotNextSecond(){
 _TimeElapsed ++;
}

/** Read property of QString m_URL. 
*/
const QString& HoBIT_Connector::getUrl()const{
	return _Url;
}

/** Write property of QString m_URL. 
*/
void HoBIT_Connector::setUrl( const QString& _newVal){
  _Url = _newVal;
  QString rawIp = _newVal;

  // Protocol in Ip?
  if(rawIp.indexOf("://") != -1)
  {
          // remove protocol
          rawIp = rawIp.mid(
                          rawIp.indexOf("://")+3,
                          rawIp.length() - (int)rawIp.indexOf("://")
                          );
  }
  // path in IP?
  if((int)rawIp.indexOf("/") != -1)
  {
          _Url = rawIp.mid( //remove path
                          0,
                          (int)rawIp.indexOf("/")
          );
  }
  else _Url = rawIp; 
}


/** Read property of int m_Port. 
*/
const int& HoBIT_Connector::getPort()const{
	return _Port;
}

/** Write property of int m_Port. 
*/
void HoBIT_Connector::setPort( const int& _newVal){
	_Port = _newVal;
}


/** Read property of int _TimeOut. 
*/
const int& HoBIT_Connector::getTimeOut()const{
	return _TimeOut;
}

/** Write property of int _TimeOut. 
*/
void HoBIT_Connector::setTimeOut( const int& _newVal){
	_TimeOut = _newVal;
}

