/****************************************************************************
** Meta object code from reading C++ file 'guidatedtransfers.h'
**
** Created: Sat Jun 25 13:26:36 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guidatedtransfers.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guidatedtransfers.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiDatedTransfers[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   19,   18,   18, 0x05,
      81,   61,   18,   18, 0x05,
     113,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     140,   18,   18,   18, 0x0a,
     187,  176,   18,   18, 0x0a,
     229,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiDatedTransfers[] = {
    "GuiDatedTransfers\0\0dt\0"
    "showInformation(banking::TransferItem)\0"
    ",reason,showJobList\0sendJobsNow(bool*,QString,bool)\0"
    "newDatedTransferRequired()\0"
    "on_buttonNewDatedTransfer_clicked()\0"
    "row,column\0on_tableWidget_cellDoubleClicked(int,int)\0"
    "on_buttonGetList_clicked()\0"
};

const QMetaObject GuiDatedTransfers::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiDatedTransfers,
      qt_meta_data_GuiDatedTransfers, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiDatedTransfers::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiDatedTransfers::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiDatedTransfers::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiDatedTransfers))
        return static_cast<void*>(const_cast< GuiDatedTransfers*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiDatedTransfers::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showInformation((*reinterpret_cast< const banking::TransferItem(*)>(_a[1]))); break;
        case 1: sendJobsNow((*reinterpret_cast< bool*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: newDatedTransferRequired(); break;
        case 3: on_buttonNewDatedTransfer_clicked(); break;
        case 4: on_tableWidget_cellDoubleClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: on_buttonGetList_clicked(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GuiDatedTransfers::showInformation(const banking::TransferItem & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GuiDatedTransfers::sendJobsNow(bool * _t1, const QString & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GuiDatedTransfers::newDatedTransferRequired()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
