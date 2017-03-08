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

#ifndef QTN_GRID_DEF_H
#define QTN_GRID_DEF_H

#include "QtitanDef.h"

#define QTN_VERSION_DATAGRID_STR QTN_QUOTE(QTN_VERSION_DATAGRID)

extern const unsigned kGridDefHeaderHeight;
extern const unsigned kGridDefHeaderWidthIndent;
extern const unsigned kGridDefHeaderHeightIndent;

extern const unsigned kGridDefColumnWidth;
extern const unsigned kGridDefColumnHeight;
extern const int kGridMinTableColumnWidth;
extern const int kGridMaxTableColumnWidth;
extern const int kGridColumnResizerIndent;
extern const unsigned kGridFieldChooserWidth;
extern const unsigned kGridFieldChooserHeight;
extern const unsigned kGridColumnAutoScrollIndent;
extern const unsigned kQuickCustomizeButtonWidth;
extern const int kGridSummaryCellBorder;
extern const int kGridColumnIndexInvalid;
extern const int kGridGroupRowIndent;
extern const int kGridNormalScrollStep;
extern const int kGridWebScrollStep;
extern const int kGridOffice11GroupRowHeihgtIncrement;
extern const int kGridOffice11GroupRowBorderIncrement;
extern const int kGridFastScrollEffectDuration;
extern const int kGridFilterPanelPeer;
extern const int kGridNewRowIndent;
extern const int kGridTextEditorPadding;
extern const int kGridPreviewExpandButtonHeight;
extern const int kGridDoubleLinesIndent;

extern const char* QtnGridColumnsString;
extern const char* QtnGridBandsString;
extern const char* QtnGridFieldChooserString;
extern const char* QtnGridRemoveColumnString;
extern const char* QtnGridGroupFootersString;
extern const char* QtnGridRemoveRowString;
extern const char* QtnGridEmptyFieldChooserColumnString;
extern const char* QtnGridEmptyFieldChooserBandString;
extern const char* QtnAddNewRowString;
extern const char* QtnGridHeaderString;
extern const char* QtnGridIsNotConfiguredString;
extern const char* QtnGridValidationErrorString;
extern const char* QtnGridDataErrorString;
extern const char* QtnGridDataErrorDescription_001_String;
extern const char* QtnGridDataErrorDescription_002_String;
extern const char* QtnGridDataErrorDescription_003_String;
extern const char* QtnGridDataErrorDescription_004_String;
extern const char* QtnGridDataErrorDescription_005_String;
extern const char* QtGridDelegateErrorString;
extern const char* QtnGridEmptyFilterString;
extern const char* QtnGridSetFilterTitleString;
extern const char* QtnGridFilterBlanksString;
extern const char* QtnGridFilterNoBlanksString;
extern const char* QtnGridFilterSelectAllString;
extern const char* QtnGridFilterUnselectAllString;
extern const char* QtnGridFilterDialogString;
extern const char* QtnGridFilterHistoyString;
extern const char* QtnGridFilterStatusText;
extern const char* QtnGridFilterHistoryText;
extern const char* QtnGridColumnMenuString;
extern const char* QtnGridPreviewRowString;
extern const char* QtnGridFixedLeftString;
extern const char* QtnGridFixedRightString;
extern const char* QtnGridFixedTopString;
extern const char* QtnGridFixedBottomString;

extern const char* QtnGridTrueString;
extern const char* QtnGridFalseString;
extern const char* QtnGridOkString;
extern const char* QtnGridCancelString;
extern const char* QtnGridCloseString;
extern const char* QtnGridResetString;
extern const char* QtnGridBackString;
extern const char* QtnGridForwardString;
extern const char* QtnGridFilterString;
extern const char* QtnGridCaseSensitiveString;
extern const char* QtnGridFiltrationString;
extern const char* QtnGridFindPlaceholderTextString;

extern const char* QtnGridActionLoadString;
extern const char* QtnGridActionSaveString;
extern const char* QtnGridActionCopyString;
extern const char* QtnGridActionPasteString;
extern const char* QtnGridActionZoomInString;
extern const char* QtnGridActionZoomOutString;
extern const char* QtnGridActionAdjustSizeString;
extern const char* QtnGridActionActualSizeString;
extern const char* QtnGridActionPrintString;
extern const char* QtnGridActionFilterString;
extern const char* QtnGridActionFindString;
extern const char* QtnGridActionBestFitString;
extern const char* QtnGridActionFooterString;

extern const char* QtnGridActionSortAscendingString;
extern const char* QtnGridActionSortDescendingString;
extern const char* QtnGridActionClearSortingString;

extern const char* QtnGridActionExpandAllString;
extern const char* QtnGridActionCollapseAllString;
extern const char* QtnGridActionGroupByThisColumnString;
extern const char* QtnGridActionGroupByBoxString;

extern const char* QtnGridActionAlignmentString;
extern const char* QtnGridActionLeftString;
extern const char* QtnGridActionRightString;
extern const char* QtnGridActionCenterString;
extern const char* QtnGridActionSumString;
extern const char* QtnGridActionMinString;
extern const char* QtnGridActionMaxString;
extern const char* QtnGridActionCountString;
extern const char* QtnGridActionAverageString;
extern const char* QtnGridActionNoneString;

extern const char* QtnGridSumTextString;
extern const char* QtnGridMinTextString;
extern const char* QtnGridMaxTextString;
extern const char* QtnGridAverageTextString;
extern const char* QtnGridCellString;
extern const char* QtnGridCellsString;
extern const char* QtnGridOpenFileString;
extern const char* QtnGridSaveAsString;

extern const char* QtnGridFilterEqualString;
extern const char* QtnGridFilterNotEqualString;
extern const char* QtnGridFilterLessString;
extern const char* QtnGridFilterLessorOrEqualString;
extern const char* QtnGridFilterGreaterString;
extern const char* QtnGridFilterGreaterOrEqualString;

extern const char* QtnGridFilterColumnString;
extern const char* QtnGridFilterFindWhatString;
extern const char* QtnGridFilterSearchInResultsString;

extern const char* QtnGridFilteORString;
extern const char* QtnGridFilterANDString;
extern const char* QtnGridFilterANDNOTString;
extern const char* QtnGridFilterNOTString;

extern const char* QtnGridFilteIsBlankString;
extern const char* QtnGridFilterIsNotBlankString;
extern const char* QtnGridFilterLikekString;
extern const char* QtnGridFilterNotLikekString;
extern const char* QtnGridCardItemTitleRecordString;

#endif //QTN_GRID_DEF_H

