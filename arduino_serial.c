#include <SoftwareSerial.h>

SoftwareSerial mySerial(12, 6); // RX, TX

void setup()  
{
  Serial.begin(57600);
  Serial.println("Goodnight moon!");

  mySerial.begin(2400);
}

void loop() // run over and over
{
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}
