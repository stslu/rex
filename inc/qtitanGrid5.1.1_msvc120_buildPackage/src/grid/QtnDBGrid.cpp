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


#include "QtnDBGrid.h"
#include "QtnDBGridTableView.h"
#include "QtnDBGridBandedTableView.h"

QTITAN_USE_NAMESPACE

/* DBGrid */
DBGrid::DBGrid(QWidget* parent)
: Grid(parent)
{
}

GridViewBase* DBGrid::createView(GridViewType type)
{
    switch (type)
    {
    case TableView:
        {
            return DBGridTableView::create(this);
        }
        break;
    case BandedTableView:
        {
            return DBGridBandedTableView::create(this);
        }
        break;
    default:
        break;
    }
    return Q_NULL;
}

