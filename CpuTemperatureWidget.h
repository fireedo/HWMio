#ifndef CPUTEMPERATUREWIDGET_H
#define CPUTEMPERATUREWIDGET_H

#include <QtCore/qglobal.h>

#if QT_VERSION_MAJOR >= 6
#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#elif QT_VERSION_MAJOR >= 5
#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#else
#error Unsupported Qt version
#endif

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
