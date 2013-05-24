/****************************************************************************
** Meta object code from reading C++ file 'guiprinterautosetup.h'
**
** Created: Sun Jun 19 13:28:59 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiprinterautosetup.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiprinterautosetup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiPrinterAutoSetup[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,
      33,   28,   20,   20, 0x05,
      54,   28,   20,   20, 0x05,
      90,   77,   20,   20, 0x05,
     107,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
     129,  126,   20,   20, 0x0a,
     152,   20,   20,   20, 0x09,
     176,   20,   20,   20, 0x09,
     221,  200,   20,   20, 0x09,
     296,  200,   20,   20, 0x09,
     372,   20,   20,   20, 0x09,
     405,  399,   20,   20, 0x09,
     449,   20,   20,   20, 0x09,
     482,   20,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiPrinterAutoSetup[] = {
    "GuiPrinterAutoSetup\0\0back()\0page\0"
    "helpRequest(QString)\0showSmallHelp(QString)\0"
    "widgetNumber\0raiseWidget(int)\0"
    "printerInstalled()\0se\0showEvent(QShowEvent*)\0"
    "on_buttonSave_clicked()\0on_buttonHelp_clicked()\0"
    "currentItem,previous\0"
    "on_listWidgetDrivers_currentItemChanged(QListWidgetItem*,QListWidgetIt"
    "em*)\0"
    "on_listWidgetPrinters_currentItemChanged(QListWidgetItem*,QListWidgetI"
    "tem*)\0"
    "on_buttonRestart_clicked()\0state\0"
    "on_checkBoxShowAllDevices_stateChanged(int)\0"
    "on_buttonDriverDetails_clicked()\0"
    "on_buttonManualSetup_clicked()\0"
};

const QMetaObject GuiPrinterAutoSetup::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiPrinterAutoSetup,
      qt_meta_data_GuiPrinterAutoSetup, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiPrinterAutoSetup::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiPrinterAutoSetup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiPrinterAutoSetup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiPrinterAutoSetup))
        return static_cast<void*>(const_cast< GuiPrinterAutoSetup*>(this));
    if (!strcmp(_clname, "Ui::GuiPrinterAutoSetupBase"))
        return static_cast< Ui::GuiPrinterAutoSetupBase*>(const_cast< GuiPrinterAutoSetup*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiPrinterAutoSetup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: helpRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: showSmallHelp((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: raiseWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: printerInstalled(); break;
        case 5: showEvent((*reinterpret_cast< QShowEvent*(*)>(_a[1]))); break;
        case 6: on_buttonSave_clicked(); break;
        case 7: on_buttonHelp_clicked(); break;
        case 8: on_listWidgetDrivers_currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 9: on_listWidgetPrinters_currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 10: on_buttonRestart_clicked(); break;
        case 11: on_checkBoxShowAllDevices_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: on_buttonDriverDetails_clicked(); break;
        case 13: on_buttonManualSetup_clicked(); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void GuiPrinterAutoSetup::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiPrinterAutoSetup::helpRequest(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GuiPrinterAutoSetup::showSmallHelp(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GuiPrinterAutoSetup::raiseWidget(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GuiPrinterAutoSetup::printerInstalled()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
