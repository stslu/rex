/****************************************************************************
** Meta object code from reading C++ file 'QtnGridBandedTableView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../grid/QtnGridBandedTableView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtnGridBandedTableView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Qtitan__GridBandedTableView_t {
    QByteArrayData data[1];
    char stringdata0[28];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridBandedTableView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridBandedTableView_t qt_meta_stringdata_Qtitan__GridBandedTableView = {
    {
QT_MOC_LITERAL(0, 0, 27) // "Qtitan::GridBandedTableView"

    },
    "Qtitan::GridBandedTableView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridBandedTableView[] = {

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

void Qtitan::GridBandedTableView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridBandedTableView::staticMetaObject = {
    { &GridTableView::staticMetaObject, qt_meta_stringdata_Qtitan__GridBandedTableView.data,
      qt_meta_data_Qtitan__GridBandedTableView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridBandedTableView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridBandedTableView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridBandedTableView.stringdata0))
        return static_cast<void*>(const_cast< GridBandedTableView*>(this));
    return GridTableView::qt_metacast(_clname);
}

int Qtitan::GridBandedTableView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridTableView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridTableBand_t {
    QByteArrayData data[3];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridTableBand_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridTableBand_t qt_meta_stringdata_Qtitan__GridTableBand = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Qtitan::GridTableBand"
QT_MOC_LITERAL(1, 22, 7), // "bestFit"
QT_MOC_LITERAL(2, 30, 0) // ""

    },
    "Qtitan::GridTableBand\0bestFit\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridTableBand[] = {

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

void Qtitan::GridTableBand::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridTableBand *_t = static_cast<GridTableBand *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->bestFit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridTableBand::staticMetaObject = {
    { &GridTableColumnBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridTableBand.data,
      qt_meta_data_Qtitan__GridTableBand,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridTableBand::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridTableBand::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridTableBand.stringdata0))
        return static_cast<void*>(const_cast< GridTableBand*>(this));
    return GridTableColumnBase::qt_metacast(_clname);
}

int Qtitan::GridTableBand::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridTableColumnBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridBandedTableColumn_t {
    QByteArrayData data[1];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridBandedTableColumn_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridBandedTableColumn_t qt_meta_stringdata_Qtitan__GridBandedTableColumn = {
    {
QT_MOC_LITERAL(0, 0, 29) // "Qtitan::GridBandedTableColumn"

    },
    "Qtitan::GridBandedTableColumn"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridBandedTableColumn[] = {

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

void Qtitan::GridBandedTableColumn::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridBandedTableColumn::staticMetaObject = {
    { &GridTableColumn::staticMetaObject, qt_meta_stringdata_Qtitan__GridBandedTableColumn.data,
      qt_meta_data_Qtitan__GridBandedTableColumn,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridBandedTableColumn::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridBandedTableColumn::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridBandedTableColumn.stringdata0))
        return static_cast<void*>(const_cast< GridBandedTableColumn*>(this));
    return GridTableColumn::qt_metacast(_clname);
}

int Qtitan::GridBandedTableColumn::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridTableColumn::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
