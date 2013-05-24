#ifndef ESATZ_H
#define ESATZ_H

#include <QObject>
#include "transferitem.h"
#include "dtasection.h"

class Esatz
{

public:
    /* Constructor for building an eSatz - string
     */
    Esatz(QString SummeKontonummer, QString SummeBLZ, QString SummeBetrag, QString Anzahl);
    
    /* Constructor for parsing an eSatz-string with loadFromString().
     */
    Esatz(){};

    QByteArray bauESatzzusammen();

   /** Loads data from an 128Bytes QString
     * Throws ExDTAError on format errors.
     * Returns length of ESatz. (Must be 128)
     */
   int loadFromString(const QByteArray & s) throw (ExDTAError);

   /**
     * Check checksums of transfers in list versus Strings in Esatz.
     */
    bool checkChecksums(banking::TransferList list);
  /**
    * For debugging
    */
   void debugOutput();

private:

    /* Diese Variable speichert die Satzlänge des Esatzes*/
    QString _ESatzlaenge;

    /* Hier wird die Satzart gespeichert Bsp: A, C oder E*/
    QString _Satzart;
    /* Beinhaltet die Anzahl der C-Sätze*/
    QString _AnzahlCsatz;

    /* Summe vom Ehemals DM-Beträgen*/
    QString _SummeDM;

    /* Summe von den Kontonummern der Empfänger*/
    QString _SummeKontonummerEmpfaenger;

    /* Summe von den BLZ der Empfänger*/
    QString _SummeBLZEmpfaenger;

    /*Summe von den Eurobeträgen*/
    QString _SummeEuroBetraege;

    DTASection section;

};

#endif // ESATZ_H
