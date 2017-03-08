/****************************************************************************
**
** Qtitan Library by Developer Machines (DataGrid component for Qt.C++)
** 
** Copyright (c) 2009-2016 Developer Machines (http://www.devmachines.com)
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

#include "QtnDBGridBandedTableView.h"

QTITAN_USE_NAMESPACE

DBGridBandedTableView* DBGridBandedTableView::create(DBGrid* grid)
{
    DBGridBandedTableView* retval = new DBGridBandedTableView(grid);
    retval->initialize();
    return retval;
}

DBGridBandedTableView::DBGridBandedTableView(DBGrid* grid)
: GridBandedTableView(grid), m_sqlModel(Q_NULL)
{
}

DBGridBandedTableView::~DBGridBandedTableView()
{
    Q_DELETE_AND_NULL(m_sqlModel);
}

bool DBGridBandedTableView::connect()
{
    Q_ASSERT(m_sqlModel == Q_NULL);
    if (!m_tableName.isEmpty() && m_dataBase.isValid())
    {
        if (!m_dataBase.isOpen() && !m_dataBase.open())
            return false;
        m_sqlModel = new QSqlTableModel(0, m_dataBase);
        m_sqlModel->setTable(m_tableName);
        m_sqlModel->select();
        setModel(m_sqlModel);
        return true;
    }
    return false;
}

void DBGridBandedTableView::setDatabase(const QSqlDatabase& dataBase)
{
    setModel(Q_NULL);
    Q_DELETE_AND_NULL(m_sqlModel);
    m_dataBase = dataBase;
    connect();
}

const QSqlDatabase& DBGridBandedTableView::dataBase() const
{
    return m_dataBase;
}

void DBGridBandedTableView::setTableName(const QString& tableName)
{
    setModel(Q_NULL);
    Q_DELETE_AND_NULL(m_sqlModel);
    m_tableName = tableName;
    connect();
}

const QString& DBGridBandedTableView::tableName() const
{
    return m_tableName;
}

void DBGridBandedTableView::raiseError(const QString& error, const QString& description)
{
    QSqlQueryModel* sqlModel = qobject_cast<QSqlQueryModel *>(modelController()->model());
    if (sqlModel == Q_NULL || sqlModel->lastError().type() == QSqlError::NoError)
    {
        GridViewBase::raiseError(error, description);
        return;
    }
    GridViewBase::raiseError(error, description + "\r\n" + 
        QString("Driver: %1. Database: %2.").arg(sqlModel->lastError().driverText()).arg(sqlModel->lastError().databaseText()));
}

