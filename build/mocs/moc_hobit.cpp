/****************************************************************************
** Meta object code from reading C++ file 'hobit.h'
**
** Created: Sat Jun 25 13:17:49 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/hobit/hobit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hobit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_HoBIT[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      25,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,    7,    6,    6, 0x05,
      42,   38,    6,    6, 0x05,
      89,   81,    6,    6, 0x05,
     128,   81,    6,    6, 0x05,
     170,    6,    6,    6, 0x05,
     190,    6,    6,    6, 0x05,
     221,  217,    6,    6, 0x05,
     258,    6,    6,    6, 0x05,
     279,  269,    6,    6, 0x05,
     296,  217,    6,    6, 0x05,
     333,  317,    6,    6, 0x05,
     378,  362,    6,    6, 0x05,
     416,  411,    6,    6, 0x05,
     452,  443,    6,    6, 0x05,
     483,  443,    6,    6, 0x05,
     509,    6,    6,    6, 0x05,
     534,  531,    6,    6, 0x05,
     565,  561,    6,    6, 0x05,
     589,  561,    6,    6, 0x05,
     609,  561,    6,    6, 0x05,
     675,  632,    6,    6, 0x05,
     727,  725,    6,    6, 0x05,
     767,  754,    6,    6, 0x05,
     815,  807,    6,    6, 0x05,
     848,  838,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
     867,    6,    6,    6, 0x0a,
     890,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_HoBIT[] = {
    "HoBIT\0\0logText\0logTextToShow(QString)\0"
    "job\0jobStatusChanged(banking::BankingJob*)\0"
    "row,job\0sentAdminJob(int,banking::BankingJob*)\0"
    "sentTransferJob(int,banking::BankingJob*)\0"
    "insertCardRequest()\0inputPinViaKeypadRequest()\0"
    "pin\0inputPinViaKeyboardRequest(QString*)\0"
    "pinWasOk()\0triesLeft\0pinWasNotOk(int)\0"
    "pinRequest(QString*)\0bankmessage,tan\0"
    "tanRequest(QString,QString*)\0"
    "certificatePath\0certificatePathRequest(QString*)\0"
    "hash\0verifyBankKeyHash(string*)\0"
    "messages\0errorMessagesReceived(QString)\0"
    "messagesReceived(QString)\0"
    "kernelThreadStarted()\0ok\0"
    "kernelThreadFinished(bool)\0msg\0"
    "sendingMessage(QString)\0responseOk(QString)\0"
    "responseError(QString)\0"
    "country,bankId,userId,serverUrl,readerName\0"
    "cardDataRead(int,QString,QString,QString,QString)\0"
    "e\0caughtException(exception)\0override,msg\0"
    "overridableErrorDetected(bool*,QString)\0"
    "message\0statusMessage(QString)\0jobNumber\0"
    "processingJob(int)\0slotAnalyzeAdminJobs()\0"
    "slotCancelKernelAction()\0"
};

const QMetaObject HoBIT::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HoBIT,
      qt_meta_data_HoBIT, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &HoBIT::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *HoBIT::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *HoBIT::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HoBIT))
        return static_cast<void*>(const_cast< HoBIT*>(this));
    return QObject::qt_metacast(_clname);
}

int HoBIT::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: logTextToShow((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: jobStatusChanged((*reinterpret_cast< banking::BankingJob*(*)>(_a[1]))); break;
        case 2: sentAdminJob((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< banking::BankingJob*(*)>(_a[2]))); break;
        case 3: sentTransferJob((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< banking::BankingJob*(*)>(_a[2]))); break;
        case 4: insertCardRequest(); break;
        case 5: inputPinViaKeypadRequest(); break;
        case 6: inputPinViaKeyboardRequest((*reinterpret_cast< QString*(*)>(_a[1]))); break;
        case 7: pinWasOk(); break;
        case 8: pinWasNotOk((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: pinRequest((*reinterpret_cast< QString*(*)>(_a[1]))); break;
        case 10: tanRequest((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 11: certificatePathRequest((*reinterpret_cast< QString*(*)>(_a[1]))); break;
        case 12: verifyBankKeyHash((*reinterpret_cast< string*(*)>(_a[1]))); break;
        case 13: errorMessagesReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: messagesReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: kernelThreadStarted(); break;
        case 16: kernelThreadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: sendingMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: responseOk((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 19: responseError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: cardDataRead((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5]))); break;
        case 21: caughtException((*reinterpret_cast< exception(*)>(_a[1]))); break;
        case 22: overridableErrorDetected((*reinterpret_cast< bool*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 23: statusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 24: processingJob((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: slotAnalyzeAdminJobs(); break;
        case 26: slotCancelKernelAction(); break;
        default: ;
        }
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void HoBIT::logTextToShow(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HoBIT::jobStatusChanged(banking::BankingJob * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HoBIT::sentAdminJob(int _t1, banking::BankingJob * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void HoBIT::sentTransferJob(int _t1, banking::BankingJob * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void HoBIT::insertCardRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void HoBIT::inputPinViaKeypadRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void HoBIT::inputPinViaKeyboardRequest(QString * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void HoBIT::pinWasOk()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void HoBIT::pinWasNotOk(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void HoBIT::pinRequest(QString * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void HoBIT::tanRequest(QString _t1, QString * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void HoBIT::certificatePathRequest(QString * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void HoBIT::verifyBankKeyHash(string * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void HoBIT::errorMessagesReceived(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void HoBIT::messagesReceived(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void HoBIT::kernelThreadStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void HoBIT::kernelThreadFinished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void HoBIT::sendingMessage(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void HoBIT::responseOk(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void HoBIT::responseError(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void HoBIT::cardDataRead(int _t1, QString _t2, QString _t3, QString _t4, QString _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void HoBIT::caughtException(exception _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void HoBIT::overridableErrorDetected(bool * _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 23
void HoBIT::statusMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void HoBIT::processingJob(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}
QT_END_MOC_NAMESPACE
