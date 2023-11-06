/*
Mp3

Hardware Setup
==================================================================================================
Components: arduino nano, DY-SV8F

Nano 5v -> 5V
Nano GND -> GND

Nano D12 -> I02

TODO :
@Ava Connect Change Code to Mega

*/

int sound_1 = 7; // D7 pin is usually referred to as 7 in Arduino
String song; // Declare song as a String
boolean waitingMessagePrinted = false; // Track if the waiting message has been printed

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10); // Set a timeout for read operations
  pinMode(sound_1, OUTPUT);

  digitalWrite(sound_1, HIGH); // Corrected typo here
  Serial.println("ALL OFF");
}

void loop() {
  if (countdown()) {
    Serial.println("Ready to go!");
    waitingMessagePrinted = false; // Reset the waiting message tracker
  }
  else if (!waitingMessagePrinted) {
    Serial.println("Waiting For Countdown, Press \"Enter\" to Start.");
    waitingMessagePrinted = true; // Set the waiting message tracker
  }
}

boolean countdown() {
  while (Serial.available()) {
    char ch = Serial.read(); // Read the incoming data as a char
    if (ch == '\n') { // Check if the character is a newline
      digitalWrite(sound_1, LOW); // turn on the sound
      for (int i = 10; i >= 0; i--) {
        Serial.println(i);
        delay(1000); // Wait for 1 second
      }
      digitalWrite(sound_1, HIGH); // turn off the sound
      return true;
    }
  }
  return false;
}


