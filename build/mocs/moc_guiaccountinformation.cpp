/****************************************************************************
** Meta object code from reading C++ file 'guiaccountinformation.h'
**
** Created: Sat Jun 25 13:24:23 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiaccountinformation.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiaccountinformation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiAccountInformation[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   23,   22,   22, 0x05,
      56,   22,   22,   22, 0x05,
      69,   22,   22,   22, 0x05,
      85,   22,   22,   22, 0x05,
     104,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
     119,   22,   22,   22, 0x09,
     152,   22,   22,   22, 0x09,
     182,   22,   22,   22, 0x09,
     224,   22,   22,   22, 0x09,
     257,   22,   22,   22, 0x09,
     304,  293,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiAccountInformation[] = {
    "GuiAccountInformation\0\0accountId\0"
    "deleteAccount(QString)\0addAccount()\0"
    "changeAccount()\0showTransactions()\0"
    "dataModified()\0on_buttonDeleteAccount_clicked()\0"
    "on_buttonAddAccount_clicked()\0"
    "on_buttonEditAccountDescription_clicked()\0"
    "on_buttonChangeAccount_clicked()\0"
    "on_buttonShowTransactions_clicked()\0"
    "row,column\0on_tableWidget_cellChanged(int,int)\0"
};

const QMetaObject GuiAccountInformation::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiAccountInformation,
      qt_meta_data_GuiAccountInformation, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiAccountInformation::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiAccountInformation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiAccountInformation::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiAccountInformation))
        return static_cast<void*>(const_cast< GuiAccountInformation*>(this));
    if (!strcmp(_clname, "Ui::GuiAccountInformation"))
        return static_cast< Ui::GuiAccountInformation*>(const_cast< GuiAccountInformation*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiAccountInformation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: deleteAccount((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: addAccount(); break;
        case 2: changeAccount(); break;
        case 3: showTransactions(); break;
        case 4: dataModified(); break;
        case 5: on_buttonDeleteAccount_clicked(); break;
        case 6: on_buttonAddAccount_clicked(); break;
        case 7: on_buttonEditAccountDescription_clicked(); break;
        case 8: on_buttonChangeAccount_clicked(); break;
        case 9: on_buttonShowTransactions_clicked(); break;
        case 10: on_tableWidget_cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void GuiAccountInformation::deleteAccount(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GuiAccountInformation::addAccount()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GuiAccountInformation::changeAccount()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void GuiAccountInformation::showTransactions()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void GuiAccountInformation::dataModified()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
