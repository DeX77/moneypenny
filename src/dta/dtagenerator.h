//
// C++ Interface: dtagenerator
//
// Description: 
//
//
// Author: Walter Roth, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DTAGENERATOR_H
#define DTAGENERATOR_H

#include <QString>
#include "kernel/helpers/bankingjob.h"

/**
Generates DTA-Files for transfers or debit notes.

	@author Walter Roth
*/
class DTAGenerator{
public:
    DTAGenerator();

    ~DTAGenerator();
    /**
      * Generates a DTA file for all jobs of JobType jobType in jobList.
      * jobType may be SingleTransfer or DebitNote.
      * Returns the file contents as an 8 bit QByteArray encoded according 
      * to DTASection global encoding setting.
      */
    static QByteArray generateDTAFile(banking::BankingJob::JobType jobType);
private:
    QString fileName;

};

#endif
