/****************************************************************************
** Meta object code from reading C++ file 'guinotimplemented.h'
**
** Created: Sun Jun 19 13:28:58 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guinotimplemented.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guinotimplemented.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiNotImplemented[] = {

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
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      26,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiNotImplemented[] = {
    "GuiNotImplemented\0\0back()\0"
    "on_pushButtonBack_clicked()\0"
};

const QMetaObject GuiNotImplemented::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiNotImplemented,
      qt_meta_data_GuiNotImplemented, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiNotImplemented::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiNotImplemented::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiNotImplemented::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiNotImplemented))
        return static_cast<void*>(const_cast< GuiNotImplemented*>(this));
    if (!strcmp(_clname, "Ui::GuiNotImplementedBase"))
        return static_cast< Ui::GuiNotImplementedBase*>(const_cast< GuiNotImplemented*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiNotImplemented::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: on_pushButtonBack_clicked(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GuiNotImplemented::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
