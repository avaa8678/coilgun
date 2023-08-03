#include <LiquidCrystal.h>// include the library code
#include <SPI.h>
#include <SD.h>
#include <TimerOne.h>

float defaultSensorValue = 509.0;

const int CS_PIN = 53;
const int SAMPLINGPERIOD = 1000;
const int BUFFERSIZE = 750;

File dataFile;
long timeStamp;
volatile int counter = 0;
volatile long timearray[BUFFERSIZE];
volatile long valarray[BUFFERSIZE];


int maxsize = 750;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
int sensorValue;

void setup() {
  Serial.begin(9600); // Initialize serial communication
  lcd.begin(16, 2);    // set up the LCD's number of columns and rows
  counter=0;
  //Serial.begin(9600);
  Serial.println("Initialiazing Card");
  pinMode(CS_PIN, OUTPUT);
  if (!SD.begin(CS_PIN))
  {
    Serial.println("Card Failure");
    return;
  }
  Serial.println("Card Ready");

  Timer1.initialize(SAMPLINGPERIOD);
  Timer1.attachInterrupt(writeToCard);

}

void loop() {
  lcd.setCursor(0,0);  // set the cursor to beginning
  
  dataFile = SD.open("log.txt", FILE_WRITE);

  timeStamp = micros();
  
  if (dataFile)
  {
    dataFile.close();
  }
  else
  {
    Serial.println("Couldn't open log file");
  }
  randomSeed(analogRead(0));

}

void writeToCard()
{  
  sensorValue = analogRead(A0); 

  Serial.println(sensorValue);
  //lcd.setCursor(0,0); // start printing on the next line
  lcd.print("Current strength");

  lcd.setCursor(0, 1);
  lcd.print(sensorValue);
  
  lcd.clear(); // reset lcd because what if the status of the magnet changes -> new display

  if (counter % 10 == 0)
  {  
    //Serial.println(micros());
    timearray[counter] = micros();   
  }
  else
  {
    timearray[counter] = micros();
  }
  valarray[counter] = sensorValue;  
  counter++;

  if (counter == maxsize)
  { 
    //Serial.println(micros());
    dataFile = SD.open("log.txt", FILE_WRITE);
    if (dataFile)
    {
      for(int i = 0; i < maxsize; i++)
      {
        if (i % 10 == 0)
        {   
          dataFile.print(i);
          dataFile.print("-");
          dataFile.println(timearray[i]);
          dataFile.print(" - ");
          dataFile.println(valarray[i]);
        }
        else
        {
          dataFile.println(timearray[i]);
          dataFile.print(" - ");
          dataFile.println(valarray[i]);
        }
          
      }
      counter = 0;
      Serial.println("done");
    } 
    dataFile.close();
  }
}
 
   
 