/****************************************************************************
** Meta object code from reading C++ file 'dtaparser.h'
**
** Created: Sat Jun 25 15:14:04 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/dta/dtaparser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dtaparser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DTAParser[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   11,   10,   10, 0x05,
      60,   51,   10,   10, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DTAParser[] = {
    "DTAParser\0\0messages\0errorMessagesReceived(QString)\0"
    "transfer\0transferCreated(banking::Transfer*)\0"
};

const QMetaObject DTAParser::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DTAParser,
      qt_meta_data_DTAParser, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DTAParser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DTAParser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DTAParser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DTAParser))
        return static_cast<void*>(const_cast< DTAParser*>(this));
    return QObject::qt_metacast(_clname);
}

int DTAParser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: errorMessagesReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: transferCreated((*reinterpret_cast< banking::Transfer*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DTAParser::errorMessagesReceived(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DTAParser::transferCreated(banking::Transfer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE