/****************************************************************************
** Meta object code from reading C++ file 'guitransfertemplates.h'
**
** Created: Sat Jun 25 13:24:22 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guitransfertemplates.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guitransfertemplates.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiTransferTemplates[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   22,   21,   21, 0x05,
      42,   21,   21,   21, 0x05,
      51,   49,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      85,   21,   21,   21, 0x0a,
     108,   21,   21,   21, 0x0a,
     134,   21,   21,   21, 0x0a,
     166,   21,   21,   21, 0x0a,
     191,   21,   21,   21, 0x0a,
     200,   21,   21,   21, 0x0a,
     210,   21,   21,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiTransferTemplates[] = {
    "GuiTransferTemplates\0\0iD\0raiseWidget(int)\0"
    "back()\0t\0raiseTemplate(banking::Transfer*)\0"
    "on_buttonUse_clicked()\0on_buttonDelete_clicked()\0"
    "on_buttonClearPurpose_clicked()\0"
    "on_tableWidget_clicked()\0cancel()\0"
    "refresh()\0"
    "slotSetTypeRadioButton(banking::BankingJob::JobType)\0"
};

const QMetaObject GuiTransferTemplates::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiTransferTemplates,
      qt_meta_data_GuiTransferTemplates, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiTransferTemplates::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiTransferTemplates::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiTransferTemplates::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiTransferTemplates))
        return static_cast<void*>(const_cast< GuiTransferTemplates*>(this));
    if (!strcmp(_clname, "Ui::GuiTransferTemplates"))
        return static_cast< Ui::GuiTransferTemplates*>(const_cast< GuiTransferTemplates*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiTransferTemplates::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: raiseWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: back(); break;
        case 2: raiseTemplate((*reinterpret_cast< banking::Transfer*(*)>(_a[1]))); break;
        case 3: on_buttonUse_clicked(); break;
        case 4: on_buttonDelete_clicked(); break;
        case 5: on_buttonClearPurpose_clicked(); break;
        case 6: on_tableWidget_clicked(); break;
        case 7: cancel(); break;
        case 8: refresh(); break;
        case 9: slotSetTypeRadioButton((*reinterpret_cast< banking::BankingJob::JobType(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void GuiTransferTemplates::raiseWidget(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GuiTransferTemplates::back()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GuiTransferTemplates::raiseTemplate(banking::Transfer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
