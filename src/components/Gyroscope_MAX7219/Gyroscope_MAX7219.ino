/*
Electronic Leveler
avaleichen@gmail.com

Hardware Setup
==================================================================================================
Components: arduino nano, MPU6050 Gyroscope, MAX7219 LED Matrix

Nano 5v   <->   6050 VCC
Nano GND  <->   6050 GND
Nano A5   <->   6050 SCL
Nano A4   <->   6050 SDA 

Nano 5v   <->   MAX7219 VCC
Nano GND  <->   MAX7219 GND
Nano D10  <->   MAX7219 CLK
Nano D11  <->   MAX7219 CS
Nano D12  <->   MAX7219 DIN
==================================================================================================
*/

#include <Wire.h>                  // Include Wire Library for I2C
#include <LedControl.h>            // Include for 8X8 Dot Matrix
#include <Kalman.h>                // Include Kalman filter library - KalmanFilter library by TKJElectronics
#include <MadgwickAHRS.h>          // Include Madgwick filter library - MadgwickAHRS algorithm
#define GYROSCOPE_SENSITIVITY 131  // used by Kalman Filter
LedControl lc = LedControl(12, 10, 11, 1);

/**********************************************************************/
/******** BELOW ARE THE VALUE WE NEED TO TUNE TO SET ******************/
/**********************************************************************/
int filter = 0;     // 0 or any value = Kalman Filter (default), 1 = Raw Data No filter, 2 = Complimentary Filter, 3 = Madgwick Filter
int tolerance = 5;  // Tolerance 0-10 = Ultra Fast - Loose accuracy but averages ok. For 11-100 is more acurrate but slower as you scale up. This is basically a delay on when you want to start
float step = 5;     // this determine the LED moving degrees - 1 means 1 degree move the LED up
/**********************************************************************/
/********  Less important tuning parameters ***************************/
/**********************************************************************/
const float pitchCenterCali = 0;  // debugging purpose for uneven surface
const float rollCenterCali = 0;   // debugging purpose for uneven surface
const int calibrateTimes = 1000;  // How may iteration you want to calibrate the gyrometer
const float dt = 0.01;            // Time interval in seconds for kalman
/**********************************************************************/
/******************** END OF VALUE TUNNING ****************************/
/**********************************************************************/
const float LEDCenter = 0;  // this determine the LED moving degree on roll when center, default better be 0, use variable just in case we need to put the 6050 in different direction
const float Low4 = LEDCenter - 4 * step;
const float Low3 = LEDCenter - 3 * step;
const float Low2 = LEDCenter - 2 * step;
const float Low1 = LEDCenter - step;  // center is between High1 > angleRollOutput > Low1, so it is in the middle of Low1 and High1
const float High1 = LEDCenter + step;
const float High2 = LEDCenter + 2 * step;
const float High3 = LEDCenter + 3 * step;
const float High4 = LEDCenter + 4 * step;

int row, col, temp;                                          // row, column for LED, temperurate received but not used
int displayCount = 0;                                        // Display counter
int gyroX, gyroY, gyroZ, anglePitchBuffer, angleRollBuffer;  // Variables for Gyroscope
long gyroXCal, gyroYCal, gyroZCal, accX, accY, accZ, accTotalVector;
float angleRollAcc, anglePitchAcc, anglePitch, angleRoll, anglePitchOutput, angleRollOutput, angleRollOutputComp, anglePitchOutputComp;
boolean setGyroAngles;
Kalman kalmanX, kalmanY;  // Create the Kalman instances
Madgwick madgwickFilter;  // Create a Madgwick filter object

void setupMpu6050Registers() {
  // Activate the MPU-6050
  Wire.beginTransmission(0x68);  // Start communicating with the MPU-6050
  Wire.write(0x6B);              // Send the requested starting register
  Wire.write(0x00);              // Set the requested starting register
  Wire.endTransmission();        // End the transmission

  // Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);  // Start communicating with the MPU-6050
  Wire.write(0x1C);              // Send the requested starting register
  Wire.write(0x10);              // Set the requested starting register
  Wire.endTransmission();        // End the transmission

  // Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);  // Start communicating with the MPU-6050
  Wire.write(0x1B);              // Send the requested starting register
  Wire.write(0x08);              // Set the requested starting register
  Wire.endTransmission();        // End the transmission
}

void calibrateMpu6050() {
  // Read the raw acc and gyro data from the MPU-6050 1000+ times
  for (int calInt = 0; calInt < calibrateTimes; calInt++) {
    readMpu6050Data();
    gyroXCal += gyroX;  // Add the gyro x offset to the gyroXCal variable
    gyroYCal += gyroY;  // Add the gyro y offset to the gyroYCal variable
    gyroZCal += gyroZ;  // Add the gyro z offset to the gyroZCal variable
    delay(3);           // Delay 3us to have 250Hz for-loop, I changed to 1us for loop of 2000
  }

  // Divide all results by calibrateTimes to get average offset
  gyroXCal /= calibrateTimes;
  gyroYCal /= calibrateTimes;
  gyroZCal /= calibrateTimes;
}

void readMpu6050Data() {
  // Read the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);  // Start communicating with the MPU-6050
  Wire.write(0x3B);              // Send the requested starting register
  Wire.endTransmission();        // End the transmission
  Wire.requestFrom(0x68, 14);    // Request 14 bytes from the MPU-6050

  // Wait until all the bytes are received
  while (Wire.available() < 14)
    ;

  // Following statements left shift 8 bits, then bitwise OR. Turns two 8-bit values into one 16-bit value
  accX = Wire.read() << 8 | Wire.read();
  accY = Wire.read() << 8 | Wire.read();
  accZ = Wire.read() << 8 | Wire.read();
  temp = Wire.read() << 8 | Wire.read();
  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();
}

void setup() {
  lc.shutdown(0, false);  // The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
  lc.setIntensity(0, 8);  // Set the brightness to a medium value

  Wire.begin();             // Start I2C
  setupMpu6050Registers();  // Setup the registers of the MPU-6050
  calibrateMpu6050();       // Calibrate MPU-6050

  Serial.begin(115200);  // Start Serial Monitor
  madgwickFilter.begin(25);
}

void loop() {
  readMpu6050Data();  // Get data from MPU-6050

  // Subtract the offset values from the raw gyro values
  gyroX -= gyroXCal;
  gyroY -= gyroYCal;
  gyroZ -= gyroZCal;
  // Gyro angle calculations . Note 0.0000611 = 1 / (250Hz x 65.5)
  anglePitch += gyroX * 0.0000610687;  // Calculate the traveled pitch angle and add this to the anglePitch variable
  angleRoll += gyroY * 0.0000610687;   // Calculate the traveled roll angle and add this to the angleRoll variable
  // 0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  anglePitch += angleRoll * sin(gyroZ * (0.0000610687 * DEG_TO_RAD));  // If the IMU has yawed transfer the roll angle to the pitch angle
  angleRoll -= anglePitch * sin(gyroZ * (0.0000610687 * DEG_TO_RAD));  // If the IMU has yawed transfer the pitch angle to the roll angle
  // Accelerometer angle calculations
  accTotalVector = sqrt((accX * accX) + (accY * accY) + (accZ * accZ));  // Calculate the total accelerometer vector
  // 57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  anglePitchAcc = asin((float)accY / accTotalVector) * RAD_TO_DEG;  // Calculate the pitch angle
  angleRollAcc = asin((float)accX / accTotalVector) * -RAD_TO_DEG;  // Calculate the roll angle
  anglePitchAcc -= pitchCenterCali;                                 // Accelerometer calibration value for pitch
  angleRollAcc -= rollCenterCali;                                   // Accelerometer calibration value for roll

  if (filter == 1) {
    // NO filter, just raw pitch and roll
    angleRollOutput = angleRoll;
    anglePitchOutput = anglePitch;
  } else if (filter == 2) {
    // drifting correction
    if (setGyroAngles) {
      // If the IMU has been running
      anglePitch = anglePitch * 0.9996 + anglePitchAcc * 0.0004;  // Correct the drift of the gyro pitch angle with the accelerometer pitch angle
      angleRoll = angleRoll * 0.9996 + angleRollAcc * 0.0004;     // Correct the drift of the gyro roll angle with the accelerometer roll angle
    } else {
      // IMU has just started
      anglePitch = anglePitchAcc;  // Set the gyro pitch angle equal to the accelerometer pitch angle
      angleRoll = angleRollAcc;    // Set the gyro roll angle equal to the accelerometer roll angle
      setGyroAngles = true;        // Set the IMU started flag
    }
    // To dampen the pitch and roll angles a complementary filter is used
    anglePitchOutput = anglePitchOutput * 0.5 + anglePitch * 0.5;  // Take 90% of the output pitch value and add 10% of the raw pitch value
    angleRollOutput = angleRollOutput * 0.5 + angleRoll * 0.5;     // Take 90% of the output roll value and add 10% of the raw roll value
  } else if (filter == 3) {
    // Gyro since we are using 250 degrees/seconds range -250 maps to a raw value of -32768 +250 maps to a raw value of 32767, so *250/32768 = *0.00763
    // since we are using 2 g range -2 g maps to a raw value of -32768 +2 g maps to a raw value of 32767, so *2/32768
    madgwickFilter.updateIMU(gyroX * 0.00762939453125, gyroY * 0.00762939453125, gyroZ * 0.00762939453125, accX * 0.00006103515625, accY * 0.00006103515625, accZ * 0.00006103515625);
    // Get the roll and pitch angles from the Madgwick filter, depends your moving direction, roll / pitch may change. In our case it is better consistent with kalman and complementary filter above
    angleRollOutput = madgwickFilter.getPitch();
    anglePitchOutput = madgwickFilter.getRoll();
  } else {
    // To dampen the pitch and roll angles a Kalman filter is used
    anglePitchOutput = kalmanX.getAngle(anglePitchAcc, gyroX / GYROSCOPE_SENSITIVITY, dt);  // Calculate the pitch angle using a Kalman filter
    angleRollOutput = kalmanY.getAngle(angleRollAcc, gyroY / GYROSCOPE_SENSITIVITY, dt);    // Calculate the roll angle using a Kalman filter
  }

  display(angleRollOutput, anglePitchOutput);
}

void display(float angleRollOutput, float anglePitchOutput) {
  // Wait until the loopTimer reaches 4000us (250Hz) before starting the next loop
  displayCount = displayCount + 1;  // Increment the display counter

  if (displayCount > tolerance) {
    lc.clearDisplay(0);  // Clear the 8X8 display

    // Get Col Position
    if (angleRollOutput < Low4) {
      row = 6;
    } else if ((angleRollOutput > Low3) && (angleRollOutput < Low2)) {
      row = 5;
    } else if ((angleRollOutput > Low2) && (angleRollOutput < Low1)) {
      row = 4;
    } else if ((angleRollOutput > Low1) && (angleRollOutput < High1)) {
      row = 3;
    } else if ((angleRollOutput > High1) && (angleRollOutput < High2)) {
      row = 2;
    } else if ((angleRollOutput > High2) && (angleRollOutput < High3)) {
      row = 1;
    } else if (angleRollOutput > High4) {
      row = 0;
    }

    // Get Row Position
    if (anglePitchOutput < Low4) {
      col = 6;
    } else if ((anglePitchOutput > Low3) && (anglePitchOutput < Low2)) {
      col = 5;
    } else if ((anglePitchOutput > Low2) && (anglePitchOutput < Low1)) {
      col = 4;
    } else if ((anglePitchOutput > Low1) && (anglePitchOutput < High1)) {
      col = 3;
    } else if ((anglePitchOutput > High1) && (anglePitchOutput < High2)) {
      col = 2;
    } else if ((anglePitchOutput > High2) && (anglePitchOutput < High3)) {
      col = 1;
    } else if (anglePitchOutput > High4) {
      col = 0;
    }

    Serial.print("Min:-10,Max:10");
    Serial.print(" | Roll:");
    Serial.print(angleRollOutput);
    Serial.print(",");
    Serial.print(" | Pitch:");
    Serial.print(anglePitchOutput);
    Serial.println();

    // Light up the 4 LED quadrant of the position of row & col (x & y axis)
    lc.setLed(0, col, row, true);
    lc.setLed(0, col + 1, row, true);
    lc.setLed(0, col, row + 1, true);
    lc.setLed(0, col + 1, row + 1, true);

    if ((row == 3) && (col == 3)) {
      lc.setRow(0, 0, B11111111);
      for (int i = 1; i < 7; i++) {
        lc.setLed(0, i, 0, true);
        lc.setLed(0, i, 7, true);
      }
      lc.setRow(0, 7, B11111111);
    } else {
      lc.setLed(0, 3, 3, true);
      lc.setLed(0, 3, 4, true);
      lc.setLed(0, 4, 3, true);
      lc.setLed(0, 4, 4, true);
    }

    displayCount = 0;
  }
}
