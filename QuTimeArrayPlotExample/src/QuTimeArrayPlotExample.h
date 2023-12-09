#ifndef Qu3DSurfacePluginExample_H
#define Qu3DSurfacePluginExample_H

#include <QMainWindow>
#include <qulogimpl.h>

class DataGenerator;
class Q3DSurface;

class QuTimeArrayPlotExample : public QMainWindow
{
    Q_OBJECT

public:
    explicit QuTimeArrayPlotExample(QWidget *parent = 0);
    ~QuTimeArrayPlotExample();

public slots:
    void showConfiguration(bool show);
    void addSurface();

private:
    QWindow *m_surface;
    QWidget *m_configW;
    DataGenerator *m_data_generator;
    QuLogImpl m_log_impl;
};

#endif // Qu3DSurfacePluginExample_H
