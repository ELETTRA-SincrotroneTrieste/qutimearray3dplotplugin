#include "qutimearray3dplotsettingswidget.h"
#include "qutimearray3dplot.h"
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QPainter>

QuTimeArray3DPlotSettingsW::QuTimeArray3DPlotSettingsW(QObject *parent, Layout layout )
    : QWidget(nullptr)
{
    QBoxLayout *lo;
    if(layout == Horizontal)
        lo = new QHBoxLayout(this);
    else
        lo = new QVBoxLayout(this);
    m_plot = qobject_cast<QuTimeArray3DPlot *>(parent);
    if(m_plot){

        QGroupBox *drawModeGroupBox = new QGroupBox("Draw", this);
        QCheckBox *wireM = new QCheckBox("Wire", this);
        QCheckBox *surfaceM = new QCheckBox("Surface", this);
        QVBoxLayout *drawLo = new QVBoxLayout(drawModeGroupBox);
        drawLo->addWidget(wireM);
        drawLo->addWidget(surfaceM);

        QGroupBox *selectionGroupBox = new QGroupBox(QStringLiteral("Selection Mode"));

        QRadioButton *modeNoneRB = new QRadioButton(this);
        modeNoneRB->setText(QStringLiteral("No selection"));
        modeNoneRB->setChecked(false);

        QRadioButton *modeItemRB = new QRadioButton(this);
        modeItemRB->setText(QStringLiteral("Item"));
        modeItemRB->setChecked(false);

        QRadioButton *modeSliceRowRB = new QRadioButton(this);
        modeSliceRowRB->setText(QStringLiteral("Row Slice"));
        modeSliceRowRB->setChecked(false);

        QRadioButton *modeSliceColumnRB = new QRadioButton(this);
        modeSliceColumnRB->setText(QStringLiteral("Column Slice"));
        modeSliceColumnRB->setChecked(false);

        QRadioButton *modeSliceRowColumnRB = new QRadioButton(this);
        modeSliceRowColumnRB->setText(QStringLiteral("Row and Column Slice"));
        modeSliceRowColumnRB->setChecked(false);

        QVBoxLayout *selectionVBox = new QVBoxLayout;
        selectionVBox->addWidget(modeNoneRB);
        selectionVBox->addWidget(modeItemRB);
        selectionVBox->addWidget(modeSliceRowRB);
        selectionVBox->addWidget(modeSliceColumnRB);
        selectionVBox->addWidget(modeSliceRowColumnRB);
        selectionGroupBox->setLayout(selectionVBox);

        QComboBox *themeList = new QComboBox(this);
        themeList->addItem(QStringLiteral("Qt"));
        themeList->addItem(QStringLiteral("Primary Colors"));
        themeList->addItem(QStringLiteral("Digia"));
        themeList->addItem(QStringLiteral("Stone Moss"));
        themeList->addItem(QStringLiteral("Army Blue"));
        themeList->addItem(QStringLiteral("Retro"));
        themeList->addItem(QStringLiteral("Ebony"));
        themeList->addItem(QStringLiteral("Isabelle"));

        QComboBox *campresetCb = new QComboBox(this);
        campresetCb->addItem("None");
        campresetCb->addItem("Front Low");
        campresetCb->addItem("Front High");
        campresetCb->addItem("Left Low");
        campresetCb->addItem("Left High");
        campresetCb->addItem("Right Low");
        campresetCb->addItem("Right");
        campresetCb->addItem("Right High");
        campresetCb->addItem("Behind Low");
        campresetCb->addItem("Behind");
        campresetCb->addItem("Behind High");
        campresetCb->addItem("Isometric Left");
        campresetCb->addItem("Isometric Left High");
        campresetCb->addItem("Isometric Right");
        campresetCb->addItem("Isometric Right High");
        campresetCb->addItem("Directly Above");
        campresetCb->addItem("Directly Above CW45");
        campresetCb->addItem("Directly Above CCW45");
        campresetCb->addItem("Front Below");
        campresetCb->addItem("Left Below");
        campresetCb->addItem("Right Below");
        campresetCb->addItem("Behind Below");
        campresetCb->addItem("Directly Below");

        QComboBox *shadowCb = new QComboBox(this);
        shadowCb->addItem("None");
        shadowCb->addItem("Low Quality");
        shadowCb->addItem("Medium Quality");
        shadowCb->addItem("High Quality");
        shadowCb->addItem("Low Quality Softened Edges");
        shadowCb->addItem("Medium Quality Softened Edges");
        shadowCb->addItem("High Quality Softened Edges");

        QGroupBox *optHintsGb = new QGroupBox("Optimization Hints", this);
        QRadioButton *optHintsDefaultRb = new QRadioButton("Default");
        QRadioButton *optHintsStaticRb = new QRadioButton("Default");
        QHBoxLayout *optHintsLo = new QHBoxLayout(optHintsGb);
        optHintsLo->addWidget(optHintsDefaultRb);
        optHintsLo->addWidget(optHintsStaticRb);

        QuTimeArray3DPlot* tarr_p = qobject_cast<QuTimeArray3DPlot *>(parent);
        QGroupBox *surfGb = new QGroupBox("Surfaces");
        surfGb->setObjectName("gb_surfaces");
        QVBoxLayout *surfLo = new QVBoxLayout(surfGb);
        foreach(QSurface3DSeries *s, tarr_p->seriesList()) {
            QCheckBox *cb = new QCheckBox(s->objectName(), surfGb);
            cb->setProperty("surface", s->objectName());
            cb->setChecked(true);
            surfLo->addWidget(cb);
            connect(cb, SIGNAL(toggled(bool)), this, SLOT(toggleSurfaceVisibility(bool)));
        }


        QGroupBox *colorGroupBox = new QGroupBox(QStringLiteral("Custom gradient"));

        QLinearGradient grBtoY(0, 0, 1, 100);
        grBtoY.setColorAt(1.0, Qt::black);
        grBtoY.setColorAt(0.67, Qt::blue);
        grBtoY.setColorAt(0.33, Qt::red);
        grBtoY.setColorAt(0.0, Qt::yellow);
        QPixmap pm(24, 100);
        QPainter pmp(&pm);
        pmp.setBrush(QBrush(grBtoY));
        pmp.setPen(Qt::NoPen);
        pmp.drawRect(0, 0, 24, 100);
        QPushButton *gradientBtoYPB = new QPushButton(this);
        gradientBtoYPB->setIcon(QIcon(pm));
        gradientBtoYPB->setIconSize(QSize(24, 100));

        QLinearGradient grGtoR(0, 0, 1, 100);
        grGtoR.setColorAt(1.0, Qt::darkGreen);
        grGtoR.setColorAt(0.5, Qt::yellow);
        grGtoR.setColorAt(0.2, Qt::red);
        grGtoR.setColorAt(0.0, Qt::darkRed);
        pmp.setBrush(QBrush(grGtoR));
        pmp.drawRect(0, 0, 24, 100);
        QPushButton *gradientGtoRPB = new QPushButton(this);
        gradientGtoRPB->setIcon(QIcon(pm));
        gradientGtoRPB->setIconSize(QSize(24, 100));

        QHBoxLayout *colorHBox = new QHBoxLayout;
        colorHBox->addWidget(gradientBtoYPB);
        colorHBox->addWidget(gradientGtoRPB);
        colorGroupBox->setLayout(colorHBox);

        lo->addWidget(surfGb); // toggle visibility of surfaces
        lo->addWidget(drawModeGroupBox);
        lo->addWidget(selectionGroupBox);
        lo->addWidget(new QLabel(QStringLiteral("Theme")));
        lo->addWidget(themeList);
        lo->addWidget(new QLabel(QStringLiteral("Camera Preset")));
        lo->addWidget(campresetCb);
        lo->addWidget(new QLabel(QStringLiteral("Shadow Quality")));
        lo->addWidget(shadowCb);
        lo->addWidget(optHintsGb);
        lo->addWidget(colorGroupBox);

        foreach(QLabel *l, findChildren<QLabel *>())
            l->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);


        connect(wireM, &QCheckBox::toggled,
                tarr_p, &QuTimeArray3DPlot::wireDrawToggled);
        connect(surfaceM, &QCheckBox::toggled,
                tarr_p, &QuTimeArray3DPlot::surfaceDrawToggled);
        connect(modeNoneRB, SIGNAL(toggled(bool)),
                         tarr_p, SLOT(toggleModeNone()));
        connect(modeItemRB,  &QRadioButton::toggled,
                         tarr_p, &QuTimeArray3DPlot::toggleModeItem);
        connect(modeSliceRowRB,  &QRadioButton::toggled,
                         tarr_p, &QuTimeArray3DPlot::toggleModeSliceRow);
        connect(modeSliceColumnRB,  &QRadioButton::toggled,
                         tarr_p, &QuTimeArray3DPlot::toggleModeSliceColumn);
        connect(themeList, SIGNAL(currentIndexChanged(int)),
                         tarr_p, SLOT(changeTheme(int)));
        // need to map campresetCb index because enum Q3DCamera::CameraPreset starts from -1
        connect(campresetCb, SIGNAL(currentIndexChanged(int)),
                this, SLOT(onCameraPresetChanged(int)));
        connect(shadowCb, SIGNAL(currentIndexChanged(int)),
                tarr_p, SLOT(changeShadowQuality(int)));
        connect(gradientBtoYPB, &QPushButton::pressed,
                         this, &QuTimeArray3DPlotSettingsW::setBlackToYellowGradient);
        connect(gradientGtoRPB, &QPushButton::pressed,
                         this, &QuTimeArray3DPlotSettingsW::setGreenToRedGradient);

        wireM->setChecked(tarr_p->seriesDrawFlags() & QSurface3DSeries::DrawWireframe);
        surfaceM->setChecked(tarr_p->seriesDrawFlags() & QSurface3DSeries::DrawSurface);
        optHintsDefaultRb->setChecked(tarr_p->optimizationHints() == QAbstract3DGraph::OptimizationDefault);
        optHintsDefaultRb->setChecked(tarr_p->optimizationHints() == QAbstract3DGraph::OptimizationStatic);

        QuTimeArray3DPlot::SelectionFlags sf = tarr_p->selectionMode();
        modeNoneRB->setChecked(sf == QuTimeArray3DPlot::SelectionNone);
        modeItemRB->setChecked(sf == QuTimeArray3DPlot::SelectionItem);
        modeSliceRowRB->setChecked(sf == QuTimeArray3DPlot::SelectionRow);
        modeSliceColumnRB->setChecked(sf == QuTimeArray3DPlot::SelectionColumn);
        modeSliceRowColumnRB->setChecked(sf == QuTimeArray3DPlot::SelectionRowAndColumn);

        qDebug() << __PRETTY_FUNCTION__ << "Selection mode" << sf << "them" << tarr_p->activeTheme()->type();

        themeList->setCurrentIndex(tarr_p->activeTheme()->type());
        campresetCb->setCurrentIndex(tarr_p->scene()->activeCamera()->cameraPreset() + 1);

    }
    else {
        lo->addWidget(new QLabel("Parent is not a QuTimeArray3DPlot", this));
    }
}

// need to map combo index because enum Q3DCamera::CameraPreset starts from -1
void QuTimeArray3DPlotSettingsW::onCameraPresetChanged(int i)
{
    if(m_plot)
        m_plot->scene()->activeCamera()->setCameraPreset(static_cast<Q3DCamera::CameraPreset>(i - 1));
}

void QuTimeArray3DPlotSettingsW::toggleSurfaceVisibility(bool visible)
{
    const QString &surfnam = sender()->property("surface").toString();
    m_plot->findSeries(surfnam)->setVisible(visible);
}

void QuTimeArray3DPlotSettingsW::setBlackToYellowGradient()
{
     QGroupBox *surfGb = findChild<QGroupBox *>("gb_surfaces");
     foreach(QCheckBox *cb, surfGb->findChildren<QCheckBox *>())
         if(cb->isChecked())
             m_plot->setBlackToYellowGradient(cb->property("surface").toString());
}

void QuTimeArray3DPlotSettingsW::setGreenToRedGradient()
{
    QGroupBox *surfGb = findChild<QGroupBox *>("gb_surfaces");
    foreach(QCheckBox *cb, surfGb->findChildren<QCheckBox *>())
        if(cb->isChecked())
            m_plot->setGreenToRedGradient(cb->property("surface").toString());
}
