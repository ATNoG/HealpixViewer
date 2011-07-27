/****************************************************************************
** Meta object code from reading C++ file 'workspace.h'
**
** Created: Wed Jul 27 12:23:16 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "workspace.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'workspace.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WorkSpace[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      76,   60,   10,   10, 0x0a,
     100,   10,   10,   10, 0x0a,
     119,   10,   10,   10, 0x0a,
     135,  132,   10,   10, 0x0a,
     177,  163,   10,   10, 0x0a,
     215,   10,   10,   10, 0x0a,
     236,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WorkSpace[] = {
    "WorkSpace\0\0e,type,source\0"
    "syncNeeded(QEvent*,int,MapViewer*)\0"
    "numberViewports\0configureWorkspace(int)\0"
    "changeToMollview()\0changeTo3D()\0on\0"
    "changeSynchronization(bool)\0e,type,viewer\0"
    "syncViewports(QEvent*,int,MapViewer*)\0"
    "selectAllViewports()\0deselectAllViewports()\0"
};

const QMetaObject WorkSpace::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WorkSpace,
      qt_meta_data_WorkSpace, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WorkSpace::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WorkSpace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WorkSpace::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WorkSpace))
        return static_cast<void*>(const_cast< WorkSpace*>(this));
    return QWidget::qt_metacast(_clname);
}

int WorkSpace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: syncNeeded((*reinterpret_cast< QEvent*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< MapViewer*(*)>(_a[3]))); break;
        case 1: configureWorkspace((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: changeToMollview(); break;
        case 3: changeTo3D(); break;
        case 4: changeSynchronization((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: syncViewports((*reinterpret_cast< QEvent*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< MapViewer*(*)>(_a[3]))); break;
        case 6: selectAllViewports(); break;
        case 7: deselectAllViewports(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void WorkSpace::syncNeeded(QEvent * _t1, int _t2, MapViewer * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
