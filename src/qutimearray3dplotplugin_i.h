#ifndef QUTIMEARRAY3DPLOTPLUGIN_INTERFACE_H
#define QUTIMEARRAY3DPLOTPLUGIN_INTERFACE_H

#include <QWidget>
#include <QString>
#include <cudata.h>

#include <QtDataVisualization/Q3DSurface>

class Cumbia;
class CuControlsReaderFactoryI;
class CumbiaPool;
class CuControlsFactoryPool;
class QDateTime;
class CuContext;

using namespace QtDataVisualization;

class QuTimeArrayReader_I {
public:
    virtual ~QuTimeArrayReader_I() {}
    virtual CuContext *getContext() const = 0;
    virtual QString link() const = 0;
    virtual void setLink(const QString &s, Q3DSurface *plot) = 0;
    virtual void unsetLink() = 0;
};

/*!
 * \brief The QuTime3DPlotPlugin_I defines the interface for the Cu3DSurface plugin.
 *
 * \note This file will be installed under the cumbia-qtcontrols include directory, alongside other
 *       cumbia-qtcontrols plugin interfaces
 *
 * \section qutime3dplot QuTime3DPlot
 *
 * \subsection props Properties
 *
 * - Axis Z is assigned to the time scale
 * - Axes auto range property is set to true
 */
class QuTimeArray3DPlotPlugin_I {
public:
    virtual ~QuTimeArray3DPlotPlugin_I() {}

    /*!
     * \brief create allocate and return a new QtDataVisualization::Q3DSurface.
     *
     * \param name the Q3DSurface name. If a plugin provides a set of Q3DSurfaces,
     *  this parameter can be used to pick a specific one.
     * \param parent the parent QWindow
     *
     * \return a new Q3DSurface
     *
     * \note Subclasses will implement the appropriate create method.
     */
    virtual Q3DSurface *create(const QString& name, QWindow *parent) {
        Q_UNUSED(name)
        Q_UNUSED(parent)
        return nullptr;
    }

    /*!
     * \brief name returns the name of the plugin
     *
     * \return  the name of the plugin
     */
    virtual QString name() const = 0;

    /*! \brief provides a brief description of the plugin and the classes it provides
     *
     * @return a string with a short description of the plugin.
     *
     * Must be implemented by subclasses
     */
    virtual QString description() const = 0;

    /*!
     * \brief widgetList provides the list of the names of the 3d surfaces offered by the plugin
     *
     * \return a QStringList with the names of the 3d surfaces  made available by the plugin
     */
    virtual QStringList catalogue() const = 0;

    /*!
     * \brief Creates a new configuration widget for the plot given as input parameter,
     *        that must be an object instantiated with the *create* method above
     *
     * \param plot3d a pointer to a surface plot obtained with the *create* method above
     * \return a new widget with controls to configure the plot.
     *
     * The ownership of the newly created widget is of the caller.
     */
    virtual QWidget* plotConfigurationWidget(QObject *plot3d) = 0;

    virtual QuTimeArrayReader_I *createReader(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI& rf) const = 0;

    virtual QuTimeArrayReader_I *createReader(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool& fpool) const = 0;
};

#define QuTimeArray3DPlotPlugin_I_iid "eu.elettra.qutils.QuTimeArray3DPlotPlugin_I"



Q_DECLARE_INTERFACE(QuTimeArray3DPlotPlugin_I, QuTimeArray3DPlotPlugin_I_iid)

#endif // QUTIMEARRAY3DPLOTPLUGIN_INTERFACE_H
