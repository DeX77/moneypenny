/****************************************************************************
** Meta object code from reading C++ file 'guifindmedium.h'
**
** Created: Sun Jun 19 13:28:55 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guifindmedium.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guifindmedium.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiFindMedium[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x09,
      31,   14,   14,   14, 0x09,
      66,   14,   14,   14, 0x09,
      98,   14,   14,   14, 0x09,
     122,   14,   14,   14, 0x09,
     146,   14,   14,   14, 0x09,
     172,   14,   14,   14, 0x09,
     206,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiFindMedium[] = {
    "GuiFindMedium\0\0slotTimerDone()\0"
    "on_radioButtonFloppy_toggled(bool)\0"
    "on_radioButtonUsb_toggled(bool)\0"
    "on_buttonHelp_clicked()\0on_buttonNext_clicked()\0"
    "on_buttonCancel_clicked()\0"
    "on_buttonSearchManually_clicked()\0"
    "on_buttonMore_clicked()\0"
};

const QMetaObject GuiFindMedium::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GuiFindMedium,
      qt_meta_data_GuiFindMedium, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiFindMedium::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiFindMedium::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiFindMedium::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiFindMedium))
        return static_cast<void*>(const_cast< GuiFindMedium*>(this));
    if (!strcmp(_clname, "Ui::GuiFindMediumBase"))
        return static_cast< Ui::GuiFindMediumBase*>(const_cast< GuiFindMedium*>(this));
    return QDialog::qt_metacast(_clname);
}

int GuiFindMedium::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotTimerDone(); break;
        case 1: on_radioButtonFloppy_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: on_radioButtonUsb_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: on_buttonHelp_clicked(); break;
        case 4: on_buttonNext_clicked(); break;
        case 5: on_buttonCancel_clicked(); break;
        case 6: on_buttonSearchManually_clicked(); break;
        case 7: on_buttonMore_clicked(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
