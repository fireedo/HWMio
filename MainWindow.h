#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBox>

class SystemInfoWidget;
class CpuTemperatureWidget;
class GpuInfoWidget;
class GpuOverclockWidget;

class MainWindow : public QMainWindow
{
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
};

#endif // MAINWINDOW_H
