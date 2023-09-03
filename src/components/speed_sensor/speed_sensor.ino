#include <LiquidCrystal.h>// include the library code

int speedPin1 = 2;      
int speedPin2 = 4;                  

int val = 0;
int val2 = 0;
int oldval = 0;
long time2;
long time1;

double LENGTH = 12.5;     

LiquidCrystal lcd(6, 7, 8, 9, 10, 11);


void setup() {
  pinMode(speedPin1, INPUT); // declare sensor as input
  pinMode(speedPin2, INPUT);
  lcd.begin(16, 2);
  Serial.begin(9600);  // for serial monitor
}


void loop() {
  lcd.setCursor(0,0);
  val = digitalRead(speedPin1);
  if (val == HIGH)
  {
    val2 = digitalRead(speedPin2);
    time1 = micros();
    Serial.print("Blocked, current val = ");
    Serial.println(val);
    // delay(5000); // to see speed
    while(val2 == 0) {
      val2 = digitalRead(speedPin2);
      if (val2 == HIGH) {
        time2 = micros();
        Serial.print("Time is ");
        double timeLapse = (time2 - time1)/1000.0;
        Serial.println(timeLapse);
        Serial.println(time2-time1);
        Serial.print("Velocity is ");
        Serial.print(LENGTH/timeLapse); // v = d/t
        lcd.print("Velocity: ");
        lcd.setCursor(0,1);
        lcd.print(LENGTH/timeLapse);
        lcd.print(" mm/millis ");
        Serial.println(" mm/millis ");


      // delay(5000); // to see speed
      }
    }
  }
}
