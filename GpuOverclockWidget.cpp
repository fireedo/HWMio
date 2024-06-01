#include "GpuOverclockWidget.h"
#include <QProcess>
#include <QDebug>

GpuOverclockWidget::GpuOverclockWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    coreClockOffset = new QSpinBox();
    coreClockOffset->setRange(-500, 500); // Example range, adjust as needed
    coreClockOffset->setSuffix(" MHz");

    memoryClockOffset = new QSpinBox();
    memoryClockOffset->setRange(-1000, 1000); // Example range, adjust as needed
    memoryClockOffset->setSuffix(" MHz");

    fanSpeed = new QSpinBox();
    fanSpeed->setRange(-1, 100); // -1 for default, 0-100 for fan speed percentage
    fanSpeed->setSuffix(" %");

    powerLimit = new QSpinBox();
    powerLimit->setRange(-1, 300); // -1 for default, 0-300 for power limit in watts
    powerLimit->setSuffix(" W");

    resultLabel = new QLineEdit();
    resultLabel->setReadOnly(true);

    applyButton = new QPushButton("Apply Overclock");
    defaultButton = new QPushButton("Set to Default");

    formLayout->addRow("Core Clock Offset:", coreClockOffset);
    formLayout->addRow("Memory Clock Offset:", memoryClockOffset);
    formLayout->addRow("Fan Speed:", fanSpeed);
    formLayout->addRow("Power Limit:", powerLimit);
    formLayout->addRow("Result:", resultLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(applyButton);
    mainLayout->addWidget(defaultButton);

    connect(applyButton, &QPushButton::clicked, this, &GpuOverclockWidget::applyOverclock);
    connect(defaultButton, &QPushButton::clicked, this, &GpuOverclockWidget::setToDefault);

    setLayout(mainLayout);
}

void GpuOverclockWidget::applyOverclock() {
    int coreOffset = coreClockOffset->value();
    int memOffset = memoryClockOffset->value();
    int fanSpd = fanSpeed->value();
    int pwrLimit = powerLimit->value();

    QProcess process;
    QString program = "python3";
    QStringList arguments;
    arguments << "gpu_overclock.py" << "0" << QString::number(coreOffset) << QString::number(memOffset)
    << QString::number(fanSpd) << QString::number(pwrLimit);

    process.start(program, arguments);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    resultLabel->setText(output.trimmed());
}

void GpuOverclockWidget::setToDefault() {
    int coreOffset = 0;
    int memOffset = 0;
    int fanSpd = -1; // Default fan speed
    int pwrLimit = -1; // Default power limit

    QProcess process;
    QString program = "python3";
    QStringList arguments;
    arguments << "gpu_overclock.py" << "0" << QString::number(coreOffset) << QString::number(memOffset)
    << QString::number(fanSpd) << QString::number(pwrLimit);

    process.start(program, arguments);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    resultLabel->setText("Set to Default: " + output.trimmed());

    coreClockOffset->setValue(coreOffset);
    memoryClockOffset->setValue(memOffset);
    fanSpeed->setValue(fanSpd);
    powerLimit->setValue(pwrLimit);
}
