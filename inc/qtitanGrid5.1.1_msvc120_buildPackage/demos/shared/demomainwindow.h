#include <QWidget>
#include <QLabel>
#include <QAction>
#include <QToolButton>
#include <QHBoxLayout>


/* ColorButton */
class ColorButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget* parent);
public:
    virtual QSize sizeHint() const;
public:
    const QColor& color() const;
    void setColor(const QColor& color);
    void setToolTip(const QString &tip);
protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);

private:
    QColor m_color;
    bool m_sunken;

private:
    Q_DISABLE_COPY(ColorButton)
};

/* DemoMainWindow */
class DemoMainWindow : public QWidget
{
    Q_OBJECT
public:
    enum MainTheme
    {
        M_THEME_0 = -1,
        M_THEME_1 = Qt::darkRed,
        M_THEME_2 = Qt::darkMagenta,
        M_THEME_3 = Qt::darkGray,
        M_THEME_4 = Qt::darkGreen,
        M_THEME_5 = Qt::darkCyan,
    };
public:
    explicit DemoMainWindow(const QString& productName, const QString& version);
    void setDemoWidget(QWidget* widget, QWidget* settings, Qt::Alignment alignment = Qt::AlignTop, bool useThemeLayout = true);
protected:
    void paintEvent(QPaintEvent* event);
    virtual QHBoxLayout* createThemeLayout();
    ColorButton* createButton(MainTheme theme);
    Qt::GlobalColor convertToColor(MainTheme id) const;
    MainTheme convertToTheme(Qt::GlobalColor color) const;
    MainTheme convertToTheme(const QColor& color) const;
    ColorButton* buttomByTheme(MainTheme id) const;
protected slots:
    void aboutLinkActivated(const QString& link);
    void colorButtonClicked();
    virtual void setShadeColor(const QColor& color) { Q_UNUSED(color);  }
    virtual void setThemeColor(const MainTheme& theme) { Q_UNUSED(theme); }
    virtual QString descriptionText() const { return QString(); }
private:
    QLabel* m_infoButton;
    QWidget* m_settings;
    QString m_productName;
    QString m_version;
    QHBoxLayout* hl;
    QVBoxLayout* vl;
    QMap<MainTheme, ColorButton*> m_buttons;
private:
    Q_DISABLE_COPY(DemoMainWindow)
};

