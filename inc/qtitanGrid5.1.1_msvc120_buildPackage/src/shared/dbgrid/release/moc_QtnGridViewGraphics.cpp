/****************************************************************************
** Meta object code from reading C++ file 'QtnGridViewGraphics.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../grid/QtnGridViewGraphics.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtnGridViewGraphics.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Qtitan__GridViewLayoutPrimitive_t {
    QByteArrayData data[3];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridViewLayoutPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridViewLayoutPrimitive_t qt_meta_stringdata_Qtitan__GridViewLayoutPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 31), // "Qtitan::GridViewLayoutPrimitive"
QT_MOC_LITERAL(1, 32, 19), // "footerUpdateTimeout"
QT_MOC_LITERAL(2, 52, 0) // ""

    },
    "Qtitan::GridViewLayoutPrimitive\0"
    "footerUpdateTimeout\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridViewLayoutPrimitive[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Qtitan::GridViewLayoutPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridViewLayoutPrimitive *_t = static_cast<GridViewLayoutPrimitive *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->footerUpdateTimeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridViewLayoutPrimitive::staticMetaObject = {
    { &ScrollableGraphicPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridViewLayoutPrimitive.data,
      qt_meta_data_Qtitan__GridViewLayoutPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridViewLayoutPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridViewLayoutPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridViewLayoutPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridViewLayoutPrimitive*>(this));
    return ScrollableGraphicPrimitive::qt_metacast(_clname);
}

int Qtitan::GridViewLayoutPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ScrollableGraphicPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridViewPrimitiveBase_t {
    QByteArrayData data[3];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridViewPrimitiveBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridViewPrimitiveBase_t qt_meta_stringdata_Qtitan__GridViewPrimitiveBase = {
    {
QT_MOC_LITERAL(0, 0, 29), // "Qtitan::GridViewPrimitiveBase"
QT_MOC_LITERAL(1, 30, 23), // "updateRowsScrollTimeout"
QT_MOC_LITERAL(2, 54, 0) // ""

    },
    "Qtitan::GridViewPrimitiveBase\0"
    "updateRowsScrollTimeout\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridViewPrimitiveBase[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Qtitan::GridViewPrimitiveBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridViewPrimitiveBase *_t = static_cast<GridViewPrimitiveBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateRowsScrollTimeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridViewPrimitiveBase::staticMetaObject = {
    { &GridViewLayoutPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridViewPrimitiveBase.data,
      qt_meta_data_Qtitan__GridViewPrimitiveBase,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridViewPrimitiveBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridViewPrimitiveBase::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridViewPrimitiveBase.stringdata0))
        return static_cast<void*>(const_cast< GridViewPrimitiveBase*>(this));
    return GridViewLayoutPrimitive::qt_metacast(_clname);
}

int Qtitan::GridViewPrimitiveBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridViewLayoutPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridTableViewPrimitive_t {
    QByteArrayData data[1];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridTableViewPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridTableViewPrimitive_t qt_meta_stringdata_Qtitan__GridTableViewPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 30) // "Qtitan::GridTableViewPrimitive"

    },
    "Qtitan::GridTableViewPrimitive"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridTableViewPrimitive[] = {

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

void Qtitan::GridTableViewPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridTableViewPrimitive::staticMetaObject = {
    { &GridViewPrimitiveBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridTableViewPrimitive.data,
      qt_meta_data_Qtitan__GridTableViewPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridTableViewPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridTableViewPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridTableViewPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridTableViewPrimitive*>(this));
    return GridViewPrimitiveBase::qt_metacast(_clname);
}

int Qtitan::GridTableViewPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridViewPrimitiveBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridCardViewPrimitive_t {
    QByteArrayData data[1];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridCardViewPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridCardViewPrimitive_t qt_meta_stringdata_Qtitan__GridCardViewPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 29) // "Qtitan::GridCardViewPrimitive"

    },
    "Qtitan::GridCardViewPrimitive"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridCardViewPrimitive[] = {

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

void Qtitan::GridCardViewPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridCardViewPrimitive::staticMetaObject = {
    { &GridViewPrimitiveBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridCardViewPrimitive.data,
      qt_meta_data_Qtitan__GridCardViewPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridCardViewPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridCardViewPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridCardViewPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridCardViewPrimitive*>(this));
    return GridViewPrimitiveBase::qt_metacast(_clname);
}

int Qtitan::GridCardViewPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridViewPrimitiveBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
