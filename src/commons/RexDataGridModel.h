#ifndef DataGridModel_H
#define DataGridModel_H

#include <QAbstractItemModel>

struct DataGridModelData;


class DataGridModel: public QAbstractItemModel
{

    Q_OBJECT
public:
    DataGridModel(QObject *parent = 0);
    virtual ~DataGridModel();

    virtual QVariant headerData(int section, Qt::Orientation orientation = Qt::Orientation::Horizontal,int role = Qt::DisplayRole) const;


    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual bool hasChildren(const QModelIndex &parent) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setMatrix( const QStringList& fieldsName, const QList<QVariantList> & datas);

    virtual int headerIndex(const QString& name) const;

     const QMap<QString,int>& getNbValuesByFieldMap() const;

    void clearMatrix();
    void clearData();

    const QList<QVariantList>& dataMatrix() const;
    const QStringList& fieldsList();

    void printMatrix();

    //!Affectation operator
    DataGridModel& operator=(const DataGridModel&);

    //!Comparator operator
    bool operator==(const DataGridModel&);

    //!Comparator operator
    bool operator!=(const DataGridModel&);

    //!Copie constructor
    DataGridModel(const DataGridModel&);

    //!cloner
    void clone(const DataGridModel&);

    //!Comparator
    bool isEqual(const DataGridModel&);

    void initNbValueByFieldMap();

private:
    DataGridModelData* d;

};

#endif // DataGridModel_H
