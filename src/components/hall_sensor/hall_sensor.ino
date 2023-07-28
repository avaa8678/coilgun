/* Hall Sensor Test */

#include <LiquidCrystal.h>// include the library code

float defaultSensorValue = 509.0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

void setup() {
  Serial.begin(9600); // Initialize serial communication
  lcd.begin(16, 2);	  // set up the LCD's number of columns and rows
}

void loop() {
   lcd.setCursor(0,0);  // set the cursor to beginning

  // read the input from A0 analog (voltage)
  int sensorValue = analogRead(A0); 

  // analog reading range is from 0 to 1023 
  // the value is around 507 or 508, the hall effect sensor shows >508 when South and <507 when North
  // original code divided by 1023, we divide by 507 to make 5V, later changed to 1023 because the voltage doesn't need to be 5V
  // convert the analog reading (which is from 0-1023) to voltage (0-5v)
  float voltage = sensorValue * (5.0 / 1023.0);
  Serial.print("Current voltage is ");
  Serial.println(voltage);
  lcd.print("Voltage:    ");  // Print a message to the LCD.
  lcd.println(voltage);
  float currentStrength = abs(sensorValue - defaultSensorValue); // strength 

  lcd.setCursor(0,1); // start printing on the next line

  // South magnet
  if ((sensorValue > defaultSensorValue) && (currentStrength > 2)) { // currentStrenght > 2 because sometimes there's a small error
    Serial.print("Wow, we detect a SOUTH magnet!! The strength is ");
    Serial.println(currentStrength);
    lcd.print("South     "); // need to add spaces because otherwise it displays some messy characters
    lcd.print(currentStrength);
  }
  else {
    if ((sensorValue < defaultSensorValue) && (currentStrength > 2)) {
      Serial.print("Wow, we detect a NORTH magnet!! The strength is ");
      Serial.println(currentStrength);
      lcd.print("North     ");
      lcd.print(currentStrength);
    }
    else 
      Serial.println("No magnet detected~");
      lcd.print("No magnet  ");
  }

  // print out the value you read:

  delay(500); // wait before entering loop again
  lcd.clear(); // reset lcd because what if the status of the magnet changes -> new display
}
