import sys
import os
import subprocess
from PyQt6.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QLineEdit, QPushButton, QMessageBox, QCheckBox, QComboBox
from PyQt6.QtCore import Qt
from pynvml import *

# Initialize NVML
nvmlInit()

class OverclockApp(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('NVIDIA GPU Overclocking')

        layout = QVBoxLayout()

        self.gpu_index_label = QLabel('Select GPU:', self)
        layout.addWidget(self.gpu_index_label)
        self.gpu_index_combo = QComboBox(self)
        self.populate_gpu_list()
        layout.addWidget(self.gpu_index_combo)

        self.gpu_offset_label = QLabel('GPU Offset:', self)
        layout.addWidget(self.gpu_offset_label)
        self.gpu_offset_input = QLineEdit(self)
        layout.addWidget(self.gpu_offset_input)
        self.default_gpu_offset_checkbox = QCheckBox('Use default GPU offset', self)
        self.default_gpu_offset_checkbox.stateChanged.connect(self.update_offsets)
        layout.addWidget(self.default_gpu_offset_checkbox)

        self.mem_offset_label = QLabel('Memory Offset:', self)
        layout.addWidget(self.mem_offset_label)
        self.mem_offset_input = QLineEdit(self)
        layout.addWidget(self.mem_offset_input)
        self.default_mem_offset_checkbox = QCheckBox('Use default Memory offset', self)
        self.default_mem_offset_checkbox.stateChanged.connect(self.update_offsets)
        layout.addWidget(self.default_mem_offset_checkbox)

        self.power_limit_label = QLabel('Power Limit (mW):', self)
        layout.addWidget(self.power_limit_label)
        self.power_limit_input = QLineEdit(self)
        layout.addWidget(self.power_limit_input)
        self.default_power_limit_checkbox = QCheckBox('Use default power limit', self)
        self.default_power_limit_checkbox.stateChanged.connect(self.update_power_limit)
        layout.addWidget(self.default_power_limit_checkbox)

        self.fan_speed_label = QLabel('Fan Speed (%):', self)
        layout.addWidget(self.fan_speed_label)
        self.fan_speed_combo = QComboBox(self)
        self.fan_speed_combo.addItem("Default")
        for i in range(10, 110, 10):
            self.fan_speed_combo.addItem(f"{i}%")
        layout.addWidget(self.fan_speed_combo)
        self.manual_fan_control_checkbox = QCheckBox('Enable manual fan control', self)
        self.manual_fan_control_checkbox.stateChanged.connect(self.update_fan_control)
        layout.addWidget(self.manual_fan_control_checkbox)

        self.apply_button = QPushButton('Apply Overclock', self)
        self.apply_button.clicked.connect(self.apply_overclock)
        layout.addWidget(self.apply_button)

        self.setLayout(layout)

    def populate_gpu_list(self):
        try:
            device_count = nvmlDeviceGetCount()
            for i in range(device_count):
                handle = nvmlDeviceGetHandleByIndex(i)
                name = nvmlDeviceGetName(handle)
                self.gpu_index_combo.addItem(f"GPU {i}: {name}", i)
        except Exception as e:
            print(f"Error populating GPU list: {str(e)}")
            QMessageBox.critical(self, "Error", "Failed to retrieve GPU list")

    def get_default_power_limit(self, gpu_index):
        try:
            result = subprocess.run(['nvidia-smi', '-i', str(gpu_index), '-q', '-d', 'POWER'], capture_output=True, text=True)
            if result.returncode != 0:
                raise Exception("nvidia-smi command failed")

            for line in result.stdout.split('\n'):
                if 'Default Power Limit' in line:
                    value_str = line.split(':')[1].strip().split(' ')[0]
                    return int(float(value_str) * 1000)  # Convert W to mW
        except Exception as e:
            print(f"Error: {str(e)}")
            return None

    def get_default_gpu_offset(self, gpu_index):
        return 0

    def get_default_mem_offset(self, gpu_index):
        return 0

    def update_offsets(self):
        gpu_index = self.gpu_index_combo.currentData()
        if self.default_gpu_offset_checkbox.isChecked():
            self.gpu_offset_input.setText(str(self.get_default_gpu_offset(gpu_index)))
        else:
            self.gpu_offset_input.clear()

        if self.default_mem_offset_checkbox.isChecked():
            self.mem_offset_input.setText(str(self.get_default_mem_offset(gpu_index)))
        else:
            self.mem_offset_input.clear()

    def update_power_limit(self):
        if self.default_power_limit_checkbox.isChecked():
            gpu_index = self.gpu_index_combo.currentData()
            power_limit = self.get_default_power_limit(gpu_index)
            if power_limit is not None:
                self.power_limit_input.setText(str(power_limit))
            else:
                QMessageBox.critical(self, "Error", "Failed to get default power limit")
                self.default_power_limit_checkbox.setChecked(False)
        else:
            self.power_limit_input.clear()

    def update_fan_control(self):
        self.fan_speed_combo.setEnabled(self.manual_fan_control_checkbox.isChecked())

    def apply_overclock(self):
        try:
            gpu_index = self.gpu_index_combo.currentData()
            gpu_offset = int(self.gpu_offset_input.text())
            mem_offset = int(self.mem_offset_input.text()) * 2

            if self.default_power_limit_checkbox.isChecked():
                power_limit = self.get_default_power_limit(gpu_index)
                if power_limit is None:
                    QMessageBox.critical(self, "Error", "Failed to get default power limit")
                    return
            else:
                power_limit = int(self.power_limit_input.text())

            gpu_handle = nvmlDeviceGetHandleByIndex(gpu_index)
            min_power_limit = nvmlDeviceGetPowerManagementLimitConstraints(gpu_handle)[0]
            max_power_limit = nvmlDeviceGetPowerManagementLimitConstraints(gpu_handle)[1]

            if power_limit < min_power_limit or power_limit > max_power_limit:
                QMessageBox.critical(self, "Error", f"Power limit must be between {min_power_limit} mW and {max_power_limit} mW")
                return

            fan_speed_script = ""
            if self.manual_fan_control_checkbox.isChecked():
                fan_speed_text = self.fan_speed_combo.currentText()
                if fan_speed_text == "Default":
                    fan_speed_script = "nvmlDeviceSetDefaultFanSpeed(myGPU)\n"
                else:
                    fan_speed = int(fan_speed_text.replace('%', ''))
                    fan_speed_script = f"nvmlDeviceSetGpuFanSpeed(myGPU, {fan_speed})\n"

            script_content = f"""
from pynvml import *
nvmlInit()
myGPU = nvmlDeviceGetHandleByIndex({gpu_index})
nvmlDeviceSetGpcClkVfOffset(myGPU, {gpu_offset})
nvmlDeviceSetMemClkVfOffset(myGPU, {mem_offset})
nvmlDeviceSetPowerManagementLimit(myGPU, {power_limit})
{fan_speed_script}
nvmlShutdown()
"""
            temp_script_path = "/tmp/temp_overclock.py"
            with open(temp_script_path, "w") as f:
                f.write(script_content)

            os.chmod(temp_script_path, 0o755)

            # Run the script using pkexec for root privileges
            result = subprocess.run(["pkexec", "python3", temp_script_path], capture_output=True, text=True)
            if result.returncode == 0:
                QMessageBox.information(self, "Success", "Overclock settings applied successfully")
            else:
                QMessageBox.critical(self, "Error", result.stderr)
        except Exception as e:
            QMessageBox.critical(self, "Error", str(e))

def main():
    app = QApplication(sys.argv)
    ex = OverclockApp()
    ex.show()
    sys.exit(app.exec())

if __name__ == '__main__':
    main()
