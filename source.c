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
const int numReadings = 10;
int readings[numReadings];
int index = 0;
int total = 0;
int average = 0;
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
        
      if(temp < 15) {
        lightBlues(255);
      } else if(temp > 20) {
        lightReds(255);
      } else {
        calcBrightness(temp);

        lightReds(curBrightness);
        delay(8);
        lightBlues(255 - curBrightness);
        delay(8);
      }

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
      rBrightness = rBrightness + (fadeAmt * rMul);
      bBrightness = bBrightness + (fadeAmt * bMul);
      
      if(rBrightness >= 255) {
        rBrightness = 255;
        rMul = -1;
        bMul = 1;
      } else if(bBrightness >= 255) {
        bBrightness = 255;
        rMul = 1;
        bMul = -1;
      }

      lightReds(rBrightness);
      lightBlues(bBrightness);
    break;

    case 4:
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
  OCR1B = 127;
  digitalWrite(bLed, HIGH);
  digitalWrite(rLed, LOW);
  delay(10);
  
  total -= readings[index];
  readings[index] = analogRead(sensorPin);
  total= total + readings[index];       
  index = index + 1;                    
 
  if (index >= numReadings) {
    index = 0;
  }              
                              
  average = total / numReadings;        
  float voltage = average * 5;
  voltage /= 1024.0;
  
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