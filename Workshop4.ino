#include <MadgwickAHRS.h>
#include <Adafruit_NeoPixel.h>
#include <RGBConverter.h>

#define PIN       6
#define NUMPIXELS 14
#include <Wire.h> // This library allows you to communicate with I2C devices.
#define DELAYVAL 500

Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float accelScale, gyroScale;

// NeoPixel configuration
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

RGBConverter rgbConverter;
double h = 1;
double s = 1;
double v = 1;
byte rgb[3];

// Status Motion Lamp
// State 0 -> Select Hue - Pitch
// State 1 -> Select Saturation - Roll
// State 2 -> Select Value - Yaw
// State 3 -> Fix color
volatile int statusLamp = 0;

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

void setup() {

  Serial.begin(9600);
  //Wire.begin(1,2);

  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pixels.begin();
}
void loop() {

  int aix, aiy, aiz; //accelerometer
  int gix, giy, giz;
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch, yaw;
  static unsigned long microsNow;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers

  // check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    // read raw data from CurieIMU
    aix = Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    aiy = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    aiz = Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)

    gix = Wire.read() << 8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
    giy = Wire.read() << 8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
    giz = Wire.read() << 8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(aix);
    ay = convertRawAcceleration(aiy);
    az = convertRawAcceleration(aiz);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // print the heading, pitch and roll
    roll = filter.getRoll();
    pitch = filter.getPitch();
    yaw = filter.getYaw();

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;

    // Only if change Hue, Saturation or Value
    if (statusLamp < 3)
    {
      // pitch only -90º to 90º = 180º
      // State 0 -> select Hue
      if (pitch >= -90 && pitch <= 90 && statusLamp == 0)
      {
        // Transform angle
        pitch = pitch + 90;
        // Obtains color cordinates from angles
        h = pitch / 18.0;
      }

      // Angles restrictions
      // roll only -90º to 90º = 180º
      // State 1 -> select Saturation
      if (roll >= -90 && roll <= 90 && statusLamp == 1)
      {
        // Transform angle
        roll = roll + 90;
        // Obtains color cordinates from angles
        s = roll / 180.0;
      }

      // State 2 -> select Value
      if (statusLamp == 2)
      {
        // yaw 0º to 360º
        v = yaw / 36.0;
      }

      // Convert to rgb
      rgbConverter.hsvToRgb(h, s, v, rgb);

      // Change color of the pixels
      for (int px = 0; px < NUMPIXELS; px++)
      {
        Serial.println(rgb[0]);
        Serial.print("\n");
        pixels.setPixelColor(px, pixels.Color(rgb[0], rgb[1], rgb[2]));
        pixels.show();
      }
    }
  }
}

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767

  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767

  float g = (gRaw * 250.0) / 32768.0;
  return g;
}
