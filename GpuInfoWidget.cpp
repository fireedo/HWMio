#include "GpuInfoWidget.h"
#include <QProcess>
#include <QStringList>
#include <QDebug>
#include <QGroupBox>

GpuInfoWidget::GpuInfoWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGroupBox *gpuInfoGroup = new QGroupBox("Informasi GPU");
    QFormLayout *gpuInfoLayout = new QFormLayout(gpuInfoGroup);

    gpuModelLabel = new QLineEdit();
    gpuModelLabel->setReadOnly(true);
    coreClockLabel = new QLineEdit();
    coreClockLabel->setReadOnly(true);
    memoryClockLabel = new QLineEdit();
    memoryClockLabel->setReadOnly(true);
    fanSpeedLabel = new QLineEdit();
    fanSpeedLabel->setReadOnly(true);
    temperatureLabel = new QLineEdit();
    temperatureLabel->setReadOnly(true);
    wattageLabel = new QLineEdit();
    wattageLabel->setReadOnly(true);
    minCoreClockLabel = new QLineEdit();
    minCoreClockLabel->setReadOnly(true);
    maxCoreClockLabel = new QLineEdit();
    maxCoreClockLabel->setReadOnly(true);
    minMemoryClockLabel = new QLineEdit();
    minMemoryClockLabel->setReadOnly(true);
    maxMemoryClockLabel = new QLineEdit();
    maxMemoryClockLabel->setReadOnly(true);
    minTemperatureLabel = new QLineEdit();
    minTemperatureLabel->setReadOnly(true);
    maxTemperatureLabel = new QLineEdit();
    maxTemperatureLabel->setReadOnly(true);
    minWattageLabel = new QLineEdit();
    minWattageLabel->setReadOnly(true);
    maxWattageLabel = new QLineEdit();
    maxWattageLabel->setReadOnly(true);

    gpuInfoLayout->addRow("Model GPU:", gpuModelLabel);
    gpuInfoLayout->addRow("Core Clock:", coreClockLabel);
    gpuInfoLayout->addRow("Min Core Clock:", minCoreClockLabel);
    gpuInfoLayout->addRow("Max Core Clock:", maxCoreClockLabel);
    gpuInfoLayout->addRow("Memory Clock:", memoryClockLabel);
    gpuInfoLayout->addRow("Min Memory Clock:", minMemoryClockLabel);
    gpuInfoLayout->addRow("Max Memory Clock:", maxMemoryClockLabel);
    gpuInfoLayout->addRow("Fan Speed:", fanSpeedLabel);
    gpuInfoLayout->addRow("Temperature:", temperatureLabel);
    gpuInfoLayout->addRow("Min Temperature:", minTemperatureLabel);
    gpuInfoLayout->addRow("Max Temperature:", maxTemperatureLabel);
    gpuInfoLayout->addRow("Wattage:", wattageLabel);
    gpuInfoLayout->addRow("Min Wattage:", minWattageLabel);
    gpuInfoLayout->addRow("Max Wattage:", maxWattageLabel);
    gpuInfoGroup->setLayout(gpuInfoLayout);
    mainLayout->addWidget(gpuInfoGroup);

    setLayout(mainLayout);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GpuInfoWidget::updateGpuInfo);
    timer->start(1000); // Update every second

    updateGpuInfo();
}

void GpuInfoWidget::updateGpuInfo() {
    QProcess process;
    QStringList arguments;
    arguments << "--query-gpu=name,clocks.gr,clocks.mem,fan.speed,temperature.gpu,power.draw"
    << "--format=csv,noheader,nounits";
    process.start("nvidia-smi", arguments);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", QString::SkipEmptyParts);

    if (!lines.isEmpty()) {
        QStringList values = lines[0].split(", ");

        if (values.size() >= 6) {
            QString gpuModel = values[0];
            int coreClock = values[1].toInt();
            int memoryClock = values[2].toInt();
            int fanSpeed = values[3].toInt();
            int temperature = values[4].toInt();
            double wattage = values[5].toDouble();

            gpuModelLabel->setText(gpuModel);
            coreClockLabel->setText(QString::number(coreClock) + " MHz");
            memoryClockLabel->setText(QString::number(memoryClock) + " MHz");
            fanSpeedLabel->setText(QString::number(fanSpeed) + " %");
            temperatureLabel->setText(QString::number(temperature) + " °C");
            wattageLabel->setText(QString::number(wattage, 'f', 2) + " W");

            // Update min and max values for core clock
            if (coreClock < minCoreClock || minCoreClock == 0) {
                minCoreClock = coreClock;
                minCoreClockLabel->setText(QString::number(minCoreClock) + " MHz");
            }
            if (coreClock > maxCoreClock) {
                maxCoreClock = coreClock;
                maxCoreClockLabel->setText(QString::number(maxCoreClock) + " MHz");
            }

            // Update min and max values for memory clock
            if (memoryClock < minMemoryClock || minMemoryClock == 0) {
                minMemoryClock = memoryClock;
                minMemoryClockLabel->setText(QString::number(minMemoryClock) + " MHz");
            }
            if (memoryClock > maxMemoryClock) {
                maxMemoryClock = memoryClock;
                maxMemoryClockLabel->setText(QString::number(maxMemoryClock) + " MHz");
            }

            // Update min and max values for temperature
            if (temperature < minTemperature || minTemperature == 0) {
                minTemperature = temperature;
                minTemperatureLabel->setText(QString::number(minTemperature) + " °C");
            }
            if (temperature > maxTemperature) {
                maxTemperature = temperature;
                maxTemperatureLabel->setText(QString::number(maxTemperature) + " °C");
            }

            // Update min and max values for wattage
            if (wattage < minWattage || minWattage == 0) {
                minWattage = wattage;
                minWattageLabel->setText(QString::number(minWattage, 'f', 2) + " W");
            }
            if (wattage > maxWattage) {
                maxWattage = wattage;
                maxWattageLabel->setText(QString::number(maxWattage, 'f', 2) + " W");
            }
        }
    }
}
