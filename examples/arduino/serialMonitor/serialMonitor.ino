#include <SoftwareSerial.h>

SoftwareSerial mySerial(11, 12); //RX,TX

String tmp; 

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Hello Cactus Micro!");
  
  // Enable esp8266 
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
};

void loop() {

  while (mySerial.available() > 0)  {
    tmp += char(mySerial.read());
    delay(2);
  }

  if(tmp.length() > 0) {
    Serial.println(tmp);
    tmp = "";
  }

  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
