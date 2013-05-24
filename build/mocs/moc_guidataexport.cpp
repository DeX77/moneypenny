/****************************************************************************
** Meta object code from reading C++ file 'guidataexport.h'
**
** Created: Sun Jun 19 13:28:58 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guidataexport.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guidataexport.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiDataExport[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      22,   14,   14,   14, 0x0a,
      49,   14,   14,   14, 0x0a,
      79,   14,   14,   14, 0x0a,
     103,   14,   14,   14, 0x0a,
     135,  129,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiDataExport[] = {
    "GuiDataExport\0\0back()\0on_buttonDetails_clicked()\0"
    "on_buttonSelectFile_clicked()\0"
    "on_buttonBack_clicked()\0"
    "on_buttonExport_clicked()\0index\0"
    "on_comboBoxExportType_currentIndexChanged(int)\0"
};

const QMetaObject GuiDataExport::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiDataExport,
      qt_meta_data_GuiDataExport, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiDataExport::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiDataExport::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiDataExport::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiDataExport))
        return static_cast<void*>(const_cast< GuiDataExport*>(this));
    if (!strcmp(_clname, "Ui::GuiDataExportBase"))
        return static_cast< Ui::GuiDataExportBase*>(const_cast< GuiDataExport*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiDataExport::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: back(); break;
        case 1: on_buttonDetails_clicked(); break;
        case 2: on_buttonSelectFile_clicked(); break;
        case 3: on_buttonBack_clicked(); break;
        case 4: on_buttonExport_clicked(); break;
        case 5: on_comboBoxExportType_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GuiDataExport::back()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
