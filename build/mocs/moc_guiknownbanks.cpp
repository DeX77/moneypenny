/****************************************************************************
** Meta object code from reading C++ file 'guiknownbanks.h'
**
** Created: Sat Jun 25 13:26:37 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiknownbanks.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiknownbanks.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiKnownBanks[] = {

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
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      22,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiKnownBanks[] = {
    "GuiKnownBanks\0\0back()\0on_buttonBack_clicked()\0"
};

const QMetaObject GuiKnownBanks::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiKnownBanks,
      qt_meta_data_GuiKnownBanks, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiKnownBanks::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiKnownBanks::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiKnownBanks::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiKnownBanks))
        return static_cast<void*>(const_cast< GuiKnownBanks*>(this));
    if (!strcmp(_clname, "Ui::GuiKnownBanksBase"))
        return static_cast< Ui::GuiKnownBanksBase*>(const_cast< GuiKnownBanks*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiKnownBanks::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: on_buttonBack_clicked(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GuiKnownBanks::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
