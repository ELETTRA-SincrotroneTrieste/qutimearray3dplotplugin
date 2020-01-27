#ifndef QUTIMEARRAYREADER_H
#define QUTIMEARRAYREADER_H

#include <QObject>
#include <cudatalistener.h>
#include <qutimearray3dplotplugin_i.h>

class QuTimeArrayReaderPrivate;

class QuTimeArrayReader : public QObject, public CuDataListener, public QuTimeArrayReader_I {
Q_OBJECT
public:
    QuTimeArrayReader(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &rf);
    QuTimeArrayReader(QObject *parent, CumbiaPool *cu_pool, const CuControlsFactoryPool &fp);
    ~QuTimeArrayReader();

signals:
    void newData(const QString& name, double timestamp_ms, const QVector<double> &xdata, const QVector<double>& ydata);
    void newData(const CuData& cud);
    void error(const QString& name, double timestamp_ms, const QString& message);

private:
    QuTimeArrayReaderPrivate *d;
    void us_to_ms(std::vector<double> & ts_us) const;

    // QuTimeArrayReader_I interface
public:
    CuContext *getContext() const;

    // QuTimeArrayReader_I interface
public:
    QString link() const;
    void setLink(const QString &s, QObject *plot);
    void unsetLink();

    // CuDataListener interface
public:
    void onUpdate(const CuData &data);
};

#endif // QUTIMEARRAYREADER_H
