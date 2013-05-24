/****************************************************************************
** Meta object code from reading C++ file 'guihelpwindow.h'
**
** Created: Sun Jun 19 13:28:58 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guihelpwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guihelpwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiHelpWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      27,   14,   14,   14, 0x08,
      51,   14,   14,   14, 0x08,
      75,   14,   14,   14, 0x08,
     102,   14,   14,   14, 0x08,
     137,  127,   14,   14, 0x08,
     175,  127,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GuiHelpWindow[] = {
    "GuiHelpWindow\0\0cancelled()\0"
    "on_buttonHome_clicked()\0on_buttonBack_clicked()\0"
    "on_buttonForward_clicked()\0"
    "on_buttonClose_clicked()\0available\0"
    "on_textBrowser_forwardAvailable(bool)\0"
    "on_textBrowser_backwardAvailable(bool)\0"
};

const QMetaObject GuiHelpWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiHelpWindow,
      qt_meta_data_GuiHelpWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiHelpWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiHelpWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiHelpWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiHelpWindow))
        return static_cast<void*>(const_cast< GuiHelpWindow*>(this));
    if (!strcmp(_clname, "Ui::GuiHelpWindowBase"))
        return static_cast< Ui::GuiHelpWindowBase*>(const_cast< GuiHelpWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiHelpWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cancelled(); break;
        case 1: on_buttonHome_clicked(); break;
        case 2: on_buttonBack_clicked(); break;
        case 3: on_buttonForward_clicked(); break;
        case 4: on_buttonClose_clicked(); break;
        case 5: on_textBrowser_forwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: on_textBrowser_backwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void GuiHelpWindow::cancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
