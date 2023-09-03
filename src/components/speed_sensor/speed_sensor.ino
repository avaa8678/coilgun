//one sensor, needs blocked -> not blocked, not blocked -> block to find speed with length of magnet

//TODO Need to clean up this code, for example, write wiring instruction etc.
            
int speedPin = 2;                        

int val = 0;
int oldval = 0;
long currtime;
long newtime;

double LENGTH = 200000.0;                


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
      currtime = micros(); // get time in ms      
    }
    oldval = 1;
  }
  else
  {
    Serial.println("NOT blocked");
    if (oldval == 1)
    {
      newtime = micros(); // get time in ms
      Serial.print("Time is ");

      double timeLapse = (newtime - currtime)/1000.0;
      Serial.println(timeLapse);
      Serial.println(newtime-currtime);
      Serial.print("Velocity is ");
      Serial.print(LENGTH/timeLapse); // v = d/t
      Serial.println(" mm/millis ");


      delay(5000); // to see speed
    }
    oldval = 0; //reset val
  }
 
}
