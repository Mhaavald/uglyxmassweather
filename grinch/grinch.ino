/* Ugly XMas Sweather Code
  by: Morten Haavaldsen

  date: 11/15/2017
  license: MIT License
  https://opensource.org/licenses/MIT

  This code is for an Ugly Xmas Sweather project.
  It consists of an Lilypad Atmega 328 mcu. 

  The motif is the Grinch.

  The components must not call delay but manage their timing using e.g. millis()
  This enables you to add new components doing cool stuff without caring too much about it messing up
  how other components work.
  E.g. adding a button is simple and button down will be detected immediately since no other
  components owns the thread in a delay.
  Of course if a coponent has lot's of work to do then it may affect how e.g. Fader works.
  
*/

// Two leds in parallell
int eyesPin = 5;

// Two leds in parallell
int mouthPin = 6;

// One led
int leftCapPin = 11;

// One led
int rightCapPin = A5;

// One led
int topCapPin = A2;


// Flashes a led on and off given specified led, how long to be on and how long to be off
class Flasher
{  
  int ledPin;      // the number of the LED pin
  long OnTime;     // milliseconds of on-time
  long OffTime;    // milliseconds of off-time

  // These maintain the current state
  int ledState;                 // ledState used to set the LED
  unsigned long previousMillis;   // will store last time LED was updated

  public:
  Flasher(int pin, long on, long off)
  {
    ledPin = pin;
    pinMode(ledPin, OUTPUT);     
      
    OnTime = on;
    OffTime = off;
    
    ledState = LOW; 
    previousMillis = 0;
  }

  void turnOff()
  {
    digitalWrite(ledPin, LOW);
  }
  
  void Update()
  {    
    unsigned long currentMillis = millis();
     
    if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
      ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(ledPin, ledState);   // Update the actual LED
    }
  }
};

// Fades an led (PWM).
// Only works on PWM pins.
// If pin is not PWM you'll just see it either being on or off.
//
class Fader
{
  int ledPin = 9;           // the PWM pin the LED is attached to
  int brightness = 0;    // how bright the LED is
  int fadeAmount = 5;    // how many points to fade the LED by
  unsigned long previousMillis; // last update of position
  int waitTime = 100; // just a default

  public: 
  Fader(int _led, int _fadeAmount, int _waitTime)
  {    
    ledPin = _led;
    fadeAmount = _fadeAmount;
    waitTime = _waitTime;
    previousMillis = 0;

    pinMode(ledPin, OUTPUT);     
  }
    
  void Update() {

    unsigned long currentMillis = millis();
    
    if(currentMillis - previousMillis >= waitTime)
    {
        analogWrite(ledPin, brightness);
        // change the brightness for next time through the loop:
        brightness = brightness + fadeAmount;
        
        // reverse the direction of the fading at the ends of the fade:
        if (brightness <= 0 || brightness >= 255) {
           fadeAmount = -fadeAmount;
        }        

        previousMillis = currentMillis; 
    };
  }  
};

// Eyes will fade on off
// The others will blink/flash
 
Fader   eyes(eyesPin, 5, 30);
Flasher mouth(mouthPin, 1000, 1000);
Flasher leftCap(leftCapPin, 30, 1000);
Flasher rightCap(rightCapPin, 30, 2000);
Flasher topCap(topCapPin, 30, 500);

void setup() 
{ 

} 
 
 
void loop() 
{ 
  eyes.Update();
  mouth.Update();
  leftCap.Update();
  rightCap.Update();
  topCap.Update();
  
}
