/*
Gyroscope + MAX7219

Hardware Setup
==================================================================================================
Components: arduino nano, MPU 6050

Nano 5v -> 6050 VCC
Nano GND -> 6050 GND
Nano A5 -> 6050 SCL
Nano A4 -> 6050 SDA 

Nano 5v -> MAX7219 VCC
Nano GND -> MAX7219 GND
Nano D10 -> MAX7219 CLK
Nano D11 -> MAX7219 CS
Nano D12 -> MAX7219 DIN
*/

#include <Wire.h>                  // Include Wire Library for I2C
#include <LedControl.h>            // Include for 8X8 Dot Matrix
#include <Kalman.h>                // Include Kalman filter library - KalmanFilter library by TKJElectronics
#define GYROSCOPE_SENSITIVITY 131  // used by Kalman Filter
LedControl lc = LedControl(12, 10, 11, 1);

/**********************************************************************/
/******** BELOW ARE THE VALUE WE NEED TO TURN TO SET CENTER ***********/
/**********************************************************************/
int filter = 2;  // 1 = Complimentary Filter, 2 = Kalman Filter (default), Any other value = No Filter
// Initial compliment for roll and pitch
const int tolerance = 10;  // Tolerance 0-10 = Ultra Fast - Loose accuracy but averages ok. For 11-100 is more acurrate but slower as you scale up. This is basically a delay on when you want to start

/**********************************************************************/
/********  Less important tuning parameters ***************************/
/**********************************************************************/
const float rollCenterCali = 1;
const float pitchCenterCali = 0;
float dt = 0.01;                  // Time interval in seconds for kalman
float step = 1;                   // this determine the LED moving steps
const int calibrateTimes = 1000;  // How may iteration you want to calibrate the gyrometer
const float rollCenter = 0;       // this determine the LED moving degree on roll when center
const float pitchCenter = 0;      // this determine the LED moving degree on roll when center
/**********************************************************************/
/******************** END OF VALUE TUNNING ****************************/
/**********************************************************************/

// Create the Kalman instances
Kalman kalmanX;
Kalman kalmanY;

const float pitchLow4 = pitchCenter - 4 * step;
const float pitchLow3 = pitchCenter - 3 * step;
const float pitchLow2 = pitchCenter - 2 * step;
const float pitchLow1 = pitchCenter - step;  // center is between High1 > angleRollOutput > Low1, so it is in the middle of Low1 and High1
const float pitchHigh1 = pitchCenter + step;
const float pitchHigh2 = pitchCenter + 2 * step;
const float pitchHigh3 = pitchCenter + 3 * step;
const float pitchHigh4 = pitchCenter + 4 * step;
const float rollLow4 = rollCenter - 4 * step;
const float rollLow3 = rollCenter - 3 * step;
const float rollLow2 = rollCenter - 2 * step;
const float rollLow1 = rollCenter - step;  // center is between High1 > angleRollOutput > Low1, so it is in the middle of Low1 and High1
const float rollHigh1 = rollCenter + step;
const float rollHigh2 = rollCenter + 2 * step;
const float rollHigh3 = rollCenter + 3 * step;
const float rollHigh4 = rollCenter + 4 * step;

int row, col, temp;                                          // row, column for LED, temperurate received but not used
int displayCount = 0;                                        // Display counter
int gyroX, gyroY, gyroZ, anglePitchBuffer, angleRollBuffer;  // Variables for Gyroscope
long gyroXCal, gyroYCal, gyroZCal, accX, accY, accZ, accTotalVector;
float angleRollAcc, anglePitchAcc, anglePitch, angleRoll, anglePitchOutput, angleRollOutput;
boolean setGyroAngles;

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
}

void loop() {
  readMpu6050Data();  // Get data from MPU-6050

  // Subtract the offset values from the raw gyro values
  gyroX -= gyroXCal;
  gyroY -= gyroYCal;
  gyroZ -= gyroZCal;

  // Gyro angle calculations . Note 0.0000611 = 1 / (250Hz x 65.5)
  anglePitch += gyroX * 0.0000611;  // Calculate the traveled pitch angle and add this to the anglePitch variable
  angleRoll += gyroY * 0.0000611;   // Calculate the traveled roll angle and add this to the angleRoll variable

  // 0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  anglePitch += angleRoll * sin(gyroZ * 0.000001066);  // If the IMU has yawed transfer the roll angle to the pitch angle
  angleRoll -= anglePitch * sin(gyroZ * 0.000001066);  // If the IMU has yawed transfer the pitch angle to the roll angle

  // Accelerometer angle calculations
  accTotalVector = sqrt((accX * accX) + (accY * accY) + (accZ * accZ));  // Calculate the total accelerometer vector

  // 57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  anglePitchAcc = asin((float)accY / accTotalVector) * RAD_TO_DEG;  // Calculate the pitch angle
  angleRollAcc = asin((float)accX / accTotalVector) * -RAD_TO_DEG;  // Calculate the roll angle

  anglePitchAcc -= pitchCenterCali;  // Accelerometer calibration value for pitch
  angleRollAcc -= rollCenterCali;    // Accelerometer calibration value for roll

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

  if (filter == 1) {
    // To dampen the pitch and roll angles a complementary filter is used
    anglePitchOutput = anglePitchOutput * 0.9 + anglePitch * 0.1;  // Take 90% of the output pitch value and add 10% of the raw pitch value
    angleRollOutput = angleRollOutput * 0.9 + angleRoll * 0.1;     // Take 90% of the output roll value and add 10% of the raw roll value
  } else if (filter == 2) {
    //  To dampen the pitch and roll angles a kalman filter is used
    anglePitchOutput = kalmanX.getAngle(anglePitchAcc, gyroX / GYROSCOPE_SENSITIVITY, dt);  // Calculate the pitch angle using a Kalman filter
    angleRollOutput = kalmanY.getAngle(angleRollAcc, gyroY / GYROSCOPE_SENSITIVITY, dt);    // Calculate the roll angle using a Kalman filter
  } else {
    anglePitchOutput = anglePitch;
    angleRollOutput = angleRoll;
  }

  // Wait until the loopTimer reaches 4000us (250Hz) before starting the next loop
  displayCount = displayCount + 1;  // Increment the display counter

  if (displayCount > tolerance) {
    lc.clearDisplay(0);  // Clear the 8X8 display

    // Get Col Position
    if (angleRollOutput < rollLow4) {
      row = 6;
    } else if ((angleRollOutput > rollLow3) && (angleRollOutput < rollLow2)) {
      row = 5;
    } else if ((angleRollOutput > rollLow2) && (angleRollOutput < rollLow1)) {
      row = 4;
    } else if ((angleRollOutput > rollLow1) && (angleRollOutput < rollHigh1)) {
      row = 3;
    } else if ((angleRollOutput > rollHigh1) && (angleRollOutput < rollHigh2)) {
      row = 2;
    } else if ((angleRollOutput > rollHigh2) && (angleRollOutput < rollHigh3)) {
      row = 1;
    } else if (angleRollOutput > rollHigh4) {
      row = 0;
    }

    // Get Row Position
    if (anglePitchOutput < pitchLow4) {
      col = 6;
    } else if ((anglePitchOutput > pitchLow3) && (anglePitchOutput < pitchLow2)) {
      col = 5;
    } else if ((anglePitchOutput > pitchLow2) && (anglePitchOutput < pitchLow1)) {
      col = 4;
    } else if ((anglePitchOutput > pitchLow1) && (anglePitchOutput < pitchHigh1)) {
      col = 3;
    } else if ((anglePitchOutput > pitchHigh1) && (anglePitchOutput < pitchHigh2)) {
      col = 2;
    } else if ((anglePitchOutput > pitchHigh2) && (anglePitchOutput < pitchHigh3)) {
      col = 1;
    } else if (anglePitchOutput > pitchHigh4) {
      col = 0;
    }
    /**************** DEBUGGING *********************/
    //Serial.print(" | angleRollOutputComp:");
    //Serial.print(angleRollOutputComp);
    //Serial.print(",");
    //Serial.print(" | anglePitchOutputComp:");
    //Serial.print(anglePitchOutputComp);

    Serial.println("Min:-50,Max:50");
    Serial.print(" | Roll:");
    Serial.print(angleRollOutput);
    Serial.print(",");
    Serial.print(" | Pitch:");
    Serial.println(anglePitchOutput);
    //Serial.print(" | Col:");
    //Serial.print(col);
    //Serial.print(" | Row:");
    //Serial.println(row);

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
