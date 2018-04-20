#ifndef RSSIGNALER_H
#define RSSIGNALER_H

#include <QObject>
#include <QMessageBox>

/**
 * This signaler is a singleton, who allows to differents module to communicate
 */

/**
 * @brief The FiltersEnum enum. To manage all kinds of filters by an index
 */
enum FiltersEnum
{
    Brand = 0,
    Model,
    Technology,
    PhysicalMeasurement,
    OutputSignal,
    TheoricalAccuracy,
    MeasurementRange,
    Unit,
    experimentation
};

class Signaler : public QObject
{
    Q_OBJECT

public:
    static Signaler* instance(QObject* parent = 0);

    static void killInstance();

signals:

    //!@brief signal_graphViewStepChanged emit when a filter is modified
    void signal_rsFilterIndexChanged();

    /**
     * @brief signal_graphViewStepChanged is emitted if the user changes a step on the graphic
     * @param step  : the current step on the spin box
     */
    void signal_graphViewStepChanged(int step);


    //!@brief signal_runReportsCalculations. This signal is emitted when the button "all data and charts" is pressed
    void signal_runReportsCalculations();

    //!@brief This signal is emitted when the button "RUN" are pressed, on graphic areas. TREND and DEVIATION
    void signal_plotCurves();

    //!@brief signal_exportToExcel. this signal is emitted when the button "Export to Excel" is pressed. The signal is caught by the Logic
    void signal_exportToExcel();

    /**
     * @brief signal_emitMessage this signal is emitted to display a popup message
     * @param icon : the icon to display
     * @param color : the colorof the text
     * @param title : the title on the message Box
     * @param msg : the message
     */
    void signal_emitMessage(QMessageBox::Icon icon, const QString& color, const QString& title, const QString& msg);

    //!@brief signal_clearGraphsAndSensorList is emitted when the button "Clear" is pressed. See in graphics area
    void signal_clearGraphsAndSensorList();

    /**
     * @brief signal_stepViewMaxChanged is emitted when the stepMax is modified. This means the graphView must update the steps display
     * @param stepMax is the new step max
     */
    void signal_stepViewMaxChanged(int stepMax);


    //!@brief signal_quit is emitted when the user chooses to close the application. a popup of confirmation will be showed
    void signal_quit();

    //!@brief signal_readyToStart is emitted when slotInitializeSystem finished the initial initializations
    void signal_readyToStart();


    //!@brief signal_computeAverage is emited when the button computeAverage is pressed. The logic catchs the signal and call the suitable method
    void signal_computeAverage();

    //! @brief signal_updateSensorsList is emitted when the button "sensors list" in the calculations options is presses. Caught by the logic, who calls the suitable methode
    void signal_updateSensorsList();

    void signal_closeAppli();

    //!@brief signal_cancel is emitted when the user pressed on "ECHAP". the calculations will the stop
    void signal_cancel();

protected:
    explicit Signaler(QObject *parent = 0):QObject(parent){}

private:
    static Signaler* s_instance ;
};



#endif // RSSIGNALER_H
