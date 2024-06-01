#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/qglobal.h>

#if QT_VERSION_MAJOR >= 6
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QPushButton>
#elif QT_VERSION_MAJOR >= 5
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QPushButton>
#else
#error Unsupported Qt version
#endif

#include "SystemInfoWidget.h"
#include "CpuTemperatureWidget.h"
#include "GpuInfoWidget.h"
#include "GpuOverclockWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void expandAllSections();
    void collapseAllSections();

private:
    QToolBox *toolBox;
    SystemInfoWidget *systemInfoWidget;
    CpuTemperatureWidget *cpuTemperatureWidget;
    GpuInfoWidget *gpuInfoWidget;
    GpuOverclockWidget *gpuOverclockWidget;
    QPushButton *expandAllButton;
    QPushButton *collapseAllButton;
};

#endif // MAINWINDOW_H
