#include "qutimearray3dplotplugin.h"
#include "qutimearray3dplot.h"
#include "qutimearray3dplotsettingswidget.h"
#include "qutimearrayreader.h"
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

QWindow *QuTimeArray3DPlotPlugin::create(const QString &name, QWindow *parent)
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
