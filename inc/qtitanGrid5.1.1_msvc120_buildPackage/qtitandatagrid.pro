TEMPLATE = subdirs

include(src/shared/platform.pri)

CONFIG += ordered

QTN_BUILD_SOURCE = 0
QTN_COMPONENT_VERSION = $$fromfile($$PWD/src/grid/dbgrid.pri, QTN_COMPONENT_VERSION)

exists($$PWD/src/shared/dbgrid/dbgrid.pro) {
    !isEmpty(QTN_COMPONENT_VERSION):QTN_BUILD_SOURCE = 1
}

equals(QTN_BUILD_SOURCE, 1) {
    message(QtitanDataGrid source code being prepared for building...)
    SUBDIRS = \
              src/shared/dbgrid \
              src/griddsgn
}

SUBDIRS += \
          demos/grid/AddingNewRowFeature \
          demos/grid/BandedTableView \
          demos/grid/ColumnEditorsFeature \
          demos/grid/CustomFilter \
          demos/grid/DragDropFeature \
          demos/grid/FixedColumns \
          demos/grid/GroupAndFooterSummaries \
          demos/grid/TableViewHorizontalLayout \
          demos/grid/HtmlManipulation \
          demos/grid/LargeDataSetExample \
          demos/grid/MSOfficeMode \
          demos/grid/MultiSelection \
          demos/grid/PreviewRow \
          demos/grid/ProgressBarEditor \
          demos/grid/RelationMode \
          demos/grid/RowAutoHeightFeature \
          demos/grid/ScrollingByPixelsFeature \
          demos/grid/StandartQtDelegates \
          demos/grid/MDIApplication \
          demos/grid/CardView \
          demos/grid/CardViewSummaries \
          demos/grid/CardViewHorizontalLayout \
          demos/grid/CardViewNewCardFeature

exists($$PWD/doc/qdocs.pri) {
    include($$PWD/doc/qdocs.pri)
}

