#include "MainWindow.h"

#if QT_VERSION_MAJOR >= 6
#include <QtWidgets/QToolBox>
#include <QtWidgets/QPushButton>
#elif QT_VERSION_MAJOR >= 5
#include <QtWidgets/QToolBox>
#include <QtWidgets/QPushButton>
#else
#error Unsupported Qt version
#endif

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), toolBox(new QToolBox(this)) {
    systemInfoWidget = new SystemInfoWidget(this);
    cpuTemperatureWidget = new CpuTemperatureWidget(this);
    gpuInfoWidget = new GpuInfoWidget(this);
    gpuOverclockWidget = new GpuOverclockWidget(this);

    toolBox->addItem(systemInfoWidget, "Informasi CPU");
    toolBox->addItem(cpuTemperatureWidget, "Temperatur CPU");
    toolBox->addItem(gpuInfoWidget, "Informasi GPU");
    toolBox->addItem(gpuOverclockWidget, "Overclock GPU");

    expandAllButton = new QPushButton("Expand All", this);
    collapseAllButton = new QPushButton("Collapse All", this);

    connect(expandAllButton, &QPushButton::clicked, this, &MainWindow::expandAllSections);
    connect(collapseAllButton, &QPushButton::clicked, this, &MainWindow::collapseAllSections);

    setCentralWidget(toolBox);
}

MainWindow::~MainWindow() {}

void MainWindow::expandAllSections() {
    // Implement the functionality to expand all sections
}

void MainWindow::collapseAllSections() {
    // Implement the functionality to collapse all sections
}
