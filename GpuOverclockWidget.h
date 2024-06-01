#ifndef GPUOVERCLOCKWIDGET_H
#define GPUOVERCLOCKWIDGET_H

#include <QtCore/qglobal.h>

#if QT_VERSION_MAJOR >= 6
#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#elif QT_VERSION_MAJOR >= 5
#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#else
#error Unsupported Qt version
#endif

class GpuOverclockWidget : public QWidget {
    Q_OBJECT
public:
    explicit GpuOverclockWidget(QWidget *parent = nullptr);

private slots:
    void applyOverclock();
    void setToDefault();

private:
    QSpinBox *coreClockOffset;
    QSpinBox *memoryClockOffset;
    QSpinBox *fanSpeed;
    QSpinBox *powerLimit;
    QLineEdit *resultLabel;
    QPushButton *applyButton;
    QPushButton *defaultButton;
};

#endif // GPUOVERCLOCKWIDGET_H
