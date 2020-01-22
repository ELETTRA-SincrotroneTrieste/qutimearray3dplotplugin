#include "QuTimeArrayPlotExample.h"
#include <quapplication.h>
#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("elettra");
    qu_app.setApplicationName("time_plot3d_plugin_example");
    QString version(CVSVERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "+390403758073");
    qu_app.setProperty("office", "T2PT025");

    QuTimeArrayPlotExample plot(nullptr);
    plot.show();
    return qu_app.exec();
}
