#include "MainWindow.h"
#include "SystemInfoWidget.h"
#include "CpuTemperatureWidget.h"
#include "GpuInfoWidget.h"
#include "GpuOverclockWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolBox>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), toolBox(new QToolBox(this))
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    systemInfoWidget = new SystemInfoWidget(this);
    cpuTemperatureWidget = new CpuTemperatureWidget(this);
    gpuInfoWidget = new GpuInfoWidget(this);
    gpuOverclockWidget = new GpuOverclockWidget(this);

    toolBox->addItem(systemInfoWidget, "Informasi CPU");
    toolBox->addItem(cpuTemperatureWidget, "CPU Temperature Information");
    toolBox->addItem(gpuInfoWidget, "GPU Information");
    toolBox->addItem(gpuOverclockWidget, "Overclock Function");

    QPushButton *expandAllButton = new QPushButton("Expand All Sections", this);
    QPushButton *collapseAllButton = new QPushButton("Collapse All Sections", this);

    connect(expandAllButton, &QPushButton::clicked, this, &MainWindow::expandAllSections);
    connect(collapseAllButton, &QPushButton::clicked, this, &MainWindow::collapseAllSections);

    layout->addWidget(toolBox);
    layout->addWidget(expandAllButton);
    layout->addWidget(collapseAllButton);

    setCentralWidget(centralWidget);
}

void MainWindow::expandAllSections()
{
    for (int i = 0; i < toolBox->count(); ++i)
    {
        toolBox->setCurrentIndex(i);
        toolBox->widget(i)->show();
    }
}

void MainWindow::collapseAllSections()
{
    for (int i = 0; i < toolBox->count(); ++i)
    {
        toolBox->widget(i)->hide();
    }
    toolBox->setCurrentIndex(0);
    toolBox->widget(0)->show();
}

MainWindow::~MainWindow()
{
    // Qt will automatically delete child widgets when the parent is deleted
}
