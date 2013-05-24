/****************************************************************************
** Meta object code from reading C++ file 'dlgstatusmessage.h'
**
** Created: Sun Jun 19 13:28:55 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/dlgstatusmessage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dlgstatusmessage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DlgStatusMessage[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   18,   17,   17, 0x0a,
      54,   18,   17,   17, 0x0a,
      84,   18,   17,   17, 0x0a,
     116,   18,   17,   17, 0x0a,
     149,   18,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DlgStatusMessage[] = {
    "DlgStatusMessage\0\0newVal\0"
    "setLabelTopLeftText(QString)\0"
    "setLabelTopRightText(QString)\0"
    "setLabelBottomLeftText(QString)\0"
    "setLabelBottomRightText(QString)\0"
    "setLabelBottomRightNumber(int)\0"
};

const QMetaObject DlgStatusMessage::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DlgStatusMessage,
      qt_meta_data_DlgStatusMessage, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DlgStatusMessage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DlgStatusMessage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DlgStatusMessage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DlgStatusMessage))
        return static_cast<void*>(const_cast< DlgStatusMessage*>(this));
    if (!strcmp(_clname, "Ui::DlgStatusMessageBase"))
        return static_cast< Ui::DlgStatusMessageBase*>(const_cast< DlgStatusMessage*>(this));
    return QDialog::qt_metacast(_clname);
}

int DlgStatusMessage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setLabelTopLeftText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: setLabelTopRightText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: setLabelBottomLeftText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: setLabelBottomRightText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: setLabelBottomRightNumber((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
