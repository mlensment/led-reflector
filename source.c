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

void setup()   
{
  TCCR1 = _BV (CS10);
  GTCCR = _BV (COM1B1) | _BV(PWM1B);
  
  //TODO
  //refactor outputs and inputs
  DDRB = 0xff;
  /* Data direction register B is 0xff which means, all are outputs */
  pinMode(sensorPin, INPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
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
    break;

    case 1:
      //measure temperature
      currentMillis = millis();
       
      if(currentMillis - previousMillis > measureInterval) { 
        previousMillis = currentMillis;
        temp = getTemperature();
      }

      if(temp > 18) {
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
        fadeAmt = -fadeAmt ; 
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


void calcBrightness(float temp) {
  if(curBrightness == nextBrightness) {
    temp -= 15;
    
    nextBrightness = (int) (temp * 255 / 5);
    if(nextBrightness > 255) {
       nextBrightness = 255;
    }
    if(nextBrightness < 0) {
       nextBrightness = 0; 
    }
    brightnessStep = (nextBrightness > curBrightness) ? 1 : -1;
  } else {
    curBrightness += brightnessStep; 
  }
}

float getTemperature() {
  lightBlues(127);
  delay(30);
 
  float voltage = analogRead(sensorPin) * 3.3 / 1024.0;
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
