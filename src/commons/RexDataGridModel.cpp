#include <QColor>
#include <QFont>

#include "RexDataGridModel.h"
#include "RSLogger.h"


struct DataGridModelData
{
    QStringList fieldsName;
    QList<QVariantList> data;
    QMap<QString,int> nbValueByFieldMap;
};

DataGridModel::DataGridModel(QObject *parent): QAbstractItemModel(parent), d (new DataGridModelData())
{

}


DataGridModel::~DataGridModel()
{
     RSLogger::instance()->info(Q_FUNC_INFO, "End the module");
    delete d;
}

const QStringList& DataGridModel::fieldsList()
{
    return d->fieldsName;
}

//!Affectation operator
DataGridModel& DataGridModel::operator=(const DataGridModel& other)
{
    if(&other != this)
    {
        if(d)
            delete d;
        clone(other);
    }
    return *this;
}

DataGridModel::DataGridModel(const DataGridModel& other)
{
    if(&other != this)
    {
        d = new DataGridModelData();
        clone(other);
    }
}

//!Comparator operator
bool DataGridModel::operator==(const DataGridModel& other)
{
    return isEqual(other);
}

//!Comparator operator
bool DataGridModel::operator!=(const DataGridModel& other)
{
    return !isEqual(other);
}

//!Copie constructor
void DataGridModel::clone(const DataGridModel& other)
{
    if(!d)
        d = new DataGridModelData();
    d->data = other.d->data;
    d->fieldsName = other.d->fieldsName;
}

bool DataGridModel::isEqual(const DataGridModel& other)
{
    if(d->fieldsName != other.d->fieldsName)
        return false;
    if(d->fieldsName != other.d->fieldsName)
        return false;

    return true;
}


void DataGridModel::setMatrix( const QStringList& fieldsName, const QList<QVariantList> & data)
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    //--- --Add the record in the table
    if(data.size() && (data.at(0).size() != fieldsName.size()))
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"data.at(0).size() != fieldsName.size()");
        return;
    }

    d->fieldsName = fieldsName;
    d->data = data;

    initNbValueByFieldMap();

    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}

const QList<QVariantList>& DataGridModel::dataMatrix() const
{
    return d->data;
}

QVariant DataGridModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    if(d->fieldsName.isEmpty() || section >= d->fieldsName.size() || section < 0)
        return " ";

    return d->fieldsName.at(section);
}

QModelIndex DataGridModel::parent(const QModelIndex & /*child*/) const
{
    return QModelIndex();
}

bool DataGridModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.model() == this || !parent.isValid())
    {
        return rowCount(parent) > 0 && columnCount(parent) > 0;
    }
    return false;
}

int DataGridModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->data.size();
}

int DataGridModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if(d->fieldsName.count() == 0)
        return 5;
    else
        return d->fieldsName.count();
}

QModelIndex DataGridModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();

    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent))
        return QModelIndex();

    return createIndex(row, column, (void*)NULL);
}


QVariant DataGridModel::data(const QModelIndex &index, int role) const
{   
    if (!index.isValid())
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"Invalid index.");
        return QVariant();
    }

    //Check empty matrix
    if(d->data.size() == 0)
        return QVariant();

    //Check the limits
    if(index.column() >= d->fieldsName.size() || index.row() >=  d->data.size())
    {
        QString msg = QString("Invalid column no. %1. Total columns count is %2.").arg(index.column()).arg(rowCount(index.parent()));
        RSLogger::instance()->info(Q_FUNC_INFO,msg);
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
    {
        QString msg = QString("Invalid row no. %1. Total rows count is %2.").arg(index.row()).arg(rowCount(index.parent()));
        RSLogger::instance()->info(Q_FUNC_INFO,msg);
        return QVariant();
    }

    if (index.column() < 0 || index.column() >= columnCount(index.parent()))
    {
        QString msg = QString("Invalid column no. %1. Total columns count is %2.").arg(index.column()).arg(columnCount(index.parent()));
        RSLogger::instance()->info(Q_FUNC_INFO,msg);
        return QVariant();
    }

    if ((role == Qt::DisplayRole || role == Qt::EditRole))
    {
        int row = index.row();
        int column = index.column();
        const QVariantList& rowList = d->data.at(row);

        if(d->fieldsName.at(column).toLower() == "fidelity" || d->fieldsName.at(column).toLower() == "noise" )
            return QString::number( rowList.at(column).toDouble(), 'f', 8 );
        else
            return rowList.at(column);
    }

    if(role == Qt::BackgroundColorRole )
    {
        QColor color;
        if( index.row() %2 == 0 )
            color = QColor(240,240,240) ;
        else
            color = QColor(255,255,255) ;
        return color;
    }
    if (index.isValid() && role == Qt::ForegroundRole )
    {
        return QVariant( QColor( Qt::black ) );
    }

    if(role == Qt::FontRole )
    {
        return QFont("Arial narrow",10);
    }

    return QVariant();
}

bool DataGridModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    if(index.column() >= d->fieldsName.size())
        return false;

    if(index.row() >=  d->data.size())
        return false;

    QVariantList& rowList = const_cast<QVariantList&>(d->data.at(index.row()));
    rowList.replace(index.column(),value);

    return true;
}

Qt::ItemFlags DataGridModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemFlags();

    return Qt::ItemIsEnabled;
}

void DataGridModel::clearMatrix()
{
    d->data.clear();
    d->fieldsName.clear();
}

void DataGridModel::clearData()
{
    d->data.clear();
}

int DataGridModel::headerIndex(const QString& name) const
{
    return d->fieldsName.indexOf(name);
}

void DataGridModel::initNbValueByFieldMap()
{
    RSLogger::instance()->info(Q_FUNC_INFO,"Start");

    d->nbValueByFieldMap.clear();

    if( d->fieldsName.isEmpty())
    {
        RSLogger::instance()->info(Q_FUNC_INFO,"End. No field");
        return;
    }

    for(int col = 0; col < columnCount(); col++)
    {
        const QString& field = d->fieldsName.at(col);
        RSLogger::instance()->info(Q_FUNC_INFO,"field = " + field);
        //d->nbValueByFieldMap[field] = 0;
        d->nbValueByFieldMap.insert(field,0);


        for(int row = 0; row < rowCount(); row++)
        {
            QModelIndex modelIndex = index(row,col);
            if(modelIndex.isValid() && !data(modelIndex).isNull())
                d->nbValueByFieldMap[field]++;
        }
    }
    RSLogger::instance()->info(Q_FUNC_INFO,"End");
}


const QMap<QString,int>& DataGridModel::getNbValuesByFieldMap() const
{
    return d->nbValueByFieldMap;

}
