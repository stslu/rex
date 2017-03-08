#ifndef IREXDATAGRID
#define IREXDATAGRID
#include <QStringList>
#include <QVariantList>
#include <QProgressBar>
class DataGridModel;

class IRexDataGrid
{
public:
    virtual bool setDataModelMatrix(const QStringList& fields, const QList<QVariantList>& data) = 0;
    virtual void setProgressBarValue(int) = 0;
    virtual QProgressBar* progressBar() = 0;
    virtual const QStringList& getFields() const = 0;
    virtual const QList<QVariantList>& getData() const = 0;
    virtual const DataGridModel* model() const = 0;
};

#endif // IREXDATAGRID

