/*
Hardware Setup
==================================================================================================
Arduino Mega 2560 + Deek-Robot Data Logging Shield v1.0
1) Hardware Connection
Arduino Mega SPI Pin D50 (MISO) <-> Data Logging Shield D12 (MISO)
Arduino Mega SPI Pin D51 (MOSI) <-> Data Logging Shield D11 (MOSI)
Arduino Mega SPI Pin D52 (SCK)  <-> Data Logging Shield D13 (SCK)
Arduino Mega SPI Pin D53 (CS)   <-> Data Logging Shield D10 

2) RTC (Real-Time Clock) model on Deek-Robot Data Logging Shield v1.0
The Data Logging Shield v1.0 uses the I2C communication protocol to communicate with the RTC
Arduino Mega I2C Pin 20 (SDA)   <-> Data Logging Shield A4
Arduino Mega I2C Pin 21 (SCL)   <-> Data Logging Shield A5

3) The SD Card needs to format to "FAT32" and must insert into the Data Logging Shield before booting
==================================================================================================

Functions
==================================================================================================
 
*/
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include <string.h>

// Set the pins used by the shield
#define SDCARD_CS 53
#define SDCARD_MOSI 51
#define SDCARD_MISO 50
#define SDCARD_SCK 52

// global variables
RTC_DS1307 rtc;
char filename[12] = { 0 };
String delimiter = ",";
String fileExtension = "csv"; // 1-4 CHAR ONLY

String fileHeader = "Current Time" + delimiter + "Seconds" + delimiter + "Currents (amps)";

void setup() {
  Serial.begin(9600);

  initialzeSPI();
  initializeSDCard();

  initialzeRTC();
  initialzeFileName();
}

void loop() {
  String currentTime = getCurrentTime();
  String currentSensorValue = getCurrentSensorValue();
  String speedSensorValue = getSpeedSensorValue();
  String hallSensorValue = getHallSensorValue();

  unsigned long milliseconds = millis();
  // Convert the elapsed time to seconds
  float seconds = milliseconds / 1000.0;
  String elapsedTime = String(seconds, 3);

  String lineOfText = currentTime + delimiter + elapsedTime + delimiter + currentSensorValue;
  writeTextToFile(lineOfText);

  // !!! not suppose to do any delay when we go alive
  randomDelay(1000);
  // !!! not suppose to do any delay when we go alive
}

void initialzeSPI() {
  // Set the SPI pins used by the SD card
  Serial.println("Initialzing SPI");
  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH);
  SPI.begin();
}

void initializeSDCard() {
  // Initialize the SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(SDCARD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");
}

void initialzeRTC() {
  // Initialize the RTC
  Wire.begin();
  rtc.begin();
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    return;
  }
  
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    return;
  }
}

void initialzeFileName() {
  DateTime now = rtc.now();

  // there is a problem that the TOTAL length of the file can not exceed 11 characters YYMMDD.csv is the way to go
  // filename[0] = (now.year() / 1000) % 10 + '0';  //To get 1st digit from year()
  // filename[1] = (now.year() / 100) % 10 + '0';   //To get 2nd digit from year()

  filename[0] = (now.year() / 10) % 10 + '0';    //To get 3rd digit from year()
  filename[1] = now.year() % 10 + '0';           //To get 4th digit from year()
  filename[2] = now.month() / 10 + '0';          //To get 1st digit from month()
  filename[3] = now.month() % 10 + '0';          //To get 2nd digit from month()
  filename[4] = now.day() / 10 + '0';            //To get 1st digit from day()
  filename[5] = now.day() % 10 + '0';            //To get 2nd digit from day()

  filename[6] = '.';
  filename[7] = fileExtension.charAt(0);
  filename[8] = fileExtension.charAt(1);
  filename[9] = fileExtension.charAt(2);
  filename[10] = fileExtension.charAt(3);

  Serial.print("Filename is ");
  Serial.println(filename);
}

String getCurrentTime() {
  // Get the current date and time from the RTC module
  DateTime now = rtc.now();

  // Access individual components
  int year = now.year();
  int month = now.month();
  int day = now.day();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();  

  // We are using this format : YYYY-MM-DD HH:MM:SS
  String time = String(year) + "-" + String(month) + "-" + String(day) + " " + String(hour) + ":" + String(minute) + ":" + String(second);

  return time;
}

String getCurrentSensorValue() {
  String sensorValue = "TODO";



  return sensorValue;
}


String getSpeedSensorValue() {
  String sensorValue = "TODO";



  return sensorValue;
}

String getHallSensorValue() {
  float defaultHallSensorValue = 509.0;
  int accuracy = 1;

  
  int sensorValue = analogRead(A0);

  // analog reading range is from 0 to 1023   
  // But i observed the value is around 507-508 i guess - hall effect sensor for North it is greater than 508, for south it is smaller than 508
  // so instead of divided by 1023 we divided by 507 to make voltage to 5v
  // convert the analog reading (which is from 0-1023) to voltage (0-5v)
  float voltage = sensorValue * (5.0 / 1023.0);
  Serial.print("Current voltage is ");
  Serial.println(voltage);

  float currentStrength = abs(sensorValue - defaultSensorValue);

  // so we are manipulate this a bit - 
  if ((sensorValue > defaultSensorValue) && (currentStrength > accuracy)) {
    Serial.print("Wow, we detect a SOUTH magnet!! The strength is ");
    Serial.print("@@@@@");
    Serial.print(sensorValue);
    Serial.print("@@@@@");
    Serial.print(defaultSensorValue);
    Serial.print("@@@@@");
    Serial.println(currentStrength);
  }

  else {
    if ((sensorValue < defaultSensorValue) && (currentStrength > accuracy)) {
      Serial.print("Wow, we detect a NORTH magnet!! The strength is ");
      Serial.print("@@@@@");
      Serial.print(sensorValue);
      Serial.print("@@@@@");
      Serial.print(defaultSensorValue);
     Serial.print("@@@@@");
      Serial.println(currentStrength);
    }
    else 
      Serial.println("No magnet detected~");
  }

  // print out the value you read:
  //Serial.print("Sensor Value is ");
  //Serial.println(sensorValue);


  String sensorValue = "TODO";



  return sensorValue;
}

void writeTextToFile(String lineOfText) {
  //Check file name exist first
  File myFile = SD.open(filename, FILE_WRITE);;
  if (!myFile) {
    Serial.print("Error opening ");
    Serial.println(filename);
    while(1); // means stop or you can do return
  }

  if (SD.exists(filename)) {
    // Open exsisting file in this case, so no need to add a header
    Serial.println(" is already exists.");
  } 
  else {
    // add a header if we have a new file, need to do this before SD.open()
    myFile.println(fileHeader);
    Serial.println(fileHeader);
    
    Serial.println(" doesn't exist.");
    Serial.print("Creating new file ");
    Serial.println(filename);
  }
  
  // print the line we want
  myFile.println(lineOfText);
  Serial.println(lineOfText);

  myFile.close();
}

void randomDelay(int time) {
  // Set the random seed using an analog input pin
  randomSeed(analogRead(0));

  // Generate a random number within the time
  int randomNumber = random(time);

  // Wait for one second before generating another random number
  delay(randomNumber);
}
