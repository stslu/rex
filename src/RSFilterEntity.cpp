#include "RSFilterEntity.h"

RSFilterEntity::RSFilterEntity(QWidget *parent) : QFrame(parent)
{
    createObjects();
    createConnections();
    createLayouts();
}

RSFilterEntity::~RSFilterEntity()
{

}

void RSFilterEntity::createObjects()
{
    m_label = new QLabel;
    m_label->setObjectName("m_label");

    m_edit = new QComboBox;
    m_edit->setObjectName("m_edit");
    m_edit->addItem(tr("All"));
    m_label->setBuddy(m_edit);
}

void RSFilterEntity::createConnections()
{
    connect(m_edit, SIGNAL(currentTextChanged(QString)), this, SIGNAL(emitCurrentIndexChanged(QString)));
}

void RSFilterEntity::createLayouts()
{
    QHBoxLayout *m_mainLayout = new QHBoxLayout;
    m_mainLayout->addWidget(m_label);
    m_mainLayout->addWidget(m_edit);
    m_mainLayout->setStretchFactor(m_label, 0);
    m_mainLayout->setStretchFactor(m_edit, 1);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setMargin(0);

    setLayout(m_mainLayout);
}

void RSFilterEntity::setLabel(const QString& label)
{
    m_label->setText(label);
}

QString RSFilterEntity::getLabel() const
{
    return m_label->text();
}

void RSFilterEntity::addItem(const QString& item)
{
    m_edit->addItem(item);
}

void RSFilterEntity::addItems(const QStringList& items)
{
    m_edit->addItems(items);
}

QString RSFilterEntity::getCurrentData() const
{
    return m_edit->currentText();
}
