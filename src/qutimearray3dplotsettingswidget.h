#ifndef QUTIMEARRAY3DPLOTSETTINGSDLG_H
#define QUTIMEARRAY3DPLOTSETTINGSDLG_H

#include <QWidget>

class QuTimeArray3DPlot;

class QuTimeArray3DPlotSettingsW : public QWidget
{
    Q_OBJECT
public:
    enum Layout { Horizontal, Vertical };

    explicit QuTimeArray3DPlotSettingsW(QObject *parent, Layout layout = Vertical);

private slots:
    void onCameraPresetChanged(int i);
    void toggleSurfaceVisibility(bool visible);
    void setBlackToYellowGradient();
    void setGreenToRedGradient();
signals:

private:
    QuTimeArray3DPlot* m_plot;
};

#endif // QUTIMEARRAY3DPLOTSETTINGSDLG_H
