#ifndef RSDATABASECONFIG_H
#define RSDATABASECONFIG_H

#include <QDialog>

namespace Ui
{
class RSDatabaseConfig;
}

class RSDatabaseConfig : public QDialog
{
    Q_OBJECT

public:
    RSDatabaseConfig(QWidget *parent = 0);
    ~RSDatabaseConfig();

    void setMessage(const QString&);
    void setG7Path(const QString&);
    void setG6Path(const QString&);
    void setG7Login(const QString& path,const QString& userName,const QString& pwd );
    void setG6Login(const QString& path,const QString& userName,const QString& pwd );

    void setLoadDeadEntitiesOption(bool);
    void setLoadNodeswithNoAstOption(bool);

    void setEnabledOptions(bool);

private:
    void createObjects();
    void createConnections();

public:
    QString getG6DatabaseFile() const;
    QString getG7DatabaseFile() const;

    QString getG6UserName() const;
    QString getG7UserName() const;

    QString getG6Pwd() const;
    QString getG7Pwd() const;

    bool loadDeadEntities() const;
    bool loadNodesWithNoAst() const;

public slots:
    void slotG6DatabaseButtonClicked();
    void slotG7DatabaseButtonClicked();

private:   
    Ui::RSDatabaseConfig *ui;
};

#endif // RSDATABASECONFIG_H
