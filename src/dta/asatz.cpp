#include "asatz.h"
#include "dateitem.h"

#include "kernel/global/HBCI_exceptions.h"

#include <QDate>

/** Der Constructor des A-Datensatzes initalisiert
    alle zugehoerigen Attribute */

Asatz::Asatz(QString ourName, QString ourAccountId, QString ourBankCode, banking::BankingJob::JobType type)
{
    _Satzlaenge = "0128";
    _Satzart = "A";

    /** Hier wird geprüft ob der TransferTyp eine überweisung ist oder eine Lastschrift*/
    switch(type)
    {
       case banking::BankingJob::SingleTransfer:
        {
            _Kennzeichen = "GK";
            break;
        }
    
        case banking::BankingJob::DebitNote:
        {
            _Kennzeichen ="LK";
            break;
        }
        default: _Kennzeichen = ""; //unknown
    }

    _BLZAuftraggeber = ourBankCode;// 8, numeric
    _NameAuftraggeber = ourName;//27, alpha
    _Dateierstellungsdatum = QDate::currentDate().toString("ddMMyy"); //6, numeric
    _BankinternesFeld = "";// 4, alpha
    _KontonummerAuftraggeber = ourAccountId;//10, numeric
    _Referenznummer = "0000000000";//10, numeric
    _Ausfuehrungsdatum = QDate::currentDate().toString("ddMMyyyy");
    _Waehrungskennzeichen = "1";

}

/** Default constructor. Use loadFromString for data loading.
  */
Asatz::Asatz()
{
}

/** Returns an ASatz as 128 Bytes QByteArray
  */
QByteArray Asatz::aSatzString()
{
    if(_Kennzeichen == "")
      throw ExDTAError("Asatz::bauASatzzusammen: Invalid transfer type.");
    section.addField( _Satzlaenge );
    section.addField( _Satzart );
    section.addField( _Kennzeichen );
    section.addField( _BLZAuftraggeber, 8, DTASection::PaddingNumeric);
    section.addBlankString( 8 );
    section.addField( _NameAuftraggeber, 27, DTASection::PaddingAlpha);
    section.addField( _Dateierstellungsdatum );
    section.addBlankString(4);
    section.addField( _KontonummerAuftraggeber, 10, DTASection::PaddingNumeric);
    section.addField( _Referenznummer, 10, DTASection::PaddingNumeric);
    section.addBlankString(15);
    section.addField( _Ausfuehrungsdatum );
    section.addBlankString(24);
    section.addField( _Waehrungskennzeichen);
    section.debugOutput("ASatz: ");
    return section.getSectionString();
}

/**Loads data from an 128Bytes QString
  * Throws ExDTAError on format errors.
  * Returns length of ASatz.
  */
int Asatz::loadFromString(const QByteArray & s)throw(ExDTAError)
{
    int length;
    section = s;
    _Satzlaenge = section.satzLength(&length);
    if(length != 128)
     throw(ExDTAError(
       ("Asatz::loadFromString: ASatz Format Error, length in first 4 bytes is not 128.\n" + section).data()));
    _Satzart = section.nextField(1);
    _Kennzeichen = section.nextField(2);
    if(_Kennzeichen == "GK")
       jobType = banking::BankingJob::SingleTransfer;
    else if(_Kennzeichen == "LK")
       jobType = banking::BankingJob::DebitNote;
    else 
      throw ExDTAError("Asatz::loadFromString: Invalid transfer type.");
    _BLZAuftraggeber= section.nextNumericField( 8 );
    section.nextField( 8 ); //May be zeros or blanks!
    _NameAuftraggeber = section.nextField( 27 );
    _Dateierstellungsdatum = section.nextNumericField( 6 );
    section.nextBlankField(4);
    _KontonummerAuftraggeber = section.nextNumericField( 10 );
    _Referenznummer = section.nextNumericField( 10 );
    section.nextBlankField(15);
    _Ausfuehrungsdatum = section.nextField( 8 ); //may be date or blanks!
    section.nextBlankField( 24 );
    _Waehrungskennzeichen = section.nextNumericField( 1 );
    section.debugOutput("ASatz: ");
    debugOutput();
    return length;
}

/**
  * Returns _Ausfuehrungsdatum or current date, if _Ausfuehrungsdatum is empty
  */
DateTime Asatz::getExecutionDate()
{
  bool ok = true;
  int y, m, d;
  QDate cd = QDate::currentDate();

   d = _Ausfuehrungsdatum.left(2).toInt(&ok);
  if(ok)
    m = _Ausfuehrungsdatum.mid(2,2).toInt(&ok);
  if(ok)
    y = _Ausfuehrungsdatum.right(4).toInt(&ok);
  if(ok)
   return DateTime(y, m, d);
  else return DateTime(cd.year(), cd.month(), cd.day());
}

/**
  * For debugging
  */
 void Asatz::debugOutput()
{
    if(! DEBUG_DTA )
     return;
    qDebug("Data of ASatz");
    qDebug(" _Satzlaenge: %s", qPrintable(_Satzlaenge));
    qDebug(" _Satzart: %s", qPrintable(_Satzart));
    qDebug(" _Kennzeichen: %s", qPrintable(_Kennzeichen));
    qDebug(" _BLZAuftraggeber: %s", qPrintable(_BLZAuftraggeber));
    qDebug(" _NameAuftraggeber: %s", qPrintable(_NameAuftraggeber));
    qDebug(" _Dateierstellungsdatum: %s", qPrintable(_Dateierstellungsdatum));
    qDebug(" _BankinternesFeld: %s", qPrintable(_BankinternesFeld));
    qDebug(" _KontonummerAuftraggeber: %s", qPrintable(_KontonummerAuftraggeber));
    qDebug(" _Referenznummer: %s", qPrintable(_Referenznummer));
    qDebug(" _Ausfuehrungsdatum: %s", qPrintable(_Ausfuehrungsdatum));
    qDebug(" _Waehrungskennzeichen: %s", qPrintable(_Waehrungskennzeichen));
    qDebug("\n");

}
