#include "RSOptionEntityCheckBox.h"
#include "RSOptionsManagerData.h"
#include "RSMessageView.h"

RSOptionEntityCheckBox::RSOptionEntityCheckBox(QWidget *parent) : IRSOptionEntity(parent)
{
    createObjects();
    createConnections();
    createLayouts();
    initializeSystem();
}

RSOptionEntityCheckBox::~RSOptionEntityCheckBox()
{

}

void RSOptionEntityCheckBox::createObjects()
{
    m_name = new QCheckBox;
    m_name->setObjectName("m_name");
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

void RSOptionEntityCheckBox::createConnections()
{
    connect(m_name, SIGNAL(clicked(bool)), this, SLOT(slotNameClicked(bool)));
    connect(m_min, SIGNAL(clicked(bool)), this, SLOT(slotOptionClicked(bool)));
    connect(m_max, SIGNAL(clicked(bool)), this, SLOT(slotOptionClicked(bool)));
    connect(m_average, SIGNAL(clicked(bool)), this, SLOT(slotOptionClicked(bool)));
}

void RSOptionEntityCheckBox::createLayouts()
{
    QVBoxLayout *m_bodyLayout = new QVBoxLayout;
    m_bodyLayout->addWidget(m_min);
    m_bodyLayout->addWidget(m_max);
    m_bodyLayout->addWidget(m_average);
    m_bodyLayout->setSpacing(0);
    m_bodyLayout->setMargin(0);
    m_body->setLayout(m_bodyLayout);

    QVBoxLayout *m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(m_name);
    m_mainLayout->addWidget(m_body);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setMargin(10);
    m_mainLayout->setAlignment(Qt::AlignTop);
    setLayout(m_mainLayout);
}

void RSOptionEntityCheckBox::initializeSystem()
{
    slotNameClicked(m_name->isChecked());
}

void RSOptionEntityCheckBox::updateNameData()
{
    int m_count = 0;
    m_count += (int)m_min->isChecked();
    m_count += (int)m_max->isChecked();
    m_count += (int)m_average->isChecked();
    bool m_data = false;

    if(m_count == 3)
    {
        m_data = true;
    }

    m_name->setChecked(m_data);
}

void RSOptionEntityCheckBox::setNameTitle(const QString& name)
{
    m_name->setText(name);
}

QString RSOptionEntityCheckBox::getNameTitle() const
{
    return m_name->text();
}

void RSOptionEntityCheckBox::setNameData(bool data)
{
    m_name->setChecked(data);
}

bool RSOptionEntityCheckBox::getNameData() const
{
    return m_name->isChecked();
}

void RSOptionEntityCheckBox::setOptionData(const QString &name)
{
    RSOptionsManagerData::Instance()->setOptionData(name, m_min->text(), m_min->isChecked());
    RSOptionsManagerData::Instance()->setOptionData(name, m_max->text(), m_max->isChecked());
    RSOptionsManagerData::Instance()->setOptionData(name, m_average->text(), m_average->isChecked());
}


void RSOptionEntityCheckBox::slotNameClicked(bool data)
{
    m_body->setVisible(data);
    m_min->setChecked(data);
    m_max->setChecked(data);
    m_average->setChecked(data);
    emit emitNameClicked(data);
}

void RSOptionEntityCheckBox::slotOptionClicked(bool data)
{
    updateNameData();
    QCheckBox *m_optionData = qobject_cast<QCheckBox*>(sender());
    QString m_optionType = m_name->text();
    QString m_optionName = m_optionData->text();
    RSOptionsManagerData::Instance()->setOptionData(m_optionType, m_optionName, data);
}


