#ifndef RSOPTIONENTITYGROUPBOX_H
#define RSOPTIONENTITYGROUPBOX_H

#include <QWidget>
#include <QFrame>
#include <QBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include "IRSOptionEntity.h"

class RSOptionEntityGroupBox : public IRSOptionEntity
{
    Q_OBJECT

public:
    RSOptionEntityGroupBox(QWidget *parent = 0);
    ~RSOptionEntityGroupBox();

protected:
    void createObjects();
    void createConnections();
    void createLayouts();

    void initializeSystem();

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
    QGroupBox *m_name;
    QCheckBox *m_min;
    QCheckBox *m_max;
    QCheckBox *m_average;
    QFrame *m_body;
};

#endif // RSOPTIONENTITYGROUPBOX_H
