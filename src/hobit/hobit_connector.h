/***************************************************************************
                          hobit_validator.h  -  description
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


#ifndef HOBIT_CONNECTOR_H
#define HOBIT_CONNECTOR_H

 
#include <QtNetwork/QTcpSocket>
#include <QTimer>

#include "globals.h"


/**Class for testing  an internet connection. Uses _TestUrl and _TestPort as connection partner.
  *@author Marcel Mulch, Hendrik Sirges modified: Walter Roth
  */

class HoBIT_Connector : public QObject{
  Q_OBJECT
  public: 

  /** The state values for the internet connection.
   *  Used by HoBIT_Connector.
   */
  typedef enum{
     ConnectionUntested,
     ConnectionConnecting,
     ConnectionFailed,
     ConnectionConnected,
     ConnectionClosed,
     ConnectionWasOk
  } ConnectionState; 

  HoBIT_Connector(QWidget *parent, QString testUrl="127.0.0.1", int testPort= 3000);
  virtual ~HoBIT_Connector();
  
  /** parent widget, needed for QMessageBox and QProgressDialog
  */
  QWidget* _Parent;
  
  
  /** Returns ConnectionState. For values see globals.h 
  */                                   
  ConnectionState connectionState();
  
  /** closes the open connection */
  void closeConnection();
  
  /** opens a new connection to _Url */
  void connectTCP();
  
    /** reconnects to _Url */
  void reconnectTCP();

  /** Read property of QString m_URL. 
  */
  const QString& getUrl()const;
  /** Write property of QString m_URL. 
  */
  void setUrl( const QString& _newVal);
  /** Read property of int m_Port. 
  */
  const int& getPort()const;
  /** Write property of int m_Port. 
  */
  void setPort( const int& _newVal);
  /** Read property of int _TimeOut. 
  */
  const int& getTimeOut()const;
  /** Write property of int _TimeOut. 
  */
  void setTimeOut( const int& _newVal);
  
  
  private:
  ConnectionState _ConnectionState;
  /** the socket
  */
  QTcpSocket _Socket;
  /** The url, to which the connection is testetd.
  */
  QString _Url;
  /**The port to be tested.
  */
  int _Port;
  /** The time out timer
  */
  QTimer _Timer;
  /**The counter for seconds
  */
  int _TimeElapsed;                       
  /** TimeOut is the time after which a connection setup is supposed to have failed. Default is 20s.
  */
  int _TimeOut;
  private slots:
  
  void slotTCPError( QAbstractSocket::SocketError socketError );
  void slotTCPConnectionEstablished();
  void slotCloseConnection();
  void slotNextSecond();
};

#endif




