######################################################################
# Hand made .pro file
# Use PREFIX=/opt for installation
######################################################################

TEMPLATE = app

TARGET = bin/moneypenny

VERSION = -3.0Beta

DISTFILES += help/*.html \
             translations/*.qm \
             internet/kppprc \
             internet/isdn \
             internet/ipppd.ippp0 \
             internet/ipppd.ippp0+1 \
             internet/device.ippp0 \
             internet/device.ippp0+1 \
             data/bankdata.txt \
             data/hbcibankdata.hbd \
             data/printerdata.xml \
             data/testpage.png \
             data/testpage.ps \
             data/readers.xml \
             data/HC_220.xml \
             data/HC_300.xml \
             data/driverdetails.xslt \
             data/certificates/postbank.pem \
             AUTHORS \
             README \
             INSTALL \
             *.kdevelop \
             configure \
             INSTALLATION \
             COPYING \
             LIESMICH \
             make_release \
             make_install \
             make_cd \
             make_debug \
             *.spec \
             start_moneypenny \
             images/*.* \


#Now set all the paths
topdir = /moneypenny

target.path = $${topdir}/bin 
target.files = bin/moneypenny2 \
               start_moneypenny


internetfiles.path = $${topdir}/internet
internetfiles.files = \
                      internet/kppprc \
                      internet/isdn \
                      internet/ipppd.ippp0 \
                      internet/ipppd.ippp0+1 \
                      internet/device.ippp0 \
                      internet/device.ippp0+1 \

helpfiles.path = $${topdir}/help
helpfiles.files = help/*.html

translationfiles.path = $${topdir}/translations
translationfiles.files = translations/*.qm

datafiles.path = $${topdir}/data
datafiles.files = data/bankdata.txt \
                  data/hbcibankdata.hbd \
                  data/printerdata.xml \
                  data/testpage.png \
                  data/testpage.ps \
                  data/readers.xml \
                  data/HC_220.xml \
                  data/HC_300.xml \
                  data/everythingHBCI.xml \
                  data/hbci_pintan.xml \
                  data/driverdetails.xslt \

certfiles.path = $${topdir}/data/certificates
certfiles.files =  data/certificates/postbank.pem \

imagefiles.path = $${topdir}/images
imagefiles.files = images/*.*

INSTALLS = target \
           helpfiles \
           internetfiles \
           translationfiles \
           datafiles \
           imagefiles \
           certfiles \

RESOURCES = moneypenny.qrc

DEPENDPATH += \
               src/banking \
               src/crypt \
               src/CVS \
               src/forms \
               src/global \
               src/gui \
               src/hobit \
               src/keyfiles \
               src/openhbci \
               src/printing \
               src/ts_files \
               src/kernel \
               src/dta \

INCLUDEPATH += $(QTDIR)/include/QtNetwork\
               $(QTDIR)/include/QtCore\
               $(QTDIR)/include/QtGui\
               $(QTDIR)/include\
               /usr/include/PCSC \
               src/banking \
               src/crypt \
               src/global \
               src/gui \
               src/hobit \
               src/keyfiles \
               src/openhbci \
               src/kernel \
               src/dta \


FORMS += \
               src/forms/dlgenterpinkeypadbase.ui \
               src/forms/dlgenterpinbase.ui \
               src/forms/dlgconfirmtransferbase.ui\
               src/forms/dlgstatusmessagebase.ui\
               src/forms/guimainbase.ui \
               src/forms/guifindmediumbase.ui \
               src/forms/guiloginpasswordbase.ui \
               src/forms/guisplashbase.ui \
               src/forms/guiwelcomebase.ui \
               src/forms/guitransfertemplatesbase.ui \
               src/forms/guijobsbase.ui \
               src/forms/guilogbase.ui \
               src/forms/guihbciwizardbase.ui \
               src/forms/guiloginbase.ui \
               src/forms/guiiniletterbase.ui \
               src/forms/guitoolsbase.ui \
               src/forms/guishowbankdatabase.ui \
               src/forms/guiaccountinformationbase.ui \
               src/forms/guitransactionlistbase.ui \
               src/forms/guitransactioninfobase.ui \
               src/forms/guigettransactionsbase.ui \
               src/forms/guitransferformbase.ui \
               src/forms/guinotimplementedbase.ui \
               src/forms/guidataexportbase.ui \
               src/forms/guitransactionprinterbase.ui \
               src/forms/guihelpwindowbase.ui \
               src/forms/guiknownbanksbase.ui \
               src/forms/guiprinterautosetupbase.ui \
               src/forms/guiprintermanualsetupbase.ui \
               src/forms/guiprinterdrivertoxmlbase.ui \
               src/forms/guihardwareconfigbase.ui \
               src/forms/guientropybase.ui \
               src/forms/guiaddaccountbase.ui \
               src/forms/guiimportbase.ui \
               src/forms/guiprogressindicatorbase.ui \
               src/forms/guidatedtransfersbase.ui \

TRANSLATIONS += \
               src/ts_files/moneypenny_de.ts \
               src/ts_files/moneypenny_ru.ts \
               src/ts_files/moneypenny_pl.ts \
               src/ts_files/moneypenny_tr.ts \


#global headers
HEADERS += \
               src/global/globals.h \
               src/banking/valueitem.h \
               src/banking/dateitem.h \
               src/banking/accountitem.h \
               src/banking/bankingjobitem.h \
               src/banking/transferitem.h \
               src/banking/transactionitem.h \
               src/banking/bankitem.h \
      \
               src/crypt/uchararray.h \
               src/crypt/cryptdata.h \
               src/crypt/crypt.h \
               src/crypt/crypttripledes.h \
               src/crypt/bignum.h \
      \
               src/hobit/hobit_crypt.h \
               src/hobit/hobit.h \
               src/hobit/hobit_creator.h \
               src/hobit/hobit_interactor.h \
               src/hobit/hobit_auth.h \
               src/hobit/hobit_string.h \
               src/hobit/hobit_validator.h \
               src/hobit/hobit_connector.h \
               src/hobit/hobit_sql.h \
               src/hobit/hobit_transfertemplate.h \
               src/hobit/hobit_tlv.h \
               src/hobit/jobmanager.h \
               src/hobit/databaseaccess.h \
      \
               src/openhbci/accountjobfactory.h \
               src/openhbci/adminjobfactory.h \
               src/openhbci/gwentransaction.h \
               src/openhbci/datedtransfer.h \
               src/openhbci/standingorder.h \
               src/openhbci/openhbcisupport.h \
      \
               src/gui/dlgenterpinkeypad.h \
               src/gui/dlgenterpin.h \
               src/gui/dlgconfirmtransfer.h \
               src/gui/dlgstatusmessage.h \
               src/gui/guimain.h \
               src/gui/guifindmedium.h \
               src/gui/guiloginpassword.h \
               src/gui/guisplash.h \
               src/gui/guiwelcome.h \
               src/gui/guitransfertemplates.h \
               src/gui/guiinteractor.h \
               src/gui/guilog.h \
               src/gui/guihbciwizard.h \
               src/gui/guilogin.h \
               src/gui/guiiniletter.h \
               src/gui/guitools.h \
               src/gui/guishowbankdata.h \
               src/gui/guiaccountinformation.h \
               src/gui/guitransactioninfo.h \
               src/gui/guitransactionlist.h \
               src/gui/guigettransactions.h \
               src/gui/guitransferform.h \
               src/gui/guidatedtransfers.h \
               src/gui/guidatedsingletransfer.h \
               src/gui/guinotimplemented.h \
               src/gui/guijobs.h \
               src/gui/guidataexport.h \
               src/gui/guitransactionprinter.h \
               src/gui/previewwidget.h \
               src/gui/guihelpwindow.h \
               src/gui/guiknownbanks.h \
               src/gui/guiprinterdrivertoxml.h \
               src/gui/guiprinterautosetup.h \
               src/gui/guiprintermanualsetup.h \
               src/gui/guihardwareconfig.h \
               src/gui/guientropy.h \
               src/gui/guiaddaccount.h \
               src/gui/guiimport.h \
               src/gui/guiprogressindicator.h \
      \
               src/keyfiles/keyrsa.h \
               src/keyfiles/keyfile.h \
               src/keyfiles/keyfileopenhbci.h \
      \
               src/printing/globalsprintersetup.h \
               src/printing/printersetupdb.h \
      \
               src/dta/dtaparser.h \
               src/dta/asatz.h \
               src/dta/csatz.h \
               src/dta/esatz.h \
               src/dta/dtagenerator.h \
               src/dta/dtasection.h \
               src/dta/exdtaerror.h \
               src/dta/dtasatz.h \

#global sources
SOURCES += \
               src/global/globals.cpp \
               src/main.cpp \
               src/banking/valueitem.cpp \
               src/banking/dateitem.cpp \
               src/banking/accountitem.cpp \
               src/banking/bankingjobitem.cpp \
               src/banking/transferitem.cpp \
               src/banking/transactionitem.cpp \
               src/banking/bankitem.cpp \
      \
               src/crypt/uchararray.cpp \
               src/crypt/cryptdata.cpp \
               src/crypt/crypt.cpp \
               src/crypt/crypttripledes.cpp \
               src/crypt/bignum.cpp \
      \
               src/hobit/hobit_crypt.cpp \
               src/hobit/hobit.cpp \
               src/hobit/hobit_creator.cpp \
               src/hobit/hobit_interactor.cpp \
               src/hobit/hobit_auth.cpp \
               src/hobit/hobit_string.cpp \
               src/hobit/hobit_validator.cpp \
               src/hobit/hobit_connector.cpp \
               src/hobit/hobit_sql.cpp \
               src/hobit/hobit_transfertemplate.cpp \
               src/hobit/hobit_tlv.cpp \
               src/hobit/jobmanager.cpp \
               src/hobit/databaseaccess.cpp \
      \
               src/openhbci/accountjobfactory.cpp \
               src/openhbci/adminjobfactory.cpp \
               src/openhbci/gwentransaction.cpp \
               src/openhbci/datedtransfer.cpp \
               src/openhbci/standingorder.cpp \
               src/openhbci/openhbcisupport.cpp \
      \
               src/gui/dlgenterpinkeypad.cpp \
               src/gui/dlgenterpin.cpp \
               src/gui/dlgconfirmtransfer.cpp \
               src/gui/dlgstatusmessage.cpp \
               src/gui/guimain.cpp \
               src/gui/guifindmedium.cpp \
               src/gui/guiloginpassword.cpp \
               src/gui/guisplash.cpp \
               src/gui/guiwelcome.cpp \
               src/gui/guitransfertemplates.cpp \
               src/gui/guiinteractor.cpp \
               src/gui/guilog.cpp \
               src/gui/guihbciwizard.cpp \
               src/gui/guilogin.cpp \
               src/gui/guiiniletter.cpp \
               src/gui/guitools.cpp \
               src/gui/guishowbankdata.cpp \
               src/gui/guiaccountinformation.cpp \
               src/gui/guitransactioninfo.cpp \
               src/gui/guitransactionlist.cpp \
               src/gui/guigettransactions.cpp \
               src/gui/guitransferform.cpp \
               src/gui/guidatedtransfers.cpp \
               src/gui/guidatedsingletransfer.cpp \
               src/gui/guinotimplemented.cpp \
               src/gui/guijobs.cpp \
               src/gui/guidataexport.cpp \
               src/gui/guitransactionprinter.cpp \
               src/gui/previewwidget.cpp \
               src/gui/guihelpwindow.cpp \
               src/gui/guiknownbanks.cpp \
               src/gui/guiprinterdrivertoxml.cpp \
               src/gui/guiprinterautosetup.cpp \
               src/gui/guiprintermanualsetup.cpp \
               src/gui/guihardwareconfig.cpp \
               src/gui/guientropy.cpp \
               src/gui/guiaddaccount.cpp \
               src/gui/guiimport.cpp \
               src/gui/guiprogressindicator.cpp \
     \
               src/keyfiles/keyrsa.cpp \
               src/keyfiles/keyfile.cpp \
               src/keyfiles/keyfileopenhbci.cpp \
      \
               src/printing/globalsprintersetup.cpp \
               src/printing/printersetupdb.cpp \
      \
               src/dta/dtaparser.cpp \
               src/dta/asatz.cpp \
               src/dta/csatz.cpp \
               src/dta/esatz.cpp \
               src/dta/dtagenerator.cpp \
               src/dta/dtasection.cpp \
               src/dta/exdtaerror.cpp \
               src/dta/dtasatz.cpp \

LIBS += -lopenhbci2 \
        -laqmoney \
        -lQtNetwork \
        -lQtXml \
        -lhbci \
        -lxerces-c \
        -lpcsclite

QMAKE_LIBDIR += \
                build/os \
                kernellibs \

#Use code optimization and no debugging for the release version
#No unused parameter warnings, as there are lots of overridden virtual functions
#with unused parameters in the code.
QMAKE_CXXFLAGS_RELEASE += -O3 \
                          -g0 \
                          -Wno-unused-parameter

#No optimization and full debugging for the debug version
QMAKE_CXXFLAGS_DEBUG += -O0 \
                        -g3 \
                        -Wno-unused-parameter

#Config settings
CONFIG += warn_on \
          qt \
          stl \

#Required for QDesktopWidget to be included into QApplication
DEFINES += QT_INCLUDE_COMPAT 

debug {
    QMAKE_CFLAGS_DEBUG += -O0 -g3
    QMAKE_CXXFLAGS_DEBUG += -O0 -g3
    DEFINES += DEBUG_HARDWARE_ON
}

#For compiling a live CD, define LIVE_CD
live_cd {
    DEFINES += LIVE_CD
}

#For compiling a udrw stick version define LIVE_UDRW
live_udrw {
    DEFINES += LIVE_CD
    DEFINES += LIVE_UDRW
}

#This compiles a Mac OS X version
mac_osx {
    DEFINES += OSX
}

#QMAKE_UIC = $(QTDIR)/bin/uic-qt4
#QMAKE_MOC = $(QTDIR)/bin/moc-qt4

# Keep the src directory clean, put the generated stuff elsewhere
MOC_DIR = build/mocs
UI_DIR = build/uis
OBJECTS_DIR = build/os
