/* Hall Sensor Test */

#include <LiquidCrystal.h>// include the library code

float defaultSensorValue = 509.0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

void setup() {
  Serial.begin(9600); // Initialize serial communication
  lcd.begin(16, 2);    // set up the LCD's number of columns and rows
}

void loop() {
   lcd.setCursor(0,0);  // set the cursor to beginning

  // read the input from A0 analog (voltage)
  int sensorValue = analogRead(A0); 

  Serial.print("The sensor value is ");
  Serial.println(sensorValue);

  //Serial.print("Current voltage is ");
  //Serial.println(voltage);
  //lcd.print("Voltage:    ");  // Print a message to the LCD.
  //lcd.println(voltage);
//  float currentStrength = abs(sensorValue - defaultSensorValue); // strength 

  lcd.setCursor(0,0); // start printing on the next line
  lcd.print("Current strength");

  lcd.setCursor(0, 1);
  lcd.print(sensorValue);
  // print out the value you read:

  delay(500); // wait before entering loop again
  lcd.clear(); // reset lcd because what if the status of the magnet changes -> new display
}
