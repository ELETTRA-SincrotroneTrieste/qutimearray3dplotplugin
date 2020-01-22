#include "qutimearray3dplot.h"
#include "qdatetime3daxisformatter.h"
#include <QHBoxLayout>
#include <quwatcher.h>
#include <quwriter.h>
#include <cucontrolsreader_abs.h>
#include <qustring.h>
#include <qustringlist.h>
#include <cucontextmenu.h>
#include <culinkstats.h>

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QValue3DAxis>
#include <QDateTime>
#include <QMap>

class SurfStyleProps {
public:
    SurfStyleProps(Q3DTheme::ColorStyle sty, const QColor& c) {
        color_style = sty;
        base_color = c;
    }
    SurfStyleProps(int s) {
        QList<QColor> colors = QList<QColor>() << QColor(Qt::green) << QColor(Qt::blue) << QColor(Qt::magenta)
                                               << QColor(Qt::gray) << QColor(Qt::yellow) << QColor(Qt::darkYellow)
                                               << QColor(Qt::darkBlue);
        base_color = colors.at(s % colors.size());
        color_style = Q3DTheme::ColorStyleUniform;
    }
    SurfStyleProps() {
        base_color = QColor(Qt::gray);
        color_style = Q3DTheme::ColorStyleUniform;
    }
    void setOn(QSurface3DSeries *s) {
        if(s->baseColor() != base_color)
            s->setBaseColor(base_color);
        if(s->colorStyle() != color_style)
            s->setColorStyle(color_style);
    }
    Q3DTheme::ColorStyle color_style;
    QColor base_color;
};

class TimeArray3DPlotPrivate {
public:
    QSurfaceDataArray *data;
    int autoscale, axis_date_time, m_maxNumRows;
    float xm, xM, ym, yM;
    bool autoscaleModeExpand;
    bool num_row_unlimited; // user explicitly called setMaxNumRows(-1)
    QString date_t_format;
    QDateTime origin_date_time;
    qint64 origin_timstamp;
    // actual milliseconds from epoch
    qreal min_ts, max_ts;
    QMap<QString, SurfStyleProps> m_surfpropmap;
};

QuTimeArray3DPlot::QuTimeArray3DPlot(QWindow *parent) :
    Q3DSurface(nullptr, parent)
{
    d = new TimeArray3DPlotPrivate();
    m_init();
}

QuTimeArray3DPlot::~QuTimeArray3DPlot()
{
    printf("\e[1;31m~QuTimerArray3DPlot\e[0m\n");
    delete d;
}

void QuTimeArray3DPlot::m_init()
{
    d->num_row_unlimited = false;
    d->m_maxNumRows = -1;

    setAxisX(new QValue3DAxis);
    setAxisY(new QValue3DAxis);
    setAxisZ(new QValue3DAxis);

    d->min_ts = d->max_ts = -1.0f;

    d->data = nullptr;

    // optimization
    setShadowQuality(ShadowQualityNone);
    setOptimizationHints(QAbstract3DGraph::OptimizationStatic);

    // axis bounds change
    connect(axisX(), SIGNAL(rangeChanged(float,float)), this, SLOT(xRangeChanged(float,float)));
    connect(axisY(), SIGNAL(rangeChanged(float,float)), this, SLOT(yRangeChanged(float,float)));
    connect(axisZ(), SIGNAL(rangeChanged(float,float)), this, SLOT(zRangeChanged(float,float)));

    // initialize time axis (Z)
    setAxisDateTime(zAxis, true);
    setOriginDateTime(QDateTime::currentDateTime());
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qint64 then = now + 10 * 1000;
    setTimeRange(now, then);

    d->xm = d->ym = 0.0f;
    d->xM = d->yM = 0.0f;
    axisX()->setLabelFormat("%.2f");
    axisZ()->setLabelFormat("%.2f");
    axisX()->setLabelAutoRotation(30);
    axisY()->setLabelAutoRotation(90);
    axisZ()->setLabelAutoRotation(30);

    // default axes titles and visibility
    axisX()->setTitle("X");
    axisY()->setTitle("Y");
    axisX()->setTitleVisible(true);
    axisY()->setTitleVisible(true);
    axisZ()->setTitle("time");
    axisZ()->setTitleVisible(true);

    // autoscale x, y, z by default
    setXAxisAutoscaleEnabled(true);
    setYAxisAutoscaleEnabled(true);
    setZAxisAutoscaleEnabled(true);

    scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetIsometricLeft);
    scene()->activeCamera()->setXRotation(-165);

    // aspect ratio: avoid long axis shrinking the others
    // The horizontal aspect ratio is the ratio of the graph scaling between the X and Z axes.
    // Value of 0.0 indicates automatic scaling according to axis ranges. Defaults to 0.0.
    //
    setHorizontalAspectRatio(1.0);
}

QSurface3DSeries *QuTimeArray3DPlot::m_add_series(const QString &name)
{
    QSurfaceDataProxy *dproxy = new QSurfaceDataProxy();
    QSurface3DSeries *series = new QSurface3DSeries(dproxy, this);
    series->setFlatShadingEnabled(series->isFlatShadingSupported());
    series->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
    series->setFlatShadingEnabled(false);
    series->setObjectName(name);
    int seriescnt = d->m_surfpropmap.size();
    d->m_surfpropmap[name] = SurfStyleProps(seriescnt);
    d->m_surfpropmap[name].setOn(series);
    addSeries(series);
    return series;
}

void QuTimeArray3DPlot::m_fix_origin_datetime(const double timestamp_ms)
{
    if(d->min_ts < 0 || timestamp_ms < d->min_ts) d->min_ts = timestamp_ms;
    else if(d->max_ts < 0 || timestamp_ms > d->max_ts) d->max_ts = timestamp_ms;
    if(!d->origin_date_time.isValid() || d->min_ts < d->origin_date_time.toMSecsSinceEpoch())
        setOriginDateTime(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(d->min_ts)));
}

QDateTime QuTimeArray3DPlot::originDateTime() const {
    return d->origin_date_time;
}

float QuTimeArray3DPlot::mapTsToPlot(qreal ts_ms) const {
    return ts_ms - d->origin_timstamp;
}

qreal QuTimeArray3DPlot::mapTsFromPlot(float ts_ms) const {
    return d->origin_timstamp + ts_ms;
}

int QuTimeArray3DPlot::maxNumRows() const {
    return d->m_maxNumRows;
}

QSurface3DSeries::DrawFlags QuTimeArray3DPlot::seriesDrawFlags() const {
    QList<QSurface3DSeries *> srs = seriesList();
    if(srs.size() > 0)
        return srs.first()->drawMode();
    return QSurface3DSeries::DrawSurfaceAndWireframe;
}

QSurface3DSeries *QuTimeArray3DPlot::findSeries(const QString &name) const
{
    foreach(QSurface3DSeries *s, seriesList())
        if(s->objectName() == name)
            return s;
    return nullptr;
}

int QuTimeArray3DPlot::axisAutoscaleEnabled() const {
    return d->autoscale;
}

bool QuTimeArray3DPlot::xAxisAutoscaleEnabled() const {
    //    return d->autoscale & xAxis;
    return axisX()->isAutoAdjustRange();
}

bool QuTimeArray3DPlot::yAxisAutoscaleEnabled() const {
    //    return d->autoscale & yAxis;
    return axisY()->isAutoAdjustRange();
}

bool QuTimeArray3DPlot::zAxisAutoscaleEnabled() const {
    //    return d->autoscale & zAxis;
    return axisZ()->isAutoAdjustRange();
}

void QuTimeArray3DPlot::addTimeArray(const QString &name, double timestamp_ms, const QVector<double> &xdata, const QVector<double> &ydata)
{
    QSurface3DSeries *series = findSeries(name);
    // d->min_ts and d->max ts store timestamp ms since epoch
    m_fix_origin_datetime(timestamp_ms);

    if(!series)
        series = m_add_series(name);

    QSurfaceDataRow *dataRow = new QSurfaceDataRow;
    dataRow->resize(xdata.size());
    if(d->m_maxNumRows < 0) {
        d->m_maxNumRows = 10000 / xdata.size();
        printf("\e[1;32mQuTimeArray3DPlot.addTimeArray: limiting num rows to %d\e[0m\n", d->m_maxNumRows);
    }

    for(int i = 0; i < xdata.size() && i < ydata.size(); i++) {
        (*dataRow)[i] = QVector3D(xdata[i], ydata[i], mapTsToPlot(timestamp_ms));
    }

    QSurfaceDataProxy *dataProxy = series->dataProxy();
    if(!dataProxy)
        series->setDataProxy(new QSurfaceDataProxy(this));
    // remove extra rows if a maxNumRows is defined
    if(!d->num_row_unlimited && d->m_maxNumRows > 0) {
        int excess_r = series->dataProxy()->rowCount() -1 - d->m_maxNumRows;
        if(excess_r > 0)
            series->dataProxy()->removeRows(0, excess_r);
    }
    if(xdata.size() > 0) {
        series->dataProxy()->addRow(dataRow);
    }
    // restore properties if changed in case of error
    d->m_surfpropmap[name].setOn(series);
}

void QuTimeArray3DPlot::onError(const QString &name, double timestamp_ms, const QString &message) {
    m_fix_origin_datetime(timestamp_ms);
    QSurface3DSeries *series = findSeries(name);
    if(!series) {
        series = m_add_series(name);
    }
    SurfStyleProps p(Q3DTheme::ColorStyleUniform, QColor(Qt::red));
    p.setOn(series);
}

void QuTimeArray3DPlot::xRangeChanged(float min, float max) {
    d->xm = min;
    d->xM = max;
}

void QuTimeArray3DPlot::yRangeChanged(float min, float max) {
    d->ym = min;
    d->yM = max;
}

void QuTimeArray3DPlot::zRangeChanged(float min, float max) {
    d->min_ts = mapTsFromPlot(min);
    d->max_ts = mapTsFromPlot(max);
}

void QuTimeArray3DPlot::setOriginDateTime(const QDateTime& originDt)
{
    d->origin_date_time = originDt;
    d->origin_timstamp = originDt.toMSecsSinceEpoch();
    foreach(QValue3DAxis *a, QVector<QValue3DAxis *>() << axisX() << axisY() << axisZ())
        if(qobject_cast<QDateTime3DAxisFormatter *>(a->formatter()))
            qobject_cast<QDateTime3DAxisFormatter *>(a->formatter())->setOriginDateTime(originDt);
    emit originDateTimeChanged(originDt);
}

/*!
 * \brief set time lower and upper bounds, in *milliseconds since epoch*.
 * \param min_ts lower bound, millis *since epoch*
 * \param max_ts upper bound, millis *since epoch*
 *
 * Calls axisZ()->setRange after mapping milliseconds since epoch to plot time coordinates,
 * expressed as difference between an origin date and the actual timestamps since epoch.
 * axisZ()->setRange will trigger rangeChanged signal
 */
void QuTimeArray3DPlot::setTimeRange(qreal min_ts, qreal max_ts) {
    printf("\e[1;33mQuTimeArray3DPlot.setTimeRange: setting %f --> %f time range\e[0m\n",
           min_ts, max_ts);
    axisZ()->setRange(mapTsToPlot(min_ts), mapTsToPlot(max_ts));
}

/*!
 * \brief Version of setTimeRange that accepts QDateTime objects, converts them into millis *since epoch*
 *        and calls setTimeRange accordingly
 * \param min lower bound in QDateTime
 * \param max upper bound in QDateTime
 */
void QuTimeArray3DPlot::setTimeRange(const QDateTime &min, const QDateTime &max) {
    setTimeRange(min.toMSecsSinceEpoch(), max.toMSecsSinceEpoch());
}

void QuTimeArray3DPlot::wireDrawToggled(bool on) {
    foreach(QSurface3DSeries *s, seriesList()) {
        QSurface3DSeries::DrawFlags f = s->drawMode();
        on ? s->setDrawMode(f | QSurface3DSeries::DrawWireframe) :
             s->setDrawMode(f & ~QSurface3DSeries::DrawWireframe);
    }
}

void QuTimeArray3DPlot::surfaceDrawToggled(bool on) {
    foreach(QSurface3DSeries *s, seriesList()) {
        QSurface3DSeries::DrawFlags f = s->drawMode();
        on ? s->setDrawMode(f | QSurface3DSeries::DrawSurface) :
             s->setDrawMode(f & ~QSurface3DSeries::DrawSurface);
    }
}

void QuTimeArray3DPlot::toggleModeNone() {
    setSelectionMode(QAbstract3DGraph::SelectionNone);
}

void QuTimeArray3DPlot::toggleModeItem() {
    setSelectionMode(QAbstract3DGraph::SelectionItem);
}

void QuTimeArray3DPlot::toggleModeSliceRow() {
    setSelectionMode(QAbstract3DGraph::SelectionItemAndRow | QAbstract3DGraph::SelectionSlice); }

void QuTimeArray3DPlot::toggleModeSliceColumn() {
    setSelectionMode(QAbstract3DGraph::SelectionItemAndColumn | QAbstract3DGraph::SelectionSlice);
}

void QuTimeArray3DPlot::setAxisAutoscaleEnabled(Qu3DAxis a, bool en) {
    if(en) {
        d->autoscale = d->autoscale | a;
        if(a & xAxis || a & yAxis)
            getXYItemRange();
        applyAutoBounds(a);
    }
    else {
        d->autoscale = d->autoscale ^ a;
    }
    qDebug() << __PRETTY_FUNCTION__ << "autoscale for " << a  << d->autoscale;
}

bool QuTimeArray3DPlot::axisAutoscaleModeExpand() const {
    return d->autoscaleModeExpand;
}

void QuTimeArray3DPlot::setAxisAutoscaleModeExpand(bool ex) {
    d->autoscaleModeExpand = ex;
}

void QuTimeArray3DPlot::setXAxisAutoscaleEnabled(bool en) {
    //    setAxisAutoscaleEnabled(xAxis, en);
    axisX()->setAutoAdjustRange(en);
}

void QuTimeArray3DPlot::setYAxisAutoscaleEnabled(bool en) {
    //    setAxisAutoscaleEnabled(yAxis, en);
    axisY()->setAutoAdjustRange(en);
}

void QuTimeArray3DPlot::setZAxisAutoscaleEnabled(bool en) {
    //    setAxisAutoscaleEnabled(zAxis, en);
    axisZ()->setAutoAdjustRange(en);
}

void QuTimeArray3DPlot::getXYItemRange() {
    QTime t;
    t.start();
    foreach(QSurface3DSeries *s, seriesList()) {
        QSurfaceDataProxy *data_proxy = s->dataProxy();
        for(int r = 0; r < data_proxy->rowCount(); r++) {
            for(int c = 0; c < data_proxy->columnCount(); c++) {
                const QSurfaceDataItem *it = data_proxy->itemAt(r, c);
                if(r == 0 && c == 0) {
                    d->xm = d->xM = it->x();
                    d->ym = d->yM = it->y();
                }
                else {
                    if(it->x() < d->xm) d->xm = it->x();
                    else if(it->x() > d->xM) d->xM = it->x();
                    if(it->y() < d->ym) d->ym = it->y();
                    else if(it->y() > d->yM) d->yM = it->y();
                }
            }
        }
    }
    qDebug() << __PRETTY_FUNCTION__ << "took " << t.elapsed() << "ms" << QDateTime::fromMSecsSinceEpoch(d->min_ts)
             << QDateTime::fromMSecsSinceEpoch(d->max_ts) << "rows" << "for " << seriesList().size() << " series";
}

void QuTimeArray3DPlot::applyAutoBounds(int axes)
{
    if((axes & xAxis) && d->xm != d->xM) {
        axisX()->disconnect(this, SLOT(xRangeChanged(float,float)));
        if(d->autoscaleModeExpand && d->xm < axisX()->min())
            setAxisXRange(d->xm, axisX()->max());
        if(d->autoscaleModeExpand && d->xM > axisX()->max())
            setAxisXRange(axisX()->min(), d->xM);
        else if(!d->autoscaleModeExpand)
            setAxisXRange(d->xm, d->xM);
        connect(axisX(), SIGNAL(rangeChanged(float,float)), this, SLOT(xRangeChanged(float,float)));
    }
    if((axes & yAxis) && d->ym != d->yM) {
        axisY()->disconnect(this, SLOT(yRangeChanged(float,float)));
        if(d->autoscaleModeExpand && d->ym < axisY()->min())
            setAxisYRange(d->ym, axisY()->max());
        if(d->autoscaleModeExpand && d->yM > axisY()->max())
            setAxisYRange(axisY()->min(), d->yM);
        else if(!d->autoscaleModeExpand)
            setAxisYRange(d->ym, d->yM);
        connect(axisY(), SIGNAL(rangeChanged(float,float)), this, SLOT(yRangeChanged(float,float)));
    }
    if((axes & zAxis)  && d->min_ts != d->max_ts) {
        axisZ()->disconnect(this, SLOT(zRangeChanged(float,float)));
        if(d->autoscaleModeExpand && d->min_ts < mapTsFromPlot(axisZ()->min())) {
            setTimeRange(d->min_ts, mapTsFromPlot(axisZ()->max()));
        }
        if(d->autoscaleModeExpand && d->max_ts >  mapTsFromPlot(axisZ()->max())) {
            setTimeRange(mapTsFromPlot(axisZ()->min()), d->max_ts);
        }
        else if(!d->autoscaleModeExpand)
            setAxisZRange(d->min_ts, d->max_ts);
        connect(axisZ(), SIGNAL(rangeChanged(float,float)), this, SLOT(zRangeChanged(float,float)));
    }
}

int QuTimeArray3DPlot::axisDateTime() {
    return d->axis_date_time;
}

void QuTimeArray3DPlot::setAxisDateTime(int axis, bool dt) {
    QValue3DAxis *a = nullptr;
    if(axis == xAxis) a = axisX();
    else if(axis == yAxis) a = axisY();
    else if(axis == zAxis) a = axisZ();

    qDebug() << __PRETTY_FUNCTION__ << axis << dt << a;
    if(dt && a) {
        d->axis_date_time = d->axis_date_time | axis;
        QDateTime3DAxisFormatter *dtf = new QDateTime3DAxisFormatter;
        dtf->setOriginDateTime(d->origin_date_time);
        a->setFormatter(dtf);
    }
    else if(a) {
        d->axis_date_time = d->axis_date_time ^ axis;
        a->setFormatter(new QValue3DAxisFormatter);
    }
}

QString QuTimeArray3DPlot::dateTimeFormat() const {
    return d->date_t_format;
}

void QuTimeArray3DPlot::setDateTimeFormat(const QString &fmt) {
    if(fmt != d->date_t_format) {
        d->date_t_format = fmt;
        QVector <QValue3DAxis *> axes = QVector<QValue3DAxis *>() << axisX() << axisY() << axisZ();
        foreach(QValue3DAxis *a, axes)
            if(qobject_cast<QDateTime3DAxisFormatter *>(a->formatter()))
                qobject_cast<QDateTime3DAxisFormatter *>(a->formatter())->setDateTimeFormat(fmt);
    }
}

bool QuTimeArray3DPlot::xAxisDateTime() const {
    return d->axis_date_time & xAxis;
}

void QuTimeArray3DPlot::setXAxisDateTime(bool dt) {
    setAxisDateTime(xAxis, dt);
}

bool QuTimeArray3DPlot::yAxisDateTime() const {
    return d->axis_date_time & yAxis;
}

void QuTimeArray3DPlot::setYAxisDateTime(bool dt)  {
    setAxisDateTime(yAxis, dt);
}

bool QuTimeArray3DPlot::zAxisDateTime() const {
    return d->axis_date_time & zAxis;
}

void QuTimeArray3DPlot::setZAxisDateTime(bool dt) {
    setAxisDateTime(zAxis, dt);
}

void QuTimeArray3DPlot::setAxisXRange(float min, float max)
{
    axisX()->setRange(min, max);
}

void QuTimeArray3DPlot::setAxisZRange(float min, float max)
{
    axisZ()->setRange(min, max);
}

void QuTimeArray3DPlot::setAxisYRange(float min, float max)
{
    axisY()->setRange(min, max);
}

void QuTimeArray3DPlot::changeTheme(int theme)
{
    activeTheme()->setType(Q3DTheme::Theme(theme));
}

void QuTimeArray3DPlot::changeShadowQuality(int shadowQuality) {
    setShadowQuality(static_cast<QAbstract3DGraph::ShadowQuality>(shadowQuality));
}

void QuTimeArray3DPlot::setMaxNumRows(int maxNumRows)
{
    if(d->m_maxNumRows != maxNumRows) {
        d->m_maxNumRows = maxNumRows;
        if(maxNumRows > 0) {
            foreach (QSurface3DSeries *s, seriesList()) {
                int excess_r = s->dataProxy()->rowCount() - maxNumRows;
                if(excess_r > 0)
                    s->dataProxy()->removeRows(0, excess_r);
            } // foreach
        }
        else
            d->num_row_unlimited = true;
    }
}

void QuTimeArray3DPlot::setBlackToYellowGradient(const QString &name)
{
    qDebug() << __PRETTY_FUNCTION__ << name;
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.33, Qt::blue);
    gr.setColorAt(0.67, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    QSurface3DSeries *s = findSeries(name);
    if(s) {
        s->setBaseGradient(gr);
        s->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    }
}

void QuTimeArray3DPlot::setGreenToRedGradient(const QString &name)
{
    qDebug() << __PRETTY_FUNCTION__ << name;
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    QSurface3DSeries *s = findSeries(name);
    if(s) {
        s->setBaseGradient(gr);
        s->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    }
}

