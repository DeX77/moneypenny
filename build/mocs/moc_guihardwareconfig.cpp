/****************************************************************************
** Meta object code from reading C++ file 'guihardwareconfig.h'
**
** Created: Sun Jun 19 13:28:59 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guihardwareconfig.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guihardwareconfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiHardwareConfig[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      30,   18,   18,   18, 0x05,
      37,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      54,   18,   18,   18, 0x0a,
      79,   18,   18,   18, 0x0a,
     103,   18,   18,   18, 0x0a,
     126,   18,   18,   18, 0x0a,
     149,   18,   18,   18, 0x0a,
     173,   18,   18,   18, 0x0a,
     197,   18,   18,   18, 0x0a,
     221,   18,   18,   18, 0x0a,
     252,   18,   18,   18, 0x0a,
     285,   18,   18,   18, 0x0a,
     309,   18,   18,   18, 0x0a,
     333,   18,   18,   18, 0x0a,
     368,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiHardwareConfig[] = {
    "GuiHardwareConfig\0\0finished()\0back()\0"
    "raiseWidget(int)\0on_buttonModem_clicked()\0"
    "on_buttonIsdn_clicked()\0on_buttonDsl_clicked()\0"
    "on_buttonLan_clicked()\0on_buttonWlan_clicked()\0"
    "on_buttonGprs_clicked()\0on_buttonSave_clicked()\0"
    "on_buttonPrinterAuto_clicked()\0"
    "on_buttonPrinterManual_clicked()\0"
    "on_buttonNext_clicked()\0on_buttonBack_clicked()\0"
    "on_buttonPredefinedModem_clicked()\0"
    "on_buttonPredefinedISDN_clicked()\0"
};

const QMetaObject GuiHardwareConfig::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiHardwareConfig,
      qt_meta_data_GuiHardwareConfig, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiHardwareConfig::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiHardwareConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiHardwareConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiHardwareConfig))
        return static_cast<void*>(const_cast< GuiHardwareConfig*>(this));
    if (!strcmp(_clname, "Ui::GuiHardwareConfigBase"))
        return static_cast< Ui::GuiHardwareConfigBase*>(const_cast< GuiHardwareConfig*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiHardwareConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finished(); break;
        case 1: back(); break;
        case 2: raiseWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: on_buttonModem_clicked(); break;
        case 4: on_buttonIsdn_clicked(); break;
        case 5: on_buttonDsl_clicked(); break;
        case 6: on_buttonLan_clicked(); break;
        case 7: on_buttonWlan_clicked(); break;
        case 8: on_buttonGprs_clicked(); break;
        case 9: on_buttonSave_clicked(); break;
        case 10: on_buttonPrinterAuto_clicked(); break;
        case 11: on_buttonPrinterManual_clicked(); break;
        case 12: on_buttonNext_clicked(); break;
        case 13: on_buttonBack_clicked(); break;
        case 14: on_buttonPredefinedModem_clicked(); break;
        case 15: on_buttonPredefinedISDN_clicked(); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void GuiHardwareConfig::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiHardwareConfig::back()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GuiHardwareConfig::raiseWidget(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
