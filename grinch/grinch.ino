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

#define ON true
#define OFF false
  
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

class SoftPWM
{
  // variables for pattern timing
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  unsigned long millisInterval = 100;
   
  // variables for software PWM
  unsigned long currentMicros = micros();
  unsigned long previousMicros = 0;
  // this is the frequency of the sw PWM
  // frequency = 1/(2 * microInterval)
  unsigned long microInterval = 250;
   
  byte pwmMax = 100;
   
  // fading (for the timing)
  int fadeIncrement = 1;

  int pinCount = 8;
  //byte pins[pinCount] = {2,3,5,6,9,10,11,12};
  // typedef for properties of each sw pwm pin
    
  typedef struct pwmPins {
    int pin;
    int pwmValue;
    bool pinState;
    int pwmTickCount;
  } pwmPin;
  
  pwmPin *myPWMpins;

   
  // create the sw pwm pins
  // these can be any I/O pin
  // that can be set to output!

  byte *pins;
  
  //byte pins[pinCount] = {2,3,5,6,9,10,11,12};
  
  public:
  SoftPWM(byte *_pins)
  {
    pinCount = sizeof(_pins);
    pins = _pins;
    pwmPin newStruct[pinCount];
  
    myPWMpins = newStruct;
   
    setupPWMpins();
    pinMode(13, OUTPUT);
  }
  
  // function to "setup" the sw pwm pin states
  // modify to suit your needs
  // this creates an alternating fade pattern
  void setupPWMpins() {
    for (int index=0; index < pinCount; index++) {
      myPWMpins[index].pin = pins[index];
   
      // mix it up a little bit
      // changes the starting pwmValue for odd and even
      if (index % 2)
        myPWMpins[index].pwmValue = 25;
      else
        myPWMpins[index].pwmValue = 75;
   
      myPWMpins[index].pinState = ON;
      myPWMpins[index].pwmTickCount = 0;
   
      // unlike analogWrite(), this is necessary
      
      pinMode(pins[index], OUTPUT);
    }
  }
   
  void pwmFadePattern() {
    // go through each sw pwm pin, and increase
    // the pwm value. this would be like
    // calling analogWrite() on each hw pwm pin
    for (int index=0; index < pinCount; index++) {
      myPWMpins[index].pwmValue += fadeIncrement;
      if (myPWMpins[index].pwmValue > 100)
        myPWMpins[index].pwmValue = 0;
    }
  }
   
  void handlePWM() {
    currentMicros = micros();
    // check to see if we need to increment our PWM counters yet
      if (currentMicros - previousMicros >= microInterval) {
      // Increment each pin's counter
      for (int index=0; index < pinCount; index++) {
      // each pin has its own tickCounter
        myPWMpins[index].pwmTickCount++;
   
      // determine if we're counting on or off time
        if (myPWMpins[index].pinState == ON) {
          // see if we hit the desired on percentage
          // not as precise as 255 or 1024, but easier to do math
          if (myPWMpins[index].pwmTickCount >= myPWMpins[index].pwmValue) {
            myPWMpins[index].pinState = OFF;
          }
        } else {
          // if it isn't on, it is off
          if (myPWMpins[index].pwmTickCount >= pwmMax) {
            myPWMpins[index].pinState = ON;
            myPWMpins[index].pwmTickCount = 0;
          }
        }
        // could probably use some bitwise optimization here, digitalWrite()
        // really slows things down after 10 pins.
        digitalWrite(myPWMpins[index].pin, myPWMpins[index].pinState);
      }
      // reset the micros() tick counter.
      digitalWrite(13, !digitalRead(13));
      previousMicros = currentMicros;
    }
  }
 
  void Update() {
    // this is the magic for sw pwm
    // need to call this anytime you
    // have a long operation
    handlePWM();
   
    // check timer for fading pattern
    // this would be the same 
    // if we used analogWrite()
    currentMillis = millis();
    if (currentMillis - previousMillis >= millisInterval) {
      // moved to own funciton for clarity
      pwmFadePattern();
   
      // setup clock for next tick
      previousMillis = currentMillis;
    }
  }
   
  // Code from james@baldengineer.com adapted as a component here.
  // email | twitter | www
  // See more at: https://www.baldengineer.com/software-pwm-with-millis.html

};
 
Fader eyes(eyesPin, 5, 30);
Flasher mouth(mouthPin, 1000, 1000);
Flasher leftCap(leftCapPin, 30, 1000);
Flasher rightCap(rightCapPin, 30, 2000);
Flasher topCap(topCapPin, 30, 500);

byte pins[8] = {2,3,5,6,9,10,11,12};

SoftPWM softPwm(pins);

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
