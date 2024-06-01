#include "SystemInfoWidget.h"
#include <unistd.h>
#include <QProcess>
#include <QGroupBox>
#include <QMessageBox>
#include <iostream>
#include <cstdio>

SystemInfoWidget::SystemInfoWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // CPU Information
    QGroupBox *cpuInfoGroup = new QGroupBox("Informasi CPU");
    QFormLayout *cpuInfoLayout = new QFormLayout(cpuInfoGroup);
    cpuNameLabel = new QLineEdit();
    cpuNameLabel->setReadOnly(true);
    cpuFreqLabel = new QLineEdit();
    cpuFreqLabel->setReadOnly(true);
    minFreqLabel = new QLineEdit();
    minFreqLabel->setReadOnly(true);
    maxFreqLabel = new QLineEdit();
    maxFreqLabel->setReadOnly(true);
    clockLabel = new QLineEdit();
    clockLabel->setReadOnly(true);
    wattageLabel = new QLineEdit();
    wattageLabel->setReadOnly(true);
    minWattageLabel = new QLineEdit();
    minWattageLabel->setReadOnly(true);
    maxWattageLabel = new QLineEdit();
    maxWattageLabel->setReadOnly(true);
    avgWattageLabel = new QLineEdit();
    avgWattageLabel->setReadOnly(true);
    vcoreLabel = new QLineEdit();
    vcoreLabel->setReadOnly(true);
    minVcoreLabel = new QLineEdit();
    minVcoreLabel->setReadOnly(true);
    maxVcoreLabel = new QLineEdit();
    maxVcoreLabel->setReadOnly(true);

    showWattageVoltageButton = new QPushButton("Show Wattage/Voltage");
    connect(showWattageVoltageButton, &QPushButton::clicked, this, &SystemInfoWidget::showPasswordDialog);

    cpuInfoLayout->addRow("Nama CPU:", cpuNameLabel);
    cpuInfoLayout->addRow("Frekuensi CPU:", cpuFreqLabel);
    cpuInfoLayout->addRow("Minimum Frequency:", minFreqLabel);
    cpuInfoLayout->addRow("Maximum Frequency:", maxFreqLabel);
    cpuInfoLayout->addRow("Real-time Clock:", clockLabel);
    cpuInfoLayout->addRow("CPU Wattage:", wattageLabel);
    cpuInfoLayout->addRow("Minimum Wattage:", minWattageLabel);
    cpuInfoLayout->addRow("Maximum Wattage:", maxWattageLabel);
    cpuInfoLayout->addRow("Average Wattage:", avgWattageLabel);
    cpuInfoLayout->addRow("VCore Voltage:", vcoreLabel);
    cpuInfoLayout->addRow("Minimum VCore:", minVcoreLabel);
    cpuInfoLayout->addRow("Maximum VCore:", maxVcoreLabel);
    cpuInfoLayout->addRow(showWattageVoltageButton);
    cpuInfoGroup->setLayout(cpuInfoLayout);
    mainLayout->addWidget(cpuInfoGroup);

    // Core Clock Information
    coreClockComboBox = new QComboBox();
    connect(coreClockComboBox, QOverload<int>::of(&QComboBox::activated), this, &SystemInfoWidget::updateCoreClock);
    mainLayout->addWidget(coreClockComboBox);

    setLayout(mainLayout);

    wattageVoltageTimer = new QTimer(this);
    connect(wattageVoltageTimer, &QTimer::timeout, this, &SystemInfoWidget::updateWattageVoltageInfo);

    updateSystemInfo();

    previousEnergyJoules = readCurrentEnergy(); // Initialize the previous energy value
    previousTime = QTime::currentTime(); // Initialize the previous time value
}

void SystemInfoWidget::updateSystemInfo() {
    QString cpuName;

    // Try to get CPU name from /proc/cpuinfo
    QFile cpuInfoFile("/proc/cpuinfo");
    if (cpuInfoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&cpuInfoFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("model name")) {
                cpuName = line.section(':', 1).trimmed();
                break;
            }
        }
        cpuInfoFile.close();
    }

    // If CPU name not found in /proc/cpuinfo, try lscpu
    if (cpuName.isEmpty()) {
        QProcess process;
        process.start("lscpu", QStringList());
        process.waitForFinished();
        QByteArray result = process.readAllStandardOutput();
        QStringList lines = QString(result).split('\n', Qt::SkipEmptyParts);
        foreach (const QString &line, lines) {
            if (line.startsWith("Model name:")) {
                cpuName = line.section(':', 1).trimmed();
                break;
            }
        }
    }

    if (!cpuName.isEmpty()) {
        cpuNameLabel->setText(cpuName);
    } else {
        cpuNameLabel->setText("Error: Model name not found");
    }

    // Try to get CPU frequency from /proc/cpuinfo
    QString cpuFreq;
    QFile cpuFreqFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    if (cpuFreqFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&cpuFreqFile);
        QString line = in.readLine();
        cpuFreq = QString::number(line.toInt() / 1000); // Convert ke MHz
        cpuFreqFile.close();
    }

    if (!cpuFreq.isEmpty()) {
        cpuFreqLabel->setText(cpuFreq + " MHz");
        // Update minimum frequency
        int currentFreq = cpuFreq.toInt();
        if (currentFreq < minFreq || minFreq == 0) {
            minFreq = currentFreq;
            minFreqLabel->setText(QString::number(minFreq) + " MHz");
        }
        // Update maximum frequency
        if (currentFreq > maxFreq) {
            maxFreq = currentFreq;
            maxFreqLabel->setText(QString::number(maxFreq) + " MHz");
        }
    } else {
        cpuFreqLabel->setText("Error: CPU frequency not found");
    }

    // Update Real-time Clock
    QProcess clockProcess;
    clockProcess.start("date", QStringList() << "+%T");
    if (clockProcess.waitForFinished()) {
        QString output = clockProcess.readAllStandardOutput();
        clockLabel->setText(output.trimmed());
    } else {
        clockLabel->setText("Error: Unable to get real-time clock");
    }

    // Update core clock combo box
    updateCoreClockComboBox();
}

void SystemInfoWidget::updateCoreClockComboBox() {
    // Clear existing items
    coreClockComboBox->clear();

    // Populate core clock info
    for (int cpu = 0; cpu < 32; ++cpu) {
        QString coreFreqPath = QString("/sys/devices/system/cpu/cpu%1/cpufreq/scaling_cur_freq").arg(cpu);
        QFile coreFreqFile(coreFreqPath);
        if (coreFreqFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&coreFreqFile);
            QString coreFreq = QString::number(in.readLine().toInt() / 1000); // Convert ke MHz
            coreClockComboBox->addItem(QString("CPU %1: %2 MHz").arg(cpu).arg(coreFreq));
            coreFreqFile.close();
        }
    }
}

void SystemInfoWidget::updateCoreClock(int index) {
    QString selectedCoreInfo = coreClockComboBox->itemText(index);
    qDebug() << "Selected Core Info:" << selectedCoreInfo;
}

void SystemInfoWidget::showPasswordDialog() {
    passwordDialog = new QDialog(this);
    passwordDialog->setWindowTitle("Enter Root Password");

    QVBoxLayout *layout = new QVBoxLayout(passwordDialog);
    QLabel *label = new QLabel("Root password is required to access CPU wattage and voltage information:");
    passwordInput = new QLineEdit();
    passwordInput->setEchoMode(QLineEdit::Password);

    passwordSubmitButton = new QPushButton("Submit");
    connect(passwordSubmitButton, &QPushButton::clicked, this, &SystemInfoWidget::handlePasswordEntered);

    layout->addWidget(label);
    layout->addWidget(passwordInput);
    layout->addWidget(passwordSubmitButton);

    passwordDialog->setLayout(layout);
    passwordDialog->exec();
}

void SystemInfoWidget::handlePasswordEntered() {
    rootPassword = passwordInput->text();
    wattageVoltageTimer->start(1000); // Update every second
    passwordDialog->close();
    updateWattageVoltageInfo();
}

void SystemInfoWidget::updateWattageVoltageInfo() {
    qDebug() << "Updating wattage and voltage information.";

    if (rootPassword.isEmpty()) {
        showPasswordDialog();
        return;
    }

    // Read current energy
    QString wattageCommand = QString("echo %1 | sudo -S cat /sys/class/powercap/intel-rapl:0/energy_uj").arg(rootPassword);
    QString wattageOutput = executeShellCommand(wattageCommand);
    qDebug() << "Wattage output:" << wattageOutput;

    bool ok;
    double currentEnergyJoules = wattageOutput.trimmed().toDouble(&ok) / 1000000.0; // Convert to joules
    if (ok) {
        QTime currentTime = QTime::currentTime();
        int elapsedTimeMs = previousTime.msecsTo(currentTime);
        double currentPowerWatts = (currentEnergyJoules - previousEnergyJoules) * 1000.0 / elapsedTimeMs; // Power in watts
        previousEnergyJoules = currentEnergyJoules;
        previousTime = currentTime;

        wattageLabel->setText(QString::number(currentPowerWatts, 'f', 2) + " W");

        // Update wattage min/max/avg values
        if (currentPowerWatts < minWattage || minWattage == 0) {
            minWattage = currentPowerWatts;
            minWattageLabel->setText(QString::number(minWattage, 'f', 2) + " W");
        }
        if (currentPowerWatts > maxWattage) {
            maxWattage = currentPowerWatts;
            maxWattageLabel->setText(QString::number(maxWattage, 'f', 2) + " W");
        }

        totalWattage += currentPowerWatts;
        wattageReadings++;
        double avgWattage = totalWattage / wattageReadings;
        avgWattageLabel->setText(QString::number(avgWattage, 'f', 2) + " W");
    } else {
        wattageLabel->setText("Error: Unable to read wattage");
    }

    // Read VCore
    QString vcoreCommand = QString("echo %1 | sudo -S rdmsr 0x198 -u --bitfield 47:32").arg(rootPassword);
    QString vcoreOutput = executeShellCommand(vcoreCommand);
    qDebug() << "VCore output:" << vcoreOutput;

    double voltageRaw = vcoreOutput.trimmed().toInt(&ok);
    if (ok) {
        double voltage = voltageRaw / 8192.0; // Convert to volts
        vcoreLabel->setText(QString::number(voltage, 'f', 2) + " V");

        // Update minimum and maximum VCore
        if (voltage < minVcore || minVcore == 0) {
            minVcore = voltage;
            minVcoreLabel->setText(QString::number(minVcore, 'f', 2) + " V");
        }
        if (voltage > maxVcore) {
            maxVcore = voltage;
            maxVcoreLabel->setText(QString::number(maxVcore, 'f', 2) + " V");
        }
    } else {
        vcoreLabel->setText("Error: Unable to read voltage");
    }
}

double SystemInfoWidget::readCurrentEnergy() {
    QString command = "cat /sys/class/powercap/intel-rapl:0/energy_uj";
    QString output = executeShellCommand(command);
    bool ok;
    double energy = output.trimmed().toDouble(&ok);
    if (ok) {
        return energy / 1000000.0; // Convert to joules
    }
    return 0.0;
}

QString SystemInfoWidget::executeShellCommand(const QString &command) {
    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    if (output.isEmpty()) {
        output = process.readAllStandardError();
    }
    return output;
}
