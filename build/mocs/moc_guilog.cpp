/****************************************************************************
** Meta object code from reading C++ file 'guilog.h'
**
** Created: Sun Jun 19 13:28:55 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guilog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guilog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiLog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      23,    7,    7,    7, 0x0a,
      42,    7,    7,    7, 0x09,
      60,    7,    7,    7, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GuiLog[] = {
    "GuiLog\0\0closeRequest()\0slotCloseRequest()\0"
    "slotShowDetails()\0slotHideDetails()\0"
};

const QMetaObject GuiLog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiLog,
      qt_meta_data_GuiLog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiLog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiLog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiLog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiLog))
        return static_cast<void*>(const_cast< GuiLog*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiLog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: closeRequest(); break;
        case 1: slotCloseRequest(); break;
        case 2: slotShowDetails(); break;
        case 3: slotHideDetails(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void GuiLog::closeRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
