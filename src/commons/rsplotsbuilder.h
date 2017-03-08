#ifndef RSPLOTSBUILDER_H
#define RSPLOTSBUILDER_H

#include   "QCustomPlot.h"
#include <QLinearGradient>
#include <QVector>
#include <QFont>

class RSPlotsBuilder
{
public:
    RSPlotsBuilder(QCustomPlot* customPlot);

    void plotHistoGram();


    QCustomPlot* m_customPlot;
};

#endif // RSPLOTSBUILDER_H
