/****************************************************************************
** Meta object code from reading C++ file 'guishowbankdata.h'
**
** Created: Sun Jun 19 13:28:56 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guishowbankdata.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guishowbankdata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiShowBankData[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      24,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      45,   16,   16,   16, 0x0a,
      56,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiShowBankData[] = {
    "GuiShowBankData\0\0back()\0helpRequest(QString)\0"
    "slotBack()\0slotHelp()\0"
};

const QMetaObject GuiShowBankData::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiShowBankData,
      qt_meta_data_GuiShowBankData, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiShowBankData::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiShowBankData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiShowBankData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiShowBankData))
        return static_cast<void*>(const_cast< GuiShowBankData*>(this));
    if (!strcmp(_clname, "Ui::GuiShowBankData"))
        return static_cast< Ui::GuiShowBankData*>(const_cast< GuiShowBankData*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiShowBankData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: helpRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: slotBack(); break;
        case 3: slotHelp(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void GuiShowBankData::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiShowBankData::helpRequest(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
