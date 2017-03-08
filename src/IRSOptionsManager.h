#ifndef IRSOPTIONSMANAGER
#define IRSOPTIONSMANAGER


class IRSOptionsManager
{
public:
    virtual double noiseFactor() const = 0;
    virtual double sigma() const = 0;
    virtual int trend() const = 0;

    virtual bool isFidelityChecked() const = 0;
    virtual bool isFidelityMinChecked() const = 0;
    virtual bool isFidelityMaxChecked() const = 0;
    virtual bool isFidelityAvgChecked() const = 0;

    virtual void loadSettings(const QString& fileName) = 0;
    virtual void saveSettings(const QString& fileName) const = 0;

};

#endif // IRSOPTIONSMANAGER

