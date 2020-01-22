#ifndef QUTIMEARRAY3DPLOTPLUGIN_H
#define QUTIMEARRAY3DPLOTPLUGIN_H

#include <qutimearray3dplotplugin_i.h>
#include <QAction>
#include <QList>
#include <QObject>
#include <cudatalistener.h>

class QuTimeArrayReaderPrivate {
public:
    CuContext *ctx;
};

class QuTimeArrayReader : public QObject, public CuDataListener, public QuTimeArrayReader_I {
Q_OBJECT
public:
    QuTimeArrayReader(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &rf);
    QuTimeArrayReader(QObject *parent, CumbiaPool *cu_pool, const CuControlsFactoryPool &fp);
    ~QuTimeArrayReader();

signals:
    void newData(const QString& name, double timestamp_ms, const QVector<double> &xdata, const QVector<double>& ydata);
    void error(const QString& name, double timestamp_ms, const QString& message);

private:
    QuTimeArrayReaderPrivate *d;

    // QuTimeArrayReader_I interface
public:
    CuContext *getContext() const;

    // QuTimeArrayReader_I interface
public:
    QString link() const;
    void setLink(const QString &s, Q3DSurface * plot);
    void unsetLink();

    // CuDataListener interface
public:
    void onUpdate(const CuData &data);
};

class QuTimeArray3DPlotPlugin : public QObject, public QuTimeArray3DPlotPlugin_I
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "qutimearray3dplotplugin.json")

public:
    explicit QuTimeArray3DPlotPlugin(QObject *parent = nullptr);
    ~QuTimeArray3DPlotPlugin();

    Q_INTERFACES(QuTimeArray3DPlotPlugin_I)

public:
    QString name() const;
    QString description() const;
    QStringList catalogue() const;
    int  axisId(const QString& xyz) const;
    float toTimestampSecs(const QDateTime &dt) const;
    QWidget *plotConfigurationWidget(QObject *plot3d);

    QuTimeArrayReader_I *createReader(QObject * parent, Cumbia *cumbia, const CuControlsReaderFactoryI &rf) const;
    QuTimeArrayReader_I *createReader(QObject * parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) const;
    QtDataVisualization::Q3DSurface *create(const QString &name, QWindow *parent);
};














#endif // CUMBIACOPYSOURCECTXMENUACTION_H
