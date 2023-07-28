//one sensor, needs blocked -> not blocked, not blocked -> block to find speed with length of magnet

//TODO Need to clean up this code, for example, write wiring instruction etc.
            
int speedPin = 2;                        

int val = 0;
int oldval = 0;
int currtime;
int newtime;

double LENGTH = 20.0;                


void setup() {
  pinMode(speedPin, INPUT); // declare sensor as input
  Serial.begin(9600);  // for serial monitor
}


void loop() {
  val = digitalRead(speedPin);
  if (val == HIGH)
  {
    Serial.print("Blocked, current val = ");
    Serial.println(val);
    delay(5000); // to see speed
    if (oldval == 0)
    {
      currtime = millis(); // get time in ms      
    }
    oldval = 1;
  }
  else
  {
    Serial.println("NOT blocked");
    if (oldval == 1)
    {
      newtime = millis(); // get time in ms
      Serial.print("Time is ");
      Serial.println(newtime - currtime);
      Serial.print("Velocity is ");
      Serial.print(LENGTH/(newtime - currtime)); // v = d/t
      Serial.println(" mm/millis ");


      delay(5000); // to see speed
    }
    oldval = 0; //reset val
  }
 
}
