/****************************************************************************
** Meta object code from reading C++ file 'guitransactionprinter.h'
**
** Created: Sun Jun 19 13:28:58 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guitransactionprinter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guitransactionprinter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiTransactionPrinter[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,
      39,   30,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      65,   22,   60,   22, 0x0a,
      97,   22,   22,   22, 0x0a,
     111,   22,   22,   22, 0x0a,
     137,   22,   22,   22, 0x0a,
     163,   22,   22,   22, 0x0a,
     199,   22,   22,   22, 0x0a,
     223,   22,   22,   22, 0x0a,
     252,   22,   22,   22, 0x0a,
     300,   22,   22,   22, 0x0a,
     331,  324,   22,   22, 0x0a,
     377,   22,   22,   22, 0x0a,
     402,   22,   22,   22, 0x0a,
     434,  426,   22,   22, 0x0a,
     468,   22,   22,   22, 0x0a,
     499,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiTransactionPrinter[] = {
    "GuiTransactionPrinter\0\0back()\0filename\0"
    "helpRequest(QString)\0bool\0"
    "on_buttonPrinterSetup_clicked()\0"
    "initPrinter()\0on_buttonColor1_clicked()\0"
    "on_buttonColor2_clicked()\0"
    "on_buttonShowPrintOptions_clicked()\0"
    "on_buttonFont_clicked()\0"
    "on_buttonMarginsOk_clicked()\0"
    "on_comboBoxBorderWidth_currentIndexChanged(int)\0"
    "on_buttonHelp_clicked()\0newVal\0"
    "setTransactionList(banking::TransactionList*)\0"
    "on_buttonPrint_clicked()\0"
    "on_buttonBack_clicked()\0checked\0"
    "on_radioButtonRange_toggled(bool)\0"
    "on_spinBoxTo_valueChanged(int)\0"
    "on_spinBoxFrom_valueChanged(int)\0"
};

const QMetaObject GuiTransactionPrinter::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiTransactionPrinter,
      qt_meta_data_GuiTransactionPrinter, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiTransactionPrinter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiTransactionPrinter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiTransactionPrinter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiTransactionPrinter))
        return static_cast<void*>(const_cast< GuiTransactionPrinter*>(this));
    if (!strcmp(_clname, "Ui::guiTransactionPrinterBase"))
        return static_cast< Ui::guiTransactionPrinterBase*>(const_cast< GuiTransactionPrinter*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiTransactionPrinter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: helpRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: { bool _r = on_buttonPrinterSetup_clicked();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: initPrinter(); break;
        case 4: on_buttonColor1_clicked(); break;
        case 5: on_buttonColor2_clicked(); break;
        case 6: on_buttonShowPrintOptions_clicked(); break;
        case 7: on_buttonFont_clicked(); break;
        case 8: on_buttonMarginsOk_clicked(); break;
        case 9: on_comboBoxBorderWidth_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: on_buttonHelp_clicked(); break;
        case 11: setTransactionList((*reinterpret_cast< banking::TransactionList*(*)>(_a[1]))); break;
        case 12: on_buttonPrint_clicked(); break;
        case 13: on_buttonBack_clicked(); break;
        case 14: on_radioButtonRange_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: on_spinBoxTo_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: on_spinBoxFrom_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void GuiTransactionPrinter::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiTransactionPrinter::helpRequest(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
