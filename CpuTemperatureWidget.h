#ifndef CPUTEMPERATUREWIDGET_H
#define CPUTEMPERATUREWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <limits>

class CpuTemperatureWidget : public QWidget {
    Q_OBJECT
public:
    CpuTemperatureWidget(QWidget *parent = nullptr);

private slots:
    void updateTemperatureInfo();
    void updateCoreTemperatures();
    double readTemperature(const QString &filePath);

private:
    QLineEdit *overallTempLabel;
    QLineEdit *minTempLabel;
    QLineEdit *maxTempLabel;
    QGroupBox *coreTempGroup;
    QVBoxLayout *coreTempLayout;
    QComboBox *coreTempComboBox;

    double minTemp = std::numeric_limits<double>::max();
    double maxTemp = std::numeric_limits<double>::min();
};

#endif // CPUTEMPERATUREWIDGET_H
