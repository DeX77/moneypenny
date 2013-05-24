/****************************************************************************
** Meta object code from reading C++ file 'guihbciwizard.h'
**
** Created: Sat Jun 25 13:24:22 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guihbciwizard.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guihbciwizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiHbciWizard[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      46,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: signature, parameters, type, tag, flags
      49,   15,   14,   14, 0x05,
     117,  114,   14,   14, 0x05,
     148,  139,   14,   14, 0x05,
     169,   14,   14,   14, 0x05,
     193,  185,   14,   14, 0x05,
     225,  220,   14,   14, 0x05,
     238,   14,   14,   14, 0x05,
     251,   14,   14,   14, 0x05,
     264,   14,   14,   14, 0x05,
     273,  220,   14,   14, 0x05,
     296,   14,   14,   14, 0x05,
     326,  306,   14,   14, 0x05,
     358,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     380,  375,   14,   14, 0x0a,
     421,  375,   14,   14, 0x0a,
     464,  375,   14,   14, 0x0a,
     511,  503,   14,   14, 0x0a,
     549,  503,   14,   14, 0x0a,
     592,  503,   14,   14, 0x0a,
     627,  375,   14,   14, 0x0a,
     673,  503,   14,   14, 0x0a,
     710,   14,   14,   14, 0x0a,
     735,   14,   14,   14, 0x0a,
     765,   14,   14,   14, 0x0a,
     799,   14,   14,   14, 0x0a,
     833,  827,   14,   14, 0x0a,
     876,   14,   14,   14, 0x0a,
     890,   14,   14,   14, 0x0a,
     922,   14,   14,   14, 0x0a,
     959,   14,   14,   14, 0x0a,
     979,   14,   14,   14, 0x0a,
    1003,   14,   14,   14, 0x0a,
    1027,   14,   14,   14, 0x0a,
    1071, 1066,   14,   14, 0x0a,
    1102,  114,   14,   14, 0x0a,
    1133,   14,   14,   14, 0x0a,
    1152,   14,   14,   14, 0x09,
    1161,   14,   14,   14, 0x09,
    1187,   14,   14,   14, 0x09,
    1211,  375,   14,   14, 0x09,
    1253,   14,   14,   14, 0x09,
    1286,   14,   14,   14, 0x09,
    1311,   14,   14,   14, 0x09,
    1338,   14,   14,   14, 0x09,
    1360,   14,   14,   14, 0x09,
    1390,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiHbciWizard[] = {
    "GuiHbciWizard\0\0showSuccessMessage,url,port,state\0"
    "testInternet(bool,QString,int,HoBIT_Connector::ConnectionState*)\0"
    "ok\0testConnection(bool*)\0helpfile\0"
    "helpRequest(QString)\0setupInternet()\0"
    "RDHMode\0setupSuccessful(HBCI_MODE)\0"
    "Page\0showLog(int)\0renewHoBIT()\0"
    "addAccount()\0dialIn()\0wizardPageChanged(int)\0"
    "aborted()\0,reason,showJobList\0"
    "sendJobsNow(bool*,QString,bool)\0"
    "entropyRequest()\0text\0"
    "on_lineEditBankCode_textChanged(QString)\0"
    "on_lineEditBankServer_textChanged(QString)\0"
    "on_lineEditUserId_textChanged(QString)\0"
    "checked\0on_radioButtonNewMedium_toggled(bool)\0"
    "on_radioButtonExistingMedium_toggled(bool)\0"
    "on_radioButtonPinTan_toggled(bool)\0"
    "on_lineEditAccountPinTan_textChanged(QString)\0"
    "on_radioButtonChipCard_toggled(bool)\0"
    "slotChangePassportType()\0"
    "on_buttonSelectFile_clicked()\0"
    "on_buttonTestConnection_clicked()\0"
    "on_buttonSendKeys_clicked()\0state\0"
    "on_checkBoxFingerprintOk_stateChanged(int)\0"
    "pageChanged()\0on_buttonHelpPassword_clicked()\0"
    "on_buttonTestInternetSetup_clicked()\0"
    "slotInternetSetup()\0on_buttonNext_clicked()\0"
    "on_buttonBack_clicked()\0"
    "on_buttonWriteDataToChipcard_clicked()\0"
    "hash\0slotVerifyBankKeyHash(string*)\0"
    "slotKernelThreadFinished(bool)\0"
    "slotAccountAdded()\0reject()\0"
    "on_buttonFinish_clicked()\0"
    "on_buttonHelp_clicked()\0"
    "on_lineEditPassword1_textChanged(QString)\0"
    "on_buttonCheckPassword_pressed()\0"
    "slotUpdateBankDataPage()\0"
    "on_buttonDetails_clicked()\0"
    "configureCardReader()\0"
    "on_buttonAddAccount_clicked()\0"
    "on_buttonDialIn_clicked()\0"
};

const QMetaObject GuiHbciWizard::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiHbciWizard,
      qt_meta_data_GuiHbciWizard, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiHbciWizard::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiHbciWizard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiHbciWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiHbciWizard))
        return static_cast<void*>(const_cast< GuiHbciWizard*>(this));
    if (!strcmp(_clname, "Ui::GuiHbciWizard"))
        return static_cast< Ui::GuiHbciWizard*>(const_cast< GuiHbciWizard*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiHbciWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: testInternet((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< HoBIT_Connector::ConnectionState*(*)>(_a[4]))); break;
        case 1: testConnection((*reinterpret_cast< bool*(*)>(_a[1]))); break;
        case 2: helpRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: setupInternet(); break;
        case 4: setupSuccessful((*reinterpret_cast< HBCI_MODE(*)>(_a[1]))); break;
        case 5: showLog((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: renewHoBIT(); break;
        case 7: addAccount(); break;
        case 8: dialIn(); break;
        case 9: wizardPageChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: aborted(); break;
        case 11: sendJobsNow((*reinterpret_cast< bool*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 12: entropyRequest(); break;
        case 13: on_lineEditBankCode_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: on_lineEditBankServer_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: on_lineEditUserId_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: on_radioButtonNewMedium_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: on_radioButtonExistingMedium_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: on_radioButtonPinTan_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: on_lineEditAccountPinTan_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: on_radioButtonChipCard_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: slotChangePassportType(); break;
        case 22: on_buttonSelectFile_clicked(); break;
        case 23: on_buttonTestConnection_clicked(); break;
        case 24: on_buttonSendKeys_clicked(); break;
        case 25: on_checkBoxFingerprintOk_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: pageChanged(); break;
        case 27: on_buttonHelpPassword_clicked(); break;
        case 28: on_buttonTestInternetSetup_clicked(); break;
        case 29: slotInternetSetup(); break;
        case 30: on_buttonNext_clicked(); break;
        case 31: on_buttonBack_clicked(); break;
        case 32: on_buttonWriteDataToChipcard_clicked(); break;
        case 33: slotVerifyBankKeyHash((*reinterpret_cast< string*(*)>(_a[1]))); break;
        case 34: slotKernelThreadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 35: slotAccountAdded(); break;
        case 36: reject(); break;
        case 37: on_buttonFinish_clicked(); break;
        case 38: on_buttonHelp_clicked(); break;
        case 39: on_lineEditPassword1_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 40: on_buttonCheckPassword_pressed(); break;
        case 41: slotUpdateBankDataPage(); break;
        case 42: on_buttonDetails_clicked(); break;
        case 43: configureCardReader(); break;
        case 44: on_buttonAddAccount_clicked(); break;
        case 45: on_buttonDialIn_clicked(); break;
        default: ;
        }
        _id -= 46;
    }
    return _id;
}

// SIGNAL 0
void GuiHbciWizard::testInternet(bool _t1, const QString & _t2, int _t3, HoBIT_Connector::ConnectionState * _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GuiHbciWizard::testConnection(bool * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GuiHbciWizard::helpRequest(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GuiHbciWizard::setupInternet()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void GuiHbciWizard::setupSuccessful(HBCI_MODE _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GuiHbciWizard::showLog(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void GuiHbciWizard::renewHoBIT()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void GuiHbciWizard::addAccount()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void GuiHbciWizard::dialIn()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void GuiHbciWizard::wizardPageChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void GuiHbciWizard::aborted()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void GuiHbciWizard::sendJobsNow(bool * _t1, const QString & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void GuiHbciWizard::entropyRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}
QT_END_MOC_NAMESPACE
