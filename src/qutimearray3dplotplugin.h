#ifndef QUTIMEARRAY3DPLOTPLUGIN_H
#define QUTIMEARRAY3DPLOTPLUGIN_H

#include <qutimearray3dplotplugin_i.h>
#include <QAction>
#include <QList>
#include <QObject>
#include <cudatalistener.h>

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
    QWindow *create(const QString &name, QWindow *parent);
};














#endif // CUMBIACOPYSOURCECTXMENUACTION_H
