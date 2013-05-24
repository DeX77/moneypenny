//
// C++ Implementation: dtagenerator
//
// Description: 
//
//
// Author: walter <walter@HP2100-Roth>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dtagenerator.h"
#include "asatz.h"
#include "csatz.h"
#include "esatz.h"

#include "../crypt/bignum.h"

#include "kernel/helpers/transfer.h"
#include "kernel/helpers/account.h"
#include "kernel/global/HBCI_exceptions.h"
#include "hobit.h"

DTAGenerator::DTAGenerator()
{
}


DTAGenerator::~DTAGenerator()
{
}

/**
  * Generates a DTA file for all jobs of JobType jobType in jobList.
  * jobType may be SingleTransfer or DebitNote.
  * Returns the file contents as an 8 bit QByteArray encoded according 
  * to DTASection global encoding setting.
  */
QByteArray DTAGenerator::generateDTAFile(banking::BankingJob::JobType jobType)
{
    //Final 8-bit encoded strings for A,C and E-Satz
    QByteArray stringA;
    QByteArray stringC;
    QByteArray stringE;

    QByteArray DtaFileContents;

    //Checksums for ESatz. BigNums required because sums may be greater than maxint.
    BigNum sumOfBankCodes;
    BigNum sumOfAccountNumbers;
    BigNum sumOfValues;
    int numberOfTransfers = 0;

    const banking::Transfer * transfer;
    banking::Account * account= HoBIT::hobit()->currentAccount();
    if(!account)
      throw ExDTAError("DTAGenerator::generateDTAFile: No account available or set.");
    QString ourName = account->ownerName().c_str();
    QString ourAccountId = HoBIT::hobit()->currentAccountId();
    QString ourBankCode = HoBIT::hobit()->currentBankCode();
    /** Calculate checksums and build cSatz*/
    int row = 0;
     while((transfer = HoBIT::hobit()->pendingTransfer(row)) != NULL)
    {
        if(transfer->type() == jobType)
        {
            sumOfAccountNumbers += transfer->otherAccountId();
            sumOfBankCodes += transfer->otherBankCode();
            sumOfValues += transfer->value().centValue();
            numberOfTransfers++;

            // build CSatz
            Csatz csatz(transfer, ourName);
            stringC.append(csatz.bauCSatzzusammen());
         }
        row++;
    }

     //Build aSatz. There must be exactly one ASatz
     Asatz aSatz(ourName, ourAccountId, ourBankCode, jobType);
     stringA = aSatz.aSatzString();

    //Build eSatz
     Esatz esatz(sumOfAccountNumbers.stringValue(),
                 sumOfBankCodes.stringValue(), 
                 sumOfValues.stringValue(),
                 QString::number(numberOfTransfers));
     stringE = esatz.bauESatzzusammen();

     DtaFileContents = stringA + stringC + stringE;

    return DtaFileContents;
}
