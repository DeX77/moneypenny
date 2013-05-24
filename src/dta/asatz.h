#ifndef ASATZ_H
#define ASATZ_H

#include <QObject>
#include "dtasection.h"
#include "kernel/helpers/transfer.h"
#include "kernel/helpers/datetime.h"
#include "dtasatz.h"

class Asatz: public DTASatz
{

public:
    /** Sets data from parameters
      */
    Asatz(QString ourName, QString ourAccount, QString ourBankCode, banking::BankingJob::JobType type);
    /** Load data later from an aSatzString
      */
    Asatz();

    /** Returns an ASatz as 128 Bytes QString
      */
    QByteArray aSatzString();

   /**Loads data from an 128Bytes QString
     * Throws ExDTAError on format errors.
     * Returns length of ASatz. (Must be 128)
     */
   int loadFromString(const QByteArray & s) throw (ExDTAError);
   /** 
     * Returns jobType for this DTA file.
     */ 
    banking::BankingJob::JobType getJobType(){return jobType;};
   /**
     * Returns _Ausfuehrungsdatum or current date, if _Ausfuehrungsdatum is empty
     */
    DateTime getExecutionDate();
  /**
    * For debugging
    */
   void debugOutput();
private:

    /** Die Kennzeichnung beinhaltet  z.B. GK oder LK*/
    QString _Kennzeichen;

    /** Die Bankleitzahl des Auftraggeber  */
    QString _BLZAuftraggeber;

    /** Name des Auftraggebers*/
    QString _NameAuftraggeber;

    /** Erstellungsdatum der Transaction*/
    QString _Dateierstellungsdatum;

    /* */
    QString _BankinternesFeld;

    /** Kontonummer des Auftraggeber*/
    QString _KontonummerAuftraggeber;

    QString _Referenznummer;

    QString _Ausfuehrungsdatum;

    /** Währungszeichen Bsp: 1 = Euro*/
    QString _Waehrungskennzeichen;

    /** The JobType for this file
     */
    banking::BankingJob::JobType jobType;
    /** The complete ASatz is in one section.
     */
    DTASection section;
};
#endif // ASATZ_H
