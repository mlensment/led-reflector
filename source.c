#include <avr/sleep.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(-1, 1); // RX, TX
//LEDs
int rLed = 0;
int bLed = 4;

//current program and program count
int curProgram = 0;
int progCount = 4;

//button stuff
int buttonPin = 2;
int buttonRead = false;

//Brightness variables for smooth light switching
int rBrightness = 255;
int bBrightness = 0;
int curBrightness = 0;
int nextBrightness = 0;
int brightnessStep;
int fadeAmt = 5;
int rMul = -1;
int bMul = 1;
int rLedOn = true;

//temperature sensor related stuff
int sensorPin = 3;
float temp;
long measureInterval = 3000;

//blink without delay
long previousMillis = 0;
unsigned long currentMillis;
long interval = 1000;

//sleep
int sleepStatus = 0;

void setup()   
{
  TCCR1 |= _BV (CS10);
  
  GTCCR |= _BV (PWM1B) | _BV(COM1B1); //Enable PWM 1B

  //TODO
  //refactor outputs and inputs
  DDRB = 0x3f;
  /* Data direction register B is 0xff which means, all are outputs */
  pinMode(sensorPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  mySerial.begin(2400);

}

void loop()
{
  readButton();
  
  switch(curProgram) {
    case 0:
      //go into power saving mode
      OCR1B = 0;
      digitalWrite(bLed, LOW);
      digitalWrite(rLed, LOW);      
      
      ADCSRA = ADCSRA & 0b01111111; /* Disable ADC, then sleep! */
      sleepNow();     // sleep function called here
      ADCSRA = ADCSRA | 0b10000000;
      break;

    case 1:
      //measure temperature
      currentMillis = millis();
       
      if(currentMillis - previousMillis > measureInterval) { 
        previousMillis = currentMillis;
        temp = getTemperature();
        mySerial.println(temp);
      }

      if(temp > 0) {
        lightReds(255);
      }
        
      // if(temp < 15) {
      //   lightBlues(255);
      // } else if(temp > 20) {
      //   lightReds(255);
      // } else {
      //   calcBrightness(temp);

      //   lightReds(curBrightness);
      //   delay(8);
      //   lightBlues(255 - curBrightness);
      //   delay(8);
      // }

    break;

    case 2:
      // police
      currentMillis = millis();
      if(currentMillis - previousMillis > interval) { 
        previousMillis = currentMillis;   
        rLedOn = !rLedOn;
      }

      if(rLedOn) {
        lightReds(255);
      } else {
        lightBlues(255);
      }
    break;

    case 3:
      //fading police
      if(rLedOn) {
        lightReds(curBrightness);
      } else {
        lightBlues(curBrightness);
      }
      
      // change the brightness for next time through the loop:
      curBrightness = curBrightness + fadeAmt;
      
      if(curBrightness == 0) {
        rLedOn = !rLedOn;
      }
      
      // reverse the direction of the fading at the ends of the fade: 
      if (curBrightness <= 0 || curBrightness >= 255) {
        fadeAmt = -fadeAmt; 
      }
      // wait for 30 milliseconds to see the dimming effect    
      delay(30);
    break;

    case 4:
      //rapid police
      lightReds(255);
      delay(50);
      lightBlues(255);
      delay(50);
    break;

    case 5:
      //blink together
      currentMillis = millis();
     
      if(currentMillis - previousMillis > interval) { 
        previousMillis = currentMillis;   

        if (curBrightness == 0)
          curBrightness = 255;
        else
          curBrightness = 0;
      }
      
      lightReds(curBrightness);
      lightBlues(curBrightness);
    break;
  }
}

void lightReds(int val) {
  OCR1B = 0;
  digitalWrite(bLed, LOW);
  analogWrite(rLed, val);
}

void lightBlues(int val) {
  digitalWrite(rLed, LOW);
  digitalWrite(bLed, HIGH);
  OCR1B = val;
}

float getTemperature() {
  lightBlues(127);
  
  GTCCR = GTCCR & 0b11011111; /* Clear COM1B1 */
  delay(2);
 
  float voltage = analogRead(sensorPin) * 3.3 / 1024.0;
  
  GTCCR = GTCCR | 0b00100000; /* set bit COM1B1 */
  
  float temperatureC = (voltage - 0.5) * 100;
  
  return temperatureC;
}

void readButton() {
  int buttonState = digitalRead(buttonPin);
  if(buttonState == LOW) {
    if(!buttonRead)
      curProgram = (curProgram < 5) ? curProgram + 1 : 0;
    buttonRead = true;
  } else {
    buttonRead = false;
  }
}

void sleepNow()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here

    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 

    attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW 

    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
  lightReds(255);
}
