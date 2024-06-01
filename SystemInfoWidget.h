#ifndef SYSTEMINFOWIDGET_H
#define SYSTEMINFOWIDGET_H

#include <QtWidgets>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QTime>

class SystemInfoWidget : public QWidget {
    Q_OBJECT
public:
    explicit SystemInfoWidget(QWidget *parent = nullptr);

private slots:
    void updateSystemInfo();
    void updateCoreClockComboBox();
    void updateCoreClock(int index);
    void startReadingWattage();
    void startReadingVcore();
    void updateWattageInfo();
    void updateVcoreInfo();

private:
    bool isRootPrivilegeObtained();
    double readCurrentEnergy();
    double readCurrentVcore();

    QLineEdit *cpuNameLabel;
    QLineEdit *cpuFreqLabel;
    QLineEdit *minFreqLabel;
    QLineEdit *maxFreqLabel;
    QLineEdit *clockLabel;
    QLineEdit *wattageLabel;
    QLineEdit *minWattageLabel;
    QLineEdit *maxWattageLabel;
    QLineEdit *avgWattageLabel;
    QLineEdit *vcoreLabel;
    QLineEdit *minVcoreLabel;
    QLineEdit *maxVcoreLabel;
    QComboBox *coreClockComboBox;

    int minFreq = 0;
    int maxFreq = 0;
    double minWattage = 0;
    double maxWattage = 0;
    double totalWattage = 0;
    int wattageReadings = 0;
    double previousEnergyJoules = 0;
    QTime previousTime;
    double minVcore = 0;
    double maxVcore = 0;
    double previousVcore = 0;
};

#endif // SYSTEMINFOWIDGET_H
