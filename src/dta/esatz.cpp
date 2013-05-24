#include "esatz.h"
#include "bignum.h"
#include "kernel/helpers/transfer.h"

/** Der Constructor des E-Datensatzes initalisiert
    alle zugehoerigen Attribute */

Esatz::Esatz(QString SummeKontonummer, QString SummeBLZ, QString SummeBetrag, QString Anzahl)
{
    _ESatzlaenge ="0128";
    _Satzart = "E";
    _AnzahlCsatz = Anzahl;// 7, numeric
    _SummeDM = _SummeDM;// 13, numeric
    _SummeKontonummerEmpfaenger = SummeKontonummer;//17, numeric
    _SummeBLZEmpfaenger = SummeBLZ;//17, numeric
    _SummeEuroBetraege = SummeBetrag; //13, numeric
 }

/** Alle Attribute eines E-Datensatzen werden in einen String zusammengesetzt*/

QByteArray Esatz::bauESatzzusammen()
{
   section = DTASection();
   section.addField(_ESatzlaenge);
   section.addField(_Satzart);
   section.addBlankString(5);
   section.addField(_AnzahlCsatz, 7, DTASection::PaddingNumeric);
   section.addField(_SummeDM, 13, DTASection::PaddingNumeric);
   section.addField(_SummeKontonummerEmpfaenger, 17, DTASection::PaddingNumeric);
   section.addField(_SummeBLZEmpfaenger, 17, DTASection::PaddingNumeric);
   section.addField(_SummeEuroBetraege, 13, DTASection::PaddingNumeric);
   return section.getSectionString();
}

/**Loads data from an 128Bytes QString
  * Throws ExDTAError on format errors.
  * Returns length of ESatz. (Must be 128)
  */
int Esatz::loadFromString(const QByteArray & s) throw (ExDTAError)
{
    int length;
    section = s;
    _ESatzlaenge = section.satzLength(&length);
    if(length != 128)
     throw(ExDTAError(("Esatz::loadFromString: ESatz Format Error, length in first 4 bytes is not 128.\n" + section).data()));
    _Satzart = section.nextField(1);
    if(_Satzart != "E")
     throw(ExDTAError(("Esatz::loadFromString: ESatz Format Error, Satzart is not E.\n" + section).data()));
   section.nextBlankField(5);
   _AnzahlCsatz = section.nextNumericField( 7 );
   _SummeDM = section.nextNumericField( 13 );
   _SummeKontonummerEmpfaenger = section.nextNumericField( 17 );
   _SummeBLZEmpfaenger = section.nextNumericField( 17 );
   _SummeEuroBetraege = section.nextNumericField( 13 );
   return length;
}

/**
  * Check checksums of transfers in list versus Strings in Esatz.
  */
bool Esatz::checkChecksums(banking::TransferList list)
{
    //Checksums for ESatz. BigNums required because sums may be greater than maxint.
    BigNum sumOfBankCodes;
    BigNum sumOfAccountNumbers;
    BigNum sumOfValues;
    int numberOfTransfers = 0;

    //Add numbers
    banking::TransferList::const_iterator it = list.begin();
     while(it != list.end())
    {
        banking::Transfer * transfer = (*it);
        sumOfAccountNumbers += transfer->otherAccountId();
        sumOfBankCodes += transfer->otherBankCode();
        sumOfValues += transfer->value().centValue();
        numberOfTransfers++;
        it++;
    }

   QString msg;
   bool numberOfTransfersOk = _AnzahlCsatz.toInt() == numberOfTransfers;
   if(numberOfTransfersOk)
    msg += "numberOfTransfers: Ok\n";
   else msg += "numberOfTransfers: ERROR!\n";
   bool sumOfAccountNumbersOk = sumOfAccountNumbers == BigNum(_SummeKontonummerEmpfaenger);
   if(sumOfAccountNumbersOk)
    msg += "sumOfAccountNumbers: Ok\n";
   else msg += "sumOfAccountNumbers: ERROR!\n";
   bool sumOfBankCodesOk = sumOfBankCodes == BigNum(_SummeBLZEmpfaenger);
   if(sumOfBankCodesOk)
    msg += "sumOfBankCodes: Ok\n";
   else msg += "sumOfBankCodes: ERROR!\n";
   bool sumOfValuesOk = sumOfValues == _SummeEuroBetraege;
   if(sumOfValuesOk)
    msg += "sumOfValues: Ok\n";
   else msg += "sumOfValues: ERROR!\n";

   qDebug("Esatz::checkChecksums: Status: \n%s", qPrintable(msg));

   bool result = numberOfTransfersOk
              && sumOfAccountNumbersOk
              && sumOfBankCodesOk
              && sumOfValuesOk;
   if (!result)
   {
     debugOutput();
     qDebug("numberOfTransfers: %i", numberOfTransfers);
     qDebug("sumOfAccountNumbers: %s", qPrintable(sumOfAccountNumbers.stringValue()));
     qDebug("sumOfBankCodes: %s", qPrintable(sumOfBankCodes.stringValue()));
     qDebug("sumOfValues: %s", qPrintable(sumOfValues.stringValue()));
   }
   return result;
}

/**
  * For debugging
  */
void Esatz::debugOutput()
{
    if(! DEBUG_DTA )
     return;
    qDebug("Data of ESatz");
    qDebug(" _ESatzlaenge: %s", qPrintable(_ESatzlaenge));
    qDebug(" _Satzart: %s", qPrintable(_Satzart));
    qDebug(" _AnzahlCsatz: %s", qPrintable(_AnzahlCsatz));
    qDebug(" _SummeDM: %s", qPrintable(_SummeDM));
    qDebug(" _SummeEuroBetraege: %s", qPrintable(_SummeEuroBetraege));
    qDebug(" _SummeKontonummerEmpfaenger: %s", qPrintable(_SummeKontonummerEmpfaenger));
    qDebug(" _SummeBLZEmpfaenger: %s", qPrintable(_SummeBLZEmpfaenger));
    qDebug("\n");
}


