/****************************************************************************
** Meta object code from reading C++ file 'guitransactionlist.h'
**
** Created: Sat Jun 25 13:24:24 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guitransactionlist.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guitransactionlist.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiTransactionList[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      44,   20,   19,   19, 0x05,
     101,   19,   19,   19, 0x05,
     119,   19,   19,   19, 0x05,
     157,   19,   19,   19, 0x05,
     179,  170,   19,   19, 0x05,
     200,   19,   19,   19, 0x05,
     220,  217,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
     243,  237,   19,   19, 0x09,
     284,   19,   19,   19, 0x09,
     322,  314,   19,   19, 0x09,
     358,   19,   19,   19, 0x09,
     387,   19,   19,   19, 0x09,
     417,   19,   19,   19, 0x09,
     441,   19,   19,   19, 0x09,
     491,  484,   19,   19, 0x09,
     518,  513,   19,   19, 0x09,
     553,  513,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiTransactionList[] = {
    "GuiTransactionList\0\0transactionList,from,to\0"
    "printTransactions(banking::TransactionList*,QDate,QDate)\0"
    "getTransactions()\0"
    "showDetails(banking::TransactionItem)\0"
    "exportData()\0helpFile\0helpRequest(QString)\0"
    "refreshRequest()\0iD\0raiseWidget(int)\0"
    "index\0on_comboBoxDate_currentIndexChanged(int)\0"
    "on_buttonUpdateData_clicked()\0row,col\0"
    "on_tableWidget_cellClicked(int,int)\0"
    "on_buttonPrintList_clicked()\0"
    "on_buttonExportList_clicked()\0"
    "on_buttonHelp_clicked()\0"
    "on_buttonCreateTransferTemplates_clicked()\0"
    "column\0slotSortByColumn(int)\0date\0"
    "on_dateEditFrom_dateChanged(QDate)\0"
    "on_dateEditTo_dateChanged(QDate)\0"
};

const QMetaObject GuiTransactionList::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiTransactionList,
      qt_meta_data_GuiTransactionList, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiTransactionList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiTransactionList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiTransactionList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiTransactionList))
        return static_cast<void*>(const_cast< GuiTransactionList*>(this));
    if (!strcmp(_clname, "Ui::GuiTransactionList"))
        return static_cast< Ui::GuiTransactionList*>(const_cast< GuiTransactionList*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiTransactionList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: printTransactions((*reinterpret_cast< banking::TransactionList*(*)>(_a[1])),(*reinterpret_cast< const QDate(*)>(_a[2])),(*reinterpret_cast< const QDate(*)>(_a[3]))); break;
        case 1: getTransactions(); break;
        case 2: showDetails((*reinterpret_cast< const banking::TransactionItem(*)>(_a[1]))); break;
        case 3: exportData(); break;
        case 4: helpRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: refreshRequest(); break;
        case 6: raiseWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: on_comboBoxDate_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: on_buttonUpdateData_clicked(); break;
        case 9: on_tableWidget_cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: on_buttonPrintList_clicked(); break;
        case 11: on_buttonExportList_clicked(); break;
        case 12: on_buttonHelp_clicked(); break;
        case 13: on_buttonCreateTransferTemplates_clicked(); break;
        case 14: slotSortByColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: on_dateEditFrom_dateChanged((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 16: on_dateEditTo_dateChanged((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void GuiTransactionList::printTransactions(banking::TransactionList * _t1, const QDate & _t2, const QDate & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GuiTransactionList::getTransactions()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GuiTransactionList::showDetails(const banking::TransactionItem & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GuiTransactionList::exportData()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void GuiTransactionList::helpRequest(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GuiTransactionList::refreshRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void GuiTransactionList::raiseWidget(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
