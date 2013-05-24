#ifndef CSATZ_H
#define CSATZ_H

#include <QObject>
#include <QVector>
#include "transferitem.h"
#include "dtasection.h"

#include "kernel/helpers/transfer.h"
#include "bankingjobitem.h"
#include "dtasatz.h"

class Csatz: public DTASatz
{

public:
    /* Der Construktor erzeugt den Csatz*/
    Csatz(const banking::Transfer * transfer, QString ourName);
    /* Constructor for parsing a cSatz-string with loadFromString().
     */
    Csatz():DTASatz(){};

    /** Baut den gesamten C-Datensatz*/
    QByteArray bauCSatzzusammen();

   /** Loads data from a QString
     * Throws ExDTAError on format errors.
     * Returns length of CSatz.
     */
   int loadFromString(const QByteArray & s) throw (ExDTAError);

   /**
     * Creates a transfer form data in CSatz.
     */
   banking::Transfer * createTransfer(banking::BankingJob::JobType type, const DateTime & executionDate);
  /**
    * For debugging
    */
   void debugOutput();

private:
    /** Convert purpose lines to QStrings and store in purposeLines
      */
    void setPurposeLines(const banking::Transfer * transfer);
    /**
      * Distribute text according to type.
      */
    void parseExtension(DTASection * section)throw (ExDTAError);

    /**Builds all extensions including extension prefix code.
      * returns number of extensions.
      */
    int buildExtensions();

    /**Build a 128Byte section with a maximum of 4 extensions (Erweiterungsteile)
     */
    DTASection buildSection(int startIndex);

    /* Beinhaltet die BLZ vom 1. Bank*/
    QString _ErstBeteiligtesKreditinstitut;

    /* Die Bankleitzahl des Empfänger  */
    QString _BLZEmpfaenger;

    /* Kontonummer des Empfängers*/
    QString _KontonummerEmpfaenger;

    /*Interne Kundennummer (Bank internes Feld)*/
    QString _InterneKundennummer;

    /* Der Textschlüssel kennzeichnet die Zahlungsart*/
    QString _Textschluessel;

    /*Textschlüsselergänzung*/
    QString _TextschluesselErgaenzung;

    /* String der nur aus Leerzeichen besteht*/
    QString _Leerzeichenpuffer;

    /* String mit Nullen gefüllt*/
    QString _BetragDM;

    /*BLZ des Auftraggbers*/
    QString _BLZAuftraggeber;

    /* Kontonummer vom Auftraggeber*/
    QString _KontonummerAuftraggeber;

    /*Value in cent
     */
    int _Betrag;
    QString _NameEmpfaenger1; //27 chars
    QString _NameEmpfaenger2; //27 chars extension 01
    QString _NameAuftraggeber1;
    QString _NameAuftraggeber2; //27 chars extension 0
    /** Currency: 1 = Euro
      */
    QString _Waehrungskennzeichen;
    /** Number of extensions (Anzahl Erweiterungsteile)
    */ 
    int nExtensions;

    /**Contains the purposelines.
     * The first line is written to section 1, the others are
     * added as extensions (Erweiterungsteile)
     */
    QVector <QString> purposeLines; 
    /**Contains the extensions including the leading two-char extension code.
     */
    QVector <QString> extensions;
    /**
      * Extensions for names (not used by Moneypenny)
      */
    QString _ArtErweiterungsTeil;
};

#endif // CSATZ_H
