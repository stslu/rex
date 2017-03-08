/****************************************************************************
** Meta object code from reading C++ file 'QtnGridFilterWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../grid/QtnGridFilterWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtnGridFilterWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Qtitan__GridFilterColumnPopup_t {
    QByteArrayData data[6];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridFilterColumnPopup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridFilterColumnPopup_t qt_meta_stringdata_Qtitan__GridFilterColumnPopup = {
    {
QT_MOC_LITERAL(0, 0, 29), // "Qtitan::GridFilterColumnPopup"
QT_MOC_LITERAL(1, 30, 16), // "selectionChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 14), // "GridSelection*"
QT_MOC_LITERAL(4, 63, 9), // "selection"
QT_MOC_LITERAL(5, 73, 12) // "oldSelection"

    },
    "Qtitan::GridFilterColumnPopup\0"
    "selectionChanged\0\0GridSelection*\0"
    "selection\0oldSelection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridFilterColumnPopup[] = {

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
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,

       0        // eod
};

void Qtitan::GridFilterColumnPopup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridFilterColumnPopup *_t = static_cast<GridFilterColumnPopup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast< GridSelection*(*)>(_a[1])),(*reinterpret_cast< GridSelection*(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::GridFilterColumnPopup::staticMetaObject = {
    { &EditorPopup::staticMetaObject, qt_meta_stringdata_Qtitan__GridFilterColumnPopup.data,
      qt_meta_data_Qtitan__GridFilterColumnPopup,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridFilterColumnPopup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridFilterColumnPopup::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridFilterColumnPopup.stringdata0))
        return static_cast<void*>(const_cast< GridFilterColumnPopup*>(this));
    return EditorPopup::qt_metacast(_clname);
}

int Qtitan::GridFilterColumnPopup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EditorPopup::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__FilterPopupWidget_t {
    QByteArrayData data[7];
    char stringdata0[104];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__FilterPopupWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__FilterPopupWidget_t qt_meta_stringdata_Qtitan__FilterPopupWidget = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Qtitan::FilterPopupWidget"
QT_MOC_LITERAL(1, 26, 19), // "selectBlanksClicked"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 4), // "link"
QT_MOC_LITERAL(4, 52, 21), // "selectNoBlanksClicked"
QT_MOC_LITERAL(5, 74, 13), // "selectClicked"
QT_MOC_LITERAL(6, 88, 15) // "unselectClicked"

    },
    "Qtitan::FilterPopupWidget\0selectBlanksClicked\0"
    "\0link\0selectNoBlanksClicked\0selectClicked\0"
    "unselectClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__FilterPopupWidget[] = {

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
       1,    1,   34,    2, 0x08 /* Private */,
       4,    1,   37,    2, 0x08 /* Private */,
       5,    1,   40,    2, 0x08 /* Private */,
       6,    1,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Qtitan::FilterPopupWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FilterPopupWidget *_t = static_cast<FilterPopupWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectBlanksClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->selectNoBlanksClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->selectClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->unselectClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Qtitan::FilterPopupWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Qtitan__FilterPopupWidget.data,
      qt_meta_data_Qtitan__FilterPopupWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::FilterPopupWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::FilterPopupWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__FilterPopupWidget.stringdata0))
        return static_cast<void*>(const_cast< FilterPopupWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Qtitan::FilterPopupWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__FilterDialog_t {
    QByteArrayData data[14];
    char stringdata0[171];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__FilterDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__FilterDialog_t qt_meta_stringdata_Qtitan__FilterDialog = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Qtitan::FilterDialog"
QT_MOC_LITERAL(1, 21, 8), // "dialogOk"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 12), // "dialogCancel"
QT_MOC_LITERAL(4, 44, 13), // "dialogClicked"
QT_MOC_LITERAL(5, 58, 16), // "QAbstractButton*"
QT_MOC_LITERAL(6, 75, 6), // "button"
QT_MOC_LITERAL(7, 82, 9), // "clickedOR"
QT_MOC_LITERAL(8, 92, 7), // "checked"
QT_MOC_LITERAL(9, 100, 10), // "clickedAND"
QT_MOC_LITERAL(10, 111, 13), // "clickedANDNOT"
QT_MOC_LITERAL(11, 125, 19), // "column1IndexChanged"
QT_MOC_LITERAL(12, 145, 5), // "index"
QT_MOC_LITERAL(13, 151, 19) // "column2IndexChanged"

    },
    "Qtitan::FilterDialog\0dialogOk\0\0"
    "dialogCancel\0dialogClicked\0QAbstractButton*\0"
    "button\0clickedOR\0checked\0clickedAND\0"
    "clickedANDNOT\0column1IndexChanged\0"
    "index\0column2IndexChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__FilterDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    1,   56,    2, 0x08 /* Private */,
       7,    1,   59,    2, 0x08 /* Private */,
       9,    1,   62,    2, 0x08 /* Private */,
      10,    1,   65,    2, 0x08 /* Private */,
      11,    1,   68,    2, 0x08 /* Private */,
      13,    1,   71,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,

       0        // eod
};

void Qtitan::FilterDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FilterDialog *_t = static_cast<FilterDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dialogOk(); break;
        case 1: _t->dialogCancel(); break;
        case 2: _t->dialogClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 3: _t->clickedOR((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->clickedAND((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->clickedANDNOT((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->column1IndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->column2IndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        }
    }
}

const QMetaObject Qtitan::FilterDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Qtitan__FilterDialog.data,
      qt_meta_data_Qtitan__FilterDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::FilterDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::FilterDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__FilterDialog.stringdata0))
        return static_cast<void*>(const_cast< FilterDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Qtitan::FilterDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
struct qt_meta_stringdata_Qtitan__FilterTextWidget_t {
    QByteArrayData data[3];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__FilterTextWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__FilterTextWidget_t qt_meta_stringdata_Qtitan__FilterTextWidget = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Qtitan::FilterTextWidget"
QT_MOC_LITERAL(1, 25, 7), // "clicked"
QT_MOC_LITERAL(2, 33, 0) // ""

    },
    "Qtitan::FilterTextWidget\0clicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__FilterTextWidget[] = {

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

void Qtitan::FilterTextWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FilterTextWidget *_t = static_cast<FilterTextWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FilterTextWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FilterTextWidget::clicked)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::FilterTextWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Qtitan__FilterTextWidget.data,
      qt_meta_data_Qtitan__FilterTextWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::FilterTextWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::FilterTextWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__FilterTextWidget.stringdata0))
        return static_cast<void*>(const_cast< FilterTextWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Qtitan::FilterTextWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Qtitan::FilterTextWidget::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_Qtitan__GridHistoryItemButton_t {
    QByteArrayData data[1];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridHistoryItemButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridHistoryItemButton_t qt_meta_stringdata_Qtitan__GridHistoryItemButton = {
    {
QT_MOC_LITERAL(0, 0, 29) // "Qtitan::GridHistoryItemButton"

    },
    "Qtitan::GridHistoryItemButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridHistoryItemButton[] = {

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

void Qtitan::GridHistoryItemButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridHistoryItemButton::staticMetaObject = {
    { &GridShadeButtonPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__GridHistoryItemButton.data,
      qt_meta_data_Qtitan__GridHistoryItemButton,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridHistoryItemButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridHistoryItemButton::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridHistoryItemButton.stringdata0))
        return static_cast<void*>(const_cast< GridHistoryItemButton*>(this));
    return GridShadeButtonPrimitive::qt_metacast(_clname);
}

int Qtitan::GridHistoryItemButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridShadeButtonPrimitive::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Qtitan__FilterHistoryPopupPrimitive_t {
    QByteArrayData data[4];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__FilterHistoryPopupPrimitive_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__FilterHistoryPopupPrimitive_t qt_meta_stringdata_Qtitan__FilterHistoryPopupPrimitive = {
    {
QT_MOC_LITERAL(0, 0, 35), // "Qtitan::FilterHistoryPopupPri..."
QT_MOC_LITERAL(1, 36, 11), // "itemClicked"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 9) // "fillItems"

    },
    "Qtitan::FilterHistoryPopupPrimitive\0"
    "itemClicked\0\0fillItems"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__FilterHistoryPopupPrimitive[] = {

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
       1,    0,   24,    2, 0x08 /* Private */,
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Qtitan::FilterHistoryPopupPrimitive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FilterHistoryPopupPrimitive *_t = static_cast<FilterHistoryPopupPrimitive *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemClicked(); break;
        case 1: _t->fillItems(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::FilterHistoryPopupPrimitive::staticMetaObject = {
    { &GridWinShadowedPrimitive::staticMetaObject, qt_meta_stringdata_Qtitan__FilterHistoryPopupPrimitive.data,
      qt_meta_data_Qtitan__FilterHistoryPopupPrimitive,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::FilterHistoryPopupPrimitive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::FilterHistoryPopupPrimitive::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__FilterHistoryPopupPrimitive.stringdata0))
        return static_cast<void*>(const_cast< FilterHistoryPopupPrimitive*>(this));
    return GridWinShadowedPrimitive::qt_metacast(_clname);
}

int Qtitan::FilterHistoryPopupPrimitive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GridWinShadowedPrimitive::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
