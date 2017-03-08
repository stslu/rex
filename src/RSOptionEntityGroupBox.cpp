#include "RSOptionEntityGroupBox.h"
#include "RSOptionsManagerData.h"

RSOptionEntityGroupBox::RSOptionEntityGroupBox(QWidget *parent) : IRSOptionEntity(parent)
{
    createObjects();
    createConnections();
    createLayouts();
    initializeSystem();
}

RSOptionEntityGroupBox::~RSOptionEntityGroupBox()
{

}

void RSOptionEntityGroupBox::createObjects()
{  
    m_name = new QGroupBox;
    m_name->setObjectName("m_name");
    m_name->setCheckable(true);
    m_name->setChecked(true);

    m_body = new QFrame;
    m_body->setObjectName("m_body");

    m_min = new QCheckBox;
    m_min->setObjectName("m_min");
    m_min->setText(tr("Min"));

    m_max = new QCheckBox;
    m_max->setObjectName("m_max");
    m_max->setText(tr("Max"));

    m_average = new QCheckBox;
    m_average->setObjectName("m_average");
    m_average->setText(tr("Average"));



}

void RSOptionEntityGroupBox::createConnections()
{
    connect(m_name, SIGNAL(clicked(bool)), this, SLOT(slotNameClicked(bool)));
    connect(m_min, SIGNAL(clicked(bool)), this, SLOT(slotOptionClicked(bool)));
    connect(m_max, SIGNAL(clicked(bool)), this, SLOT(slotOptionClicked(bool)));
    connect(m_average, SIGNAL(clicked(bool)), this, SLOT(slotOptionClicked(bool)));
}

void RSOptionEntityGroupBox::createLayouts()
{
    QVBoxLayout *m_bodyLayout = new QVBoxLayout;
    m_bodyLayout->addWidget(m_min);
    m_bodyLayout->addWidget(m_max);
    m_bodyLayout->addWidget(m_average);
    m_bodyLayout->setSpacing(0);
    m_bodyLayout->setMargin(0);
    m_body->setLayout(m_bodyLayout);

    QVBoxLayout *m_nameLayout = new QVBoxLayout;
    m_nameLayout->addWidget(m_body);
    m_nameLayout->setSpacing(10);
    m_nameLayout->setMargin(10);
    m_name->setLayout(m_nameLayout);

    QVBoxLayout *m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(m_name);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setMargin(10);
    m_mainLayout->setAlignment(Qt::AlignTop);
    setLayout(m_mainLayout);
}

void RSOptionEntityGroupBox::initializeSystem()
{
    slotNameClicked(m_name->isChecked());
}

void RSOptionEntityGroupBox::setNameTitle(const QString& name)
{
    m_name->setTitle(name);
}

QString RSOptionEntityGroupBox::getNameTitle() const
{
    return m_name->title();
}

void RSOptionEntityGroupBox::setNameData(bool data)
{
    m_name->setChecked(data);
}

bool RSOptionEntityGroupBox::getNameData() const
{
    return m_name->isChecked();
}

void RSOptionEntityGroupBox::setOptionData(const QString& name)
{
    RSOptionsManagerData::Instance()->setOptionData(name, m_min->text(), m_min->isChecked());
    RSOptionsManagerData::Instance()->setOptionData(name, m_max->text(), m_max->isChecked());
    RSOptionsManagerData::Instance()->setOptionData(name, m_average->text(), m_average->isChecked());
}

void RSOptionEntityGroupBox::slotNameClicked(bool data)
{
    emit emitNameClicked(data);
}

void RSOptionEntityGroupBox::slotOptionClicked(bool data)
{
    QCheckBox *m_optionData = qobject_cast<QCheckBox*>(sender());
    QString m_optionType = m_name->title();
    QString m_optionName = m_optionData->text();
    RSOptionsManagerData::Instance()->setOptionData(m_optionType, m_optionName, data);
}
