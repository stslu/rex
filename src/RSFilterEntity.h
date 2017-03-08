#ifndef RSFILTERENTITY_H
#define RSFILTERENTITY_H

#include <QWidget>
#include <QFrame>
#include <QLayout>

#include <QLabel>
#include <QComboBox>

class RSFilterEntity : public QFrame
{
    Q_OBJECT

public:
    RSFilterEntity(QWidget *parent = 0);
    ~RSFilterEntity();

private:
    void createObjects();
    void createConnections();
    void createLayouts();

public:
    void setLabel(const QString& label);
    QString getLabel() const;
    void addItem(const QString& item);
    void addItems(const QStringList& items);
    QString getCurrentData() const;

private:
    QLabel *m_label;
    QComboBox *m_edit;
};

#endif // RSFILTERENTITY_H
