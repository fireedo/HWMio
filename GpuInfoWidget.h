#ifndef GPUINFOWIDGET_H
#define GPUINFOWIDGET_H

#include <QtCore/qglobal.h>

#if QT_VERSION_MAJOR >= 6
#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtCore/QTimer>
#elif QT_VERSION_MAJOR >= 5
#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtCore/QTimer>
#else
#error Unsupported Qt version
#endif

class GpuInfoWidget : public QWidget {
    Q_OBJECT
public:
    explicit GpuInfoWidget(QWidget *parent = nullptr);

private slots:
    void updateGpuInfo();

private:
    QLineEdit *gpuModelLabel;
    QLineEdit *coreClockLabel;
    QLineEdit *memoryClockLabel;
    QLineEdit *fanSpeedLabel;
    QLineEdit *temperatureLabel;
    QLineEdit *wattageLabel;
    QLineEdit *minCoreClockLabel;
    QLineEdit *maxCoreClockLabel;
    QLineEdit *minMemoryClockLabel;
    QLineEdit *maxMemoryClockLabel;
    QLineEdit *minTemperatureLabel;
    QLineEdit *maxTemperatureLabel;
    QLineEdit *minWattageLabel;
    QLineEdit *maxWattageLabel;

    int minCoreClock = 0;
    int maxCoreClock = 0;
    int minMemoryClock = 0;
    int maxMemoryClock = 0;
    int minTemperature = 0;
    int maxTemperature = 0;
    double minWattage = 0.0;
    double maxWattage = 0.0;
};

#endif // GPUINFOWIDGET_H
