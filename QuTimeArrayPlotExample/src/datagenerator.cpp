#include "datagenerator.h"
#include <QTimer>
#include <math.h>
#include <QRandomGenerator>
#include <QDateTime>

DataGenerator::DataGenerator(const QString &name, QObject *parent, int size, double min, double max) : QObject(parent)
{
    m_t = 0;
    m_min = min;
    m_max = max;
    m_size = size;
    setObjectName(name);
    QTimer *t = new QTimer(this);
    t->setInterval(2000);
    connect(t, SIGNAL(timeout()), this, SLOT(generate()));
    t->start();
}

void DataGenerator::generate()
{
    QRandomGenerator rg;
    time_t tp;
    rg.seed(time(&tp));
    double A = rg.generateDouble() * m_max/2.0;
    QVector<double> xd, yd;
    for(int x = 0; x < m_size; x++) {
        xd << x;
        yd << m_min + m_max/2.0 + A * sin(x * 0.01f);
    }
    m_t = QDateTime::currentMSecsSinceEpoch();
    emit onNewData(objectName(), m_t, xd, yd);
}
