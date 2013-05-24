/***************************************************************************
                          hobit_sql  -  description
                             -------------------
    begin                : Fr Sept 9 2005
    copyright            : (C) 2005 by Marcel Mulch & Thomas Richter, Walter Roth 2007
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


#include "hobit_sql.h"
#include <qstring.h>
#include <qstringlist.h>
#include <openhbci2/date.h>
#include <openhbci2/value.h>
#include <openhbci2/pointer.h>
using namespace HBCI;

/** base constructor of the SQL Management class */
HoBIT_SQL::HoBIT_SQL(){
     qDebug("Constructor HoBIT SQL"); 

}


/** base destructor */
HoBIT_SQL::~HoBIT_SQL(){

}

/** set all lists that are exported to the database */
void HoBIT_SQL::setAll (	banking::TransactionList transactions, 
   			 	list<Pointer<DatedTransfer> > datedtransfers,
				list<Pointer<StandingOrder> > standingorders){

setTransactions(transactions);
setDatedTransfers(datedtransfers);
setStandingOrders(standingorders);
}



/** set the transaction list to be exported */
   void HoBIT_SQL::setTransactions(banking::TransactionList transactions){
   _Transactions = transactions;
   }
   
   
   /** set the dated transfer list to be exported */
   void HoBIT_SQL::setDatedTransfers(list<Pointer<DatedTransfer> > datedtransfers){
   _Datedtransfers = datedtransfers;
   }
   
   
   /** set the standing order list to be exported */
   void HoBIT_SQL::setStandingOrders(list<Pointer<StandingOrder> > standingorders){
   _Standingorders = standingorders;
   }
   
   
   /** method that generates and writes an sql source file  */
   void HoBIT_SQL::writeSqlFile(QString path){
   
    _file = new QFile(path);
    if ( _file->open( QIODevice::WriteOnly ) ) {
        QTextStream stream( _file );
	
		// drop the old database if exists
		stream << "DROP DATABASE IF EXISTS dbmoneypenny;\n\n\n";
		stream << "CREATE DATABASE dbmoneypenny;\n\n\n";
		stream << "USE dbmoneypenny;\n\n\n";
	
	
        	createTables(stream);
                insertTransactions(stream);
                insertDatedTransfers(stream);
                insertStandingOrders(stream);
        _file->close();
	
    }
    
    	// execute command which initializes the database
    	
	QString command = "mysql -h ";
	command += MYSQL_HOST;
	command += " -u ";
	command += MYSQL_USER;
	command += " --password=";
	command += MYSQL_PASSWORD;
	command += " < ";
	command += path;
	qDebug("Initializing database with command: %s", qPrintable(command));
	system(command.toLatin1().data());
   
   
   }
   
 
   /** creates the sql tables */
   void HoBIT_SQL::createTables(QTextStream &stream){
   
   QString create;

   create = "CREATE TABLE `DatedTransfers` ( \n" 
       "`transferID` int(11) NOT NULL auto_increment, \n" 
       "`datedtransfer_date` date default '0000-00-00', \n " 
       "`datedtransfer_value` varchar(100) default '0.00', \n" 
       "`textkey` varchar(100) default '', \n"
       "`textkeyext` varchar(100) default '', \n"
       "`ouraccId` varchar(100) default '', \n " 
       "`ourbankCode` varchar(100) default '', \n " 
       "`ourcountry` varchar(100) default '',\n"
       "`otheraccId` varchar(100) default '', \n "
       "`otherbankCode` varchar(100) default '', \n "
       "`othercountry` varchar(100) default '',\n" 
       "`othername` varchar(100) default '', \n "        
       "`purpose` varchar(100) default '',\n"
       "`jobid` varchar(100) default '',\n"
       " PRIMARY KEY  (`transferID`) \n " 
       ") TYPE=MyISAM COMMENT='Saves the dated Transfers'; \n";
   
   stream << create << "\n\n\n";
   

   create = "CREATE TABLE `StandingOrders` ( \n" 
       "`standingorderID` int(11) NOT NULL auto_increment, \n" 
       "`standingorder_date` date default '0000-00-00', \n " 
       "`standingorder_value` varchar(100) default '0.00', \n"
       "`start_date` date default '0000-00-00', \n " 
       "`end_date` date default '0000-00-00', \n " 
       "`textkey` varchar(100) default '', \n"
       "`textkeyext` varchar(100) default '', \n"
       "`ouraccId` varchar(100) default '', \n " 
       "`ourbankCode` varchar(100) default '', \n " 
       "`ourcountry` varchar(100) default '',\n"
       "`otheraccId` varchar(100) default '', \n "
       "`otherbankCode` varchar(100) default '', \n "
       "`othercountry` varchar(100) default '',\n" 
       "`othername` varchar(100) default '', \n "        
       "`purpose` varchar(100) default '',\n"
       "`jobid` varchar(100) default '',\n"
       "`unit` varchar(100) default '', \n " 
       "`turnus` varchar(100) default '', \n " 
       "`day` varchar(100) default '', \n " 
       "`yearly` varchar(100) default '', \n " 
       "`weekly` varchar(100) default '', \n " 
       " PRIMARY KEY  (`standingorderID`) \n " 
       ") TYPE=INNODB COMMENT='Saves the Standing Orders'; \n";
   	    
	    
   stream << create << "\n\n\n";
   
   
   create = "CREATE TABLE `Transactions` ( \n"
  	    "`transactionID` int(11) NOT NULL auto_increment, \n"
  	    "`valutadate` date default '0000-00-00', \n"
            "`transaction_date` date default '0000-00-00', \n"
            "`storedate` date default '0000-00-00', \n"
            "`transaction_value` varchar(100) default '0.00', \n"
            "`transaction_key` varchar(100) default '', \n"
            "`custref` varchar(100) default '', \n"
            "`transaction_text` varchar(100) default '', \n"
            "`textkey` varchar(100) default '', \n"
            "`primanota` varchar(100) default '',\n"
            "`ourbankCode` varchar(100) default '',\n"
            "`ouraccId` varchar(100) default '', \n"
            "`otherbankCode` varchar(100) default '',\n"
            "`otheraccId` varchar(100) default '',\n"
  	    "`othername` varchar(100) default '',\n"
  	    " PRIMARY KEY  (`transactionID`) \n"
            ") TYPE=MyISAM COMMENT='Saves the Accounthistory'; \n";
	    
	    
     stream << create << "\n\n\n";
   
   }
      
   
/** creates inserts for moneypenny's transactions */
void HoBIT_SQL::insertTransactions(QTextStream &stream){

banking::TransactionList::iterator transit;

// iterate through the transaction list
for (transit = _Transactions.begin(); transit != _Transactions.end(); transit++){
	
	// valuta date
	QString valutadate = QString((*transit)->valutaDate().year().c_str());
	valutadate += "-";
	valutadate += QString((*transit)->valutaDate().month().c_str());
	valutadate += "-";
	valutadate += QString((*transit)->valutaDate().day().c_str());
	
	// bookingDate
	QString bookingDate = QString((*transit)->bookingDate().year().c_str());
	bookingDate += "-";
	bookingDate += QString((*transit)->bookingDate().month().c_str());
	bookingDate += "-";
	bookingDate += QString((*transit)->bookingDate().day().c_str());
	
	// date
	QString storedate = QString((*transit)->storeDate().year().c_str());
	storedate += "-";
	storedate += QString((*transit)->storeDate().month().c_str());
	storedate += "-";
	storedate += QString((*transit)->storeDate().day().c_str());
	
	// value
        QString sValue;
	sValue.setNum((*transit)->value().doubleValue(), 'f',2);
	sValue +=(*transit)->currency().c_str();
	// key
	QString key = (*transit)->key().c_str();
		
	
	//custref 
	QString custref = (*transit)->custref().c_str();
		

	//text 
	QString text = (*transit)->text().c_str();
	
	//textkey
	QString textkey = (*transit)->textkey().c_str();
	
	// primanota
	QString primanota = (*transit)->primanota().c_str();	


	// our bankcode
	QString ourbankcode = (*transit)->ourBankCode().c_str();
	
	// our accountid = 
	QString ouraccountid = (*transit)->ourAccountId().c_str();

	
	// other bankcode
	QString otherbankcode = (*transit)->otherBankCode().c_str();
	
	// other accountid = 
	QString otheraccountid = (*transit)->otherAccountId().c_str();

	
	// other name
	QString othername = (*transit)->otherName1().c_str();
	
        stream << "INSERT INTO Transactions (valutadate, \n"
                  "transaction_date, storedate, transaction_value,\n"
                  "transaction_key, custref, transaction_text,\n"
                  "textkey, primanota, ourbankCode,\n"
                  "ouraccId, otherbankCode, otheraccId,\n"
                  "othername) VALUES ('" << valutadate << "',\n"
                  "'" << bookingDate << "' , '" << storedate << "' ,\n"
                  "'" << sValue << "' , '" << key << "' ,\n"
                  "'" << custref << "' , '" << text << "' ,\n"
                  "'" << textkey << "' , '" << primanota << "' ,\n"
                  "'" << ourbankcode << "' , '" << ouraccountid << "' ,\n"
                  "'" << otherbankcode << "' , '" << otheraccountid << "' ,\n"
                  "'" << othername << "');\n\n\n";

}

}

/** creates inserts for moneypenny's dated transfer's */
 void HoBIT_SQL::insertDatedTransfers(QTextStream &stream){

  list<Pointer<DatedTransfer> >::iterator datit;

// iteare through the dated transfers list
  for (datit = _Datedtransfers.begin(); datit != _Datedtransfers.end(); datit++){

	// date
    QString date = QString::number((*datit).ref().date().year());
    date += "-";
    date += QString::number((*datit).ref().date().month());
    date += "-";
    date += QString::number((*datit).ref().date().day());
	


    	
	// value
    QString value = (*datit).ref().value().toString().c_str();
		
	//textkey
    QString textkey = QString::number((*datit).ref().textKey());

    	//textkey
    QString textkeyext = QString::number((*datit).ref().textKeyExt());

    	
	// our bankcode
    QString ourbankcode = (*datit).ref().bankCode().c_str();
	
    // our country
    QString ourcountry = QString::number((*datit).ref().country()); 
    
        
	// our accountid = 
    QString ouraccountid = (*datit).ref().accountId().c_str();

	
	// other bankcode
    QString otherbankcode = (*datit).ref().otherBankId().c_str();
	
	// other accountid = 
    QString otheraccountid = (*datit).ref().otherAccountId().c_str();
    
        // our country
    QString othercountry = QString::number((*datit).ref().otherCountry());

	// other name
    QString othername = (*datit).ref().otherName().c_str();

    // other name
    QString purpose = (*datit).ref().purpose().c_str();

     // other name
    QString jobid = (*datit).ref().jobId().c_str();

    
    	
      stream << "INSERT INTO DatedTransfers (datedtransfer_date, datedtransfer_value, textkey," 
               << "textkeyext, ouraccId, ourbankCode, ourcountry, otheraccId, otherbankCode,"
               << "otherCountry, othername, purpose, jobid) VALUES ('" << date << "',\n"
        "'" << value << "' , '" << textkey << "' ,\n"
        "'" << textkeyext << "' , '" << ouraccountid << "' ,\n"  
        "'" << ourbankcode << "' , '" << ourcountry << "' ,\n"
        "'" << otheraccountid << "' , '" << otherbankcode << "' ,\n"
        "'" << othercountry << "' , '" << othername << "' ,\n"
        "'" << purpose << "' , '" << jobid << "');\n\n\n";

  }

}

/** creates inserts for moneypenny's standing order's */
 void HoBIT_SQL::insertStandingOrders(QTextStream &stream){
   
   list<Pointer<StandingOrder> >::iterator standit;
   
  //iteare through the standing orders list
  for (standit = _Standingorders.begin(); standit != _Standingorders.end(); standit++){
    
    	// date
    QString date = QString::number((*standit).ref().date().year());
    date += "-";
    date += QString::number((*standit).ref().date().month());
    date += "-";
    date += QString::number((*standit).ref().date().day());
	
    QString fdate = QString::number((*standit).ref().firstDate().year());
    fdate += "-";
    fdate += QString::number((*standit).ref().date().month());
    fdate += "-";
    fdate += QString::number((*standit).ref().date().day());
    
    QString ldate = QString::number((*standit).ref().lastDate().year());
    ldate += "-";
    ldate += QString::number((*standit).ref().date().month());
    ldate += "-";
    ldate += QString::number((*standit).ref().date().day());


    	
	// value
    QString value = (*standit).ref().value().toString().c_str();
		
	//textkey
    QString textkey = QString::number((*standit).ref().textKey());

    	//textkey
    QString textkeyext = QString::number((*standit).ref().textKeyExt());

    	
	// our bankcode
    QString ourbankcode = (*standit).ref().bankCode().c_str();
	
    // our country
    QString ourcountry = QString::number((*standit).ref().country()); 
    
        
	// our accountid = 
    QString ouraccountid = (*standit).ref().accountId().c_str();

	
	// other bankcode
    QString otherbankcode = (*standit).ref().otherBankId().c_str();
	
	// other accountid = 
    QString otheraccountid = (*standit).ref().otherAccountId().c_str();
    
        // our country
    QString othercountry = QString::number((*standit).ref().otherCountry());

	// other name
    QString othername = (*standit).ref().otherName().c_str();

    // other name
    QString purpose = (*standit).ref().purpose().c_str();

     // other name
    QString jobid = (*standit).ref().jobId().c_str();
    
    
    QString unit = (*standit).ref().unit().c_str();

    QString turnus = QString::number((*standit).ref().turnus());
    
    QString day = QString::number((*standit).ref().day());
    
    QString yearly = (*standit).ref().yearly().c_str();
    
    QString weekly;
    
    if ((*standit).ref().weekly()) 
             weekly = "yes";
    else weekly = "no";
   
        
        
    	
    stream << "INSERT INTO StandingOrders (standingorder_date, standingorder_value," 
           <<  "start_date, end_date, textkey, textkeyext, ouraccId, ourbankCode," 
           <<  "ourcountry, otheraccId, otherbankCode, otherCountry, othername, "
           <<  "purpose, jobid, unit, turnus, day, yearly, weekly) VALUES ('" << date << "',\n"
        "'" << value << "' , '" << fdate << "' ,\n"
        "'" << ldate << "' , '" << textkey << "' ,\n"
        "'" << textkeyext << "' , '" << ouraccountid << "' ,\n"  
        "'" << ourbankcode << "' , '" << ourcountry << "' ,\n"
        "'" << otheraccountid << "' , '" << otherbankcode << "' ,\n"
        "'" << othercountry << "' , '" << othername << "' ,\n"
        "'" << purpose << "' , '" << jobid <<  "' ,\n"
        "'" << unit << "' , '" << turnus <<  "' ,\n"
        "'" << day << "' , '" << yearly <<  "' ,\n"
        "'" << weekly << "');\n\n\n";
   	
  }
}
