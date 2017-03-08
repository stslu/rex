/****************************************************************************
** Meta object code from reading C++ file 'QtnGridTableViewBase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../grid/QtnGridTableViewBase.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtnGridTableViewBase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Qtitan__GridShadeButtonPrimitive_t {
    QByteArrayData data[3];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridShadeButtonPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridShadeButtonPrimitive_t qt_meta_stringdata_Qtitan__GridShadeButtonPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Qtitan::GridShadeButtonPrimitive"
QT_MOC_LITERAL(1, 33, 7), // "clicked"
QT_MOC_LITERAL(2, 41, 0) // ""

    },
    "Qtitan::GridShadeButtonPrimitive\0"
    "clicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridShadeButtonPrimitive[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void Qtitan::GridShadeButtonPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridShadeButtonPrimitive *_t = static_cast<GridShadeButtonPrimitive *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GridShadeButtonPrimitive::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridShadeButtonPrimitive::clicked)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridShadeButtonPrimitive::staticMetaObject = {
    { &GraphicPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridShadeButtonPrimitive.data,
      qt_meta_data_Qtitan__GridShadeButtonPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridShadeButtonPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridShadeButtonPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridShadeButtonPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridShadeButtonPrimitive*>(this));
    return GraphicPrimitive::qt_metacast(_clname);
}

int Qtitan::GridShadeButtonPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphicPrimitive::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Qtitan::GridShadeButtonPrimitive::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_Qtitan__GridTabColumnPrimitiveBase_t {
    QByteArrayData data[1];
    char stringdata0[35];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridTabColumnPrimitiveBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridTabColumnPrimitiveBase_t qt_meta_stringdata_Qtitan__GridTabColumnPrimitiveBase = {
    {
QT_MOC_LITERAL(0, 0, 34) // "Qtitan::GridTabColumnPrimitiv..."

    },
    "Qtitan::GridTabColumnPrimitiveBase"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridTabColumnPrimitiveBase[] = {

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

void Qtitan::GridTabColumnPrimitiveBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridTabColumnPrimitiveBase::staticMetaObject = {
    { &GridShadeButtonPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridTabColumnPrimitiveBase.data,
      qt_meta_data_Qtitan__GridTabColumnPrimitiveBase,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridTabColumnPrimitiveBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridTabColumnPrimitiveBase::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridTabColumnPrimitiveBase.stringdata0))
        return static_cast<void*>(const_cast< GridTabColumnPrimitiveBase*>(this));
    return GridShadeButtonPrimitive::qt_metacast(_clname);
}

int Qtitan::GridTabColumnPrimitiveBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridShadeButtonPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridCardColumnTabPrimitive_t {
    QByteArrayData data[4];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridCardColumnTabPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridCardColumnTabPrimitive_t qt_meta_stringdata_Qtitan__GridCardColumnTabPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 34), // "Qtitan::GridCardColumnTabPrim..."
QT_MOC_LITERAL(1, 35, 15), // "checkBoxClicked"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 7) // "checked"

    },
    "Qtitan::GridCardColumnTabPrimitive\0"
    "checkBoxClicked\0\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridCardColumnTabPrimitive[] = {

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
       1,    1,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void Qtitan::GridCardColumnTabPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridCardColumnTabPrimitive *_t = static_cast<GridCardColumnTabPrimitive *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->checkBoxClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridCardColumnTabPrimitive::staticMetaObject = {
    { &GridTabColumnPrimitiveBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridCardColumnTabPrimitive.data,
      qt_meta_data_Qtitan__GridCardColumnTabPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridCardColumnTabPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridCardColumnTabPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridCardColumnTabPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridCardColumnTabPrimitive*>(this));
    return GridTabColumnPrimitiveBase::qt_metacast(_clname);
}

int Qtitan::GridCardColumnTabPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridTabColumnPrimitiveBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridTableCellPrimitiveAbstract_t {
    QByteArrayData data[1];
    char stringdata0[39];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridTableCellPrimitiveAbstract_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridTableCellPrimitiveAbstract_t qt_meta_stringdata_Qtitan__GridTableCellPrimitiveAbstract = {
    {
QT_MOC_LITERAL(0, 0, 38) // "Qtitan::GridTableCellPrimitiv..."

    },
    "Qtitan::GridTableCellPrimitiveAbstract"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridTableCellPrimitiveAbstract[] = {

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

void Qtitan::GridTableCellPrimitiveAbstract::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridTableCellPrimitiveAbstract::staticMetaObject = {
    { &EmbedGraphicPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridTableCellPrimitiveAbstract.data,
      qt_meta_data_Qtitan__GridTableCellPrimitiveAbstract,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridTableCellPrimitiveAbstract::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridTableCellPrimitiveAbstract::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridTableCellPrimitiveAbstract.stringdata0))
        return static_cast<void*>(const_cast< GridTableCellPrimitiveAbstract*>(this));
    return EmbedGraphicPrimitive::qt_metacast(_clname);
}

int Qtitan::GridTableCellPrimitiveAbstract::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EmbedGraphicPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridTableCellGraphic_t {
    QByteArrayData data[3];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridTableCellGraphic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridTableCellGraphic_t qt_meta_stringdata_Qtitan__GridTableCellGraphic = {
    {
QT_MOC_LITERAL(0, 0, 28), // "Qtitan::GridTableCellGraphic"
QT_MOC_LITERAL(1, 29, 22), // "activateButtonsTimeout"
QT_MOC_LITERAL(2, 52, 0) // ""

    },
    "Qtitan::GridTableCellGraphic\0"
    "activateButtonsTimeout\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridTableCellGraphic[] = {

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

void Qtitan::GridTableCellGraphic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridTableCellGraphic *_t = static_cast<GridTableCellGraphic *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->activateButtonsTimeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridTableCellGraphic::staticMetaObject = {
    { &GridTableCellPrimitiveAbstract::staticMetaObject, qt_meta_stringdata_Qtitan__GridTableCellGraphic.data,
      qt_meta_data_Qtitan__GridTableCellGraphic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridTableCellGraphic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridTableCellGraphic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridTableCellGraphic.stringdata0))
        return static_cast<void*>(const_cast< GridTableCellGraphic*>(this));
    if (!strcmp(_clname, "GridEditorSite"))
        return static_cast< GridEditorSite*>(const_cast< GridTableCellGraphic*>(this));
    return GridTableCellPrimitiveAbstract::qt_metacast(_clname);
}

int Qtitan::GridTableCellGraphic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridTableCellPrimitiveAbstract::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridFieldChooserPrimitive_t {
    QByteArrayData data[1];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridFieldChooserPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridFieldChooserPrimitive_t qt_meta_stringdata_Qtitan__GridFieldChooserPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 33) // "Qtitan::GridFieldChooserPrimi..."

    },
    "Qtitan::GridFieldChooserPrimitive"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridFieldChooserPrimitive[] = {

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

void Qtitan::GridFieldChooserPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridFieldChooserPrimitive::staticMetaObject = {
    { &GridWinShadowedPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridFieldChooserPrimitive.data,
      qt_meta_data_Qtitan__GridFieldChooserPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridFieldChooserPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridFieldChooserPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridFieldChooserPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridFieldChooserPrimitive*>(this));
    return GridWinShadowedPrimitive::qt_metacast(_clname);
}

int Qtitan::GridFieldChooserPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridWinShadowedPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridFindPanelPrimitive_t {
    QByteArrayData data[10];
    char stringdata0[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridFindPanelPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridFindPanelPrimitive_t qt_meta_stringdata_Qtitan__GridFindPanelPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 30), // "Qtitan::GridFindPanelPrimitive"
QT_MOC_LITERAL(1, 31, 19), // "updateLayoutTimeout"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 20), // "caseSensitiveClicked"
QT_MOC_LITERAL(4, 73, 7), // "checked"
QT_MOC_LITERAL(5, 81, 13), // "filterClicked"
QT_MOC_LITERAL(6, 95, 10), // "textEdited"
QT_MOC_LITERAL(7, 106, 4), // "text"
QT_MOC_LITERAL(8, 111, 11), // "backClicked"
QT_MOC_LITERAL(9, 123, 14) // "forwardClicked"

    },
    "Qtitan::GridFindPanelPrimitive\0"
    "updateLayoutTimeout\0\0caseSensitiveClicked\0"
    "checked\0filterClicked\0textEdited\0text\0"
    "backClicked\0forwardClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridFindPanelPrimitive[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    1,   45,    2, 0x08 /* Private */,
       5,    1,   48,    2, 0x08 /* Private */,
       6,    1,   51,    2, 0x08 /* Private */,
       8,    0,   54,    2, 0x0a /* Public */,
       9,    0,   55,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Qtitan::GridFindPanelPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridFindPanelPrimitive *_t = static_cast<GridFindPanelPrimitive *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateLayoutTimeout(); break;
        case 1: _t->caseSensitiveClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->filterClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->backClicked(); break;
        case 5: _t->forwardClicked(); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridFindPanelPrimitive::staticMetaObject = {
    { &GraphicPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridFindPanelPrimitive.data,
      qt_meta_data_Qtitan__GridFindPanelPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridFindPanelPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridFindPanelPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridFindPanelPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridFindPanelPrimitive*>(this));
    return GraphicPrimitive::qt_metacast(_clname);
}

int Qtitan::GridFindPanelPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphicPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridFilterPanelPrimitive_t {
    QByteArrayData data[7];
    char stringdata0[113];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridFilterPanelPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridFilterPanelPrimitive_t qt_meta_stringdata_Qtitan__GridFilterPanelPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Qtitan::GridFilterPanelPrimitive"
QT_MOC_LITERAL(1, 33, 17), // "checkFilterActive"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 7), // "checked"
QT_MOC_LITERAL(4, 60, 17), // "filterTextClicked"
QT_MOC_LITERAL(5, 78, 18), // "closeFilterClicked"
QT_MOC_LITERAL(6, 97, 15) // "autoHideClicked"

    },
    "Qtitan::GridFilterPanelPrimitive\0"
    "checkFilterActive\0\0checked\0filterTextClicked\0"
    "closeFilterClicked\0autoHideClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridFilterPanelPrimitive[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       4,    0,   37,    2, 0x0a /* Public */,
       5,    0,   38,    2, 0x0a /* Public */,
       6,    0,   39,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Qtitan::GridFilterPanelPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridFilterPanelPrimitive *_t = static_cast<GridFilterPanelPrimitive *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->checkFilterActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->filterTextClicked(); break;
        case 2: _t->closeFilterClicked(); break;
        case 3: _t->autoHideClicked(); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridFilterPanelPrimitive::staticMetaObject = {
    { &GraphicPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridFilterPanelPrimitive.data,
      qt_meta_data_Qtitan__GridFilterPanelPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridFilterPanelPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridFilterPanelPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridFilterPanelPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridFilterPanelPrimitive*>(this));
    return GraphicPrimitive::qt_metacast(_clname);
}

int Qtitan::GridFilterPanelPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphicPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridColumnPrimitive_t {
    QByteArrayData data[1];
    char stringdata0[28];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridColumnPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridColumnPrimitive_t qt_meta_stringdata_Qtitan__GridColumnPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 27) // "Qtitan::GridColumnPrimitive"

    },
    "Qtitan::GridColumnPrimitive"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridColumnPrimitive[] = {

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

void Qtitan::GridColumnPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridColumnPrimitive::staticMetaObject = {
    { &GraphicPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridColumnPrimitive.data,
      qt_meta_data_Qtitan__GridColumnPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridColumnPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridColumnPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridColumnPrimitive.stringdata0))
        return static_cast<void*>(const_cast< GridColumnPrimitive*>(this));
    return GraphicPrimitive::qt_metacast(_clname);
}

int Qtitan::GridColumnPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphicPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__QuickCustomizationPopup_t {
    QByteArrayData data[4];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__QuickCustomizationPopup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__QuickCustomizationPopup_t qt_meta_stringdata_Qtitan__QuickCustomizationPopup = {
    {
QT_MOC_LITERAL(0, 0, 31), // "Qtitan::QuickCustomizationPopup"
QT_MOC_LITERAL(1, 32, 18), // "columnStateChanged"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 5) // "state"

    },
    "Qtitan::QuickCustomizationPopup\0"
    "columnStateChanged\0\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__QuickCustomizationPopup[] = {

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
       1,    1,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void Qtitan::QuickCustomizationPopup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QuickCustomizationPopup *_t = static_cast<QuickCustomizationPopup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->columnStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::QuickCustomizationPopup::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Qtitan__QuickCustomizationPopup.data,
      qt_meta_data_Qtitan__QuickCustomizationPopup,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::QuickCustomizationPopup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::QuickCustomizationPopup::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__QuickCustomizationPopup.stringdata0))
        return static_cast<void*>(const_cast< QuickCustomizationPopup*>(this));
    return QWidget::qt_metacast(_clname);
}

int Qtitan::QuickCustomizationPopup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
