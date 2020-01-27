#include "qutimearrayreader.h"
#include <cucontext.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cucontrolsreader_abs.h>
#include <qustring.h>
#include <culinkstats.h>
#include <QDateTime>

class QuTimeArrayReaderPrivate {
public:
    CuContext *ctx;
};


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

void QuTimeArrayReader::us_to_ms(std::vector<double> &ts_us) const {
    for(size_t i = 0; i < ts_us.size(); i++)
        ts_us[i] = ts_us[i] * 1000.0;
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

void QuTimeArrayReader::setLink(const QString &s, QObject *plot)
{
    CuControlsReaderA * r = d->ctx->replace_reader(s.toStdString(), this);
    connect(this, SIGNAL(newData(QString,double,QVector<double>,QVector<double>)),
            plot, SLOT(addTimeArray(QString,double, const QVector<double> &, const QVector<double> &)));
    connect(this, SIGNAL(error(QString,double,QString)), plot, SLOT(onError(QString,double,QString)));
    connect(this, SIGNAL(newData(CuData)), plot, SLOT(onNewData(CuData)));
    if(r)
        r->setSource(s);
}

void QuTimeArrayReader::unsetLink()
{
    d->ctx->disposeReader();
}

void QuTimeArrayReader::onUpdate(const CuData &data)
{
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
            if(data.containsKey("time_scale_us")) {
                std::vector<double> todoublev;
                va.toVector<double>(todoublev);
                std::vector<double> ts_ms = data["time_scale_us"].toDoubleVector(); // micros
                us_to_ms(ts_ms);
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
                            y.push_back(todoublev[r * y_siz + i]);
                        }
                        emit newData(src, ts_ms[r], QVector<double>(x.begin(), x.end()), QVector<double>(y.begin(), y.end()));
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
                        emit newData(src, tsms, QVector<double>(x.begin(), x.end()), QVector<double>(y.begin(), y.end()));
                }
                else {
                    for(size_t i = 0; i < y.size(); i++)
                        x.push_back(i);
                    emit newData(src, tsms, QVector<double>(x.begin(), x.end()), QVector<double>(y.begin(), y.end()));
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
    emit newData(data);
}
