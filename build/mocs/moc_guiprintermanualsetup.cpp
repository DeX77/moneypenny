/****************************************************************************
** Meta object code from reading C++ file 'guiprintermanualsetup.h'
**
** Created: Sun Jun 19 13:28:59 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiprintermanualsetup.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiprintermanualsetup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiPrinterManualSetup[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,
      35,   30,   22,   22, 0x05,
      56,   30,   22,   22, 0x05,
      79,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
     101,   98,   22,   22, 0x0a,
     129,  124,   22,   22, 0x09,
     182,   22,   22,   22, 0x09,
     206,  124,   22,   22, 0x09,
     265,  124,   22,   22, 0x09,
     317,   22,   22,   22, 0x09,
     350,  341,   22,   22, 0x09,
     386,   22,   22,   22, 0x09,
     419,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiPrinterManualSetup[] = {
    "GuiPrinterManualSetup\0\0back()\0page\0"
    "helpRequest(QString)\0showSmallHelp(QString)\0"
    "printerInstalled()\0se\0showEvent(QShowEvent*)\0"
    "item\0on_listWidgetDrivers_itemActivated(QListWidgetItem*)\0"
    "on_buttonSave_clicked()\0"
    "on_listWidgetManufacturers_itemActivated(QListWidgetItem*)\0"
    "on_listWidgetModels_itemActivated(QListWidgetItem*)\0"
    "on_buttonHelp_clicked()\0itemtext\0"
    "on_comboBoxPorts_activated(QString)\0"
    "on_buttonDriverDetails_clicked()\0"
    "on_buttonBack_clicked()\0"
};

const QMetaObject GuiPrinterManualSetup::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiPrinterManualSetup,
      qt_meta_data_GuiPrinterManualSetup, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiPrinterManualSetup::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiPrinterManualSetup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiPrinterManualSetup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiPrinterManualSetup))
        return static_cast<void*>(const_cast< GuiPrinterManualSetup*>(this));
    if (!strcmp(_clname, "Ui::GuiPrinterManualSetupBase"))
        return static_cast< Ui::GuiPrinterManualSetupBase*>(const_cast< GuiPrinterManualSetup*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiPrinterManualSetup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: helpRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: showSmallHelp((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: printerInstalled(); break;
        case 4: showEvent((*reinterpret_cast< QShowEvent*(*)>(_a[1]))); break;
        case 5: on_listWidgetDrivers_itemActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 6: on_buttonSave_clicked(); break;
        case 7: on_listWidgetManufacturers_itemActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 8: on_listWidgetModels_itemActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 9: on_buttonHelp_clicked(); break;
        case 10: on_comboBoxPorts_activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: on_buttonDriverDetails_clicked(); break;
        case 12: on_buttonBack_clicked(); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void GuiPrinterManualSetup::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiPrinterManualSetup::helpRequest(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GuiPrinterManualSetup::showSmallHelp(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GuiPrinterManualSetup::printerInstalled()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
