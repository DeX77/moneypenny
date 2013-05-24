/****************************************************************************
** Meta object code from reading C++ file 'guiaddaccount.h'
**
** Created: Sat Jun 25 13:26:37 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiaddaccount.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiaddaccount.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiAddAccount[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      30,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   14,   14,   14, 0x0a,
      61,   14,   14,   14, 0x0a,
      83,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiAddAccount[] = {
    "GuiAddAccount\0\0accountAdded()\0back()\0"
    "on_buttonBack_clicked()\0on_buttonOk_clicked()\0"
    "validateInput()\0"
};

const QMetaObject GuiAddAccount::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiAddAccount,
      qt_meta_data_GuiAddAccount, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiAddAccount::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiAddAccount::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiAddAccount::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiAddAccount))
        return static_cast<void*>(const_cast< GuiAddAccount*>(this));
    if (!strcmp(_clname, "Ui::GuiAddAccountBase"))
        return static_cast< Ui::GuiAddAccountBase*>(const_cast< GuiAddAccount*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiAddAccount::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accountAdded(); break;
        case 1: back(); break;
        case 2: on_buttonBack_clicked(); break;
        case 3: on_buttonOk_clicked(); break;
        case 4: validateInput(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void GuiAddAccount::accountAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiAddAccount::back()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
