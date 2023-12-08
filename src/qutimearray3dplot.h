#ifndef QUTIMEARRAY3DPLOT_H
#define QUTIMEARRAY3DPLOT_H

#include <Q3DSurface>
#include <QDate>

class TimeArray3DPlotPrivate;
class CuData;


/*! \brief
 *
 * - Axis Z is assigned to the time scale
 * - Axes auto range property is set to true
 */
class QuTimeArray3DPlot : public Q3DSurface
{
    Q_OBJECT
    Q_PROPERTY(int  xAxisAutoscaleEnabled READ xAxisAutoscaleEnabled WRITE setXAxisAutoscaleEnabled)
    Q_PROPERTY(int  yAxisAutoscaleEnabled READ yAxisAutoscaleEnabled WRITE setYAxisAutoscaleEnabled)
    Q_PROPERTY(int  zAxisAutoscaleEnabled READ zAxisAutoscaleEnabled WRITE setZAxisAutoscaleEnabled)
    Q_PROPERTY(int  maxNumRows READ maxNumRows WRITE setMaxNumRows)

    Q_PROPERTY(bool axisAutoscaleModeExpand READ axisAutoscaleModeExpand WRITE setAxisAutoscaleModeExpand)
    Q_PROPERTY(bool xAxisDateTime READ xAxisDateTime WRITE setXAxisDateTime)
    Q_PROPERTY(bool yAxisDateTime READ yAxisDateTime WRITE setYAxisDateTime)
    Q_PROPERTY(bool zAxisDateTime READ zAxisDateTime WRITE setZAxisDateTime)
    Q_PROPERTY(QString dateTimeFormat READ dateTimeFormat WRITE setDateTimeFormat)
    Q_PROPERTY(QDateTime originDateTime READ originDateTime WRITE setOriginDateTime NOTIFY originDateTimeChanged)

public:
    enum Qu3DAxis { InvalidAxis = 0x0, xAxis = 0x01, yAxis = 0x02, zAxis = 0x04 };

    QuTimeArray3DPlot(QWindow *parent);
    ~QuTimeArray3DPlot();

    QDateTime originDateTime() const;

    float mapTsToPlot(qreal ts_ms) const;
    qreal mapTsFromPlot(float ts_ms) const;

    int maxNumRows() const;

    QSurface3DSeries::DrawFlags seriesDrawFlags() const;

    QSurface3DSeries *findSeries(const QString& name) const;

// slots to update data and provide error messages
public slots:
    void addTimeArray(const QString& name, double timestamp_secs,
                      const QVector<double> &xdata, const QVector<double> &ydata);
    void onError(const QString& name, double timestamp_ms, const QString& message);
    void onNewData(const CuData& da);
    void clear();

public slots:

    // begin axis autoscale methods
    int axisAutoscaleEnabled() const;
    void setAxisAutoscaleEnabled(Qu3DAxis a, bool en);
    bool axisAutoscaleModeExpand() const;
    void setAxisAutoscaleModeExpand(bool ex);
    // shortcuts
    bool xAxisAutoscaleEnabled() const;
    bool yAxisAutoscaleEnabled() const;
    bool zAxisAutoscaleEnabled() const;
    void setXAxisAutoscaleEnabled(bool en);
    void setYAxisAutoscaleEnabled(bool en);
    void setZAxisAutoscaleEnabled(bool en);

    void getXYItemRange();
    void applyAutoBounds(int axes = xAxis|yAxis|zAxis);
    // end axis autoscale methods

    // Date time axis formatter
    int axisDateTime();
    void setAxisDateTime(int axis, bool dt);
    QString dateTimeFormat() const;
    void setDateTimeFormat(const QString& fmt);

    // shortcuts for axis Date time
    bool xAxisDateTime() const;
    void setXAxisDateTime(bool dt);
    bool yAxisDateTime() const;
    void setYAxisDateTime(bool dt);
    bool zAxisDateTime() const;
    void setZAxisDateTime(bool dt);

    void setAxisZRange(float min, float max);
    void setAxisXRange(float min, float max);
    void setAxisYRange(float min, float max);

    void setOriginDateTime(const QDateTime &originDt);

    void setTimeRange(qreal min_ts, qreal max_ts);
    void setTimeRange(const QDateTime& min, const QDateTime& max);


    // settings
    //
    void wireDrawToggled(bool on);
    void surfaceDrawToggled(bool on);
    void toggleModeNone();
    void toggleModeItem();
    void toggleModeSliceRow();
    void toggleModeSliceColumn();

    void setBlackToYellowGradient(const QString& name);
    void setGreenToRedGradient(const QString& name);
    void changeTheme(int theme);
    void changeShadowQuality(int shadowQuality);

    void setMaxNumRows(int maxNumRows);

signals:
    void originDateTimeChanged(const QDateTime& originDateTime);

protected:

protected slots:
    void xRangeChanged(float min, float max);
    void yRangeChanged(float min, float max);
    void zRangeChanged(float min, float max);

private:
    void m_init();
    QSurface3DSeries *m_add_series(const QString& name);
    void m_fix_origin_datetime(const double timestamp_ms);
    TimeArray3DPlotPrivate *d;
};

#endif
