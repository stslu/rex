#ifndef RSOPTIONENTITYCHECKBOX_H
#define RSOPTIONENTITYCHECKBOX_H

#include <QWidget>
#include <QFrame>
#include <QLayout>
#include <QCheckBox>
#include "IRSOptionEntity.h"

class RSOptionEntityCheckBox : public IRSOptionEntity
{
    Q_OBJECT

public:
    RSOptionEntityCheckBox(QWidget *parent = 0);
    ~RSOptionEntityCheckBox();

private:
    void createObjects();
    void createConnections();
    void createLayouts();

    void initializeSystem();
    void updateNameData();

public:
    void setNameTitle(const QString& name);
    QString getNameTitle() const;
    void setNameData(bool data);
    bool getNameData() const;
    void setOptionData(const QString& name);

public slots:
    void slotNameClicked(bool data);
    void slotOptionClicked(bool data);

private:
    QCheckBox *m_name;
    QCheckBox *m_min;
    QCheckBox *m_max;
    QCheckBox *m_average;
    QFrame *m_body;
};

#endif // RSOPTIONENTITY_H
