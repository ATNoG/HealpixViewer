/****************************************************************************
** Meta object code from reading C++ file 'workspace.h'
**
** Created: Mon Jun 6 10:59:34 2011
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
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      27,   11,   10,   10, 0x0a,
      51,   10,   10,   10, 0x0a,
      70,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WorkSpace[] = {
    "WorkSpace\0\0numberViewports\0"
    "configureWorkspace(int)\0changeToMollview()\0"
    "changeTo3D()\0"
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
        case 0: configureWorkspace((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: changeToMollview(); break;
        case 2: changeTo3D(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
