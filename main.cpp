#include <QtWidgets>
#include "SystemInfoWidget.h"
#include "CpuTemperatureWidget.h"
#include "GpuInfoWidget.h"
#include "GpuOverclockWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);

    SystemInfoWidget *systemInfo = new SystemInfoWidget();
    CpuTemperatureWidget *cpuTemp = new CpuTemperatureWidget();
    GpuInfoWidget *gpuInfo = new GpuInfoWidget();
    GpuOverclockWidget *gpuOverclock = new GpuOverclockWidget();

    layout->addWidget(systemInfo);
    layout->addWidget(cpuTemp);
    layout->addWidget(gpuInfo);
    layout->addWidget(gpuOverclock);

    window.setLayout(layout);
    window.show();

    return app.exec();
}
