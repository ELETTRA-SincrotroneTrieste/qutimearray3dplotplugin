#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>

class DataGenerator : public QObject
{
    Q_OBJECT
public:
    explicit DataGenerator(const QString& name, QObject *parent, int size, double min, double max);

protected slots:
    void generate();
signals:
    void onNewData(const QString& objectNam, double timestamp, const QVector<double> &xd, const QVector<double> &yd);

private:
    long m_t;
    int m_size, m_min, m_max;
};

#endif // DATAGENERATOR_H
