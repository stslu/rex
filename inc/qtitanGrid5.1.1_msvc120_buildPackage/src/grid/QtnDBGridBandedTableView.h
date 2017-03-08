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

#ifndef QTN_DBGRIDBANDEDTABLEVIEW_H
#define QTN_DBGRIDBANDEDTABLEVIEW_H

#include <QtSql>
#include "QtnDBGrid.h"
#include "QtnGridBandedTableView.h"

QTITAN_BEGIN_NAMESPACE

class QTITAN_EXPORT DBGridBandedTableView: public GridBandedTableView
{
public:
    DBGridBandedTableView(DBGrid* grid);
    virtual ~DBGridBandedTableView();
    void setDatabase(const QSqlDatabase& dataBase);
    const QSqlDatabase& dataBase() const;
    void setTableName(const QString& tableName);
    const QString& tableName() const;
    static DBGridBandedTableView* create(DBGrid* grid);
protected:
    virtual void raiseError(const QString& error, const QString& description);
private:
    bool connect();
    QSqlDatabase m_dataBase;
    QString m_tableName;
    QSqlTableModel* m_sqlModel;
};

QTITAN_END_NAMESPACE

#endif //QTN_DBGRIDBANDEDTABLEVIEW_H

