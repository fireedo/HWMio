#ifndef GPUOVERCLOCKWIDGET_H
#define GPUOVERCLOCKWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>

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
    QLineEdit *resultLabel;
    QPushButton *applyButton;
    QPushButton *defaultButton;
};

#endif // GPUOVERCLOCKWIDGET_H
