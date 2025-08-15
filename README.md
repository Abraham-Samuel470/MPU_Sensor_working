# MPU6050 + Madgwick Filter + OLED Display on ESP32

This project demonstrates how to use the **MPU6050** IMU with an **ESP32**, process orientation data using the **Madgwick filter** (without a magnetometer), and display the results on an **SSD1306 OLED**.  
It also calculates a simple **tremor index** from the gyroscope readings, which can be useful for motion analysis projects like tremor detection.

## 📷 Features
- **MPU6050** IMU (Accelerometer + Gyroscope) for motion sensing.
- **Madgwick filter** for accurate roll, pitch, and yaw estimation.
- **SSD1306 OLED** display to show live orientation data.
- CSV-friendly serial output for future ML integration.
- Tremor index calculation for motion/tremor detection.
- Runs on **ESP32** with a stable **100 Hz update rate**.

## 🛠 Hardware Required
- ESP32 Dev Module
- MPU6050 IMU sensor
- SSD1306 OLED display (I2C, 128×64)
- Jumper wires

## 🔌 Wiring
| ESP32 Pin | MPU6050 | OLED |
|-----------|---------|------|
| 3.3V      | VCC     | VCC  |
| GND       | GND     | GND  |
| GPIO21    | SDA     | SDA  |
| GPIO22    | SCL     | SCL  |

> Both MPU6050 and OLED share the **same I2C bus**.

## 📦 Libraries Used
Install these libraries from Arduino IDE Library Manager:
- **[MPU6050_light](https://github.com/rfetick/MPU6050_light)**
- **[Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)**
- **[Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)**
- **[MadgwickAHRS](https://github.com/arduino-libraries/MadgwickAHRS)**

## ⚙️ How It Works
1. Initializes the MPU6050 and calibrates accelerometer & gyroscope.
2. Reads raw accel & gyro data.
3. Passes readings into the Madgwick filter to estimate roll, pitch, and yaw.
4. Applies exponential smoothing to reduce noise.
5. Displays orientation and tremor index on OLED.
6. Prints CSV-formatted data over Serial at **115200 baud**.

## 📊 Sample Serial Output
ROLL,1.23,PITCH,-0.56,YAW,45.67
ROLL,1.21,PITCH,-0.54,YAW,45.69
## 📺 OLED Display Example
Madgwick (no mag)
Roll : 1.2
Pitch: -0.5
Yaw : 45.7
Tremor idx: 0.8
## 🚀 Getting Started
1. Connect the components as per wiring diagram.
2. Install all required libraries.
3. Upload the code to your ESP32.
4. Open Serial Monitor at `115200` baud to see CSV output.
5. Observe live roll, pitch, yaw, and tremor readings on OLED.

## 📌 Notes
- Yaw will **drift** over time since there's no magnetometer.
- You can tweak the `alpha` value to smooth or speed up response.
- Tremor index is a simple sum of gyro absolute values and can be improved for specific applications.

## 📜 License
This project is open-source and licensed under the MIT License.
