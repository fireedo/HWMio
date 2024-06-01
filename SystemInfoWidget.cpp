#include "SystemInfoWidget.h"
#include <unistd.h>
#include <QProcess>
#include <QGroupBox>

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
    cpuInfoGroup->setLayout(cpuInfoLayout);
    mainLayout->addWidget(cpuInfoGroup);

    // Core Clock Information
    coreClockComboBox = new QComboBox();
    connect(coreClockComboBox, QOverload<int>::of(&QComboBox::activated), this, &SystemInfoWidget::updateCoreClock);
    mainLayout->addWidget(coreClockComboBox);

    setLayout(mainLayout);

    // Set timer untuk memperbarui real-time clock dan frekuensi CPU setiap 0,5 detik
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SystemInfoWidget::updateSystemInfo);
    timer->start(500); // Interval dalam milidetik

    // Set timer untuk mulai membaca wattage dan vcore setelah 0,2 detik
    QTimer::singleShot(200, this, &SystemInfoWidget::startReadingWattage);
    QTimer::singleShot(200, this, &SystemInfoWidget::startReadingVcore);

    updateSystemInfo();
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

void SystemInfoWidget::startReadingWattage() {
    // Set timer untuk memperbarui wattage setiap 0,5 detik
    QTimer *wattageTimer = new QTimer(this);
    connect(wattageTimer, &QTimer::timeout, this, &SystemInfoWidget::updateWattageInfo);
    wattageTimer->start(500); // Interval dalam milidetik

    // Inisialisasi waktu dan energi sebelumnya
    previousTime = QTime::currentTime();
    previousEnergyJoules = readCurrentEnergy();
}

double SystemInfoWidget::readCurrentEnergy() {
    QFile energyFile("/sys/class/powercap/intel-rapl:0/energy_uj");
    if (energyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&energyFile);
        double currentEnergyMicroJoules = in.readLine().toDouble();
        energyFile.close();
        return currentEnergyMicroJoules / 1000000.0; // Convert to joules
    }
    return 0.0;
}

void SystemInfoWidget::updateWattageInfo() {
    double currentEnergyJoules = readCurrentEnergy();
    QTime currentTime = QTime::currentTime();
    int elapsedTime = previousTime.msecsTo(currentTime); // Elapsed time in milliseconds

    if (elapsedTime > 0) {
        double powerWatts = ((currentEnergyJoules - previousEnergyJoules) / (elapsedTime / 1000.0)); // Convert to watts

        // Display the wattage information in the GUI with appropriate units
        wattageLabel->setText(QString::number(powerWatts, 'f', 2) + " W");

        // Update minimum wattage
        if (powerWatts < minWattage || minWattage == 0) {
            minWattage = powerWatts;
            minWattageLabel->setText(QString::number(minWattage, 'f', 2) + " W");
        }
        // Update maximum wattage
        if (powerWatts > maxWattage) {
            maxWattage = powerWatts;
            maxWattageLabel->setText(QString::number(maxWattage, 'f', 2) + " W");
        }
        // Update average wattage
        totalWattage += powerWatts;
        wattageReadings++;
        double avgWattage = totalWattage / wattageReadings;
        avgWattageLabel->setText(QString::number(avgWattage, 'f', 2) + " W");

        previousEnergyJoules = currentEnergyJoules;
        previousTime = currentTime;
    }
}

void SystemInfoWidget::startReadingVcore() {
    // Set timer untuk memperbarui vcore setiap 0,5 detik
    QTimer *vcoreTimer = new QTimer(this);
    connect(vcoreTimer, &QTimer::timeout, this, &SystemInfoWidget::updateVcoreInfo);
    vcoreTimer->start(500); // Interval dalam milidetik

    // Inisialisasi vcore sebelumnya
    previousVcore = readCurrentVcore();
}

double SystemInfoWidget::readCurrentVcore() {
    QProcess process;
    QStringList arguments;
    arguments << "rdmsr" << "0x198" << "-u" << "--bitfield" << "47:32";
    process.start("pkexec", arguments);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    bool ok;
    double voltageRaw = output.trimmed().toInt(&ok);
    if (ok) {
        double voltage = voltageRaw / 8192.0; // Convert to volts
        return voltage;
    } else {
        return 0.0;
    }
}

void SystemInfoWidget::updateVcoreInfo() {
    double currentVcore = readCurrentVcore();

    // Display the vcore information in the GUI with appropriate units
    vcoreLabel->setText(QString::number(currentVcore, 'f', 2) + " V");

    // Update minimum vcore
    if (currentVcore < minVcore || minVcore == 0) {
        minVcore = currentVcore;
        minVcoreLabel->setText(QString::number(minVcore, 'f', 2) + " V");
    }
    // Update maximum vcore
    if (currentVcore > maxVcore) {
        maxVcore = currentVcore;
        maxVcoreLabel->setText(QString::number(maxVcore, 'f', 2) + " V");
    }
}

bool SystemInfoWidget::isRootPrivilegeObtained() {
    // Check if the process is running with root privileges
    return (geteuid() == 0);
}
