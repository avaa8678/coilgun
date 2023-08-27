/*
Gyroscope test

Hardware Setup
==================================================================================================
Components: arduino nano, MPU 6050

Nano 5v -> 6050 VCC
Nano GND -> 6050 GND
Nano A5 -> 6050 SCL
Nano A4 -> 6050 SDA 

TODO :
@Ava Connect LED matrix 788 BS to nano

*/

/*
Begin Code for gyroscope
*/
#include <Wire.h>

const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int minVal = 265;
int maxVal = 402;

double x;
double y;
double z;

/*
End Code for gyroscope
*/

/*
Begin Code for LED Matrix 788BS
*/

#define ROW_1 2
#define ROW_2 3
#define ROW_3 4
#define ROW_4 5
#define ROW_5 6
#define ROW_6 7
#define ROW_7 8
#define ROW_8 9

#define COL_1 10
#define COL_2 11
#define COL_3 12
#define COL_4 13
#define COL_5 A0
#define COL_6 A1
#define COL_7 A2
#define COL_8 A3

const int centerX = 230;
const int centerY = 90;
const int range = 5;

const byte rows[] = {
  ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8
};

byte r1c1[] = { 0b11000000, 0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r1c2[] = { 0b01100000, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r1c3[] = { 0b00110000, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r1c4[] = { 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r1c5[] = { 0b00001100, 0b00001100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r1c6[] = { 0b00000110, 0b00000110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r1c7[] = { 0b00000011, 0b00000011, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };

byte r2c1[] = { 0b00000000, 0b11000000, 0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r2c2[] = { 0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r2c3[] = { 0b00000000, 0b00110000, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r2c4[] = { 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r2c5[] = { 0b00000000, 0b00001100, 0b00001100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r2c6[] = { 0b00000000, 0b00000110, 0b00000110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r2c7[] = { 0b00000000, 0b00000011, 0b00000011, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };

byte r3c1[] = { 0b00000000, 0b00000000, 0b11000000, 0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r3c2[] = { 0b00000000, 0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r3c3[] = { 0b00000000, 0b00000000, 0b00110000, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r3c4[] = { 0b00000000, 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r3c5[] = { 0b00000000, 0b00000000, 0b00001100, 0b00001100, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r3c6[] = { 0b00000000, 0b00000000, 0b00000110, 0b00000110, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };
byte r3c7[] = { 0b00000000, 0b00000000, 0b00000011, 0b00000011, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };

byte r4c1[] = { 0b00000000, 0b00000000, 0b00000000, 0b11000000, 0b11000000, 0b00000000, 0b00000000, 0b00000000 };
byte r4c2[] = { 0b00000000, 0b00000000, 0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000, 0b00000000 };
byte r4c3[] = { 0b00000000, 0b00000000, 0b00000000, 0b00110000, 0b00110000, 0b00000000, 0b00000000, 0b00000000 };
byte r4c4[] = { 0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000 };
byte r4c5[] = { 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00001100, 0b00000000, 0b00000000, 0b00000000 };
byte r4c6[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000110, 0b00000110, 0b00000000, 0b00000000, 0b00000000 };
byte r4c7[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000011, 0b00000000, 0b00000000, 0b00000000 };

byte r5c1[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11000000, 0b11000000, 0b00000000, 0b00000000 };
byte r5c2[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000 };
byte r5c3[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00110000, 0b00110000, 0b00000000, 0b00000000 };
byte r5c4[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000 };
byte r5c5[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00001100, 0b00000000, 0b00000000 };
byte r5c6[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000110, 0b00000110, 0b00000000, 0b00000000 };
byte r5c7[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000011, 0b00000000, 0b00000000 };

byte r6c1[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11000000, 0b11000000, 0b00000000 };
byte r6c2[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01100000, 0b01100000, 0b00000000 };
byte r6c3[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00110000, 0b00110000, 0b00000000 };
byte r6c4[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00011000, 0b00000000 };
byte r6c5[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00001100, 0b00000000 };
byte r6c6[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000110, 0b00000110, 0b00000000 };
byte r6c7[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000011, 0b00000000 };

byte r7c1[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11000000, 0b11000000 };
byte r7c2[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01100000, 0b01100000 };
byte r7c3[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00110000, 0b00110000 };
byte r7c4[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00011000 };
byte r7c5[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001100, 0b00001100 };
byte r7c6[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000110, 0b00000110 };
byte r7c7[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000011 };

byte LEDGrid[7][7][8];

/*
byte LEDGrid[7][7][8] = {     
    {r1c1, r1c2, r1c3, r1c4, r1c5, r1c6, r1c7},
    {r2c1, r2c2, r2c3, r2c4, r2c5, r2c6, r2c7},
    {r3c1, r3c2, r3c3, r3c4, r3c5, r3c6, r3c7},
    {r4c1, r4c2, r4c3, r4c4, r4c5, r4c6, r4c7},
    {r5c1, r5c2, r5c3, r5c4, r5c5, r5c6, r5c7},
    {r6c1, r6c2, r6c3, r6c4, r6c5, r6c6, r6c7},
    {r7c1, r7c2, r7c3, r7c4, r7c5, r7c6, r7c7} 
};
*/

/*byte core[] = {  B00000000,
                 B00000000,
                 B01110111,
                 B11111111,
                 B11111111,
                 B01111110,
                 B00111100,
                 B00011000
              };
              */
//byte star1[] = {B00000110, B11000010, B10100100, B00011000, B00011000, B00100101, B01000011, B01100000};
//byte star2[] = {B00110000, B00100000, B00100111, B00011001, B10011000, B11100100, B00000100, B00001100};
//byte star3[] = {B00000110, B10001000, B10010000, B01011100, B00111010, B00001001, B00010001, B01100000};
/*
byte ALL[] = {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111};
byte EX[] = {B00000000, B00010000, B00010000, B00010000, B00010000, B00000000, B00010000, B00000000};
byte A[] = {B00000000, B00111100, B01100110, B01100110, B01111110, B01100110, B01100110, B01100110};
byte B[] = {B01111000, B01001000, B01001000, B01110000, B01001000, B01000100, B01000100, B01111100};
byte C[] = {B00000000, B00011110, B00100000, B01000000, B01000000, B01000000, B00100000, B00011110};
byte D[] = {B00000000, B00111000, B00100100, B00100010, B00100010, B00100100, B00111000, B00000000};
byte E[] = {B00000000, B00111100, B00100000, B00111000, B00100000, B00100000, B00111100, B00000000};
byte F[] = {B00000000, B00111100, B00100000, B00111000, B00100000, B00100000, B00100000, B00000000};
byte G[] = {B00000000, B00111110, B00100000, B00100000, B00101110, B00100010, B00111110, B00000000};
byte H[] = {B00000000, B00100100, B00100100, B00111100, B00100100, B00100100, B00100100, B00000000};
byte I[] = {B00000000, B00111000, B00010000, B00010000, B00010000, B00010000, B00111000, B00000000};
byte J[] = {B00000000, B00011100, B00001000, B00001000, B00001000, B00101000, B00111000, B00000000};
byte K[] = {B00000000, B00100100, B00101000, B00110000, B00101000, B00100100, B00100100, B00000000};
byte L[] = {B00000000, B00100000, B00100000, B00100000, B00100000, B00100000, B00111100, B00000000};
byte M[] = {B00000000, B00000000, B01000100, B10101010, B10010010, B10000010, B10000010, B00000000};
byte N[] = {B00000000, B00100010, B00110010, B00101010, B00100110, B00100010, B00000000, B00000000};
byte O[] = {B00000000, B00111100, B01000010, B01000010, B01000010, B01000010, B00111100, B00000000};
byte P[] = {B00000000, B00111000, B00100100, B00100100, B00111000, B00100000, B00100000, B00000000};
byte Q[] = {B00000000, B00111100, B01000010, B01000010, B01000010, B01000110, B00111110, B00000001};
byte R[] = {B00000000, B00111000, B00100100, B00100100, B00111000, B00100100, B00100100, B00000000};
byte S[] = {B00000000, B00111100, B00100000, B00111100, B00000100, B00000100, B00111100, B00000000};
byte T[] = {B00000000, B01111100, B00010000, B00010000, B00010000, B00010000, B00010000, B00000000};
byte U[] = {B00000000, B01000010, B01000010, B01000010, B01000010, B01000010, B01111110, B00000000};
byte V[] = {B00000000, B00100010, B00100010, B00100010, B00010100, B00010100, B00001000, B00000000};
byte W[] = {B00000000, B10000010, B10010010, B01010100, B01010100, B00101000, B00000000, B00000000};
byte X[] = {B00000000, B01000010, B00100100, B00011000, B00011000, B00100100, B01000010, B00000000};
byte Y[] = {B00000000, B01000100, B00101000, B00010000, B00010000, B00010000, B00010000, B00000000};
byte Z[] = {B00000000, B00111100, B00000100, B00001000, B00010000, B00100000, B00111100, B00000000};
*/

float timeCount = 0;
byte middle[] = { 0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000 };
byte E[] = { B00000000, B00111100, B00100000, B00111000, B00100000, B00100000, B00111100, B00000000 };


/*
End Code for LED Matrix 788BS
*/

void setup() {
  Serial.begin(9600);
  //setup for Gyroscope
  memcpy(LEDGrid[0][0], r1c1, sizeof(r1c1));
  memcpy(LEDGrid[0][1], r1c2, sizeof(r1c2));
  memcpy(LEDGrid[0][2], r1c3, sizeof(r1c3));
  memcpy(LEDGrid[0][3], r1c4, sizeof(r1c4));
  memcpy(LEDGrid[0][4], r1c5, sizeof(r1c5));
  memcpy(LEDGrid[0][5], r1c6, sizeof(r1c6));
  memcpy(LEDGrid[0][6], r1c7, sizeof(r1c7));

  memcpy(LEDGrid[1][0], r2c1, sizeof(r2c1));
  memcpy(LEDGrid[1][1], r2c2, sizeof(r2c2));
  memcpy(LEDGrid[1][2], r2c3, sizeof(r2c3));
  memcpy(LEDGrid[1][3], r2c4, sizeof(r2c4));
  memcpy(LEDGrid[1][4], r2c5, sizeof(r2c5));
  memcpy(LEDGrid[1][5], r2c6, sizeof(r2c6));
  memcpy(LEDGrid[1][6], r2c7, sizeof(r2c7));

  memcpy(LEDGrid[2][0], r3c1, sizeof(r3c1));
  memcpy(LEDGrid[2][1], r3c2, sizeof(r3c2));
  memcpy(LEDGrid[2][2], r3c3, sizeof(r3c3));
  memcpy(LEDGrid[2][3], r3c4, sizeof(r3c4));
  memcpy(LEDGrid[2][4], r3c5, sizeof(r3c5));
  memcpy(LEDGrid[2][5], r3c6, sizeof(r3c6));
  memcpy(LEDGrid[2][6], r3c7, sizeof(r3c7));

  memcpy(LEDGrid[3][0], r4c1, sizeof(r4c1));
  memcpy(LEDGrid[3][1], r4c2, sizeof(r4c2));
  memcpy(LEDGrid[3][2], r4c3, sizeof(r4c3));
  memcpy(LEDGrid[3][3], r4c4, sizeof(r4c4));
  memcpy(LEDGrid[3][4], r4c5, sizeof(r4c5));
  memcpy(LEDGrid[3][5], r4c6, sizeof(r4c6));
  memcpy(LEDGrid[3][6], r4c7, sizeof(r4c7));

  memcpy(LEDGrid[4][0], r5c1, sizeof(r5c1));
  memcpy(LEDGrid[4][1], r5c2, sizeof(r5c2));
  memcpy(LEDGrid[4][2], r5c3, sizeof(r5c3));
  memcpy(LEDGrid[4][3], r5c4, sizeof(r5c4));
  memcpy(LEDGrid[4][4], r5c5, sizeof(r5c5));
  memcpy(LEDGrid[4][5], r5c6, sizeof(r5c6));
  memcpy(LEDGrid[4][6], r5c7, sizeof(r5c7));

  memcpy(LEDGrid[5][0], r6c1, sizeof(r6c1));
  memcpy(LEDGrid[5][1], r6c2, sizeof(r6c2));
  memcpy(LEDGrid[5][2], r6c3, sizeof(r6c3));
  memcpy(LEDGrid[5][3], r6c4, sizeof(r6c4));
  memcpy(LEDGrid[5][4], r6c5, sizeof(r6c5));
  memcpy(LEDGrid[5][5], r6c6, sizeof(r6c6));
  memcpy(LEDGrid[5][6], r6c7, sizeof(r6c7));

  memcpy(LEDGrid[6][0], r7c1, sizeof(r7c1));
  memcpy(LEDGrid[6][1], r7c2, sizeof(r7c2));
  memcpy(LEDGrid[6][2], r7c3, sizeof(r7c3));
  memcpy(LEDGrid[6][3], r7c4, sizeof(r7c4));
  memcpy(LEDGrid[6][4], r7c5, sizeof(r7c5));
  memcpy(LEDGrid[6][5], r7c6, sizeof(r7c6));
  memcpy(LEDGrid[6][6], r7c7, sizeof(r7c7));

  Gyroscope_Setup();
  //setup for LED Matrix
  LED_788BS_Setup();

  drawScreen(middle);
}

void Gyroscope_Setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void LED_788BS_Setup() {
  for (byte i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
}

void loop() {

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  int xAng = map(AcX, minVal, maxVal, -90, 90);
  int yAng = map(AcY, minVal, maxVal, -90, 90);
  int zAng = map(AcZ, minVal, maxVal, -90, 90);

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  Serial.print("AngleX= ");
  Serial.println(x);

  Serial.print("AngleY= ");
  Serial.println(y);

  Serial.print("AngleZ= ");
  Serial.println(z);

  int col = 4;
  int row = 4;

  if (x > centerX + range) {  //go left NOTE - X will become larger when go left
    col--;
    if (x > centerX + 3 * range) {
      col--;
    }
    if (x > centerX + 5 * range) {
      col--;
    }
  } else if (x < centerX - range) {  // go right
    col++;
    if (x < centerX - 3 * range) {
      col++;
    }
    if (x < centerX - 5 * range) {
      col++;
    }
  }

  if (y < centerY - range) {  //go ups NOTE - X will become smaller when go up
    row--;
    if (y < centerY - 3 * range) {
      row--;
    }
    if (y < centerY - 5 * range) {
      row--;
    }
  } else if (y > centerY + range) {  // go down
    row++;
    if (y > centerY + 3 * range) {
      row++;
    }
    if (y > centerY + 5 * range) {
      row++;
    }
  }

  Serial.print("@@@2@@@ row = ");
  Serial.print(row);
  Serial.print(" col = ");
  Serial.println(col);

  drawScreen(LEDGrid[row - 1][col - 1]);
  //delay(400);

  Serial.println("-----------------------------------------");
}

void drawScreen(byte buffer2[]) {

  // Turn on each row in series
  for (byte i = 0; i < 8; i++) {
    setColumns(buffer2[i]);  // Set columns for this specific row

    digitalWrite(rows[i], HIGH);
    delay(2);  // Set this to 50 or 100 if you want to see the multiplexing effect!
    digitalWrite(rows[i], LOW);
  }
}

void setColumns(byte b) {
  digitalWrite(COL_1, (~b >> 7) & 0x01);  // Get the 1st bit: 10000000
  digitalWrite(COL_2, (~b >> 6) & 0x01);  // Get the 2nd bit: 01000000
  digitalWrite(COL_3, (~b >> 5) & 0x01);  // Get the 3rd bit: 00100000
  digitalWrite(COL_4, (~b >> 4) & 0x01);  // Get the 4th bit: 00010000
  digitalWrite(COL_5, (~b >> 3) & 0x01);  // Get the 5th bit: 00001000
  digitalWrite(COL_6, (~b >> 2) & 0x01);  // Get the 6th bit: 00000100
  digitalWrite(COL_7, (~b >> 1) & 0x01);  // Get the 7th bit: 00000010
  digitalWrite(COL_8, (~b >> 0) & 0x01);  // Get the 8th bit: 00000001

  // If the polarity of your matrix is the opposite of mine
  // remove all the '~' above.
}
