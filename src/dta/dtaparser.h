/***************************************************************************
                          dtaparser.h  -  description
                             -------------------
    begin                : Mi Jun 3 2009
    copyright            : (C) 2011 Walter Roth,
  ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef DTAPARSER_H
#define DTAPARSER_H

//true = DEBUG Info is sent to stderr
#define DEBUG_READDTA true

#include <QFile>
#include <QTextStream>
#include <QVector>

#include "kernel/helpers/transfer.h"
#include "dtasection.h"

class DTAParser : public QObject
{
    Q_OBJECT

    public:
        DTAParser();

        /**
        * read DTA/DTAUS Files
        * @param fileName: the file to be read
        * return true = file was correct
        * return false = file was corrupt
        *
        */
        bool readDTAFile ( const QString fileName )throw(ExDTAError);

    private:
 
        /**
        * filter DTA-File and create Transfers
        * @param DTAfile: the file which is to read
        * return true = transfers created
        * return false = creating transfers failed
        *
        */
        bool processDTA ( const QByteArray DTAstring )throw(ExDTAError);

    signals:
        /**
        * Sends Error Messages
        */
        void errorMessagesReceived ( const QString & messages );
        /**
        * Sends Transfers
        */
        void transferCreated(banking::Transfer *transfer);
};

#endif // DTAPARSER_H
