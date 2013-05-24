/****************************************************************************
** Meta object code from reading C++ file 'guiimport.h'
**
** Created: Sat Jun 25 13:26:37 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guiimport.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guiimport.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiImport[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      18,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   10,   10,   10, 0x0a,
      63,   10,   10,   10, 0x0a,
      87,   10,   10,   10, 0x0a,
     122,  113,   10,   10, 0x0a,
     177,  164,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiImport[] = {
    "GuiImport\0\0back()\0importSuccessful()\0"
    "on_buttonImport_clicked()\0"
    "on_buttonBack_clicked()\0"
    "on_buttonBrowse_clicked()\0transfer\0"
    "slotCreateTransferJob(banking::Transfer*)\0"
    "override,msg\0slotOverridableErrorDetected(bool*,QString)\0"
};

const QMetaObject GuiImport::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiImport,
      qt_meta_data_GuiImport, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiImport::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiImport::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiImport::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiImport))
        return static_cast<void*>(const_cast< GuiImport*>(this));
    if (!strcmp(_clname, "Ui::GuiImportBase"))
        return static_cast< Ui::GuiImportBase*>(const_cast< GuiImport*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiImport::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: importSuccessful(); break;
        case 2: on_buttonImport_clicked(); break;
        case 3: on_buttonBack_clicked(); break;
        case 4: on_buttonBrowse_clicked(); break;
        case 5: slotCreateTransferJob((*reinterpret_cast< banking::Transfer*(*)>(_a[1]))); break;
        case 6: slotOverridableErrorDetected((*reinterpret_cast< bool*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void GuiImport::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiImport::importSuccessful()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
