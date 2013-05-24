/****************************************************************************
** Meta object code from reading C++ file 'guitools.h'
**
** Created: Sat Jun 25 13:24:23 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/guitools.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guitools.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiTools[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: signature, parameters, type, tag, flags
      44,   10,    9,    9, 0x05,
     109,    9,    9,    9, 0x05,
     139,  130,    9,    9, 0x05,
     162,    9,    9,    9, 0x05,
     186,  173,    9,    9, 0x05,
     208,    9,    9,    9, 0x05,
     226,  215,    9,    9, 0x05,
     246,    9,    9,    9, 0x05,
     273,    9,    9,    9, 0x05,
     302,    9,    9,    9, 0x05,
     319,    9,    9,    9, 0x05,
     336,    9,    9,    9, 0x05,
     353,    9,    9,    9, 0x05,
     360,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     376,    9,    9,    9, 0x0a,
     407,    9,    9,    9, 0x0a,
     438,    9,    9,    9, 0x0a,
     467,    9,    9,    9, 0x0a,
     496,    9,    9,    9, 0x0a,
     530,    9,    9,    9, 0x0a,
     557,    9,    9,    9, 0x0a,
     581,    9,    9,    9, 0x0a,
     615,  609,    9,    9, 0x0a,
     653,    9,    9,    9, 0x0a,
     677,    9,    9,    9, 0x0a,
     710,    9,    9,    9, 0x0a,
     737,    9,    9,    9, 0x0a,
     769,    9,    9,    9, 0x0a,
     815,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GuiTools[] = {
    "GuiTools\0\0showSuccessMessage,url,port,state\0"
    "testInternet(bool,QString,int,HoBIT_Connector::ConnectionState*)\0"
    "helpRequest(QString)\0fileName\0"
    "importRDH2Key(QString)\0sqlquery()\0"
    "autoShutdown\0setShutdownMode(bool)\0"
    "back()\0newTimeOut\0timeOutChanged(int)\0"
    "saveKnoppixConfiguration()\0"
    "buildPrinterDriverDatabase()\0"
    "deleteLogFiles()\0raiseWidget(int)\0"
    "refreshRequest()\0send()\0showIniLetter()\0"
    "on_buttonGetSystemId_clicked()\0"
    "on_buttonGetAccounts_clicked()\0"
    "on_buttonKeyChange_clicked()\0"
    "on_buttonIniLetter_clicked()\0"
    "on_buttonConnectionTest_clicked()\0"
    "on_buttonConsole_clicked()\0"
    "on_buttonHelp_clicked()\0"
    "on_buttonSQLQuery_clicked()\0state\0"
    "on_checkBoxShutdown_stateChanged(int)\0"
    "on_buttonBack_clicked()\0"
    "on_lineEditTimeOut_textChanged()\0"
    "on_buttonGetKeys_clicked()\0"
    "on_buttonSaveOsConfig_clicked()\0"
    "on_buttonBuildPrinterDriverDatabase_clicked()\0"
    "on_buttonDeleteLogFiles_clicked()\0"
};

const QMetaObject GuiTools::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GuiTools,
      qt_meta_data_GuiTools, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiTools::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiTools::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiTools::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiTools))
        return static_cast<void*>(const_cast< GuiTools*>(this));
    if (!strcmp(_clname, "Ui::GuiTools"))
        return static_cast< Ui::GuiTools*>(const_cast< GuiTools*>(this));
    return QWidget::qt_metacast(_clname);
}

int GuiTools::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: testInternet((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< HoBIT_Connector::ConnectionState*(*)>(_a[4]))); break;
        case 1: helpRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: importRDH2Key((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: sqlquery(); break;
        case 4: setShutdownMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: back(); break;
        case 6: timeOutChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: saveKnoppixConfiguration(); break;
        case 8: buildPrinterDriverDatabase(); break;
        case 9: deleteLogFiles(); break;
        case 10: raiseWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: refreshRequest(); break;
        case 12: send(); break;
        case 13: showIniLetter(); break;
        case 14: on_buttonGetSystemId_clicked(); break;
        case 15: on_buttonGetAccounts_clicked(); break;
        case 16: on_buttonKeyChange_clicked(); break;
        case 17: on_buttonIniLetter_clicked(); break;
        case 18: on_buttonConnectionTest_clicked(); break;
        case 19: on_buttonConsole_clicked(); break;
        case 20: on_buttonHelp_clicked(); break;
        case 21: on_buttonSQLQuery_clicked(); break;
        case 22: on_checkBoxShutdown_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: on_buttonBack_clicked(); break;
        case 24: on_lineEditTimeOut_textChanged(); break;
        case 25: on_buttonGetKeys_clicked(); break;
        case 26: on_buttonSaveOsConfig_clicked(); break;
        case 27: on_buttonBuildPrinterDriverDatabase_clicked(); break;
        case 28: on_buttonDeleteLogFiles_clicked(); break;
        default: ;
        }
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void GuiTools::testInternet(bool _t1, const QString & _t2, int _t3, HoBIT_Connector::ConnectionState * _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GuiTools::helpRequest(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GuiTools::importRDH2Key(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GuiTools::sqlquery()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void GuiTools::setShutdownMode(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GuiTools::back()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void GuiTools::timeOutChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void GuiTools::saveKnoppixConfiguration()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void GuiTools::buildPrinterDriverDatabase()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void GuiTools::deleteLogFiles()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void GuiTools::raiseWidget(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void GuiTools::refreshRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void GuiTools::send()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void GuiTools::showIniLetter()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}
QT_END_MOC_NAMESPACE
