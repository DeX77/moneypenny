#include "csatz.h"
#include "dtasection.h"

#include "../banking/bankingjobitem.h" // needed for to8bit()

/** Der Constructor des C-Datensatzes initalisiert
    alle zugehoerigen Attribute */

Csatz::Csatz(const banking::Transfer * transfer, QString ourName)
{

        _Satzart = "C";
        _ErstBeteiligtesKreditinstitut = transfer->ourBankCode().c_str(); //8, numeric, data are optional 
        _BLZEmpfaenger = transfer->otherBankCode().c_str(); //8, numeric
        _KontonummerEmpfaenger = transfer->otherAccountId().c_str(); // 10, "numerisch"

        _InterneKundennummer = "";// 13, numeric
        
        switch(transfer->type())
        {
           case banking::BankingJob::SingleTransfer:
             {
                 _Textschluessel = "51";
                 _TextschluesselErgaenzung= "000";
             } break;
           case banking::BankingJob::DebitNote:
             {
                 _Textschluessel ="04";
                 _TextschluesselErgaenzung= "000";
             }break;
           default: //no exception in constructor
            {
                 _Textschluessel = "00";
                 _TextschluesselErgaenzung= "000";
             } break;
        }

        _BetragDM = ""; // 11, numeric
        _BLZAuftraggeber = transfer->ourBankCode().c_str();// 8, numeric
        _KontonummerAuftraggeber =  transfer->ourAccountId().c_str(); // 10, numeric
        _Betrag=  transfer->value().centValue();
        _NameEmpfaenger1 = transfer->otherName1().substr(0,27).c_str();// 27, alpha
        _NameEmpfaenger2 = transfer->otherName2().substr(0,27).c_str();// 27, alpha
        _NameAuftraggeber1 =  ourName.left(27);// 27, alpha
        _NameAuftraggeber2 =  ourName.mid(27, 27);// 27, alpha
        _Waehrungskennzeichen = "1";
        _ArtErweiterungsTeil ="02";
        setPurposeLines(transfer);
}

/** Alle Attribute eines C-Datensatzen werden in einen String zusammengesetzt*/

QByteArray Csatz::bauCSatzzusammen()
{
    QByteArray cSatzString; //totallength is 256 + n * 128
    QString cSatzLength; //4 digits
    DTASection section1; //stored at sections[0]! 4 digits SatzLength + 124chars data gives a total length is 128
    DTASection section2; //total length is 128
    //Sections 3 to 6 may be empty
    QVector<DTASection> sections;

    buildExtensions();
    QString anzahlErweiterungsteile = QString("%1").arg(nExtensions , 2, 10, QChar('0')); //two digits required, add leading zero for 0 to 9

    // leave the 4 chars Satzlaenge for later, add everything for constant sections 1 and 2 of cSatz.
    section1.addField( _Satzart); 
    section1.addField( _ErstBeteiligtesKreditinstitut, 8, DTASection::PaddingNumeric);
    section1.addField( _BLZEmpfaenger, 8, DTASection::PaddingNumeric); 
    section1.addField( _KontonummerEmpfaenger, 10, DTASection::PaddingNumeric);
    section1.addField( _InterneKundennummer, 13, DTASection::PaddingNumeric);
    section1.addField( _Textschluessel); 
    section1.addField( _TextschluesselErgaenzung);
    section1.addField( " " ); 
    section1.addField( _BetragDM, 11, DTASection::PaddingNumeric);  //should be zero
    section1.addField( _BLZAuftraggeber , 8, DTASection::PaddingNumeric); 
    section1.addField( _KontonummerAuftraggeber, 10, DTASection::PaddingNumeric);
    section1.addField( QString::number(_Betrag), 11, DTASection::PaddingNumeric);
    section1.addBlankString(3); 
    section1.addField( _NameEmpfaenger1, 27, DTASection::PaddingAlpha);
    section1.addBlankString(8);
    sections.append(section1);
    section1.debugOutput("S1:");

    section2.addField( _NameAuftraggeber1, 27, DTASection::PaddingAlpha);
    section2.addField( purposeLines[0], 27, DTASection::PaddingAlpha);  //first line of purpose
    section2.addField( _Waehrungskennzeichen, 1, DTASection::PaddingNumeric); 
    section2.addBlankString(2);
    section2.addField( anzahlErweiterungsteile, 2, DTASection::PaddingNumeric);

   //Add extensions as sections of 128 byte length. The first 2 extensions fit into the 
   //second section within the 256 byte mininmum cSatz-length.
   //Add 128 Byte sections from the 3rd extension.
   int nExtension = 0;
   //the first 2 extensions go to second section 
   while ((nExtension < nExtensions) && (nExtension < 2))
   {
     section2.addField( extensions[nExtension], 29, DTASection::PaddingAlpha); //these are 29 chars 
     nExtension ++;
   }
   sections.append(section2);
   section2.debugOutput("S2:");
  
   //Extensions 3 to 15 go to section 3 to 6 (indices 2 to 5) as blocks of 4 extensions 
   while(nExtension <= nExtensions)
   {
     sections.append(buildSection(nExtension));
     nExtension += 4;
   }
 
   //calculate total length
   int length = 128 * sections.size(); //128 byte per section
   //make a 4 digit string and prepend it to section 1
   QString satzLaenge = QString("%1").arg(length, 4, 10, QChar(' '));
   sections[0].insert(0, satzLaenge);
 
   //finally add it all
   for(int i = 0; i < sections.size(); i++)
    cSatzString += sections[i].getSectionString(); 
   return cSatzString;
}

/**Builds all extensions including extension prefix code.
  * returns number of extensions.
  */
int Csatz::buildExtensions()
{
     nExtensions = 0;
     //The first purpose line is already in section 1
     for(int line = 1; line < purposeLines.size(); line++ )
     {
       extensions.append("02" + purposeLines[line]); 
       nExtensions ++;
     }

     //add name extensions last
     if(_NameEmpfaenger2.length() > 0) 
     {
         extensions.append( "01" + _NameEmpfaenger2);
         nExtensions ++;
     }
     if(_NameAuftraggeber2.length() > 0)
     {
         extensions.append( "03" + _NameAuftraggeber2);
         nExtensions ++;
     }
     return nExtensions;
}

/**Build a 128Byte section with a maximum of 4 extensions (Erweiterungsteile)
 */
DTASection Csatz::buildSection(int startIndex)
{
  DTASection section;
  int nExtension = startIndex;
  while((nExtension <= nExtensions) && (nExtension < startIndex + 4))
  {
    section.addField( extensions[nExtension], 29, DTASection::PaddingAlpha); 
    nExtension ++;
  }
  section.debugOutput("S:");
  return section;
}

/** In der Methode setPurposeLines() werden alle Erweiterungsteile erzeugt
    und auf eine bestimmte Laenge normiert */

void Csatz::setPurposeLines(const banking::Transfer * transfer)
{
    int nLines = 0;

    /** Add 27char wide lines to purposeLines27 for use in Erweiterungsteil.
     */
    list<string>::const_iterator it = transfer->purposeLines().begin();
    while(it != transfer->purposeLines().end())
    {
        purposeLines.append((*it).c_str());
        it++;
        nLines++;
    }
    nExtensions = nLines - 1;
    if(transfer->otherName2().length() > 0)
      nExtensions++;
}

/** Loads data from a QString
  * Throws ExDTAError on format errors.
  * Returns number of processed chars. 
  ***** This may be != cSatzLength ********
  */
int Csatz::loadFromString(const QByteArray & s) throw (ExDTAError)
{
    int cSatzLength = 0; //245 to 622 Length without trailing blanks!
    length = 0; //total amount of bytes in Csatz is 128 bytes per segment.

    //Clear all fields that may not be defined in s
    purposeLines.clear();
    _NameEmpfaenger2 = "";
    _NameAuftraggeber2 = "";

    DTASection section1 = s.mid(0,128);//  4 digits SatzLength + 124chars data gives a total length is 128
    length += 128;
    if (DEBUG_DTA)
      section1.debugOutput("CSatz, Section1: ");
    DTASection section2  = s.mid(128, 128);; //starts at 128, total length is 128
    length += 128;
    if (DEBUG_DTA)
      section2.debugOutput("CSatz, Section2: ");

    //Parse section 1
    section1.satzLength(&cSatzLength);
    _Satzart = section1.nextField( 1 );
    if(_Satzart != "C") //May be "E"
       return 0;
    _ErstBeteiligtesKreditinstitut = section1.nextNumericField( 8 );
    _BLZEmpfaenger = section1.nextNumericField( 8 ); 
    _KontonummerEmpfaenger = section1.nextNumericField( 10 );
    _InterneKundennummer = section1.nextNumericField( 13 );
    _Textschluessel = section1.nextNumericField( 2 ); 
    _TextschluesselErgaenzung = section1.nextNumericField( 3 ); 
    section1.nextBlankField( 1 );
    _BetragDM = section1.nextNumericField( 11 );  //should be zero
    _BLZAuftraggeber = section1.nextNumericField( 8 ); 
    _KontonummerAuftraggeber = section1.nextNumericField( 10 );
    _Betrag = section1.nextNumericField( 11 ).toInt();
    section1.nextBlankField(3); 
    _NameEmpfaenger1 = section1.nextField( 27 ).trimmed();
    section1.nextBlankField(8);
    
    //Parse Section 2
    section2.debugOutput("CSatz, Section2: ");
    section2.goToFirstField();
    _NameAuftraggeber1 = section2.nextField( 27 ).trimmed();
    purposeLines.append(section2.nextField( 27 ).trimmed());  //first line of purpose
    _Waehrungskennzeichen = section2.nextNumericField( 1 ); 
    section2.nextBlankField(2);
   
   //Parse extensions (Erweiterungsteile)
   QString anzahlErweiterungsteile = section2.nextNumericField( 2 ); 
   nExtensions = anzahlErweiterungsteile.toInt();
   int nExtension = 1;
   //the first 2 extensions are in second section 
   while ((nExtension <= nExtensions) && (nExtension < 3))
   {
      parseExtension(&section2);
      nExtension ++;
   }
   
   if(nExtensions > 2)
   {
     //Extensions 3 to 15 are in section 3 to 6 (indices 2 to 5) as blocks of 4 extensions 
     DTASection section = DTASection(s.mid(256, 128));
     length += 128;
     section.goToFirstField();
     if (DEBUG_DTA)
       section2.debugOutput("CSatz, ExtensionSection: ");

     while(nExtension <= nExtensions)
     {
       if((nExtension - 2) %4)
       {
        section = DTASection(s.mid(256 + 128 * ((nExtension -2) / 4), 128));
        section.goToFirstField();
       }
       parseExtension(&section);
       nExtension ++;
     }
   }

   if( DEBUG_DTA)
     debugOutput();

   return length;
 }

void Csatz::parseExtension(DTASection * section)throw (ExDTAError)
{
   QString type = section->nextNumericField( 2 );
   QString text = section->nextField(27).trimmed(); //these are 27 chars 

   if(type == "01")
     _NameEmpfaenger2 = text;
   else if(type == "02")
     purposeLines.append(text);
   else if(type =="03")
     _NameAuftraggeber2 = text;
   else throw ExDTAError("Csatz::parseExtension: Unknown extension type.");
}


/**
  * Creates a transfer form data in CSatz. Transfer is ISO-8859-15 encoded.
  */
banking::Transfer * Csatz::createTransfer(banking::BankingJob::JobType type, const DateTime & executionDate)
{ 
  list <string> purposeList;
  for (int i = 0; i < purposeLines.size(); i++)
    purposeList.push_back(qPrintable(purposeLines[i]));

  banking::Transfer * transfer = new banking::Transfer(type, 
                               banking::Value((double)_Betrag / 100.0),
                               qPrintable(_KontonummerAuftraggeber),
                               "", //IBAN
                               qPrintable(_BLZAuftraggeber),
                               "", //BIC
                               BANKTYPE_SPARKASSE,
                               qPrintable(_KontonummerEmpfaenger),
                               "", //other IBAN
                               qPrintable(_BLZEmpfaenger),
                               "", //other BIC
                               banking::to8Bit(_NameEmpfaenger1),
                               banking::to8Bit(_NameEmpfaenger2),
                               purposeList,
                               "", //custRef
                               "EUR",
                               "280",
                               "280"
                                );
   transfer->setFirstExcecutionDate(executionDate);
   return transfer;
}

/**
  * For debugging
  */
 void Csatz::debugOutput()
{
    if(! DEBUG_DTA )
     return;
    qDebug("Data of CSatz");
    qDebug("  _Satzart: %s", qPrintable( _Satzart  ));
    qDebug("  _ErstBeteiligtesKreditinstitut: %s", qPrintable( _ErstBeteiligtesKreditinstitut  ));
    qDebug("  _BLZEmpfaenger: %s", qPrintable( _BLZEmpfaenger  ));
    qDebug("  _KontonummerEmpfaenger: %s", qPrintable( _KontonummerEmpfaenger  ));
    qDebug("  _NameEmpfaenger1: %s", qPrintable( _NameEmpfaenger1  ));
    qDebug("  _NameEmpfaenger2: %s", qPrintable( _NameEmpfaenger2  ));
    qDebug("  _InterneKundennummer: %s", qPrintable( _InterneKundennummer  ));
    qDebug("  _Textschluessel: %s", qPrintable( _Textschluessel  ));
    qDebug("  _TextschluesselErgaenzung: %s", qPrintable( _TextschluesselErgaenzung  ));
    qDebug("  _Leerzeichenpuffer: %s", qPrintable( _Leerzeichenpuffer  ));
    qDebug("  _BetragDM: %s", qPrintable( _BetragDM  ));
    qDebug("  _BLZAuftraggeber: %s", qPrintable( _BLZAuftraggeber  ));
    qDebug("  _KontonummerAuftraggeber: %s", qPrintable( _KontonummerAuftraggeber  ));
    qDebug("  _NameAuftraggeber1: %s", qPrintable( _NameAuftraggeber1  ));
    qDebug("  _NameAuftraggeber2: %s", qPrintable( _NameAuftraggeber2  ));
    qDebug("  _Betrag: %i", _Betrag);

    for(int line = 0; line < purposeLines.size(); line ++)
       qDebug("%s", qPrintable( " purposeLine" + QString::number(line) + ": " + purposeLines[line]));

    qDebug("\n");
}
