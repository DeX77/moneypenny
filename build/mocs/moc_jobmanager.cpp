/****************************************************************************
** Meta object code from reading C++ file 'jobmanager.h'
**
** Created: Sat Jun 25 13:17:50 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/hobit/jobmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jobmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_JobManager[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      23,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   12,   11,   11, 0x05,
      45,   41,   11,   11, 0x05,
      92,   84,   11,   11, 0x05,
     131,   11,   11,   11, 0x05,
     152,   84,   11,   11, 0x05,
     194,   11,   11,   11, 0x05,
     214,   11,   11,   11, 0x05,
     245,  241,   11,   11, 0x05,
     282,   11,   11,   11, 0x05,
     303,  293,   11,   11, 0x05,
     320,  241,   11,   11, 0x05,
     357,  341,   11,   11, 0x05,
     402,  386,   11,   11, 0x05,
     440,  435,   11,   11, 0x05,
     476,  467,   11,   11, 0x05,
     507,  467,   11,   11, 0x05,
     533,   11,   11,   11, 0x05,
     558,  555,   11,   11, 0x05,
     589,  585,   11,   11, 0x05,
     613,  585,   11,   11, 0x05,
     633,  585,   11,   11, 0x05,
     699,  656,   11,   11, 0x05,
     762,  749,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     802,   11,   11,   11, 0x08,
     822,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_JobManager[] = {
    "JobManager\0\0e\0caughtException(exception)\0"
    "job\0jobStatusChanged(banking::BankingJob*)\0"
    "row,job\0sentAdminJob(int,banking::BankingJob*)\0"
    "adminJobsToAnalyze()\0"
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
    "override,msg\0overridableErrorDetected(bool*,QString)\0"
    "slotThreadStarted()\0slotThreadFinished()\0"
};

const QMetaObject JobManager::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_JobManager,
      qt_meta_data_JobManager, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &JobManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *JobManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *JobManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_JobManager))
        return static_cast<void*>(const_cast< JobManager*>(this));
    if (!strcmp(_clname, "C_Listener"))
        return static_cast< C_Listener*>(const_cast< JobManager*>(this));
    return QThread::qt_metacast(_clname);
}

int JobManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: caughtException((*reinterpret_cast< exception(*)>(_a[1]))); break;
        case 1: jobStatusChanged((*reinterpret_cast< banking::BankingJob*(*)>(_a[1]))); break;
        case 2: sentAdminJob((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< banking::BankingJob*(*)>(_a[2]))); break;
        case 3: adminJobsToAnalyze(); break;
        case 4: sentTransferJob((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< banking::BankingJob*(*)>(_a[2]))); break;
        case 5: insertCardRequest(); break;
        case 6: inputPinViaKeypadRequest(); break;
        case 7: inputPinViaKeyboardRequest((*reinterpret_cast< QString*(*)>(_a[1]))); break;
        case 8: pinWasOk(); break;
        case 9: pinWasNotOk((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: pinRequest((*reinterpret_cast< QString*(*)>(_a[1]))); break;
        case 11: tanRequest((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 12: certificatePathRequest((*reinterpret_cast< QString*(*)>(_a[1]))); break;
        case 13: verifyBankKeyHash((*reinterpret_cast< string*(*)>(_a[1]))); break;
        case 14: errorMessagesReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: messagesReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: kernelThreadStarted(); break;
        case 17: kernelThreadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: sendingMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 19: responseOk((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: responseError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 21: cardDataRead((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5]))); break;
        case 22: overridableErrorDetected((*reinterpret_cast< bool*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 23: slotThreadStarted(); break;
        case 24: slotThreadFinished(); break;
        default: ;
        }
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void JobManager::caughtException(exception _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void JobManager::jobStatusChanged(banking::BankingJob * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void JobManager::sentAdminJob(int _t1, banking::BankingJob * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void JobManager::adminJobsToAnalyze()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void JobManager::sentTransferJob(int _t1, banking::BankingJob * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void JobManager::insertCardRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void JobManager::inputPinViaKeypadRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void JobManager::inputPinViaKeyboardRequest(QString * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void JobManager::pinWasOk()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void JobManager::pinWasNotOk(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void JobManager::pinRequest(QString * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void JobManager::tanRequest(QString _t1, QString * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void JobManager::certificatePathRequest(QString * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void JobManager::verifyBankKeyHash(string * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void JobManager::errorMessagesReceived(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void JobManager::messagesReceived(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void JobManager::kernelThreadStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void JobManager::kernelThreadFinished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void JobManager::sendingMessage(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void JobManager::responseOk(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void JobManager::responseError(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void JobManager::cardDataRead(int _t1, QString _t2, QString _t3, QString _t4, QString _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void JobManager::overridableErrorDetected(bool * _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}
QT_END_MOC_NAMESPACE
