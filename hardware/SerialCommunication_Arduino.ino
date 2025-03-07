
/*Serial communication from Arduino to Raspberry Pi
Currently a number x is sent from the Raspberry Pi to the Aruduino. The Arduino acts accordingly to which number is sent, then sends a "confirmation" back to the Pi. 
*/

int x;
int LED = 3;

void setup() {
  Serial.begin(9600); // baud rate of the arduino and pi must be the same
  Serial.setTimeout(1);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    while (!Serial.available());
    x = Serial.readString().toInt(); //Reading from the serial port
    
    //determining what action to take based on what was sent from Pi
    if (x == 4) {
      digitalWrite(LED, HIGH);
      Serial.println("LED is on");
    }
    else if (x == 8) {
      digitalWrite(LED, LOW);
      Serial.println("LED is off");
    }
}
