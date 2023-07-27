/* Hall Sensor Test */

float defaultSensorValue = 509.0;

void setup() {
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  // read the input from A0 analog
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
  if ((sensorValue > defaultSensorValue) && (currentStrength > 1)) {
    Serial.print("Wow, we detect a SOUTH magnet!! The strength is ");
    Serial.print("@@@@@");
    Serial.print(sensorValue);
    Serial.print("@@@@@");
    Serial.print(defaultSensorValue);
    Serial.print("@@@@@");
    Serial.println(currentStrength);
  }
  else {
    if ((sensorValue < defaultSensorValue) && (currentStrength > 1)) {
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

  delay(500);
}
