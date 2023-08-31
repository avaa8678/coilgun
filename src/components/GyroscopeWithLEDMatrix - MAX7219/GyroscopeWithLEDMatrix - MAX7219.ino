
#include <Wire.h>        // Include Wire Library for I2C
#include <LedControl.h>  // Include for 8X8 Dot Matrix
LedControl lc = LedControl(12, 10, 11, 1);

/**********************************************************************/
/******** BELOW ARE THE VALUE WE NEED TO TURN TO SET CENTER ***********/
/**********************************************************************/
const float RollCenter = -64.28;
const float PitchCenter = -0.88;
float step = 10;                   // there still some drifting, we need some space
const int tolerance = 50;         // Tolerance 0-10 = Ultra Fast - Loose accuracy but averages ok. For 11-100 is more acurrate but slower as you scale up. This is basically a delay on when you want to start
const int calibrateTimes = 1000;  // How may iteration you want to calibrate the gyrometer
/**********************************************************************/
/******************** END OF VALUE TUNNING ****************************/
/**********************************************************************/

const float PitchLow4 = PitchCenter - 4 * step;
const float PitchLow3 = PitchCenter - 3 * step;
const float PitchLow2 = PitchCenter - 2 * step;
const float PitchLow1 = PitchCenter - step;  // center is between High1 > angle_roll_output > Low1, so it is in the middle of Low1 and High1
const float PitchHigh1 = PitchCenter + step;
const float PitchHigh2 = PitchCenter + 2 * step;
const float PitchHigh3 = PitchCenter + 3 * step;
const float PitchHigh4 = PitchCenter + 4 * step;
const float RollLow4 = RollCenter - 4 * step;
const float RollLow3 = RollCenter - 3 * step;
const float RollLow2 = RollCenter - 2 * step;
const float RollLow1 = RollCenter - step;  // center is between High1 > angle_roll_output > Low1, so it is in the middle of Low1 and High1
const float RollHigh1 = RollCenter + step;
const float RollHigh2 = RollCenter + 2 * step;
const float RollHigh3 = RollCenter + 3 * step;
const float RollHigh4 = RollCenter + 4 * step;

int row, col, temp;                                                 // row, column for LED, temperurate received but not used
int displaycount = 0;                                               // Display counter
int gyro_x, gyro_y, gyro_z, angle_pitch_buffer, angle_roll_buffer;  // Variables for Gyroscope
long gyro_x_cal, gyro_y_cal, gyro_z_cal, acc_x, acc_y, acc_z, acc_total_vector;
float angle_roll_acc, angle_pitch_acc, angle_pitch, angle_roll, angle_pitch_output, angle_roll_output;
boolean set_gyro_angles;

void setup_mpu_6050_registers() {
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);  //Start communicating with the MPU-6050
  Wire.write(0x6B);              //Send the requested starting register
  Wire.write(0x00);              //Set the requested starting register
  Wire.endTransmission();        //End the transmission

  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);  //Start communicating with the MPU-6050
  Wire.write(0x1C);              //Send the requested starting register
  Wire.write(0x10);              //Set the requested starting register
  Wire.endTransmission();        //End the transmission

  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);  //Start communicating with the MPU-6050
  Wire.write(0x1B);              //Send the requested starting register
  Wire.write(0x08);              //Set the requested starting register
  Wire.endTransmission();        //End the transmission
}

void calibrate_mpu_6050() {
  //Read the raw acc and gyro data from the MPU-6050 1000+ times
  for (int cal_int = 0; cal_int < calibrateTimes; cal_int++) {
    read_mpu_6050_data();
    gyro_x_cal += gyro_x;  //Add the gyro x offset to the gyro_x_cal variable
    gyro_y_cal += gyro_y;  //Add the gyro y offset to the gyro_y_cal variable
    gyro_z_cal += gyro_z;  //Add the gyro z offset to the gyro_z_cal variable
    delay(1);              //Delay 3us to have 250Hz for-loop, i changed to 1us for loop of 2000
  }

  // Divide all results by calibrateTimes to get average offset
  gyro_x_cal /= calibrateTimes;
  gyro_y_cal /= calibrateTimes;
  gyro_z_cal /= calibrateTimes;
}

void read_mpu_6050_data() {
  //Read the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);  //Start communicating with the MPU-6050
  Wire.write(0x3B);              //Send the requested starting register
  Wire.endTransmission();        //End the transmission
  Wire.requestFrom(0x68, 14);    //Request 14 bytes from the MPU-6050

  //Wait until all the bytes are received
  while (Wire.available() < 14)
    ;

  //Following statements left shift 8 bits, then bitwise OR. Turns two 8-bit values into one 16-bit value
  acc_x = Wire.read() << 8 | Wire.read();
  acc_y = Wire.read() << 8 | Wire.read();
  acc_z = Wire.read() << 8 | Wire.read();
  temp = Wire.read() << 8 | Wire.read();
  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();
}

void setup() {
  lc.shutdown(0, false);  // the MAX72XX is in power-saving mode on startup, we have to do a wakeup call

  Wire.begin();                //Start I2C
  setup_mpu_6050_registers();  //Setup the registers of the MPU-6050
  calibrate_mpu_6050();        //calibrate mpu 6050

  Serial.begin(115200);  // Start Serial Monitor
}

void loop() {
  read_mpu_6050_data();  // Get data from MPU-6050

  //Subtract the offset values from the raw gyro values
  gyro_x -= gyro_x_cal;
  gyro_y -= gyro_y_cal;
  gyro_z -= gyro_z_cal;

  //Gyro angle calculations . Note 0.0000611 = 1 / (250Hz x 65.5)
  angle_pitch += gyro_x * 0.0000611;  //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += gyro_y * 0.0000611;   //Calculate the traveled roll angle and add this to the angle_roll variable

  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  angle_pitch += angle_roll * sin(gyro_z * 0.000001066);  //If the IMU has yawed transfer the roll angle to the pitch angle
  angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);  //If the IMU has yawed transfer the pitch angle to the roll angle

  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x * acc_x) + (acc_y * acc_y) + (acc_z * acc_z));  //Calculate the total accelerometer vector

  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_y / acc_total_vector) * 57.296;  //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x / acc_total_vector) * -57.296;  //Calculate the roll angle
  angle_pitch_acc -= 0.0;                                            //Accelerometer calibration value for pitch
  angle_roll_acc -= 0.0;                                             //Accelerometer calibration value for roll

  if (set_gyro_angles) {
    //If the IMU has been running
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;  //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;     //Correct the drift of the gyro roll angle with the accelerometer roll angle
  } else {
    //IMU has just started
    angle_pitch = angle_pitch_acc;  //Set the gyro pitch angle equal to the accelerometer pitch angle
    angle_roll = angle_roll_acc;    //Set the gyro roll angle equal to the accelerometer roll angle
    set_gyro_angles = true;         //Set the IMU started flag
  }

  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;  //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;     //Take 90% of the output roll value and add 10% of the raw roll value
  //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop

  displaycount = displaycount + 1;  // Increment the display counter

  if (displaycount > tolerance) {
    lc.clearDisplay(0);  // clear the 8X8 display

    //Get Col Position
    if (angle_roll_output < RollLow4) {
      row = 6;
    } else if ((angle_roll_output > RollLow3) && (angle_roll_output < RollLow2)) {
      row = 5;
    } else if ((angle_roll_output > RollLow2) && (angle_roll_output < RollLow1)) {
      row = 4;
    } else if ((angle_roll_output > RollLow1) && (angle_roll_output < RollHigh1)) {
      row = 3;
    } else if ((angle_roll_output > RollHigh1) && (angle_roll_output < RollHigh2)) {
      row = 2;
    } else if ((angle_roll_output > RollHigh2) && (angle_roll_output < RollHigh3)) {
      row = 1;
    } else if (angle_roll_output > RollHigh4) {
      row = 0;
    }

    //Get row Position
    if (angle_pitch_output < PitchLow4) {
      col = 6;
    } else if ((angle_pitch_output > PitchLow3) && (angle_pitch_output < PitchLow2)) {
      col = 5;
    } else if ((angle_pitch_output > PitchLow2) && (angle_pitch_output < PitchLow1)) {
      col = 4;
    } else if ((angle_pitch_output > PitchLow1) && (angle_pitch_output < PitchHigh1)) {
      col = 3;
    } else if ((angle_pitch_output > PitchHigh1) && (angle_pitch_output < PitchHigh2)) {
      col = 2;
    } else if ((angle_pitch_output > PitchHigh2) && (angle_pitch_output < PitchHigh3)) {
      col = 1;
    } else if (angle_pitch_output > PitchHigh4) {
      col = 0;
    }
    /**************** DEBUGGING *********************/

    Serial.print(" | Roll: ");
    Serial.print(angle_roll_output);
    Serial.print(" | Pitch: ");
    Serial.print(angle_pitch_output);
    Serial.print(" | col: ");
    Serial.print(col);
    Serial.print(" | row: ");
    Serial.println(row);
/*    
    Serial.print(" | RollLow4: ");
    Serial.print(RollLow4);
    Serial.print(" | RollLow3: ");
    Serial.print(RollLow3);
    Serial.print(" | RollLow2: ");
    Serial.print(RollLow2);
    Serial.print(" | RollLow1: ");
    Serial.print(RollLow1);
    Serial.print(" | RollHigh1: ");
    Serial.print(RollHigh1);
    Serial.print(" | RollHigh2: ");
    Serial.print(RollHigh2);
    Serial.print(" | RollHigh3: ");
    Serial.print(RollHigh1);
    Serial.print(" | RollHigh4: ");
    Serial.print(RollHigh4);
    Serial.print(" | PitchLow4: ");
    Serial.print(PitchLow4);
    Serial.print(" | PitchLow3: ");
    Serial.print(PitchLow3);
    Serial.print(" | PitchLow2: ");
    Serial.print(PitchLow2);
    Serial.print(" | PitchLow1: ");
    Serial.print(PitchLow1);
    Serial.print(" | PitchHigh1: ");
    Serial.print(PitchHigh1);
    Serial.print(" | PitchHigh2: ");
    Serial.print(PitchHigh2);
    Serial.print(" | PitchHigh3: ");
    Serial.print(PitchHigh3);
    Serial.print(" | PitchHigh4: ");
    Serial.println(PitchHigh4);
*/
    lc.setIntensity(0, 8);  // set the brightness to a medium values

    //Light Up the 4 LED quadrant of the position of row & col (x & y axis)
    lc.setLed(0, col, row, true);
    lc.setLed(0, col + 1, row, true);
    lc.setLed(0, col, row + 1, true);
    lc.setLed(0, col + 1, row + 1, true);

    if ((row == 3) && (col == 3)) {
      lc.setIntensity(0, 16);
      lc.setRow(0, 0, B11111111);
      for (int i = 1; i < 7; i++) {
        lc.setRow(0, i, B10000001);
      }
      lc.setRow(0, 7, B11111111);
    } else {
      lc.setIntensity(0, 8);
      lc.setRow(0, 3, B00011000);
      lc.setRow(0, 4, B00011000);
    }
    displaycount = 0;
  }
}
