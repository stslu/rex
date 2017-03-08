/****************************************************************************
** Meta object code from reading C++ file 'QtnGridModelController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../grid/QtnGridModelController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtnGridModelController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Qtitan__GridModelDataBinding_t {
    QByteArrayData data[1];
    char stringdata0[29];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridModelDataBinding_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridModelDataBinding_t qt_meta_stringdata_Qtitan__GridModelDataBinding = {
    {
QT_MOC_LITERAL(0, 0, 28) // "Qtitan::GridModelDataBinding"

    },
    "Qtitan::GridModelDataBinding"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridModelDataBinding[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Qtitan::GridModelDataBinding::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridModelDataBinding::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Qtitan__GridModelDataBinding.data,
      qt_meta_data_Qtitan__GridModelDataBinding,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridModelDataBinding::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridModelDataBinding::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridModelDataBinding.stringdata0))
        return static_cast<void*>(const_cast< GridModelDataBinding*>(this));
    return QObject::qt_metacast(_clname);
}

int Qtitan::GridModelDataBinding::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridModelController_t {
    QByteArrayData data[39];
    char stringdata0[576];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridModelController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridModelController_t qt_meta_stringdata_Qtitan__GridModelController = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Qtitan::GridModelController"
QT_MOC_LITERAL(1, 28, 13), // "selectChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 15), // "aboutToBeSorted"
QT_MOC_LITERAL(4, 59, 5), // "bool&"
QT_MOC_LITERAL(5, 65, 6), // "cancel"
QT_MOC_LITERAL(6, 72, 13), // "sortCompleted"
QT_MOC_LITERAL(7, 86, 12), // "sortProgress"
QT_MOC_LITERAL(8, 99, 5), // "value"
QT_MOC_LITERAL(9, 105, 17), // "aboutToBeFiltered"
QT_MOC_LITERAL(10, 123, 15), // "filterCompleted"
QT_MOC_LITERAL(11, 139, 14), // "filterProgress"
QT_MOC_LITERAL(12, 154, 26), // "aboutToBeSummaryCalculated"
QT_MOC_LITERAL(13, 181, 16), // "summaryCompleted"
QT_MOC_LITERAL(14, 198, 15), // "summaryProgress"
QT_MOC_LITERAL(15, 214, 16), // "aboutToBeChanged"
QT_MOC_LITERAL(16, 231, 7), // "changed"
QT_MOC_LITERAL(17, 239, 20), // "modelColumnsInserted"
QT_MOC_LITERAL(18, 260, 6), // "parent"
QT_MOC_LITERAL(19, 267, 5), // "start"
QT_MOC_LITERAL(20, 273, 3), // "end"
QT_MOC_LITERAL(21, 277, 17), // "modelColumnsMoved"
QT_MOC_LITERAL(22, 295, 12), // "sourceParent"
QT_MOC_LITERAL(23, 308, 11), // "sourceStart"
QT_MOC_LITERAL(24, 320, 9), // "sourceEnd"
QT_MOC_LITERAL(25, 330, 17), // "destinationParent"
QT_MOC_LITERAL(26, 348, 17), // "destinationColumn"
QT_MOC_LITERAL(27, 366, 19), // "modelColumnsRemoved"
QT_MOC_LITERAL(28, 386, 18), // "modelLayoutChanged"
QT_MOC_LITERAL(29, 405, 16), // "modelDataChanged"
QT_MOC_LITERAL(30, 422, 7), // "topLeft"
QT_MOC_LITERAL(31, 430, 11), // "bottomRight"
QT_MOC_LITERAL(32, 442, 26), // "modelRowsAboutToBeInserted"
QT_MOC_LITERAL(33, 469, 25), // "modelRowsAboutToBeRemoved"
QT_MOC_LITERAL(34, 495, 17), // "modelRowsInserted"
QT_MOC_LITERAL(35, 513, 16), // "modelRowsRemoved"
QT_MOC_LITERAL(36, 530, 19), // "modelAboutToBeReset"
QT_MOC_LITERAL(37, 550, 10), // "modelReset"
QT_MOC_LITERAL(38, 561, 14) // "modelDestroyed"

    },
    "Qtitan::GridModelController\0selectChanged\0"
    "\0aboutToBeSorted\0bool&\0cancel\0"
    "sortCompleted\0sortProgress\0value\0"
    "aboutToBeFiltered\0filterCompleted\0"
    "filterProgress\0aboutToBeSummaryCalculated\0"
    "summaryCompleted\0summaryProgress\0"
    "aboutToBeChanged\0changed\0modelColumnsInserted\0"
    "parent\0start\0end\0modelColumnsMoved\0"
    "sourceParent\0sourceStart\0sourceEnd\0"
    "destinationParent\0destinationColumn\0"
    "modelColumnsRemoved\0modelLayoutChanged\0"
    "modelDataChanged\0topLeft\0bottomRight\0"
    "modelRowsAboutToBeInserted\0"
    "modelRowsAboutToBeRemoved\0modelRowsInserted\0"
    "modelRowsRemoved\0modelAboutToBeReset\0"
    "modelReset\0modelDestroyed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridModelController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  134,    2, 0x06 /* Public */,
       3,    1,  135,    2, 0x06 /* Public */,
       6,    0,  138,    2, 0x06 /* Public */,
       7,    1,  139,    2, 0x06 /* Public */,
       9,    1,  142,    2, 0x06 /* Public */,
      10,    0,  145,    2, 0x06 /* Public */,
      11,    1,  146,    2, 0x06 /* Public */,
      12,    1,  149,    2, 0x06 /* Public */,
      13,    0,  152,    2, 0x06 /* Public */,
      14,    1,  153,    2, 0x06 /* Public */,
      15,    0,  156,    2, 0x06 /* Public */,
      16,    0,  157,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    3,  158,    2, 0x09 /* Protected */,
      21,    5,  165,    2, 0x09 /* Protected */,
      27,    3,  176,    2, 0x09 /* Protected */,
      28,    0,  183,    2, 0x09 /* Protected */,
      29,    2,  184,    2, 0x09 /* Protected */,
      32,    3,  189,    2, 0x09 /* Protected */,
      33,    3,  196,    2, 0x09 /* Protected */,
      34,    3,  203,    2, 0x09 /* Protected */,
      35,    3,  210,    2, 0x09 /* Protected */,
      36,    0,  217,    2, 0x09 /* Protected */,
      37,    0,  218,    2, 0x09 /* Protected */,
      38,    0,  219,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,   18,   19,   20,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int, QMetaType::QModelIndex, QMetaType::Int,   22,   23,   24,   25,   26,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,   18,   19,   20,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,   30,   31,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,   18,   19,   20,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,   18,   19,   20,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,   18,   19,   20,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,   18,   19,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Qtitan::GridModelController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridModelController *_t = static_cast<GridModelController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectChanged(); break;
        case 1: _t->aboutToBeSorted((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->sortCompleted(); break;
        case 3: _t->sortProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->aboutToBeFiltered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->filterCompleted(); break;
        case 6: _t->filterProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->aboutToBeSummaryCalculated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->summaryCompleted(); break;
        case 9: _t->summaryProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->aboutToBeChanged(); break;
        case 11: _t->changed(); break;
        case 12: _t->modelColumnsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 13: _t->modelColumnsMoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< const QModelIndex(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 14: _t->modelColumnsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 15: _t->modelLayoutChanged(); break;
        case 16: _t->modelDataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 17: _t->modelRowsAboutToBeInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 18: _t->modelRowsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 19: _t->modelRowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 20: _t->modelRowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 21: _t->modelAboutToBeReset(); break;
        case 22: _t->modelReset(); break;
        case 23: _t->modelDestroyed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GridModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::selectChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (GridModelController::*_t)(bool & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::aboutToBeSorted)) {
                *result = 1;
            }
        }
        {
            typedef void (GridModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::sortCompleted)) {
                *result = 2;
            }
        }
        {
            typedef void (GridModelController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::sortProgress)) {
                *result = 3;
            }
        }
        {
            typedef void (GridModelController::*_t)(bool & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::aboutToBeFiltered)) {
                *result = 4;
            }
        }
        {
            typedef void (GridModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::filterCompleted)) {
                *result = 5;
            }
        }
        {
            typedef void (GridModelController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::filterProgress)) {
                *result = 6;
            }
        }
        {
            typedef void (GridModelController::*_t)(bool & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::aboutToBeSummaryCalculated)) {
                *result = 7;
            }
        }
        {
            typedef void (GridModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::summaryCompleted)) {
                *result = 8;
            }
        }
        {
            typedef void (GridModelController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::summaryProgress)) {
                *result = 9;
            }
        }
        {
            typedef void (GridModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::aboutToBeChanged)) {
                *result = 10;
            }
        }
        {
            typedef void (GridModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridModelController::changed)) {
                *result = 11;
            }
        }
    }
}

const QMetaObject Qtitan::GridModelController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Qtitan__GridModelController.data,
      qt_meta_data_Qtitan__GridModelController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridModelController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridModelController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridModelController.stringdata0))
        return static_cast<void*>(const_cast< GridModelController*>(this));
    return QObject::qt_metacast(_clname);
}

int Qtitan::GridModelController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 24;
    }
    return _id;
}

// SIGNAL 0
void Qtitan::GridModelController::selectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Qtitan::GridModelController::aboutToBeSorted(bool & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Qtitan::GridModelController::sortCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void Qtitan::GridModelController::sortProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Qtitan::GridModelController::aboutToBeFiltered(bool & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Qtitan::GridModelController::filterCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void Qtitan::GridModelController::filterProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Qtitan::GridModelController::aboutToBeSummaryCalculated(bool & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Qtitan::GridModelController::summaryCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 8, Q_NULLPTR);
}

// SIGNAL 9
void Qtitan::GridModelController::summaryProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Qtitan::GridModelController::aboutToBeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, Q_NULLPTR);
}

// SIGNAL 11
void Qtitan::GridModelController::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 11, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
