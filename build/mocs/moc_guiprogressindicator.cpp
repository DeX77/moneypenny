/****************************************************************************
** Meta object code from reading C++ file 'guiprogressindicator.h'
**
** Created: Sun Jun 19 13:28:59 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiprogressindicator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiprogressindicator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiProgressIndicator[] = {

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
      22,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,   21,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiProgressIndicator[] = {
    "GuiProgressIndicator\0\0cancel()\0"
    "on_buttonCancel_clicked()\0"
};

const QMetaObject GuiProgressIndicator::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GuiProgressIndicator,
      qt_meta_data_GuiProgressIndicator, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiProgressIndicator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiProgressIndicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiProgressIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiProgressIndicator))
        return static_cast<void*>(const_cast< GuiProgressIndicator*>(this));
    if (!strcmp(_clname, "Ui::GuiProgressIndicatorBase"))
        return static_cast< Ui::GuiProgressIndicatorBase*>(const_cast< GuiProgressIndicator*>(this));
    return QDialog::qt_metacast(_clname);
}

int GuiProgressIndicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cancel(); break;
        case 1: on_buttonCancel_clicked(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GuiProgressIndicator::cancel()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
