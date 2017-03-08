/****************************************************************************
**
** Qtitan Library by Developer Machines (DataGrid component for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/


#include "QtnGridDef.h"


const unsigned kGridDefHeaderHeight = 45;
const unsigned kGridDefHeaderHeightIndent = 7;
const unsigned kGridDefHeaderWidthIndent = 5;

const unsigned kGridDefColumnWidth = 65;
const unsigned kGridDefColumnHeight = 20;
const int kGridMinTableColumnWidth = 20;
const int kGridMaxTableColumnWidth = 10000;
const int kGridColumnResizerIndent = 4;
const int kGridSummaryCellBorder = 4;

const unsigned kGridFieldChooserWidth = 180;
const unsigned kGridFieldChooserHeight = 300;

const unsigned kGridColumnAutoScrollIndent = 35;
const unsigned kQuickCustomizeButtonWidth = 15;

const int kGridColumnIndexInvalid = -1;
const int kGridGroupRowIndent = 20;

const int kGridNormalScrollStep = 24;
const int kGridWebScrollStep = 24;
const int kGridNewRowIndent = 5;
const int kGridOffice11GroupRowHeihgtIncrement = 13;
const int kGridOffice11GroupRowBorderIncrement = 1;

const int kGridFastScrollEffectDuration = 500;
const int kGridFilterPanelPeer = 5;
const int kGridTextEditorPadding = 4;
const int kGridPreviewExpandButtonHeight = 16;
const int kGridDoubleLinesIndent = 1/*px*/;

const char* QtnGridColumnsString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Columns");
const char* QtnGridBandsString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Bands");
const char* QtnGridFieldChooserString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Field Chooser");
const char* QtnGridRemoveColumnString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Hide This Column");
const char* QtnGridGroupFootersString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Group Footers");
const char* QtnGridRemoveRowString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Remove Row");
const char* QtnGridEmptyFieldChooserColumnString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Drag and drop columns here to customize layout");
const char* QtnGridEmptyFieldChooserBandString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Drag and drop bands here to customize layout");
const char* QtnGridHeaderString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Drag a column header here to group by that column");
const char* QtnAddNewRowString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Click here to add a new row");
const char* QtnGridIsNotConfiguredString = QT_TRANSLATE_NOOP("Qtitan::Grid", "The grid is not configured to show data.");
const char* QtnGridValidationErrorString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Validation Error");
const char* QtnGridDataErrorString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Data Error");
const char* QtnGridDataErrorDescription_001_String = QT_TRANSLATE_NOOP("Qtitan::Grid", "Can't set value to model.");
const char* QtnGridDataErrorDescription_002_String = QT_TRANSLATE_NOOP("Qtitan::Grid", "Can't add new row to the model. Data provider is invalid.");
const char* QtnGridDataErrorDescription_003_String = QT_TRANSLATE_NOOP("Qtitan::Grid", "Can't add new row to the model. The row is not visible.");
const char* QtnGridDataErrorDescription_004_String = QT_TRANSLATE_NOOP("Qtitan::Grid", "Can't submit row data to model.");
const char* QtnGridDataErrorDescription_005_String = QT_TRANSLATE_NOOP("Qtitan::Grid", "Can't remove row.");
const char* QtGridDelegateErrorString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Qt-delegate is not configured properly.");
const char* QtnGridEmptyFilterString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Filter is Empty");
const char* QtnGridSetFilterTitleString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Set Filter by Value");
const char* QtnGridFilterBlanksString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Blanks");
const char* QtnGridFilterNoBlanksString = QT_TRANSLATE_NOOP("Qtitan::Grid", "No blanks");
const char* QtnGridFilterSelectAllString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Select All");
const char* QtnGridFilterUnselectAllString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Unselect All");
const char* QtnGridFilterDialogString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Filter Dialog");
const char* QtnGridFilterHistoyString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Filter History");
const char* QtnGridFilterStatusText = QT_TRANSLATE_NOOP("Qtitan::Grid", "Quantity %1  <b>Filtered %2</b>   Conditions %3   ");
const char* QtnGridFilterHistoryText = QT_TRANSLATE_NOOP("Qtitan::Grid", "Filter History");

const char* QtnGridColumnMenuString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Column Menu");
const char* QtnGridPreviewRowString = QT_TRANSLATE_NOOP("Qtitan::Grid", "PREVIEW");

const char* QtnGridFixedLeftString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Fixed At Left");
const char* QtnGridFixedRightString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Fixed At Right");
const char* QtnGridFixedTopString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Fixed At Top");
const char* QtnGridFixedBottomString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Fixed At Bottom");

const char* QtnGridTrueString = QT_TRANSLATE_NOOP("Qtitan::Grid", "true");
const char* QtnGridFalseString = QT_TRANSLATE_NOOP("Qtitan::Grid", "false");
const char* QtnGridOkString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Ok");
const char* QtnGridCancelString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Cancel");
const char* QtnGridCloseString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Close");
const char* QtnGridResetString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Reset");
const char* QtnGridBackString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Back");
const char* QtnGridForwardString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Forward");
const char* QtnGridCaseSensitiveString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Case Sensitive");
const char* QtnGridFiltrationString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Filtration");
const char* QtnGridFindPlaceholderTextString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Type here to find a text");

const char* QtnGridFilterString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Filter");

const char* QtnGridActionLoadString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Load");
const char* QtnGridActionSaveString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Save");
const char* QtnGridActionCopyString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Copy");
const char* QtnGridActionPasteString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Paste");
const char* QtnGridActionZoomInString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Zoom In");
const char* QtnGridActionZoomOutString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Zoom Out");
const char* QtnGridActionAdjustSizeString = QT_TRANSLATE_NOOP("Qtitan::Grid", "A&djust Size");
const char* QtnGridActionActualSizeString = QT_TRANSLATE_NOOP("Qtitan::Grid", "&Actual Size");
const char* QtnGridActionPrintString = QT_TRANSLATE_NOOP("Qtitan::Grid", "&Print");
const char* QtnGridActionFilterString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Filter");
const char* QtnGridActionFindString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Find");
const char* QtnGridActionBestFitString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Best Fit");
const char* QtnGridActionFooterString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Footer");
const char* QtnGridActionSortAscendingString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Sort Ascending");
const char* QtnGridActionSortDescendingString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Sort Descending");
const char* QtnGridActionClearSortingString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Clear Sorting");
const char* QtnGridActionExpandAllString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Expand All");
const char* QtnGridActionCollapseAllString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Collapse All");

const char* QtnGridActionGroupByThisColumnString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Group By This Column");
const char* QtnGridActionGroupByBoxString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Group By Box");


const char* QtnGridActionAlignmentString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Alignment");

const char* QtnGridActionLeftString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Left");
const char* QtnGridActionRightString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Right");
const char* QtnGridActionCenterString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Center");


const char* QtnGridActionSumString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Sum");
const char* QtnGridActionMinString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Min");
const char* QtnGridActionMaxString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Max");
const char* QtnGridActionCountString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Count");
const char* QtnGridActionAverageString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Average");
const char* QtnGridActionNoneString = QT_TRANSLATE_NOOP("Qtitan::Grid", "None");

const char* QtnGridSumTextString = QT_TRANSLATE_NOOP("Qtitan::Grid", "SUM");
const char* QtnGridMinTextString = QT_TRANSLATE_NOOP("Qtitan::Grid", "MIN");
const char* QtnGridMaxTextString = QT_TRANSLATE_NOOP("Qtitan::Grid", "MAX");
const char* QtnGridAverageTextString = QT_TRANSLATE_NOOP("Qtitan::Grid", "AVG");

const char* QtnGridCellString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Cell");
const char* QtnGridCellsString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Cells");

const char* QtnGridOpenFileString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Open File");
const char* QtnGridSaveAsString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Save As");

const char* QtnGridFilterEqualString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Equal");
const char* QtnGridFilterNotEqualString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Not Equal");
const char* QtnGridFilterLessString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Less");
const char* QtnGridFilterLessorOrEqualString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Less or Equal");
const char* QtnGridFilterGreaterString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Greater");
const char* QtnGridFilterGreaterOrEqualString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Greater or Equal");

const char* QtnGridFilterColumnString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Column:");
const char* QtnGridFilterFindWhatString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Find what:");
const char* QtnGridFilterSearchInResultsString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Search in results");

const char* QtnGridFilteORString = QT_TRANSLATE_NOOP("Qtitan::Grid", "OR");
const char* QtnGridFilterANDString = QT_TRANSLATE_NOOP("Qtitan::Grid", "AND");
const char* QtnGridFilterANDNOTString = QT_TRANSLATE_NOOP("Qtitan::Grid", "AND NOT");
const char* QtnGridFilterNOTString = QT_TRANSLATE_NOOP("Qtitan::Grid", "NOT");

const char* QtnGridFilteIsBlankString = QT_TRANSLATE_NOOP("Qtitan::Grid", "is Blank");
const char* QtnGridFilterIsNotBlankString = QT_TRANSLATE_NOOP("Qtitan::Grid", "is not Blank");

const char* QtnGridFilterLikekString = QT_TRANSLATE_NOOP("Qtitan::Grid", "LIKE");
const char* QtnGridFilterNotLikekString = QT_TRANSLATE_NOOP("Qtitan::Grid", "NOT LIKE");

const char* QtnGridCardItemTitleRecordString = QT_TRANSLATE_NOOP("Qtitan::Grid", "Record");

