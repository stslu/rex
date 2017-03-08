/****************************************************************************
** Meta object code from reading C++ file 'QtnGridEditors.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../grid/QtnGridEditors.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtnGridEditors.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Qtitan__GridEditorRepository_t {
    QByteArrayData data[1];
    char stringdata0[29];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridEditorRepository_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridEditorRepository_t qt_meta_stringdata_Qtitan__GridEditorRepository = {
    {
QT_MOC_LITERAL(0, 0, 28) // "Qtitan::GridEditorRepository"

    },
    "Qtitan::GridEditorRepository"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridEditorRepository[] = {

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

void Qtitan::GridEditorRepository::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridEditorRepository::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Qtitan__GridEditorRepository.data,
      qt_meta_data_Qtitan__GridEditorRepository,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridEditorRepository::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridEditorRepository::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridEditorRepository.stringdata0))
        return static_cast<void*>(const_cast< GridEditorRepository*>(this));
    return QObject::qt_metacast(_clname);
}

int Qtitan::GridEditorRepository::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridStringEditor_t {
    QByteArrayData data[4];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridStringEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridStringEditor_t qt_meta_stringdata_Qtitan__GridStringEditor = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Qtitan::GridStringEditor"
QT_MOC_LITERAL(1, 25, 10), // "textEdited"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 4) // "text"

    },
    "Qtitan::GridStringEditor\0textEdited\0"
    "\0text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridStringEditor[] = {

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
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Qtitan::GridStringEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridStringEditor *_t = static_cast<GridStringEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridStringEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridStringEditor.data,
      qt_meta_data_Qtitan__GridStringEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridStringEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridStringEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridStringEditor.stringdata0))
        return static_cast<void*>(const_cast< GridStringEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridStringEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridNumericEditor_t {
    QByteArrayData data[4];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridNumericEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridNumericEditor_t qt_meta_stringdata_Qtitan__GridNumericEditor = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Qtitan::GridNumericEditor"
QT_MOC_LITERAL(1, 26, 12), // "valueChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 1) // "d"

    },
    "Qtitan::GridNumericEditor\0valueChanged\0"
    "\0d"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridNumericEditor[] = {

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
    QMetaType::Void, QMetaType::Double,    3,

       0        // eod
};

void Qtitan::GridNumericEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridNumericEditor *_t = static_cast<GridNumericEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridNumericEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridNumericEditor.data,
      qt_meta_data_Qtitan__GridNumericEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridNumericEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridNumericEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridNumericEditor.stringdata0))
        return static_cast<void*>(const_cast< GridNumericEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridNumericEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridProgressEditor_t {
    QByteArrayData data[4];
    char stringdata0[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridProgressEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridProgressEditor_t qt_meta_stringdata_Qtitan__GridProgressEditor = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Qtitan::GridProgressEditor"
QT_MOC_LITERAL(1, 27, 12), // "valueChanged"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 5) // "value"

    },
    "Qtitan::GridProgressEditor\0valueChanged\0"
    "\0value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridProgressEditor[] = {

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

void Qtitan::GridProgressEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridProgressEditor *_t = static_cast<GridProgressEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridProgressEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridProgressEditor.data,
      qt_meta_data_Qtitan__GridProgressEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridProgressEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridProgressEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridProgressEditor.stringdata0))
        return static_cast<void*>(const_cast< GridProgressEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridProgressEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridCheckBoxEditor_t {
    QByteArrayData data[4];
    char stringdata0[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridCheckBoxEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridCheckBoxEditor_t qt_meta_stringdata_Qtitan__GridCheckBoxEditor = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Qtitan::GridCheckBoxEditor"
QT_MOC_LITERAL(1, 27, 12), // "stateChanged"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 5) // "state"

    },
    "Qtitan::GridCheckBoxEditor\0stateChanged\0"
    "\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridCheckBoxEditor[] = {

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

void Qtitan::GridCheckBoxEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridCheckBoxEditor *_t = static_cast<GridCheckBoxEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridCheckBoxEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridCheckBoxEditor.data,
      qt_meta_data_Qtitan__GridCheckBoxEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridCheckBoxEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridCheckBoxEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridCheckBoxEditor.stringdata0))
        return static_cast<void*>(const_cast< GridCheckBoxEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridCheckBoxEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridComboBoxEditor_t {
    QByteArrayData data[7];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridComboBoxEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridComboBoxEditor_t qt_meta_stringdata_Qtitan__GridComboBoxEditor = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Qtitan::GridComboBoxEditor"
QT_MOC_LITERAL(1, 27, 19), // "currentIndexChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 5), // "index"
QT_MOC_LITERAL(4, 54, 12), // "resolveValue"
QT_MOC_LITERAL(5, 67, 9), // "QVariant&"
QT_MOC_LITERAL(6, 77, 5) // "value"

    },
    "Qtitan::GridComboBoxEditor\0"
    "currentIndexChanged\0\0index\0resolveValue\0"
    "QVariant&\0value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridComboBoxEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       4,    2,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 5,    3,    6,

       0        // eod
};

void Qtitan::GridComboBoxEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridComboBoxEditor *_t = static_cast<GridComboBoxEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->resolveValue((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridComboBoxEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridComboBoxEditor.data,
      qt_meta_data_Qtitan__GridComboBoxEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridComboBoxEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridComboBoxEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridComboBoxEditor.stringdata0))
        return static_cast<void*>(const_cast< GridComboBoxEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridComboBoxEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridDateTimeEditor_t {
    QByteArrayData data[4];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridDateTimeEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridDateTimeEditor_t qt_meta_stringdata_Qtitan__GridDateTimeEditor = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Qtitan::GridDateTimeEditor"
QT_MOC_LITERAL(1, 27, 15), // "dateTimeChanged"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 8) // "datetime"

    },
    "Qtitan::GridDateTimeEditor\0dateTimeChanged\0"
    "\0datetime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridDateTimeEditor[] = {

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
    QMetaType::Void, QMetaType::QDateTime,    3,

       0        // eod
};

void Qtitan::GridDateTimeEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridDateTimeEditor *_t = static_cast<GridDateTimeEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dateTimeChanged((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridDateTimeEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridDateTimeEditor.data,
      qt_meta_data_Qtitan__GridDateTimeEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridDateTimeEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridDateTimeEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridDateTimeEditor.stringdata0))
        return static_cast<void*>(const_cast< GridDateTimeEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridDateTimeEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridPictureEditorRepository_t {
    QByteArrayData data[3];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridPictureEditorRepository_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridPictureEditorRepository_t qt_meta_stringdata_Qtitan__GridPictureEditorRepository = {
    {
QT_MOC_LITERAL(0, 0, 35), // "Qtitan::GridPictureEditorRepo..."
QT_MOC_LITERAL(1, 36, 17), // "hidePictureEditor"
QT_MOC_LITERAL(2, 54, 0) // ""

    },
    "Qtitan::GridPictureEditorRepository\0"
    "hidePictureEditor\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridPictureEditorRepository[] = {

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

void Qtitan::GridPictureEditorRepository::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridPictureEditorRepository *_t = static_cast<GridPictureEditorRepository *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->hidePictureEditor(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridPictureEditorRepository::staticMetaObject = {
    { &GridEditorRepository::staticMetaObject, qt_meta_stringdata_Qtitan__GridPictureEditorRepository.data,
      qt_meta_data_Qtitan__GridPictureEditorRepository,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridPictureEditorRepository::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridPictureEditorRepository::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridPictureEditorRepository.stringdata0))
        return static_cast<void*>(const_cast< GridPictureEditorRepository*>(this));
    return GridEditorRepository::qt_metacast(_clname);
}

int Qtitan::GridPictureEditorRepository::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorRepository::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridPictureEditor_t {
    QByteArrayData data[3];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridPictureEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridPictureEditor_t qt_meta_stringdata_Qtitan__GridPictureEditor = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Qtitan::GridPictureEditor"
QT_MOC_LITERAL(1, 26, 14), // "pictureChanged"
QT_MOC_LITERAL(2, 41, 0) // ""

    },
    "Qtitan::GridPictureEditor\0pictureChanged\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridPictureEditor[] = {

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

void Qtitan::GridPictureEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridPictureEditor *_t = static_cast<GridPictureEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pictureChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridPictureEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridPictureEditor.data,
      qt_meta_data_Qtitan__GridPictureEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridPictureEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridPictureEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridPictureEditor.stringdata0))
        return static_cast<void*>(const_cast< GridPictureEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridPictureEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__CustomComboBox_t {
    QByteArrayData data[3];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__CustomComboBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__CustomComboBox_t qt_meta_stringdata_Qtitan__CustomComboBox = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Qtitan::CustomComboBox"
QT_MOC_LITERAL(1, 23, 11), // "dataChanged"
QT_MOC_LITERAL(2, 35, 0) // ""

    },
    "Qtitan::CustomComboBox\0dataChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__CustomComboBox[] = {

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

void Qtitan::CustomComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CustomComboBox *_t = static_cast<CustomComboBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dataChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CustomComboBox::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CustomComboBox::dataChanged)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::CustomComboBox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Qtitan__CustomComboBox.data,
      qt_meta_data_Qtitan__CustomComboBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::CustomComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::CustomComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__CustomComboBox.stringdata0))
        return static_cast<void*>(const_cast< CustomComboBox*>(this));
    return QWidget::qt_metacast(_clname);
}

int Qtitan::CustomComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void Qtitan::CustomComboBox::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_Qtitan__EditorPopup_t {
    QByteArrayData data[5];
    char stringdata0[52];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__EditorPopup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__EditorPopup_t qt_meta_stringdata_Qtitan__EditorPopup = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Qtitan::EditorPopup"
QT_MOC_LITERAL(1, 20, 7), // "okClick"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 11), // "cancelClick"
QT_MOC_LITERAL(4, 41, 10) // "closeClick"

    },
    "Qtitan::EditorPopup\0okClick\0\0cancelClick\0"
    "closeClick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__EditorPopup[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x09 /* Protected */,
       3,    0,   30,    2, 0x09 /* Protected */,
       4,    0,   31,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Qtitan::EditorPopup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorPopup *_t = static_cast<EditorPopup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClick(); break;
        case 1: _t->cancelClick(); break;
        case 2: _t->closeClick(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::EditorPopup::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Qtitan__EditorPopup.data,
      qt_meta_data_Qtitan__EditorPopup,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::EditorPopup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::EditorPopup::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__EditorPopup.stringdata0))
        return static_cast<void*>(const_cast< EditorPopup*>(this));
    return QWidget::qt_metacast(_clname);
}

int Qtitan::EditorPopup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_Qtitan__PictureComboBox_t {
    QByteArrayData data[3];
    char stringdata0[40];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__PictureComboBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__PictureComboBox_t qt_meta_stringdata_Qtitan__PictureComboBox = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Qtitan::PictureComboBox"
QT_MOC_LITERAL(1, 24, 14), // "pictureChanged"
QT_MOC_LITERAL(2, 39, 0) // ""

    },
    "Qtitan::PictureComboBox\0pictureChanged\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__PictureComboBox[] = {

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
       1,    0,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Qtitan::PictureComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PictureComboBox *_t = static_cast<PictureComboBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pictureChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::PictureComboBox::staticMetaObject = {
    { &CustomComboBox::staticMetaObject, qt_meta_stringdata_Qtitan__PictureComboBox.data,
      qt_meta_data_Qtitan__PictureComboBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::PictureComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::PictureComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__PictureComboBox.stringdata0))
        return static_cast<void*>(const_cast< PictureComboBox*>(this));
    return CustomComboBox::qt_metacast(_clname);
}

int Qtitan::PictureComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CustomComboBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__PictureEditorWidget_t {
    QByteArrayData data[12];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__PictureEditorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__PictureEditorWidget_t qt_meta_stringdata_Qtitan__PictureEditorWidget = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Qtitan::PictureEditorWidget"
QT_MOC_LITERAL(1, 28, 14), // "pictureChanged"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 4), // "load"
QT_MOC_LITERAL(4, 49, 4), // "save"
QT_MOC_LITERAL(5, 54, 4), // "copy"
QT_MOC_LITERAL(6, 59, 5), // "paste"
QT_MOC_LITERAL(7, 65, 6), // "zoomIn"
QT_MOC_LITERAL(8, 72, 7), // "zoomOut"
QT_MOC_LITERAL(9, 80, 10), // "adjustSize"
QT_MOC_LITERAL(10, 91, 10), // "actualSize"
QT_MOC_LITERAL(11, 102, 5) // "print"

    },
    "Qtitan::PictureEditorWidget\0pictureChanged\0"
    "\0load\0save\0copy\0paste\0zoomIn\0zoomOut\0"
    "adjustSize\0actualSize\0print"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__PictureEditorWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   65,    2, 0x0a /* Public */,
       4,    0,   66,    2, 0x0a /* Public */,
       5,    0,   67,    2, 0x0a /* Public */,
       6,    0,   68,    2, 0x0a /* Public */,
       7,    0,   69,    2, 0x0a /* Public */,
       8,    0,   70,    2, 0x0a /* Public */,
       9,    0,   71,    2, 0x0a /* Public */,
      10,    0,   72,    2, 0x0a /* Public */,
      11,    0,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Qtitan::PictureEditorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PictureEditorWidget *_t = static_cast<PictureEditorWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pictureChanged(); break;
        case 1: _t->load(); break;
        case 2: _t->save(); break;
        case 3: _t->copy(); break;
        case 4: _t->paste(); break;
        case 5: _t->zoomIn(); break;
        case 6: _t->zoomOut(); break;
        case 7: _t->adjustSize(); break;
        case 8: _t->actualSize(); break;
        case 9: _t->print(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PictureEditorWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PictureEditorWidget::pictureChanged)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::PictureEditorWidget::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_Qtitan__PictureEditorWidget.data,
      qt_meta_data_Qtitan__PictureEditorWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::PictureEditorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::PictureEditorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__PictureEditorWidget.stringdata0))
        return static_cast<void*>(const_cast< PictureEditorWidget*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int Qtitan::PictureEditorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Qtitan::PictureEditorWidget::pictureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_Qtitan__TableComboBox_t {
    QByteArrayData data[5];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__TableComboBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__TableComboBox_t qt_meta_stringdata_Qtitan__TableComboBox = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Qtitan::TableComboBox"
QT_MOC_LITERAL(1, 22, 15), // "focusRowChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 11), // "oldRowIndex"
QT_MOC_LITERAL(4, 51, 8) // "rowIndex"

    },
    "Qtitan::TableComboBox\0focusRowChanged\0"
    "\0oldRowIndex\0rowIndex"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__TableComboBox[] = {

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
       1,    2,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,

       0        // eod
};

void Qtitan::TableComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TableComboBox *_t = static_cast<TableComboBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->focusRowChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::TableComboBox::staticMetaObject = {
    { &CustomComboBox::staticMetaObject, qt_meta_stringdata_Qtitan__TableComboBox.data,
      qt_meta_data_Qtitan__TableComboBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::TableComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::TableComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__TableComboBox.stringdata0))
        return static_cast<void*>(const_cast< TableComboBox*>(this));
    return CustomComboBox::qt_metacast(_clname);
}

int Qtitan::TableComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CustomComboBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridPictureComboBoxEditorRepository_t {
    QByteArrayData data[3];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridPictureComboBoxEditorRepository_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridPictureComboBoxEditorRepository_t qt_meta_stringdata_Qtitan__GridPictureComboBoxEditorRepository = {
    {
QT_MOC_LITERAL(0, 0, 43), // "Qtitan::GridPictureComboBoxEd..."
QT_MOC_LITERAL(1, 44, 17), // "hidePictureEditor"
QT_MOC_LITERAL(2, 62, 0) // ""

    },
    "Qtitan::GridPictureComboBoxEditorRepository\0"
    "hidePictureEditor\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridPictureComboBoxEditorRepository[] = {

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

void Qtitan::GridPictureComboBoxEditorRepository::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridPictureComboBoxEditorRepository *_t = static_cast<GridPictureComboBoxEditorRepository *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->hidePictureEditor(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridPictureComboBoxEditorRepository::staticMetaObject = {
    { &GridEditorRepository::staticMetaObject, qt_meta_stringdata_Qtitan__GridPictureComboBoxEditorRepository.data,
      qt_meta_data_Qtitan__GridPictureComboBoxEditorRepository,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridPictureComboBoxEditorRepository::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridPictureComboBoxEditorRepository::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridPictureComboBoxEditorRepository.stringdata0))
        return static_cast<void*>(const_cast< GridPictureComboBoxEditorRepository*>(this));
    return GridEditorRepository::qt_metacast(_clname);
}

int Qtitan::GridPictureComboBoxEditorRepository::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorRepository::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridPictureComboBoxEditor_t {
    QByteArrayData data[3];
    char stringdata0[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridPictureComboBoxEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridPictureComboBoxEditor_t qt_meta_stringdata_Qtitan__GridPictureComboBoxEditor = {
    {
QT_MOC_LITERAL(0, 0, 33), // "Qtitan::GridPictureComboBoxEd..."
QT_MOC_LITERAL(1, 34, 14), // "pictureChanged"
QT_MOC_LITERAL(2, 49, 0) // ""

    },
    "Qtitan::GridPictureComboBoxEditor\0"
    "pictureChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridPictureComboBoxEditor[] = {

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

void Qtitan::GridPictureComboBoxEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridPictureComboBoxEditor *_t = static_cast<GridPictureComboBoxEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pictureChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridPictureComboBoxEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridPictureComboBoxEditor.data,
      qt_meta_data_Qtitan__GridPictureComboBoxEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridPictureComboBoxEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridPictureComboBoxEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridPictureComboBoxEditor.stringdata0))
        return static_cast<void*>(const_cast< GridPictureComboBoxEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridPictureComboBoxEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridMemoEditorRepository_t {
    QByteArrayData data[1];
    char stringdata0[33];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridMemoEditorRepository_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridMemoEditorRepository_t qt_meta_stringdata_Qtitan__GridMemoEditorRepository = {
    {
QT_MOC_LITERAL(0, 0, 32) // "Qtitan::GridMemoEditorRepository"

    },
    "Qtitan::GridMemoEditorRepository"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridMemoEditorRepository[] = {

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

void Qtitan::GridMemoEditorRepository::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridMemoEditorRepository::staticMetaObject = {
    { &GridEditorRepository::staticMetaObject, qt_meta_stringdata_Qtitan__GridMemoEditorRepository.data,
      qt_meta_data_Qtitan__GridMemoEditorRepository,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridMemoEditorRepository::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridMemoEditorRepository::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridMemoEditorRepository.stringdata0))
        return static_cast<void*>(const_cast< GridMemoEditorRepository*>(this));
    return GridEditorRepository::qt_metacast(_clname);
}

int Qtitan::GridMemoEditorRepository::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorRepository::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridMemoEditor_t {
    QByteArrayData data[3];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridMemoEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridMemoEditor_t qt_meta_stringdata_Qtitan__GridMemoEditor = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Qtitan::GridMemoEditor"
QT_MOC_LITERAL(1, 23, 11), // "textChanged"
QT_MOC_LITERAL(2, 35, 0) // ""

    },
    "Qtitan::GridMemoEditor\0textChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridMemoEditor[] = {

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

void Qtitan::GridMemoEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridMemoEditor *_t = static_cast<GridMemoEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridMemoEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridMemoEditor.data,
      qt_meta_data_Qtitan__GridMemoEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridMemoEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridMemoEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridMemoEditor.stringdata0))
        return static_cast<void*>(const_cast< GridMemoEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridMemoEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__TextComboBox_t {
    QByteArrayData data[3];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__TextComboBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__TextComboBox_t qt_meta_stringdata_Qtitan__TextComboBox = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Qtitan::TextComboBox"
QT_MOC_LITERAL(1, 21, 11), // "textChanged"
QT_MOC_LITERAL(2, 33, 0) // ""

    },
    "Qtitan::TextComboBox\0textChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__TextComboBox[] = {

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

void Qtitan::TextComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TextComboBox *_t = static_cast<TextComboBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::TextComboBox::staticMetaObject = {
    { &CustomComboBox::staticMetaObject, qt_meta_stringdata_Qtitan__TextComboBox.data,
      qt_meta_data_Qtitan__TextComboBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::TextComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::TextComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__TextComboBox.stringdata0))
        return static_cast<void*>(const_cast< TextComboBox*>(this));
    return CustomComboBox::qt_metacast(_clname);
}

int Qtitan::TextComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CustomComboBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__TextComboBoxPopup_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__TextComboBoxPopup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__TextComboBoxPopup_t qt_meta_stringdata_Qtitan__TextComboBoxPopup = {
    {
QT_MOC_LITERAL(0, 0, 25) // "Qtitan::TextComboBoxPopup"

    },
    "Qtitan::TextComboBoxPopup"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__TextComboBoxPopup[] = {

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

void Qtitan::TextComboBoxPopup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::TextComboBoxPopup::staticMetaObject = {
    { &CustomComboBoxPopup::staticMetaObject, qt_meta_stringdata_Qtitan__TextComboBoxPopup.data,
      qt_meta_data_Qtitan__TextComboBoxPopup,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::TextComboBoxPopup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::TextComboBoxPopup::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__TextComboBoxPopup.stringdata0))
        return static_cast<void*>(const_cast< TextComboBoxPopup*>(this));
    return CustomComboBoxPopup::qt_metacast(_clname);
}

int Qtitan::TextComboBoxPopup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CustomComboBoxPopup::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridBlobEditorRepository_t {
    QByteArrayData data[1];
    char stringdata0[33];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridBlobEditorRepository_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridBlobEditorRepository_t qt_meta_stringdata_Qtitan__GridBlobEditorRepository = {
    {
QT_MOC_LITERAL(0, 0, 32) // "Qtitan::GridBlobEditorRepository"

    },
    "Qtitan::GridBlobEditorRepository"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridBlobEditorRepository[] = {

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

void Qtitan::GridBlobEditorRepository::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridBlobEditorRepository::staticMetaObject = {
    { &GridMemoEditorRepository::staticMetaObject, qt_meta_stringdata_Qtitan__GridBlobEditorRepository.data,
      qt_meta_data_Qtitan__GridBlobEditorRepository,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridBlobEditorRepository::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridBlobEditorRepository::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridBlobEditorRepository.stringdata0))
        return static_cast<void*>(const_cast< GridBlobEditorRepository*>(this));
    return GridMemoEditorRepository::qt_metacast(_clname);
}

int Qtitan::GridBlobEditorRepository::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridMemoEditorRepository::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridBlobEditor_t {
    QByteArrayData data[3];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridBlobEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridBlobEditor_t qt_meta_stringdata_Qtitan__GridBlobEditor = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Qtitan::GridBlobEditor"
QT_MOC_LITERAL(1, 23, 11), // "textChanged"
QT_MOC_LITERAL(2, 35, 0) // ""

    },
    "Qtitan::GridBlobEditor\0textChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridBlobEditor[] = {

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

void Qtitan::GridBlobEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridBlobEditor *_t = static_cast<GridBlobEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridBlobEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridBlobEditor.data,
      qt_meta_data_Qtitan__GridBlobEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridBlobEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridBlobEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridBlobEditor.stringdata0))
        return static_cast<void*>(const_cast< GridBlobEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridBlobEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridDelegateAdapterEditorRepository_t {
    QByteArrayData data[10];
    char stringdata0[170];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridDelegateAdapterEditorRepository_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridDelegateAdapterEditorRepository_t qt_meta_stringdata_Qtitan__GridDelegateAdapterEditorRepository = {
    {
QT_MOC_LITERAL(0, 0, 43), // "Qtitan::GridDelegateAdapterEd..."
QT_MOC_LITERAL(1, 44, 19), // "delegateCloseEditor"
QT_MOC_LITERAL(2, 64, 0), // ""
QT_MOC_LITERAL(3, 65, 8), // "QWidget*"
QT_MOC_LITERAL(4, 74, 6), // "editor"
QT_MOC_LITERAL(5, 81, 34), // "QAbstractItemDelegate::EndEdi..."
QT_MOC_LITERAL(6, 116, 4), // "hint"
QT_MOC_LITERAL(7, 121, 18), // "delegateCommitData"
QT_MOC_LITERAL(8, 140, 23), // "delegateSizeHintChanged"
QT_MOC_LITERAL(9, 164, 5) // "index"

    },
    "Qtitan::GridDelegateAdapterEditorRepository\0"
    "delegateCloseEditor\0\0QWidget*\0editor\0"
    "QAbstractItemDelegate::EndEditHint\0"
    "hint\0delegateCommitData\0delegateSizeHintChanged\0"
    "index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridDelegateAdapterEditorRepository[] = {

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
       1,    2,   34,    2, 0x08 /* Private */,
       1,    1,   39,    2, 0x28 /* Private | MethodCloned */,
       7,    1,   42,    2, 0x08 /* Private */,
       8,    1,   45,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QModelIndex,    9,

       0        // eod
};

void Qtitan::GridDelegateAdapterEditorRepository::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridDelegateAdapterEditorRepository *_t = static_cast<GridDelegateAdapterEditorRepository *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->delegateCloseEditor((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemDelegate::EndEditHint(*)>(_a[2]))); break;
        case 1: _t->delegateCloseEditor((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 2: _t->delegateCommitData((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 3: _t->delegateSizeHintChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        }
    }
}

const QMetaObject Qtitan::GridDelegateAdapterEditorRepository::staticMetaObject = {
    { &GridEditorRepository::staticMetaObject, qt_meta_stringdata_Qtitan__GridDelegateAdapterEditorRepository.data,
      qt_meta_data_Qtitan__GridDelegateAdapterEditorRepository,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridDelegateAdapterEditorRepository::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridDelegateAdapterEditorRepository::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridDelegateAdapterEditorRepository.stringdata0))
        return static_cast<void*>(const_cast< GridDelegateAdapterEditorRepository*>(this));
    return GridEditorRepository::qt_metacast(_clname);
}

int Qtitan::GridDelegateAdapterEditorRepository::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorRepository::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridDelegateAdapterEditor_t {
    QByteArrayData data[1];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridDelegateAdapterEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridDelegateAdapterEditor_t qt_meta_stringdata_Qtitan__GridDelegateAdapterEditor = {
    {
QT_MOC_LITERAL(0, 0, 33) // "Qtitan::GridDelegateAdapterEd..."

    },
    "Qtitan::GridDelegateAdapterEditor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridDelegateAdapterEditor[] = {

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

void Qtitan::GridDelegateAdapterEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridDelegateAdapterEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridDelegateAdapterEditor.data,
      qt_meta_data_Qtitan__GridDelegateAdapterEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridDelegateAdapterEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridDelegateAdapterEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridDelegateAdapterEditor.stringdata0))
        return static_cast<void*>(const_cast< GridDelegateAdapterEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridDelegateAdapterEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridTableEditor_t {
    QByteArrayData data[3];
    char stringdata0[38];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridTableEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridTableEditor_t qt_meta_stringdata_Qtitan__GridTableEditor = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Qtitan::GridTableEditor"
QT_MOC_LITERAL(1, 24, 12), // "focusChanged"
QT_MOC_LITERAL(2, 37, 0) // ""

    },
    "Qtitan::GridTableEditor\0focusChanged\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridTableEditor[] = {

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

void Qtitan::GridTableEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridTableEditor *_t = static_cast<GridTableEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->focusChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridTableEditor::staticMetaObject = {
    { &GridEditorBase::staticMetaObject, qt_meta_stringdata_Qtitan__GridTableEditor.data,
      qt_meta_data_Qtitan__GridTableEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridTableEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridTableEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridTableEditor.stringdata0))
        return static_cast<void*>(const_cast< GridTableEditor*>(this));
    return GridEditorBase::qt_metacast(_clname);
}

int Qtitan::GridTableEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridEditorBase::qt_metacall(_c, _id, _a);
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
