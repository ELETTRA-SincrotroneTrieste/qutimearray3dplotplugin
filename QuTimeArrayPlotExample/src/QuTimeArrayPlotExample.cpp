#include "QuTimeArrayPlotExample.h"


#include <cumbiapool.h>
#include <qustring.h>
#include <qustringlist.h>
#include <cucontextactionbridge.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <cucontrolsfactorypool.h>
#include <qthreadseventbridgefactory.h>

#ifdef QUMBIA_EPICS_CONTROLS_VERSION
#include <cumbiaepics.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cuepics-world.h>
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cumbiatango.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#endif
#ifdef CUMBIA_RANDOM_VERSION
#include <cumbiarandom.h>
#include <curndreader.h>
#include <curndactionfactories.h>
#include <cumbiarndworld.h>
#endif

#include <cupluginloader.h>
#include <qutimearray3dplotplugin_i.h>
#include <Q3DSurface>
#include <QHBoxLayout>
#include <QVariant>
#include <QCoreApplication>
#include <QMenuBar>
#include <QScreen>

#include "datagenerator.h"


QuTimeArrayPlotExample::QuTimeArrayPlotExample(QWidget *parent) :
    QMainWindow(parent) {
    m_data_generator = nullptr;
    QWidget *centralW = new QWidget(this);
    QHBoxLayout *lo = new QHBoxLayout(centralW);
    setCentralWidget(centralW);
    CuControlsFactoryPool m_ctrl_factory_pool;
    CumbiaPool *cu_pool = new CumbiaPool();
    // instantiate CumbiaTango
    // setup Cumbia pool and register cumbia implementations for tango and epics
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
#endif
#ifdef CUMBIA_RANDOM_VERSION
    CumbiaRandom *cura = new CumbiaRandom(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaRNDWorld rndw;
    cu_pool->registerCumbiaImpl("random", cura);
    m_ctrl_factory_pool.registerImpl("random", CuRNDReaderFactory());
    m_ctrl_factory_pool.setSrcPatterns("random", rndw.srcPatterns());
    cu_pool->setSrcPatterns("random", rndw.srcPatterns());
    cura->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
#endif
    CuPluginLoader ploader;
    QObject *pl_obj;
    QuTimeArray3DPlotPlugin_I *pi = ploader.get<QuTimeArray3DPlotPlugin_I>("qutimearray3dplotplugin.so", &pl_obj);
    if(pi) {
        m_surface = pi->create("QuTimeArray3DPlot", 0);
        // m_surface->setFlags(m_surface->flags() ^ Qt::FramelessWindowHint);
        m_surface->setProperty("maxNumRows", 1000);
        m_surface->resize(800, 800);
        QWidget *container = QWidget::createWindowContainer(m_surface, centralW);

        // readers from cmd line, if specified, otherwise dummy sin data generator
        for(int i = 1; i < qApp->arguments().size(); i++) {
            QuTimeArrayReader_I *reader = pi->createReader(m_surface, cu_pool, m_ctrl_factory_pool);
            reader->setLink(qApp->arguments()[i], m_surface);
        }
        QSize screenSize = m_surface->screen()->size();
        container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
        container->setMaximumSize(screenSize);
        container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        container->setFocusPolicy(Qt::StrongFocus);
        lo->addWidget(container);

        QMenuBar *mb  = menuBar();
        QMenu *m = mb->addMenu("File");
        QAction *showConf = new QAction("Settings", this);
        showConf->setCheckable(true);
        connect(showConf, SIGNAL(toggled(bool)), this, SLOT(showConfiguration(bool)));
        m->addAction(showConf);

        if(qApp->arguments().size()  < 2) {
            m_data_generator = new DataGenerator("Surface 1", this, 1000, 0, 10);
            connect(m_data_generator, SIGNAL(onNewData(QString,double,QVector<double>,QVector<double>)), m_surface,
                    SLOT(addTimeArray(QString,double,QVector<double>,QVector<double>)));
            QAction *addSurface = new QAction("Add surface", this);
            connect(addSurface, SIGNAL(triggered()), this, SLOT(addSurface()));
            m->addAction(addSurface);
        }

        m->addAction("Quit", this, SLOT(close()));
    }
    else
        perr("Qu3DSurfacePluginExample: failed to load plugin: %s", "cu3dsurfaceplugin.so");
}

QuTimeArrayPlotExample::~QuTimeArrayPlotExample() {
    if(m_data_generator)
        delete m_data_generator;
    delete m_surface;
}

void QuTimeArrayPlotExample::showConfiguration(bool show) {
    if(!show && m_configW)
        delete m_configW;
    else if(show) {
        CuPluginLoader ploader;
        QObject *pl_obj;
        QuTimeArray3DPlotPlugin_I *pi = ploader.get<QuTimeArray3DPlotPlugin_I>("qutimearray3dplotplugin.so", &pl_obj);
        if(pi) {
            m_configW = pi->plotConfigurationWidget(m_surface);
            centralWidget()->findChild<QHBoxLayout *>()->addWidget(m_configW);
        }
    }
}

void QuTimeArrayPlotExample::addSurface()
{
    int generators_count = findChildren<DataGenerator *>().size() + 1;
    DataGenerator *dg = new DataGenerator(QString("Surface %1").arg(generators_count), this, 1000, 0, 10);
    connect(dg, SIGNAL(onNewData(QString,double,QVector<double>,QVector<double>)), m_surface, SLOT(addTimeArray(QString,double,QVector<double>,QVector<double>)));
}
