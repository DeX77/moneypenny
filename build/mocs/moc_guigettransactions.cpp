/****************************************************************************
** Meta object code from reading C++ file 'guigettransactions.h'
**
** Created: Sat Jun 25 13:24:24 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guigettransactions.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guigettransactions.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiGetTransactions[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      34,   20,   19,   19, 0x05,
      62,   19,   19,   19, 0x05,
      76,   19,   19,   19, 0x05,
      93,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
     104,   19,   19,   19, 0x09,
     127,   19,   19,   19, 0x09,
     161,  153,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiGetTransactions[] = {
    "GuiGetTransactions\0\0,,showJobList\0"
    "sendNow(bool*,QString,bool)\0jobsCreated()\0"
    "raiseWidget(int)\0canceled()\0"
    "on_buttonGet_clicked()\0on_buttonCancel_clicked()\0"
    "checked\0on_radioButtonDated_toggled(bool)\0"
};

const QMetaObject GuiGetTransactions::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiGetTransactions,
      qt_meta_data_GuiGetTransactions, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiGetTransactions::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiGetTransactions::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiGetTransactions::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiGetTransactions))
        return static_cast<void*>(const_cast< GuiGetTransactions*>(this));
    if (!strcmp(_clname, "Ui::GuiGetTransactionsBase"))
        return static_cast< Ui::GuiGetTransactionsBase*>(const_cast< GuiGetTransactions*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiGetTransactions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sendNow((*reinterpret_cast< bool*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: jobsCreated(); break;
        case 2: raiseWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: canceled(); break;
        case 4: on_buttonGet_clicked(); break;
        case 5: on_buttonCancel_clicked(); break;
        case 6: on_radioButtonDated_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void GuiGetTransactions::sendNow(bool * _t1, const QString & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GuiGetTransactions::jobsCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GuiGetTransactions::raiseWidget(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GuiGetTransactions::canceled()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
