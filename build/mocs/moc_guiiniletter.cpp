/****************************************************************************
** Meta object code from reading C++ file 'guiiniletter.h'
**
** Created: Sat Jun 25 13:24:23 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiiniletter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiiniletter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiIniLetter[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      27,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   13,   13,   13, 0x0a,
      61,   13,   13,   13, 0x08,
      89,   13,   13,   13, 0x08,
     121,   13,   13,   13, 0x08,
     151,   13,   13,   13, 0x08,
     178,   13,   13,   13, 0x08,
     204,   13,   13,   13, 0x08,
     236,   13,   13,   13, 0x08,
     255,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GuiIniLetter[] = {
    "GuiIniLetter\0\0terminated()\0"
    "addPrinterRequest()\0initPrinter()\0"
    "on_buttonPrintNow_clicked()\0"
    "on_buttonPrinterSetup_clicked()\0"
    "on_buttonAddPrinter_clicked()\0"
    "on_buttonPreview_clicked()\0"
    "on_buttonCancel_clicked()\0"
    "on_buttonSkipPrinting_clicked()\0"
    "showReadableText()\0showPreviewText()\0"
};

const QMetaObject GuiIniLetter::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiIniLetter,
      qt_meta_data_GuiIniLetter, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiIniLetter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiIniLetter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiIniLetter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiIniLetter))
        return static_cast<void*>(const_cast< GuiIniLetter*>(this));
    if (!strcmp(_clname, "Ui::GuiIniLetter"))
        return static_cast< Ui::GuiIniLetter*>(const_cast< GuiIniLetter*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiIniLetter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: terminated(); break;
        case 1: addPrinterRequest(); break;
        case 2: initPrinter(); break;
        case 3: on_buttonPrintNow_clicked(); break;
        case 4: on_buttonPrinterSetup_clicked(); break;
        case 5: on_buttonAddPrinter_clicked(); break;
        case 6: on_buttonPreview_clicked(); break;
        case 7: on_buttonCancel_clicked(); break;
        case 8: on_buttonSkipPrinting_clicked(); break;
        case 9: showReadableText(); break;
        case 10: showPreviewText(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void GuiIniLetter::terminated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiIniLetter::addPrinterRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE