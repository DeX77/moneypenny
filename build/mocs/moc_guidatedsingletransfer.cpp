/****************************************************************************
** Meta object code from reading C++ file 'guidatedsingletransfer.h'
**
** Created: Sat Jun 25 13:26:37 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guidatedsingletransfer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guidatedsingletransfer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiDatedSingleTransfer[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiDatedSingleTransfer[] = {
    "GuiDatedSingleTransfer\0\0"
    "on_buttonExecute_clicked()\0"
};

const QMetaObject GuiDatedSingleTransfer::staticMetaObject = {
    { &GuiTransferForm::staticMetaObject, qt_meta_stringdata_GuiDatedSingleTransfer,
      qt_meta_data_GuiDatedSingleTransfer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiDatedSingleTransfer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiDatedSingleTransfer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiDatedSingleTransfer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiDatedSingleTransfer))
        return static_cast<void*>(const_cast< GuiDatedSingleTransfer*>(this));
    return GuiTransferForm::qt_metacast(_clname);
}

int GuiDatedSingleTransfer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GuiTransferForm::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_buttonExecute_clicked(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE