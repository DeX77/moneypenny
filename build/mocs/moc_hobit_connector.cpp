/****************************************************************************
** Meta object code from reading C++ file 'hobit_connector.h'
**
** Created: Sun Jun 19 13:28:53 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/hobit/hobit_connector.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hobit_connector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_HoBIT_Connector[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      29,   17,   16,   16, 0x08,
      72,   16,   16,   16, 0x08,
     103,   16,   16,   16, 0x08,
     125,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HoBIT_Connector[] = {
    "HoBIT_Connector\0\0socketError\0"
    "slotTCPError(QAbstractSocket::SocketError)\0"
    "slotTCPConnectionEstablished()\0"
    "slotCloseConnection()\0slotNextSecond()\0"
};

const QMetaObject HoBIT_Connector::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HoBIT_Connector,
      qt_meta_data_HoBIT_Connector, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &HoBIT_Connector::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *HoBIT_Connector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *HoBIT_Connector::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HoBIT_Connector))
        return static_cast<void*>(const_cast< HoBIT_Connector*>(this));
    return QObject::qt_metacast(_clname);
}

int HoBIT_Connector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotTCPError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 1: slotTCPConnectionEstablished(); break;
        case 2: slotCloseConnection(); break;
        case 3: slotNextSecond(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
