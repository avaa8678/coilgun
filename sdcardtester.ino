#include <SPI.h>
#include <SD.h>
#include <TimerOne.h>

const int CS_PIN = 53;
const int SAMPLINGPERIOD = 10000;
const int BUFFERSIZE = 750;

File dataFile;
long timeStamp;
volatile int counter = 0;
volatile long valarray[BUFFERSIZE];

//long valarray[] = {};
//int counter;
int maxsize = 750;

void setup() {
  // put your setup code here, to run once:
  counter=0;
  Serial.begin(9600);
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
  // put your main code here, to run repeatedly:
  //timeStamp = millis();
  dataFile = SD.open("log.txt", FILE_WRITE);

  timeStamp = micros();
  
  if (dataFile)
  {
      
    //dataFile.print(timeStamp);
    //Serial.print(timeStamp);

    //dataFile.println(" ms, The current is ");
    //Serial.println("ms, The current is");

  //  dataFile.println("sensorValue");
  //  Serial.println("sensorValue");

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
  
    if (counter % 10 == 0)
    {  Serial.println(micros());
       valarray[counter] = micros();   
    }
    else
      valarray[counter] = micros();
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
        {   dataFile.print(i);
            dataFile.print("-");
            dataFile.println(valarray[i]);}
        else
          dataFile.println(valarray[i]);
      }
      counter = 0;
      Serial.println("done");
    } 
    //dataFile.println("collect print console start");    
    //dataFile.println(micros());
    //Serial.println(micros());
    //dataFile.println(micros());
    //dataFile.println("collect print console end");    
    dataFile.close();
  }
 
   
  //Serial.println(" hello");
}