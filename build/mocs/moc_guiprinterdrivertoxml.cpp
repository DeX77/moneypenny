/****************************************************************************
** Meta object code from reading C++ file 'guiprinterdrivertoxml.h'
**
** Created: Sun Jun 19 13:28:58 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiprinterdrivertoxml.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiprinterdrivertoxml.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiPrinterDriverToXml[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   22,   22,   22, 0x0a,
      55,   53,   22,   22, 0x0a,
      80,   22,   22,   22, 0x09,
     105,   22,   22,   22, 0x09,
     139,   22,   22,   22, 0x09,
     172,   22,   22,   22, 0x09,
     197,   22,   22,   22, 0x09,
     221,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiPrinterDriverToXml[] = {
    "GuiPrinterDriverToXml\0\0back()\0"
    "showEvent(QShowEvent*)\0e\0"
    "closeEvent(QCloseEvent*)\0"
    "on_buttonBuild_clicked()\0"
    "on_buttonBrowseSavePath_clicked()\0"
    "on_buttonBrowseCupsDir_clicked()\0"
    "on_buttonAbort_clicked()\0"
    "on_buttonBack_clicked()\0on_buttonSave_clicked()\0"
};

const QMetaObject GuiPrinterDriverToXml::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiPrinterDriverToXml,
      qt_meta_data_GuiPrinterDriverToXml, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiPrinterDriverToXml::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiPrinterDriverToXml::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiPrinterDriverToXml::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiPrinterDriverToXml))
        return static_cast<void*>(const_cast< GuiPrinterDriverToXml*>(this));
    if (!strcmp(_clname, "Ui::GuiPrinterDriverToXmlBase"))
        return static_cast< Ui::GuiPrinterDriverToXmlBase*>(const_cast< GuiPrinterDriverToXml*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiPrinterDriverToXml::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: showEvent((*reinterpret_cast< QShowEvent*(*)>(_a[1]))); break;
        case 2: closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 3: on_buttonBuild_clicked(); break;
        case 4: on_buttonBrowseSavePath_clicked(); break;
        case 5: on_buttonBrowseCupsDir_clicked(); break;
        case 6: on_buttonAbort_clicked(); break;
        case 7: on_buttonBack_clicked(); break;
        case 8: on_buttonSave_clicked(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void GuiPrinterDriverToXml::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
