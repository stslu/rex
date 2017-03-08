/****************************************************************************
**
** Qtitan Framework by Developer Machines
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

#include "QtnGridDsgnPlugin.h"
#include "QtitanGrid.h"
#include "QtitanDBGrid.h"

#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowManagerInterface>
#include <QDesignerFormWindowInterface>
#include <QDesignerPropertyEditorInterface>

QTITAN_USE_NAMESPACE

/* GridDsgnPlugin */
GridDsgnPlugin::GridDsgnPlugin(QObject *parent)
    : QObject(parent), m_core(0)
{
    initialized = false;
}

void GridDsgnPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (initialized)
        return;

    initialized = true;
    m_core = core;
}

bool GridDsgnPlugin::isInitialized() const
{
    return initialized;
}

QWidget *GridDsgnPlugin::createWidget(QWidget *parent)
{
    Grid* ptr = new Grid(parent);
    return ptr;
}

QString GridDsgnPlugin::name() const
{
    return create_qtitan_className("Grid");
}

QString GridDsgnPlugin::group() const
{
    return "Developer Machines: Components";
}

QIcon GridDsgnPlugin::icon() const
{
    return QIcon(":res/grid_icon.png");
}

QString GridDsgnPlugin::toolTip() const
{
    return "Advanced DataGrid for Qt.C++";
}

QString GridDsgnPlugin::whatsThis() const
{
    return "Advanced DataGrid for Qt.C++";
}

bool GridDsgnPlugin::isContainer() const
{
    return false;
}

QString GridDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"" + create_qtitan_className("Grid") + "\" name=\"grid\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>300</width>\n"
           "    <height>200</height>\n"
           "   </rect>\n"
           "  </property>\n"
           "  <property name=\"toolTip\" >\n"
           "   <string>Advanced DataGrid for Qt</string>\n"
           "  </property>\n"
           "  <property name=\"whatsThis\" >\n"
           "   <string>Advanced DataGrid for Qt.</string>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>\n";
}

QString GridDsgnPlugin::includeFile() const
{
    return "QtitanGrid.h";
}




/* DBGridDsgnPlugin */
DBGridDsgnPlugin::DBGridDsgnPlugin(QObject *parent)
    : QObject(parent), m_core(0)
{
    initialized = false;
}

void DBGridDsgnPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (initialized)
        return;

    initialized = true;
    m_core = core;
}

bool DBGridDsgnPlugin::isInitialized() const
{
    return initialized;
}

QWidget *DBGridDsgnPlugin::createWidget(QWidget *parent)
{
    DBGrid* ptr = new DBGrid(parent);
    return ptr;
}

QString DBGridDsgnPlugin::name() const
{
    return create_qtitan_className("DBGrid");
}

QString DBGridDsgnPlugin::group() const
{
    return "Developer Machines: Components";
}

QIcon DBGridDsgnPlugin::icon() const
{
    return QIcon(":res/grid_icon.png");
}

QString DBGridDsgnPlugin::toolTip() const
{
    return tr("Advanced DataGrid for Qt with Database support.");
}

QString DBGridDsgnPlugin::whatsThis() const
{
    return tr("Advanced DataGrid for Qt with Database support.");
}

bool DBGridDsgnPlugin::isContainer() const
{
    return false;
}

QString DBGridDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"" + create_qtitan_className("DBGrid") + "\" name=\"grid\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>300</width>\n"
           "    <height>200</height>\n"
           "   </rect>\n"
           "  </property>\n"
           "  <property name=\"toolTip\" >\n"
           "   <string>Advanced DataGrid for Qt with Database support.</string>\n"
           "  </property>\n"
           "  <property name=\"whatsThis\" >\n"
           "   <string>Advanced DataGrid for Qt with Database support.</string>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>\n";
}

QString DBGridDsgnPlugin::includeFile() const
{
    return "QtitanDBGrid.h";
}
