/****************************************************************************
** Meta object code from reading C++ file 'guitransactioninfo.h'
**
** Created: Sun Jun 19 13:28:57 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guitransactioninfo.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guitransactioninfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiTransactionInfo[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiTransactionInfo[] = {
    "GuiTransactionInfo\0\0canceled()\0"
    "on_buttonBack_clicked()\0"
};

const QMetaObject GuiTransactionInfo::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiTransactionInfo,
      qt_meta_data_GuiTransactionInfo, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiTransactionInfo::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiTransactionInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiTransactionInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiTransactionInfo))
        return static_cast<void*>(const_cast< GuiTransactionInfo*>(this));
    if (!strcmp(_clname, "Ui::GuiTransactionInfoBase"))
        return static_cast< Ui::GuiTransactionInfoBase*>(const_cast< GuiTransactionInfo*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiTransactionInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: canceled(); break;
        case 1: on_buttonBack_clicked(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GuiTransactionInfo::canceled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
