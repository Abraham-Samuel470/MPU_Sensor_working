#include <Wire.h>
#include <MPU6050_light.h>      // rfetick
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MadgwickAHRS.h>       // Arduino Madgwick

// --- Display ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// --- IMU ---
MPU6050 mpu(Wire);

// --- Madgwick filter ---
Madgwick filter;

// Sampling settings
const float SAMPLE_FREQ_HZ = 100.0;   // target loop rate
const uint32_t SAMPLE_PERIOD_US = 1000000.0 / SAMPLE_FREQ_HZ;

// Optional extra smoothing on Euler angles (EMA)
float alpha = 0.2; // 0..1 (lower = smoother)
float roll_s = 0, pitch_s = 0, yaw_s = 0;

// Timing
uint32_t lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  // I2C on ESP32 defaults
  Wire.begin(21, 22);

  // --- OLED init ---
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // --- IMU init ---
  byte status = mpu.begin();
  Serial.print("MPU6050 status: "); Serial.println(status);
  while (status != 0) { delay(100); }  // stop if not connected

  Serial.println("Keep the sensor still, calibrating...");
  delay(1000);
  mpu.calcOffsets(); // calibrates accel & gyro
  Serial.println("Calibration done.");

  // Optional: tweak internal complementary filter weight
  // mpu.setFilterGyroCoef(0.95f);  // (default ~0.98)

  // --- Madgwick init ---
  filter.begin(SAMPLE_FREQ_HZ);

  // Splash
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("MPU + Madgwick (no mag)");
  display.println("Sampling @ 100 Hz");
  display.display();
  delay(1000);

  lastUpdate = micros();
}

void loop() {
  // Keep loop timing steady for filter accuracy
  uint32_t now = micros();
  if ((now - lastUpdate) < SAMPLE_PERIOD_US) return;
  lastUpdate += SAMPLE_PERIOD_US;

  // Update raw sensor readings
  mpu.update();

  // Get accel (g) and gyro (deg/s) from rfetick lib
  float ax = mpu.getAccX();
  float ay = mpu.getAccY();
  float az = mpu.getAccZ();
  float gx_dps = mpu.getGyroX();
  float gy_dps = mpu.getGyroY();
  float gz_dps = mpu.getGyroZ();

  // Convert gyro to rad/s for Madgwick
  const float DEG2RAD = 0.017453292519943295f;
  float gx = gx_dps * DEG2RAD;
  float gy = gy_dps * DEG2RAD;
  float gz = gz_dps * DEG2RAD;

  // Madgwick IMU update (no magnetometer)
  // Some lib variants use updateIMU(gx, gy, gz, ax, ay, az)
  // Arduino Madgwick supports updateIMU like this:
  filter.updateIMU(gx, gy, gz, ax, ay, az);

  // Get Euler angles in degrees
  float roll  = filter.getRoll();   // rotation around X
  float pitch = filter.getPitch();  // rotation around Y
  float yaw   = filter.getYaw();    // rotation around Z (will drift slowly w/o mag)

  // Extra exponential smoothing (optional)
  roll_s  = alpha * roll  + (1 - alpha) * roll_s;
  pitch_s = alpha * pitch + (1 - alpha) * pitch_s;
  yaw_s   = alpha * yaw   + (1 - alpha) * yaw_s;

  // ---- Serial (CSV friendly for future ML) ----
  Serial.print("ROLL,");  Serial.print(roll_s, 2);  Serial.print(",");
  Serial.print("PITCH,"); Serial.print(pitch_s, 2); Serial.print(",");
  Serial.print("YAW,");   Serial.println(yaw_s, 2);

  // ---- OLED ----
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Madgwick (no mag)");
  display.print("Roll : ");  display.println(roll_s, 1);
  display.print("Pitch: ");  display.println(pitch_s, 1);
  display.print("Yaw  : ");  display.println(yaw_s, 1);

  // Bonus: show tremor index (quick visual cue)
  // Simple measure: absolute gyro magnitude (deg/s)
  float tremorIdx = fabs(gx_dps) + fabs(gy_dps) + fabs(gz_dps);
  display.print("Tremor idx: ");
  display.println(tremorIdx, 1);

  display.display();
}
