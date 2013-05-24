/****************************************************************************
** Meta object code from reading C++ file 'guiloginpassword.h'
**
** Created: Sun Jun 19 13:28:55 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiloginpassword.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiloginpassword.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiLoginPassword[] = {

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
      18,   17,   17,   17, 0x0a,
      43,   17,   17,   17, 0x0a,
      67,   17,   17,   17, 0x0a,
      91,   17,   17,   17, 0x0a,
     119,  111,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiLoginPassword[] = {
    "GuiLoginPassword\0\0on_buttonLogin_clicked()\0"
    "on_buttonHelp_clicked()\0on_buttonExit_clicked()\0"
    "slotInternetSetup()\0checked\0"
    "on_checkBoxShowPassword_toggled(bool)\0"
};

const QMetaObject GuiLoginPassword::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GuiLoginPassword,
      qt_meta_data_GuiLoginPassword, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiLoginPassword::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiLoginPassword::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiLoginPassword::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiLoginPassword))
        return static_cast<void*>(const_cast< GuiLoginPassword*>(this));
    if (!strcmp(_clname, "Ui::GuiLoginPasswordBase"))
        return static_cast< Ui::GuiLoginPasswordBase*>(const_cast< GuiLoginPassword*>(this));
    return QDialog::qt_metacast(_clname);
}

int GuiLoginPassword::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_buttonLogin_clicked(); break;
        case 1: on_buttonHelp_clicked(); break;
        case 2: on_buttonExit_clicked(); break;
        case 3: slotInternetSetup(); break;
        case 4: on_checkBoxShowPassword_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
