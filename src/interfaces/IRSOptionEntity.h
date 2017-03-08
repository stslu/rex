#ifndef IRSOPTIONENTITY_H
#define IRSOPTIONENTITY_H

#include <QWidget>
#include <QFrame>

class IRSOptionEntity : public QFrame
{
    Q_OBJECT

public:
    IRSOptionEntity(QWidget *parent = 0);
    virtual ~IRSOptionEntity();

public:
    virtual void setNameTitle(const QString& name) = 0;
    virtual QString getNameTitle() const = 0;
    virtual void setNameData(bool data) = 0;
    virtual bool getNameData() const = 0;
    virtual void setOptionData(const QString& name) = 0;

};

#endif // IRSOPTIONENTITY_H
