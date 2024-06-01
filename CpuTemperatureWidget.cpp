#include "CpuTemperatureWidget.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDir>
#include <QDebug>
#include <limits>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

CpuTemperatureWidget::CpuTemperatureWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // CPU Temperature Information
    QGroupBox *tempInfoGroup = new QGroupBox("Informasi Suhu CPU");
    QFormLayout *tempInfoLayout = new QFormLayout(tempInfoGroup);
    overallTempLabel = new QLineEdit();
    overallTempLabel->setReadOnly(true);
    minTempLabel = new QLineEdit();
    minTempLabel->setReadOnly(true);
    maxTempLabel = new QLineEdit();
    maxTempLabel->setReadOnly(true);

    tempInfoLayout->addRow("Suhu Keseluruhan:", overallTempLabel);
    tempInfoLayout->addRow("Suhu Minimum:", minTempLabel);
    tempInfoLayout->addRow("Suhu Maksimum:", maxTempLabel);
    tempInfoGroup->setLayout(tempInfoLayout);
    mainLayout->addWidget(tempInfoGroup);

    // Per-core Temperature Information
    coreTempGroup = new QGroupBox("Suhu Masing-masing Core");
    coreTempLayout = new QVBoxLayout(coreTempGroup);
    coreTempComboBox = new QComboBox();
    coreTempLayout->addWidget(coreTempComboBox);
    coreTempGroup->setLayout(coreTempLayout);
    mainLayout->addWidget(coreTempGroup);

    setLayout(mainLayout);

    // Set timer untuk memperbarui suhu setiap 0,5 detik
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CpuTemperatureWidget::updateTemperatureInfo);
    timer->start(500); // Interval dalam milidetik

    updateTemperatureInfo();
}

void CpuTemperatureWidget::updateTemperatureInfo() {
    double overallTemp = readTemperature("/sys/class/thermal/thermal_zone0/temp") / 1000.0; // Assuming thermal_zone0 is overall CPU temperature
    overallTempLabel->setText(QString::number(overallTemp, 'f', 2) + " °C");

    // Update minimum and maximum temperature
    if (overallTemp < minTemp || minTemp == std::numeric_limits<double>::max()) {
        minTemp = overallTemp;
    }
    if (overallTemp > maxTemp || maxTemp == std::numeric_limits<double>::min()) {
        maxTemp = overallTemp;
    }
    minTempLabel->setText(QString::number(minTemp, 'f', 2) + " °C");
    maxTempLabel->setText(QString::number(maxTemp, 'f', 2) + " °C");

    // Update core temperatures
    updateCoreTemperatures();
}

void CpuTemperatureWidget::updateCoreTemperatures() {
    coreTempComboBox->clear();

    double totalTemp = 0;
    int coreCount = 0;
    double minCoreTemp = std::numeric_limits<double>::max();
    double maxCoreTemp = std::numeric_limits<double>::min();

    // Populate core temperatures
    QDir hwmonDir("/sys/class/hwmon");
    QStringList hwmonEntries = hwmonDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QMap<int, double> coreTemps; // Using QMap to sort by core index

    foreach (const QString &hwmonEntry, hwmonEntries) {
        QDir tempDir(hwmonDir.absoluteFilePath(hwmonEntry));
        QStringList tempFiles = tempDir.entryList(QStringList() << "temp*_input", QDir::Files);

        foreach (const QString &tempFile, tempFiles) {
            QString tempPath = tempDir.absoluteFilePath(tempFile);
            double coreTemp = readTemperature(tempPath) / 1000.0;

            // Extract core number from filename
            #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QRegularExpression regex("temp(\\d+)_input");
            QRegularExpressionMatch match = regex.match(tempFile);
            if (match.hasMatch()) {
                int coreIndex = match.captured(1).toInt() - 2; // Adjusting index to start from core0
                #else
                QRegExp regex("temp(\\d+)_input");
                if (regex.indexIn(tempFile) != -1) {
                    int coreIndex = regex.cap(1).toInt() - 2; // Adjusting index to start from core0
                    #endif
                    if (coreIndex >= 0) {
                        coreTemps[coreIndex] = coreTemp;
                    }
                }
            }
        }

        QMapIterator<int, double> i(coreTemps);
        while (i.hasNext()) {
            i.next();
            coreTempComboBox->addItem(QString("Core %1: %2 °C").arg(i.key()).arg(i.value(), 0, 'f', 2));

            totalTemp += i.value();
            coreCount++;

            if (i.value() < minCoreTemp) {
                minCoreTemp = i.value();
            }

            if (i.value() > maxCoreTemp) {
                maxCoreTemp = i.value();
            }
        }

        if (coreCount > 0) {
            double avgCoreTemp = totalTemp / coreCount;
            minTempLabel->setText(QString::number(minCoreTemp, 'f', 2) + " °C");
            maxTempLabel->setText(QString::number(maxCoreTemp, 'f', 2) + " °C");
            overallTempLabel->setText(QString::number(avgCoreTemp, 'f', 2) + " °C");
        }
    }

    double CpuTemperatureWidget::readTemperature(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Cannot open file for reading:" << filePath;
            return 0;
        }

        QTextStream in(&file);
        double temp = in.readLine().toDouble();
        file.close();
        return temp;
    }
