#include "qutimearray3dplotplugin.h"
#include "qutimearray3dplot.h"
#include "qutimearray3dplotsettingswidget.h"
#include <QClipboard>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>
#include <QDateTime>
#include <cucontext.h>
#include "qutimearray3dplot.h"
#include <cumacros.h>
#include <culinkstats.h>
#include <cudata.h>
#include <qustring.h>

QuTimeArrayReader::QuTimeArrayReader(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &rf)
    : QObject(parent) {
     d = new QuTimeArrayReaderPrivate;
     d->ctx = new CuContext(cumbia, rf);
}

QuTimeArrayReader::QuTimeArrayReader(QObject *parent, CumbiaPool *cu_pool, const CuControlsFactoryPool &fp)
    : QObject(parent) {
    d = new QuTimeArrayReaderPrivate;
    d->ctx = new CuContext(cu_pool, fp);
}

QuTimeArrayReader::~QuTimeArrayReader() {
    delete d;
}

CuContext *QuTimeArrayReader::getContext() const
{
    return d->ctx;
}

QString QuTimeArrayReader::link() const
{
    if(CuControlsReaderA* r = d->ctx->getReader())
        return r->source();
    return "";
}

void QuTimeArrayReader::setLink(const QString &s, Q3DSurface *plot)
{
    CuControlsReaderA * r = d->ctx->replace_reader(s.toStdString(), this);
    connect(this, SIGNAL(newData(QString,double,QVector<double>,QVector<double>)),
            plot, SLOT(addTimeArray(QString,double, const QVector<double> &, const QVector<double> &)));
    connect(this, SIGNAL(error(QString,double,QString)), plot, SLOT(onError(QString,double,QString)));
    if(r)
        r->setSource(s);
}

void QuTimeArrayReader::unsetLink()
{
    d->ctx->disposeReader();
}

QuTimeArrayReader_I *QuTimeArray3DPlotPlugin::createReader(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &rf) const
{
    return new QuTimeArrayReader(parent, cumbia, rf);
}

QuTimeArrayReader_I *QuTimeArray3DPlotPlugin::createReader(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) const
{
    return new QuTimeArrayReader(parent, cumbia_pool, fpool);
}

QuTimeArray3DPlotPlugin::QuTimeArray3DPlotPlugin(QObject *parent)
    : QObject (parent)
{

}

QuTimeArray3DPlotPlugin::~QuTimeArray3DPlotPlugin()
{

}

Q3DSurface *QuTimeArray3DPlotPlugin::create(const QString &name, QWindow *parent)
{
    if(name == "QuTimeArray3DPlot") {
        return new QuTimeArray3DPlot(parent);
    }
    perr("QuTimeArray3DPlotPlugin.create: QuTimeArray3DPlotPlugin does not provide the QuTimeArray3DPlot \"%s\"", qstoc(name));
    return nullptr;
}

QString QuTimeArray3DPlotPlugin::name() const
{
    return "QuTimeArray3DPlotPlugin";
}

QString QuTimeArray3DPlotPlugin::description() const
{
    return QString("%1 provides a widget to render 3D surface plots and to view them by rotating the scene freely").arg(name());
}

QStringList QuTimeArray3DPlotPlugin::catalogue() const
{
    return QStringList() << "QuTimeArray3DPlot";
}

int QuTimeArray3DPlotPlugin::axisId(const QString &xyz) const
{
    if(xyz.compare("x", Qt::CaseInsensitive) == 0)
        return QuTimeArray3DPlot::xAxis;
    else if(xyz.compare("y", Qt::CaseInsensitive) == 0)
        return QuTimeArray3DPlot::yAxis;
    else if(xyz.compare("z", Qt::CaseInsensitive) == 0)
        return QuTimeArray3DPlot::zAxis;
    return QuTimeArray3DPlot::InvalidAxis;
}

QWidget *QuTimeArray3DPlotPlugin::plotConfigurationWidget(QObject *plot3d)
{
    return new QuTimeArray3DPlotSettingsW(plot3d);
}


void QuTimeArrayReader::onUpdate(const CuData &data)
{
    printf("QuTimeArrayReader.onUpdate: \e[1;34m%s\e[0m\n", data.toString().c_str());
    bool err = data["err"].toBool();
    QString src = QuString(data["src"].toString());
    // update link statistics
    d->ctx->getLinkStats()->addOperation();
    if(err) {
        d->ctx->getLinkStats()->addError(data["msg"].toString());
    }
    else {
        const CuVariant& va = data["value"];
        if(va.isValid() && va.getFormat() == CuVariant::Vector) {
            std::vector<double> x, y;
            if(data.containsKey("time_scale_ms")) {
                std::vector<double> todoublev;
                va.toVector<double>(todoublev);
                std::vector<double> ts_ms;
                data["time_scale_ms"].toVector<double>(ts_ms);
                size_t rows = ts_ms.size();
                size_t y_siz = todoublev.size() / rows;
                if(todoublev.size() % rows != 0)
                    perr("QuTimeArray3DPlot.onUpdate total data size %ld is not a multiple of "
                         "timestamps size %ld", todoublev.size(), ts_ms.size());
                else {
                    // data size is consistent with timestamp size
                    for(size_t r = 0; r < rows; r++) {
                        for(size_t i = 0; i < y_siz; i++) {
                            x.push_back(static_cast<double>(i));
                            y.push_back(todoublev[i * y_siz + i]);
                        }
                        emit newData(src, ts_ms[r], QVector<double>::fromStdVector(x), QVector<double>::fromStdVector(y));
                    }
                }
            } // end if data contains time_scale_ms
            else if(data.containsKey("timestamp_ms")) {
                double tsms;
                data["timestamp_ms"].to<double>(tsms);
                if(data.containsKey("value")) {
                    data["value"].toVector<double>(y);
                }
                if(data.containsKey("x_values")) {
                    data["x_values"].toVector<double>(x);
                    if(x.size() == y.size())
                        emit newData(src, tsms, QVector<double>::fromStdVector(x), QVector<double>::fromStdVector(y));
                }
                else {
                    for(int i = 0; i < y.size(); i++)
                        x.push_back(i);
                    emit newData(src, tsms, QVector<double>::fromStdVector(x), QVector<double>::fromStdVector(y));
                }
            }
        }
        else if(va.isValid()) {
            QString msg = "QuTimeArrayReader.onUpdate: Unsupported format " + QString::fromStdString(va.dataFormatStr(va.getFormat()));
            if(data.containsKey("timestamp_ms"))
            emit error(src, static_cast<double>(data["timestamp_ms"].toULongInt()), msg);
        }
        else
            emit error(src, static_cast<double>(QDateTime::currentMSecsSinceEpoch()), QuString(data["msg"]));
    }
}
