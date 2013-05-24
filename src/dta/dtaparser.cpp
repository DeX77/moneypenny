/***************************************************************************
                          dtaparser.cpp  -  description
                             -------------------
    begin                : Mi Jun 3 2009
    copyright            : (C) 2009 by Robin Golgowski,
                           (C) 2009 Eduard Mertke
    email                : robin.golgowski@stud.fh-swf.de,
                           eduard.mertke@stud.fh-swf.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dtaparser.h"
#include "asatz.h"
#include "csatz.h"
#include "esatz.h"
#include <QDebug>

DTAParser::DTAParser()
{
 qDebug ("Constructor DTAParser");
}



/*!
    \fn DTAParser::readDTA(const QString fileName)
        read DTA/DTAUS Files
        notice: reads only first line in files, couse DTA has only one line
        calls: processDTA(const QString _DTAString)
        return true = file was correct
        return false = file was corrupt

 */
bool DTAParser::readDTAFile ( const QString fileName )throw(ExDTAError)
{
#if DEBUG_READDTA
    qDebug ( "DTAParser::readDTA()" );
#endif

    QByteArray DTAString; //the whole file contents
    QFile dtaFile ( fileName );
    
    if ( dtaFile.open ( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        DTAString = dtaFile.readAll(); //8bit IBM 850 or ISO 8859 encoded text

        if(DTAString.indexOf("\n") >= 0)
         {
          emit errorMessagesReceived ( 
            tr( "Error: File contains linefeeds. This is not a DTA formatted file." ) + "\n"
          + tr("FileName:") + " " + fileName);
          return false;
         }
        if(DTAString.length() % 128 != 0)
         {
          emit errorMessagesReceived ( 
            tr( "Error: File length is not dividable by 128. This is either no DTA formatted file or it is damaged. " ) + "\n"
          + tr("FileName:") + " " + fileName);
          return false;
         }
    }
    else
    {
        emit errorMessagesReceived ( tr ( "Error - Can not open file for reading." ) );
        return false;
    }

#if DEBUG_READDTA
    qDebug() << "DTAString: %s"  << qPrintable(DTASection::fromDTA8Bit(DTAString));
#endif

   return processDTA ( DTAString );
}

/*!
    \fn DTAParser::processDTA(const QString DTAString)
        filter DTA-File and create Transfers
        return true = transfers created
        return false = creating transfers failed

 */
bool DTAParser::processDTA ( const QByteArray DTAString )throw(ExDTAError)
{
 int pos =0;
 Asatz aSatz;
 pos = aSatz.loadFromString(DTAString.left(128));
 
 Csatz cSatz;
 QVector <Csatz> cSegments;
 int charsProcessed = 0;
 do{
  charsProcessed = cSatz.loadFromString(DTAString.mid(pos, -1)); //take the whole rest of the string
  pos += charsProcessed;
  if(charsProcessed > 0)
    cSegments.append(cSatz); 
 }while (charsProcessed > 0);
 
 banking::TransferList transferList;
 for(int i = 0; i < cSegments.size(); i++)
     transferList.push_back(cSegments[i].createTransfer(aSatz.getJobType(), aSatz.getExecutionDate()));

 Esatz eSatz;
 eSatz.loadFromString(DTAString.mid(pos, -1));
 
  bool result = eSatz.checkChecksums(transferList);

  if (result)
  {
    banking::TransferList::iterator it = transferList.begin();
    while (it != transferList.end())
    {
      emit transferCreated((*it));
      it++;
    }
  }
  else emit errorMessagesReceived(tr("WARNING: Checksums do not match! The file is either damaged or manipulated!"));
  return result;
}
