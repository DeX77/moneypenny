/****************************************************************************
** Meta object code from reading C++ file 'guitransferform.h'
**
** Created: Sat Jun 25 13:26:36 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guitransferform.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guitransferform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiTransferForm[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      29,   16,   16,   16, 0x05,
      51,   46,   16,   16, 0x05,
     100,   97,   16,   16, 0x05,
     135,  128,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     152,   16,   16,   16, 0x0a,
     179,   16,   16,   16, 0x0a,
     213,   16,   16,   16, 0x0a,
     245,   16,   16,   16, 0x0a,
     254,   16,   16,   16, 0x0a,
     288,  283,   16,   16, 0x0a,
     330,  283,   16,   16, 0x0a,
     377,  283,   16,   16, 0x0a,
     423,  283,   16,   16, 0x0a,
     464,  283,   16,   16, 0x0a,
     511,  505,   16,   16, 0x0a,
     555,  547,   16,   16, 0x0a,
     579,  547,   16,   16, 0x0a,
     604,   16,   16,   16, 0x0a,
     619,   16,   16,   16, 0x0a,
     643,  634,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiTransferForm[] = {
    "GuiTransferForm\0\0cancelled()\0"
    "refreshRequest()\0type\0"
    "templateRequest(banking::BankingJob::JobType)\0"
    ",,\0sendNow(bool*,QString,bool)\0PageId\0"
    "raiseWidget(int)\0on_buttonExecute_clicked()\0"
    "on_buttonSaveAsTemplate_clicked()\0"
    "on_buttonFromTemplate_clicked()\0"
    "reject()\0on_buttonClearForm_clicked()\0"
    "text\0on_lineEditOtherName_textChanged(QString)\0"
    "on_lineEditOtherAccountId_textChanged(QString)\0"
    "on_lineEditOtherBankCode_textChanged(QString)\0"
    "on_lineEditPurpose1_textChanged(QString)\0"
    "on_lineEditPurpose2_textChanged(QString)\0"
    "value\0on_scrollBarLines_valueChanged(int)\0"
    "purpose\0setPurpose(QStringList)\0"
    "setPurpose(list<string>)\0slotNextLine()\0"
    "slotPrevLine()\0transfer\0"
    "slotShowTemplateData(banking::Transfer*)\0"
};

const QMetaObject GuiTransferForm::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiTransferForm,
      qt_meta_data_GuiTransferForm, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiTransferForm::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiTransferForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiTransferForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiTransferForm))
        return static_cast<void*>(const_cast< GuiTransferForm*>(this));
    if (!strcmp(_clname, "Ui::GuiTransferForm"))
        return static_cast< Ui::GuiTransferForm*>(const_cast< GuiTransferForm*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiTransferForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cancelled(); break;
        case 1: refreshRequest(); break;
        case 2: templateRequest((*reinterpret_cast< banking::BankingJob::JobType(*)>(_a[1]))); break;
        case 3: sendNow((*reinterpret_cast< bool*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 4: raiseWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: on_buttonExecute_clicked(); break;
        case 6: on_buttonSaveAsTemplate_clicked(); break;
        case 7: on_buttonFromTemplate_clicked(); break;
        case 8: reject(); break;
        case 9: on_buttonClearForm_clicked(); break;
        case 10: on_lineEditOtherName_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: on_lineEditOtherAccountId_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: on_lineEditOtherBankCode_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: on_lineEditPurpose1_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: on_lineEditPurpose2_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: on_scrollBarLines_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: setPurpose((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 17: setPurpose((*reinterpret_cast< const list<string>(*)>(_a[1]))); break;
        case 18: slotNextLine(); break;
        case 19: slotPrevLine(); break;
        case 20: slotShowTemplateData((*reinterpret_cast< banking::Transfer*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void GuiTransferForm::cancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GuiTransferForm::refreshRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GuiTransferForm::templateRequest(banking::BankingJob::JobType _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GuiTransferForm::sendNow(bool * _t1, const QString & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GuiTransferForm::raiseWidget(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
