/****************************************************************************
** Meta object code from reading C++ file 'QtnGrid.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../grid/QtnGrid.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtnGrid.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Qtitan__Grid_t {
    QByteArrayData data[9];
    char stringdata0[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__Grid_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__Grid_t qt_meta_stringdata_Qtitan__Grid = {
    {
QT_MOC_LITERAL(0, 0, 12), // "Qtitan::Grid"
QT_MOC_LITERAL(1, 13, 8), // "viewType"
QT_MOC_LITERAL(2, 22, 12), // "GridViewType"
QT_MOC_LITERAL(3, 35, 8), // "rootView"
QT_MOC_LITERAL(4, 44, 13), // "GridViewBase*"
QT_MOC_LITERAL(5, 58, 13), // "UndefinedView"
QT_MOC_LITERAL(6, 72, 9), // "TableView"
QT_MOC_LITERAL(7, 82, 15), // "BandedTableView"
QT_MOC_LITERAL(8, 98, 8) // "CardView"

    },
    "Qtitan::Grid\0viewType\0GridViewType\0"
    "rootView\0GridViewBase*\0UndefinedView\0"
    "TableView\0BandedTableView\0CardView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__Grid[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   14, // properties
       1,   20, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, 0x80000000 | 2, 0x0009510b,
       3, 0x80000000 | 4, 0x00095009,

 // enums: name, flags, count, data
       2, 0x0,    4,   24,

 // enum data: key, value
       5, uint(Qtitan::Grid::UndefinedView),
       6, uint(Qtitan::Grid::TableView),
       7, uint(Qtitan::Grid::BandedTableView),
       8, uint(Qtitan::Grid::CardView),

       0        // eod
};

void Qtitan::Grid::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< GridViewBase* >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Grid *_t = static_cast<Grid *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< GridViewType*>(_v) = _t->viewType(); break;
        case 1: *reinterpret_cast< GridViewBase**>(_v) = _t->rootView(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        Grid *_t = static_cast<Grid *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setViewType(*reinterpret_cast< GridViewType*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
}

const QMetaObject Qtitan::Grid::staticMetaObject = {
    { &GraphicControl::staticMetaObject, qt_meta_stringdata_Qtitan__Grid.data,
      qt_meta_data_Qtitan__Grid,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::Grid::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::Grid::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__Grid.stringdata0))
        return static_cast<void*>(const_cast< Grid*>(this));
    return GraphicControl::qt_metacast(_clname);
}

int Qtitan::Grid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphicControl::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_Qtitan__GridRowsLayoutEngine_t {
    QByteArrayData data[3];
    char stringdata0[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridRowsLayoutEngine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridRowsLayoutEngine_t qt_meta_stringdata_Qtitan__GridRowsLayoutEngine = {
    {
QT_MOC_LITERAL(0, 0, 28), // "Qtitan::GridRowsLayoutEngine"
QT_MOC_LITERAL(1, 29, 19), // "updateNewRowOpacity"
QT_MOC_LITERAL(2, 49, 0) // ""

    },
    "Qtitan::GridRowsLayoutEngine\0"
    "updateNewRowOpacity\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridRowsLayoutEngine[] = {

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

void Qtitan::GridRowsLayoutEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridRowsLayoutEngine *_t = static_cast<GridRowsLayoutEngine *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateNewRowOpacity(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Qtitan::GridRowsLayoutEngine::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Qtitan__GridRowsLayoutEngine.data,
      qt_meta_data_Qtitan__GridRowsLayoutEngine,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridRowsLayoutEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridRowsLayoutEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridRowsLayoutEngine.stringdata0))
        return static_cast<void*>(const_cast< GridRowsLayoutEngine*>(this));
    return QObject::qt_metacast(_clname);
}

int Qtitan::GridRowsLayoutEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_Qtitan__GridViewBase_t {
    QByteArrayData data[119];
    char stringdata0[1734];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qtitan__GridViewBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qtitan__GridViewBase_t qt_meta_stringdata_Qtitan__GridViewBase = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Qtitan::GridViewBase"
QT_MOC_LITERAL(1, 21, 10), // "rowClicked"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 18), // "RowClickEventArgs*"
QT_MOC_LITERAL(4, 52, 4), // "args"
QT_MOC_LITERAL(5, 57, 13), // "rowDblClicked"
QT_MOC_LITERAL(6, 71, 11), // "cellClicked"
QT_MOC_LITERAL(7, 83, 19), // "CellClickEventArgs*"
QT_MOC_LITERAL(8, 103, 14), // "cellDblClicked"
QT_MOC_LITERAL(9, 118, 17), // "summaryCalculated"
QT_MOC_LITERAL(10, 136, 11), // "contextMenu"
QT_MOC_LITERAL(11, 148, 21), // "ContextMenuEventArgs*"
QT_MOC_LITERAL(12, 170, 5), // "error"
QT_MOC_LITERAL(13, 176, 15), // "ErrorEventArgs*"
QT_MOC_LITERAL(14, 192, 14), // "columnsUpdated"
QT_MOC_LITERAL(15, 207, 14), // "groupExpanding"
QT_MOC_LITERAL(16, 222, 15), // "GridModelGroup*"
QT_MOC_LITERAL(17, 238, 5), // "group"
QT_MOC_LITERAL(18, 244, 5), // "bool&"
QT_MOC_LITERAL(19, 250, 5), // "allow"
QT_MOC_LITERAL(20, 256, 13), // "groupExpanded"
QT_MOC_LITERAL(21, 270, 15), // "groupCollapsing"
QT_MOC_LITERAL(22, 286, 14), // "groupCollapsed"
QT_MOC_LITERAL(23, 301, 11), // "rowRemoving"
QT_MOC_LITERAL(24, 313, 21), // "RowRemovingEventArgs*"
QT_MOC_LITERAL(25, 335, 15), // "filterActivated"
QT_MOC_LITERAL(26, 351, 17), // "filterDeactivated"
QT_MOC_LITERAL(27, 369, 15), // "focusRowChanged"
QT_MOC_LITERAL(28, 385, 11), // "oldRowIndex"
QT_MOC_LITERAL(29, 397, 8), // "rowIndex"
QT_MOC_LITERAL(30, 406, 18), // "focusColumnChanged"
QT_MOC_LITERAL(31, 425, 14), // "oldColumnIndex"
QT_MOC_LITERAL(32, 440, 11), // "columnIndex"
QT_MOC_LITERAL(33, 452, 23), // "editorRepositoryCreated"
QT_MOC_LITERAL(34, 476, 26), // "const GridDataTableColumn*"
QT_MOC_LITERAL(35, 503, 6), // "column"
QT_MOC_LITERAL(36, 510, 21), // "GridEditorRepository*"
QT_MOC_LITERAL(37, 532, 10), // "repository"
QT_MOC_LITERAL(38, 543, 13), // "editorCreated"
QT_MOC_LITERAL(39, 557, 11), // "GridEditor*"
QT_MOC_LITERAL(40, 569, 6), // "editor"
QT_MOC_LITERAL(41, 576, 16), // "editorDestroying"
QT_MOC_LITERAL(42, 593, 15), // "editorModifying"
QT_MOC_LITERAL(43, 609, 21), // "editorEditModeChanged"
QT_MOC_LITERAL(44, 631, 14), // "editorStarting"
QT_MOC_LITERAL(45, 646, 16), // "EditorEventArgs*"
QT_MOC_LITERAL(46, 663, 13), // "editorStarted"
QT_MOC_LITERAL(47, 677, 13), // "editorStoping"
QT_MOC_LITERAL(48, 691, 12), // "editorStoped"
QT_MOC_LITERAL(49, 704, 16), // "editorValidating"
QT_MOC_LITERAL(50, 721, 26), // "EditorValidationEventArgs*"
QT_MOC_LITERAL(51, 748, 18), // "relationConnecting"
QT_MOC_LITERAL(52, 767, 18), // "RelationEventArgs*"
QT_MOC_LITERAL(53, 786, 13), // "startDragDrop"
QT_MOC_LITERAL(54, 800, 9), // "DragArgs*"
QT_MOC_LITERAL(55, 810, 11), // "endDragDrop"
QT_MOC_LITERAL(56, 822, 13), // "checkDragDrop"
QT_MOC_LITERAL(57, 836, 13), // "DragDropArgs*"
QT_MOC_LITERAL(58, 850, 8), // "dragDrop"
QT_MOC_LITERAL(59, 859, 19), // "newRowEditorClicked"
QT_MOC_LITERAL(60, 879, 16), // "NewRowEventArgs*"
QT_MOC_LITERAL(61, 896, 13), // "startGrouping"
QT_MOC_LITERAL(62, 910, 16), // "ColumnGroupArgs*"
QT_MOC_LITERAL(63, 927, 16), // "selectionChanged"
QT_MOC_LITERAL(64, 944, 14), // "GridSelection*"
QT_MOC_LITERAL(65, 959, 9), // "selection"
QT_MOC_LITERAL(66, 969, 12), // "oldSelection"
QT_MOC_LITERAL(67, 982, 17), // "zoomFactorChanged"
QT_MOC_LITERAL(68, 1000, 6), // "factor"
QT_MOC_LITERAL(69, 1007, 11), // "zoomChanged"
QT_MOC_LITERAL(70, 1019, 5), // "value"
QT_MOC_LITERAL(71, 1025, 25), // "summaryFooterTextChanging"
QT_MOC_LITERAL(72, 1051, 21), // "SummaryTextEventArgs*"
QT_MOC_LITERAL(73, 1073, 24), // "summaryGroupTextChanging"
QT_MOC_LITERAL(74, 1098, 26), // "SummaryGroupTextEventArgs*"
QT_MOC_LITERAL(75, 1125, 20), // "previewLinkActivated"
QT_MOC_LITERAL(76, 1146, 14), // "const GridRow*"
QT_MOC_LITERAL(77, 1161, 3), // "row"
QT_MOC_LITERAL(78, 1165, 6), // "anchor"
QT_MOC_LITERAL(79, 1172, 19), // "columnLinkActivated"
QT_MOC_LITERAL(80, 1192, 21), // "const GridColumnBase*"
QT_MOC_LITERAL(81, 1214, 19), // "editorLinkActivated"
QT_MOC_LITERAL(82, 1234, 20), // "EditorLinkEventArgs*"
QT_MOC_LITERAL(83, 1255, 14), // "slot_removeRow"
QT_MOC_LITERAL(84, 1270, 8), // "sortNone"
QT_MOC_LITERAL(85, 1279, 13), // "sortAscending"
QT_MOC_LITERAL(86, 1293, 14), // "sortDescending"
QT_MOC_LITERAL(87, 1308, 10), // "hideColumn"
QT_MOC_LITERAL(88, 1319, 13), // "groupByColumn"
QT_MOC_LITERAL(89, 1333, 12), // "alignLeftAct"
QT_MOC_LITERAL(90, 1346, 13), // "alignRightAct"
QT_MOC_LITERAL(91, 1360, 14), // "alignCenterAct"
QT_MOC_LITERAL(92, 1375, 10), // "groupByBox"
QT_MOC_LITERAL(93, 1386, 12), // "fieldChooser"
QT_MOC_LITERAL(94, 1399, 14), // "showHideFooter"
QT_MOC_LITERAL(95, 1414, 20), // "showHideGroupFooters"
QT_MOC_LITERAL(96, 1435, 9), // "expandAll"
QT_MOC_LITERAL(97, 1445, 11), // "collapseAll"
QT_MOC_LITERAL(98, 1457, 18), // "printAreaRequested"
QT_MOC_LITERAL(99, 1476, 9), // "QPrinter*"
QT_MOC_LITERAL(100, 1486, 7), // "printer"
QT_MOC_LITERAL(101, 1494, 6), // "zoomIn"
QT_MOC_LITERAL(102, 1501, 7), // "zoomOut"
QT_MOC_LITERAL(103, 1509, 12), // "printPreview"
QT_MOC_LITERAL(104, 1522, 16), // "showFieldChooser"
QT_MOC_LITERAL(105, 1539, 16), // "hideFieldChooser"
QT_MOC_LITERAL(106, 1556, 21), // "isFieldChooserVisible"
QT_MOC_LITERAL(107, 1578, 15), // "showFilterPanel"
QT_MOC_LITERAL(108, 1594, 15), // "hideFilterPanel"
QT_MOC_LITERAL(109, 1610, 13), // "showFindPanel"
QT_MOC_LITERAL(110, 1624, 13), // "hideFindPanel"
QT_MOC_LITERAL(111, 1638, 6), // "cancel"
QT_MOC_LITERAL(112, 1645, 18), // "isFindPanelVisible"
QT_MOC_LITERAL(113, 1664, 8), // "findNext"
QT_MOC_LITERAL(114, 1673, 12), // "findPrevious"
QT_MOC_LITERAL(115, 1686, 19), // "executeFilterDialog"
QT_MOC_LITERAL(116, 1706, 7), // "bestFit"
QT_MOC_LITERAL(117, 1714, 15), // "showContextMenu"
QT_MOC_LITERAL(118, 1730, 3) // "pos"

    },
    "Qtitan::GridViewBase\0rowClicked\0\0"
    "RowClickEventArgs*\0args\0rowDblClicked\0"
    "cellClicked\0CellClickEventArgs*\0"
    "cellDblClicked\0summaryCalculated\0"
    "contextMenu\0ContextMenuEventArgs*\0"
    "error\0ErrorEventArgs*\0columnsUpdated\0"
    "groupExpanding\0GridModelGroup*\0group\0"
    "bool&\0allow\0groupExpanded\0groupCollapsing\0"
    "groupCollapsed\0rowRemoving\0"
    "RowRemovingEventArgs*\0filterActivated\0"
    "filterDeactivated\0focusRowChanged\0"
    "oldRowIndex\0rowIndex\0focusColumnChanged\0"
    "oldColumnIndex\0columnIndex\0"
    "editorRepositoryCreated\0"
    "const GridDataTableColumn*\0column\0"
    "GridEditorRepository*\0repository\0"
    "editorCreated\0GridEditor*\0editor\0"
    "editorDestroying\0editorModifying\0"
    "editorEditModeChanged\0editorStarting\0"
    "EditorEventArgs*\0editorStarted\0"
    "editorStoping\0editorStoped\0editorValidating\0"
    "EditorValidationEventArgs*\0"
    "relationConnecting\0RelationEventArgs*\0"
    "startDragDrop\0DragArgs*\0endDragDrop\0"
    "checkDragDrop\0DragDropArgs*\0dragDrop\0"
    "newRowEditorClicked\0NewRowEventArgs*\0"
    "startGrouping\0ColumnGroupArgs*\0"
    "selectionChanged\0GridSelection*\0"
    "selection\0oldSelection\0zoomFactorChanged\0"
    "factor\0zoomChanged\0value\0"
    "summaryFooterTextChanging\0"
    "SummaryTextEventArgs*\0summaryGroupTextChanging\0"
    "SummaryGroupTextEventArgs*\0"
    "previewLinkActivated\0const GridRow*\0"
    "row\0anchor\0columnLinkActivated\0"
    "const GridColumnBase*\0editorLinkActivated\0"
    "EditorLinkEventArgs*\0slot_removeRow\0"
    "sortNone\0sortAscending\0sortDescending\0"
    "hideColumn\0groupByColumn\0alignLeftAct\0"
    "alignRightAct\0alignCenterAct\0groupByBox\0"
    "fieldChooser\0showHideFooter\0"
    "showHideGroupFooters\0expandAll\0"
    "collapseAll\0printAreaRequested\0QPrinter*\0"
    "printer\0zoomIn\0zoomOut\0printPreview\0"
    "showFieldChooser\0hideFieldChooser\0"
    "isFieldChooserVisible\0showFilterPanel\0"
    "hideFilterPanel\0showFindPanel\0"
    "hideFindPanel\0cancel\0isFindPanelVisible\0"
    "findNext\0findPrevious\0executeFilterDialog\0"
    "bestFit\0showContextMenu\0pos"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qtitan__GridViewBase[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      75,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      42,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  389,    2, 0x06 /* Public */,
       5,    1,  392,    2, 0x06 /* Public */,
       6,    1,  395,    2, 0x06 /* Public */,
       8,    1,  398,    2, 0x06 /* Public */,
       9,    0,  401,    2, 0x06 /* Public */,
      10,    1,  402,    2, 0x06 /* Public */,
      12,    1,  405,    2, 0x06 /* Public */,
      14,    0,  408,    2, 0x06 /* Public */,
      15,    2,  409,    2, 0x06 /* Public */,
      20,    1,  414,    2, 0x06 /* Public */,
      21,    2,  417,    2, 0x06 /* Public */,
      22,    1,  422,    2, 0x06 /* Public */,
      23,    1,  425,    2, 0x06 /* Public */,
      25,    0,  428,    2, 0x06 /* Public */,
      26,    0,  429,    2, 0x06 /* Public */,
      27,    2,  430,    2, 0x06 /* Public */,
      30,    2,  435,    2, 0x06 /* Public */,
      33,    2,  440,    2, 0x06 /* Public */,
      38,    1,  445,    2, 0x06 /* Public */,
      41,    1,  448,    2, 0x06 /* Public */,
      42,    1,  451,    2, 0x06 /* Public */,
      43,    1,  454,    2, 0x06 /* Public */,
      44,    1,  457,    2, 0x06 /* Public */,
      46,    1,  460,    2, 0x06 /* Public */,
      47,    1,  463,    2, 0x06 /* Public */,
      48,    1,  466,    2, 0x06 /* Public */,
      49,    1,  469,    2, 0x06 /* Public */,
      51,    1,  472,    2, 0x06 /* Public */,
      53,    1,  475,    2, 0x06 /* Public */,
      55,    1,  478,    2, 0x06 /* Public */,
      56,    1,  481,    2, 0x06 /* Public */,
      58,    1,  484,    2, 0x06 /* Public */,
      59,    1,  487,    2, 0x06 /* Public */,
      61,    1,  490,    2, 0x06 /* Public */,
      63,    2,  493,    2, 0x06 /* Public */,
      67,    1,  498,    2, 0x06 /* Public */,
      69,    1,  501,    2, 0x06 /* Public */,
      71,    1,  504,    2, 0x06 /* Public */,
      73,    1,  507,    2, 0x06 /* Public */,
      75,    2,  510,    2, 0x06 /* Public */,
      79,    2,  515,    2, 0x06 /* Public */,
      81,    1,  520,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      83,    0,  523,    2, 0x08 /* Private */,
      84,    0,  524,    2, 0x08 /* Private */,
      85,    0,  525,    2, 0x08 /* Private */,
      86,    0,  526,    2, 0x08 /* Private */,
      87,    0,  527,    2, 0x08 /* Private */,
      88,    0,  528,    2, 0x08 /* Private */,
      89,    0,  529,    2, 0x08 /* Private */,
      90,    0,  530,    2, 0x08 /* Private */,
      91,    0,  531,    2, 0x08 /* Private */,
      92,    0,  532,    2, 0x08 /* Private */,
      93,    0,  533,    2, 0x08 /* Private */,
      94,    0,  534,    2, 0x08 /* Private */,
      95,    0,  535,    2, 0x08 /* Private */,
      96,    0,  536,    2, 0x0a /* Public */,
      97,    0,  537,    2, 0x0a /* Public */,
      98,    1,  538,    2, 0x0a /* Public */,
     101,    0,  541,    2, 0x0a /* Public */,
     102,    0,  542,    2, 0x0a /* Public */,
     103,    0,  543,    2, 0x0a /* Public */,
     104,    0,  544,    2, 0x0a /* Public */,
     105,    0,  545,    2, 0x0a /* Public */,
     106,    0,  546,    2, 0x0a /* Public */,
     107,    0,  547,    2, 0x0a /* Public */,
     108,    0,  548,    2, 0x0a /* Public */,
     109,    0,  549,    2, 0x0a /* Public */,
     110,    1,  550,    2, 0x0a /* Public */,
     110,    0,  553,    2, 0x2a /* Public | MethodCloned */,
     112,    0,  554,    2, 0x0a /* Public */,
     113,    0,  555,    2, 0x0a /* Public */,
     114,    0,  556,    2, 0x0a /* Public */,
     115,    0,  557,    2, 0x0a /* Public */,
     116,    0,  558,    2, 0x0a /* Public */,
     117,    1,  559,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7,    4,
    QMetaType::Void, 0x80000000 | 7,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,    4,
    QMetaType::Void, 0x80000000 | 13,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 18,   17,   19,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 18,   17,   19,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 24,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   28,   29,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   31,   32,
    QMetaType::Void, 0x80000000 | 34, 0x80000000 | 36,   35,   37,
    QMetaType::Void, 0x80000000 | 39,   40,
    QMetaType::Void, 0x80000000 | 39,   40,
    QMetaType::Void, 0x80000000 | 39,   40,
    QMetaType::Void, 0x80000000 | 39,   40,
    QMetaType::Void, 0x80000000 | 45,    4,
    QMetaType::Void, 0x80000000 | 45,    4,
    QMetaType::Void, 0x80000000 | 45,    4,
    QMetaType::Void, 0x80000000 | 45,    4,
    QMetaType::Void, 0x80000000 | 50,    4,
    QMetaType::Void, 0x80000000 | 52,    4,
    QMetaType::Void, 0x80000000 | 54,    4,
    QMetaType::Void, 0x80000000 | 54,    4,
    QMetaType::Void, 0x80000000 | 57,    4,
    QMetaType::Void, 0x80000000 | 57,    4,
    QMetaType::Void, 0x80000000 | 60,    4,
    QMetaType::Void, 0x80000000 | 62,    4,
    QMetaType::Void, 0x80000000 | 64, 0x80000000 | 64,   65,   66,
    QMetaType::Void, QMetaType::Double,   68,
    QMetaType::Void, QMetaType::Int,   70,
    QMetaType::Void, 0x80000000 | 72,    4,
    QMetaType::Void, 0x80000000 | 74,    4,
    QMetaType::Void, 0x80000000 | 76, QMetaType::QString,   77,   78,
    QMetaType::Void, 0x80000000 | 80, QMetaType::QString,   35,   78,
    QMetaType::Void, 0x80000000 | 82,    4,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 99,  100,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,  111,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,  118,

       0        // eod
};

void Qtitan::GridViewBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GridViewBase *_t = static_cast<GridViewBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rowClicked((*reinterpret_cast< RowClickEventArgs*(*)>(_a[1]))); break;
        case 1: _t->rowDblClicked((*reinterpret_cast< RowClickEventArgs*(*)>(_a[1]))); break;
        case 2: _t->cellClicked((*reinterpret_cast< CellClickEventArgs*(*)>(_a[1]))); break;
        case 3: _t->cellDblClicked((*reinterpret_cast< CellClickEventArgs*(*)>(_a[1]))); break;
        case 4: _t->summaryCalculated(); break;
        case 5: _t->contextMenu((*reinterpret_cast< ContextMenuEventArgs*(*)>(_a[1]))); break;
        case 6: _t->error((*reinterpret_cast< ErrorEventArgs*(*)>(_a[1]))); break;
        case 7: _t->columnsUpdated(); break;
        case 8: _t->groupExpanding((*reinterpret_cast< GridModelGroup*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->groupExpanded((*reinterpret_cast< GridModelGroup*(*)>(_a[1]))); break;
        case 10: _t->groupCollapsing((*reinterpret_cast< GridModelGroup*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 11: _t->groupCollapsed((*reinterpret_cast< GridModelGroup*(*)>(_a[1]))); break;
        case 12: _t->rowRemoving((*reinterpret_cast< RowRemovingEventArgs*(*)>(_a[1]))); break;
        case 13: _t->filterActivated(); break;
        case 14: _t->filterDeactivated(); break;
        case 15: _t->focusRowChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 16: _t->focusColumnChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: _t->editorRepositoryCreated((*reinterpret_cast< const GridDataTableColumn*(*)>(_a[1])),(*reinterpret_cast< GridEditorRepository*(*)>(_a[2]))); break;
        case 18: _t->editorCreated((*reinterpret_cast< GridEditor*(*)>(_a[1]))); break;
        case 19: _t->editorDestroying((*reinterpret_cast< GridEditor*(*)>(_a[1]))); break;
        case 20: _t->editorModifying((*reinterpret_cast< GridEditor*(*)>(_a[1]))); break;
        case 21: _t->editorEditModeChanged((*reinterpret_cast< GridEditor*(*)>(_a[1]))); break;
        case 22: _t->editorStarting((*reinterpret_cast< EditorEventArgs*(*)>(_a[1]))); break;
        case 23: _t->editorStarted((*reinterpret_cast< EditorEventArgs*(*)>(_a[1]))); break;
        case 24: _t->editorStoping((*reinterpret_cast< EditorEventArgs*(*)>(_a[1]))); break;
        case 25: _t->editorStoped((*reinterpret_cast< EditorEventArgs*(*)>(_a[1]))); break;
        case 26: _t->editorValidating((*reinterpret_cast< EditorValidationEventArgs*(*)>(_a[1]))); break;
        case 27: _t->relationConnecting((*reinterpret_cast< RelationEventArgs*(*)>(_a[1]))); break;
        case 28: _t->startDragDrop((*reinterpret_cast< DragArgs*(*)>(_a[1]))); break;
        case 29: _t->endDragDrop((*reinterpret_cast< DragArgs*(*)>(_a[1]))); break;
        case 30: _t->checkDragDrop((*reinterpret_cast< DragDropArgs*(*)>(_a[1]))); break;
        case 31: _t->dragDrop((*reinterpret_cast< DragDropArgs*(*)>(_a[1]))); break;
        case 32: _t->newRowEditorClicked((*reinterpret_cast< NewRowEventArgs*(*)>(_a[1]))); break;
        case 33: _t->startGrouping((*reinterpret_cast< ColumnGroupArgs*(*)>(_a[1]))); break;
        case 34: _t->selectionChanged((*reinterpret_cast< GridSelection*(*)>(_a[1])),(*reinterpret_cast< GridSelection*(*)>(_a[2]))); break;
        case 35: _t->zoomFactorChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 36: _t->zoomChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 37: _t->summaryFooterTextChanging((*reinterpret_cast< SummaryTextEventArgs*(*)>(_a[1]))); break;
        case 38: _t->summaryGroupTextChanging((*reinterpret_cast< SummaryGroupTextEventArgs*(*)>(_a[1]))); break;
        case 39: _t->previewLinkActivated((*reinterpret_cast< const GridRow*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 40: _t->columnLinkActivated((*reinterpret_cast< const GridColumnBase*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 41: _t->editorLinkActivated((*reinterpret_cast< EditorLinkEventArgs*(*)>(_a[1]))); break;
        case 42: _t->slot_removeRow(); break;
        case 43: _t->sortNone(); break;
        case 44: _t->sortAscending(); break;
        case 45: _t->sortDescending(); break;
        case 46: _t->hideColumn(); break;
        case 47: _t->groupByColumn(); break;
        case 48: _t->alignLeftAct(); break;
        case 49: _t->alignRightAct(); break;
        case 50: _t->alignCenterAct(); break;
        case 51: _t->groupByBox(); break;
        case 52: _t->fieldChooser(); break;
        case 53: _t->showHideFooter(); break;
        case 54: _t->showHideGroupFooters(); break;
        case 55: _t->expandAll(); break;
        case 56: _t->collapseAll(); break;
        case 57: _t->printAreaRequested((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        case 58: _t->zoomIn(); break;
        case 59: _t->zoomOut(); break;
        case 60: _t->printPreview(); break;
        case 61: _t->showFieldChooser(); break;
        case 62: _t->hideFieldChooser(); break;
        case 63: { bool _r = _t->isFieldChooserVisible();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 64: _t->showFilterPanel(); break;
        case 65: _t->hideFilterPanel(); break;
        case 66: _t->showFindPanel(); break;
        case 67: _t->hideFindPanel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 68: _t->hideFindPanel(); break;
        case 69: { bool _r = _t->isFindPanelVisible();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 70: _t->findNext(); break;
        case 71: _t->findPrevious(); break;
        case 72: _t->executeFilterDialog(); break;
        case 73: _t->bestFit(); break;
        case 74: _t->showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GridViewBase::*_t)(RowClickEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::rowClicked)) {
                *result = 0;
            }
        }
        {
            typedef void (GridViewBase::*_t)(RowClickEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::rowDblClicked)) {
                *result = 1;
            }
        }
        {
            typedef void (GridViewBase::*_t)(CellClickEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::cellClicked)) {
                *result = 2;
            }
        }
        {
            typedef void (GridViewBase::*_t)(CellClickEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::cellDblClicked)) {
                *result = 3;
            }
        }
        {
            typedef void (GridViewBase::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::summaryCalculated)) {
                *result = 4;
            }
        }
        {
            typedef void (GridViewBase::*_t)(ContextMenuEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::contextMenu)) {
                *result = 5;
            }
        }
        {
            typedef void (GridViewBase::*_t)(ErrorEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::error)) {
                *result = 6;
            }
        }
        {
            typedef void (GridViewBase::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::columnsUpdated)) {
                *result = 7;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridModelGroup * , bool & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::groupExpanding)) {
                *result = 8;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridModelGroup * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::groupExpanded)) {
                *result = 9;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridModelGroup * , bool & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::groupCollapsing)) {
                *result = 10;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridModelGroup * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::groupCollapsed)) {
                *result = 11;
            }
        }
        {
            typedef void (GridViewBase::*_t)(RowRemovingEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::rowRemoving)) {
                *result = 12;
            }
        }
        {
            typedef void (GridViewBase::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::filterActivated)) {
                *result = 13;
            }
        }
        {
            typedef void (GridViewBase::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::filterDeactivated)) {
                *result = 14;
            }
        }
        {
            typedef void (GridViewBase::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::focusRowChanged)) {
                *result = 15;
            }
        }
        {
            typedef void (GridViewBase::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::focusColumnChanged)) {
                *result = 16;
            }
        }
        {
            typedef void (GridViewBase::*_t)(const GridDataTableColumn * , GridEditorRepository * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorRepositoryCreated)) {
                *result = 17;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridEditor * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorCreated)) {
                *result = 18;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridEditor * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorDestroying)) {
                *result = 19;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridEditor * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorModifying)) {
                *result = 20;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridEditor * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorEditModeChanged)) {
                *result = 21;
            }
        }
        {
            typedef void (GridViewBase::*_t)(EditorEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorStarting)) {
                *result = 22;
            }
        }
        {
            typedef void (GridViewBase::*_t)(EditorEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorStarted)) {
                *result = 23;
            }
        }
        {
            typedef void (GridViewBase::*_t)(EditorEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorStoping)) {
                *result = 24;
            }
        }
        {
            typedef void (GridViewBase::*_t)(EditorEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorStoped)) {
                *result = 25;
            }
        }
        {
            typedef void (GridViewBase::*_t)(EditorValidationEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorValidating)) {
                *result = 26;
            }
        }
        {
            typedef void (GridViewBase::*_t)(RelationEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::relationConnecting)) {
                *result = 27;
            }
        }
        {
            typedef void (GridViewBase::*_t)(DragArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::startDragDrop)) {
                *result = 28;
            }
        }
        {
            typedef void (GridViewBase::*_t)(DragArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::endDragDrop)) {
                *result = 29;
            }
        }
        {
            typedef void (GridViewBase::*_t)(DragDropArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::checkDragDrop)) {
                *result = 30;
            }
        }
        {
            typedef void (GridViewBase::*_t)(DragDropArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::dragDrop)) {
                *result = 31;
            }
        }
        {
            typedef void (GridViewBase::*_t)(NewRowEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::newRowEditorClicked)) {
                *result = 32;
            }
        }
        {
            typedef void (GridViewBase::*_t)(ColumnGroupArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::startGrouping)) {
                *result = 33;
            }
        }
        {
            typedef void (GridViewBase::*_t)(GridSelection * , GridSelection * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::selectionChanged)) {
                *result = 34;
            }
        }
        {
            typedef void (GridViewBase::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::zoomFactorChanged)) {
                *result = 35;
            }
        }
        {
            typedef void (GridViewBase::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::zoomChanged)) {
                *result = 36;
            }
        }
        {
            typedef void (GridViewBase::*_t)(SummaryTextEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::summaryFooterTextChanging)) {
                *result = 37;
            }
        }
        {
            typedef void (GridViewBase::*_t)(SummaryGroupTextEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::summaryGroupTextChanging)) {
                *result = 38;
            }
        }
        {
            typedef void (GridViewBase::*_t)(const GridRow * , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::previewLinkActivated)) {
                *result = 39;
            }
        }
        {
            typedef void (GridViewBase::*_t)(const GridColumnBase * , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::columnLinkActivated)) {
                *result = 40;
            }
        }
        {
            typedef void (GridViewBase::*_t)(EditorLinkEventArgs * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GridViewBase::editorLinkActivated)) {
                *result = 41;
            }
        }
    }
}

const QMetaObject Qtitan::GridViewBase::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Qtitan__GridViewBase.data,
      qt_meta_data_Qtitan__GridViewBase,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Qtitan::GridViewBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qtitan::GridViewBase::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Qtitan__GridViewBase.stringdata0))
        return static_cast<void*>(const_cast< GridViewBase*>(this));
    return QObject::qt_metacast(_clname);
}

int Qtitan::GridViewBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 75)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 75;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 75)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 75;
    }
    return _id;
}

// SIGNAL 0
void Qtitan::GridViewBase::rowClicked(RowClickEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Qtitan::GridViewBase::rowDblClicked(RowClickEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Qtitan::GridViewBase::cellClicked(CellClickEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Qtitan::GridViewBase::cellDblClicked(CellClickEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Qtitan::GridViewBase::summaryCalculated()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void Qtitan::GridViewBase::contextMenu(ContextMenuEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Qtitan::GridViewBase::error(ErrorEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Qtitan::GridViewBase::columnsUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void Qtitan::GridViewBase::groupExpanding(GridModelGroup * _t1, bool & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Qtitan::GridViewBase::groupExpanded(GridModelGroup * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Qtitan::GridViewBase::groupCollapsing(GridModelGroup * _t1, bool & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Qtitan::GridViewBase::groupCollapsed(GridModelGroup * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Qtitan::GridViewBase::rowRemoving(RowRemovingEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void Qtitan::GridViewBase::filterActivated()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void Qtitan::GridViewBase::filterDeactivated()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void Qtitan::GridViewBase::focusRowChanged(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void Qtitan::GridViewBase::focusColumnChanged(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void Qtitan::GridViewBase::editorRepositoryCreated(const GridDataTableColumn * _t1, GridEditorRepository * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void Qtitan::GridViewBase::editorCreated(GridEditor * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void Qtitan::GridViewBase::editorDestroying(GridEditor * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void Qtitan::GridViewBase::editorModifying(GridEditor * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void Qtitan::GridViewBase::editorEditModeChanged(GridEditor * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void Qtitan::GridViewBase::editorStarting(EditorEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 23
void Qtitan::GridViewBase::editorStarted(EditorEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void Qtitan::GridViewBase::editorStoping(EditorEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 25
void Qtitan::GridViewBase::editorStoped(EditorEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 25, _a);
}

// SIGNAL 26
void Qtitan::GridViewBase::editorValidating(EditorValidationEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 27
void Qtitan::GridViewBase::relationConnecting(RelationEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 27, _a);
}

// SIGNAL 28
void Qtitan::GridViewBase::startDragDrop(DragArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 29
void Qtitan::GridViewBase::endDragDrop(DragArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 29, _a);
}

// SIGNAL 30
void Qtitan::GridViewBase::checkDragDrop(DragDropArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 30, _a);
}

// SIGNAL 31
void Qtitan::GridViewBase::dragDrop(DragDropArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 31, _a);
}

// SIGNAL 32
void Qtitan::GridViewBase::newRowEditorClicked(NewRowEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 32, _a);
}

// SIGNAL 33
void Qtitan::GridViewBase::startGrouping(ColumnGroupArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 33, _a);
}

// SIGNAL 34
void Qtitan::GridViewBase::selectionChanged(GridSelection * _t1, GridSelection * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 34, _a);
}

// SIGNAL 35
void Qtitan::GridViewBase::zoomFactorChanged(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 35, _a);
}

// SIGNAL 36
void Qtitan::GridViewBase::zoomChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 36, _a);
}

// SIGNAL 37
void Qtitan::GridViewBase::summaryFooterTextChanging(SummaryTextEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 37, _a);
}

// SIGNAL 38
void Qtitan::GridViewBase::summaryGroupTextChanging(SummaryGroupTextEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 38, _a);
}

// SIGNAL 39
void Qtitan::GridViewBase::previewLinkActivated(const GridRow * _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 39, _a);
}

// SIGNAL 40
void Qtitan::GridViewBase::columnLinkActivated(const GridColumnBase * _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 40, _a);
}

// SIGNAL 41
void Qtitan::GridViewBase::editorLinkActivated(EditorLinkEventArgs * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 41, _a);
}
QT_END_MOC_NAMESPACE
