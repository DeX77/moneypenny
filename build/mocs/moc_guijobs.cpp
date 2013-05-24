/****************************************************************************
** Meta object code from reading C++ file 'guijobs.h'
**
** Created: Sun Jun 19 13:28:58 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guijobs.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guijobs.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiJobs[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,    9,    8,    8, 0x05,
      40,    8,    8,    8, 0x05,
      58,   54,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      94,    8,    8,    8, 0x0a,
     121,    8,    8,    8, 0x0a,
     150,    8,    8,    8, 0x0a,
     183,    8,    8,    8, 0x0a,
     211,    8,    8,    8, 0x0a,
     251,  244,    8,    8, 0x0a,
     275,   54,    8,    8, 0x0a,
     325,  317,    8,    8, 0x0a,
     371,  317,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiJobs[] = {
    "GuiJobs\0\0,,\0sendNow(bool*,QString,bool)\0"
    "jobsCreated()\0job\0editTransferJob(banking::Transfer*)\0"
    "on_buttonEditJob_clicked()\0"
    "on_buttonDeleteJob_clicked()\0"
    "on_buttonDeleteAllJobs_clicked()\0"
    "on_buttonSendJobs_clicked()\0"
    "on_buttonSaveAsDTAFile_clicked()\0"
    "enable\0setButtonsEnabled(bool)\0"
    "slotUpdateJobStatus(banking::BankingJob*)\0"
    "row,job\0slotSentTransferJob(int,banking::BankingJob*)\0"
    "slotSentAdminJob(int,banking::BankingJob*)\0"
};

const QMetaObject GuiJobs::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiJobs,
      qt_meta_data_GuiJobs, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiJobs::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiJobs::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiJobs::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiJobs))
        return static_cast<void*>(const_cast< GuiJobs*>(this));
    if (!strcmp(_clname, "Ui::GuiJobs"))
        return static_cast< Ui::GuiJobs*>(const_cast< GuiJobs*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiJobs::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sendNow((*reinterpret_cast< bool*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: jobsCreated(); break;
        case 2: editTransferJob((*reinterpret_cast< banking::Transfer*(*)>(_a[1]))); break;
        case 3: on_buttonEditJob_clicked(); break;
        case 4: on_buttonDeleteJob_clicked(); break;
        case 5: on_buttonDeleteAllJobs_clicked(); break;
        case 6: on_buttonSendJobs_clicked(); break;
        case 7: on_buttonSaveAsDTAFile_clicked(); break;
        case 8: setButtonsEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: slotUpdateJobStatus((*reinterpret_cast< banking::BankingJob*(*)>(_a[1]))); break;
        case 10: slotSentTransferJob((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< banking::BankingJob*(*)>(_a[2]))); break;
        case 11: slotSentAdminJob((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< banking::BankingJob*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void GuiJobs::sendNow(bool * _t1, const QString & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GuiJobs::jobsCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GuiJobs::editTransferJob(banking::Transfer * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
