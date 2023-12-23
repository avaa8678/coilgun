#include <LiquidCrystal_I2C.h>// include the library code
// #include <LCD.h>



int speedPin1 = 3;      
int speedPin2 = 5;
//d11 -- d7(lcd)
//d10 -- d6
//d9 -- d5 (shorted to e)
//d8 -- d4
//d7 -- e
//d6 -- rs
//rs -- d6
             


int val = 0;
int val2 = 0;
int oldval = 0;
long time2;
long time1;


double LENGTH = 0.0128;    


LiquidCrystal_I2C lcd(0x27, 16, 2);




void setup() {
  pinMode(speedPin1, INPUT); // declare sensor as input
  pinMode(speedPin2, INPUT);
  // lcd.begin();
  // lcd.home();
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  // lcd.print("THIS WILL PRINT");
  // lcd.begin(16, 2);
  Serial.begin(9600);  // for serial monitor
  // Serial.print("Should have printed to lcd");
  
}




void loop() {
  lcd.setCursor(0,0);
  // lcd.print("Hello!");
  // Serial.print("PRINT");
  val = digitalRead(speedPin1);
  // Serial.println(val);
  if (val == HIGH)
  {
   time1 = micros(); 
    // while (val == HIGH){
    //   val = digitalRead(speedPin1);
    // }
    // time1 = micros();
    // lcd.println("I am LOW");
    // Serial.println("I am LOW");
    val2 = digitalRead(speedPin2);
    // Serial.print("Blocked, current val = ");
    // Serial.println(val);
    //delay(5000); // to see speed
    int counter = 0;
    while(val2 == 0 && counter == 0) {
      val2 = digitalRead(speedPin2);
      if (val2 == HIGH) {
        time2 = micros();
        while(val2 == HIGH) {
          val2 = digitalRead(speedPin2);
        }
        counter++;
        // Serial.println("I am LOooW");
        // time2 = micros();
        Serial.print("Time from ");
        Serial.print(time1);
        Serial.print(" to ");
        Serial.print(time2);
        double timeLapse = (time2 - time1)/1000000.0;
        Serial.println(timeLapse);
        Serial.println(time2-time1);
        // Serial.print(LENGTH/timeLapse);
        Serial.print("Velocity is ");
        Serial.print(LENGTH/timeLapse); // v = d/t
        lcd.print("Velocity: ");
        lcd.setCursor(0,1);
        lcd.print(LENGTH/timeLapse);
        lcd.print(" m/s ");
        Serial.println(" m/s ");




      //  delay(5000); // to see speed
      }
    }
  }
}
