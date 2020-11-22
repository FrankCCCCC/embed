/*
  Debounce

  Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
  press), the output pin is toggled from LOW to HIGH or HIGH to LOW. There's a
  minimum delay between toggles to debounce the circuit (i.e. to ignore noise).

  The circuit:
  - LED attached from pin 13 to ground
  - pushbutton attached from pin 2 to +5V
  - 10 kilohm resistor attached from pin 2 to ground

  - Note: On most Arduino boards, there is already an LED on the board connected
    to pin 13, so you don't need any extra components for this example.

  created 21 Nov 2006
  by David A. Mellis
  modified 30 Aug 2011
  by Limor Fried
  modified 28 Dec 2012
  by Mike Walters
  modified 30 Aug 2016
  by Arturo Guadalupi

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Debounce
*/

#include <Stepper.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;    // the number of the pushbutton pin
const int buttonPin2 = 3;    // the number of the pushbutton pin
const int ledPin = 11;      // the number of the LED pin
const int ledPinG = 10;      // the number of the LED pin
const int ledPin2 = 12;      // the number of the LED pin

// Variables will change:
int buttonState;             // the current reading from the input pin
int buttonState2;             // the current reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
//unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Button Pressed Time
int b1_time = 0;
int b2_time = 0;

//Button Reading
int reading = 0;
int reading2 = 0;

//Timer State
int s = 0;
int s2 = 0;

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 5, 8, 7, 6);

void set_timer(){
  noInterrupts(); // atomic access to timer reg.
  TCCR1A = 0;    TCCR1B = 0;    TCNT1 = 0;
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1<<CS12) | (1<<CS10); // 1024 prescaler
  OCR1A = 7812;  // give 0.5 sec at 16 MHz/1024
  
  TIMSK1 |= (1<<OCIE1A); // enable timer compare int.
  sei(); // enable all interrupts
}

void cap_timer(){
    if(buttonState){
      b1_time++; 
      Serial.print("Button 1 Inc");
      Serial.print(b1_time);
      Serial.print("\n");
    }else{
      if(b1_time > 0){
        b1_time--;
        Serial.print("Button 1 Dec");
        Serial.print(b1_time);
        Serial.print("\n");
      }
    }
    
    if(s){s = 0;}
    else{s = 1;}
}

void cap_timer2(){
    if(buttonState2){
      b2_time++; 
      Serial.print("Button");
      Serial.print(" 2 ");
      Serial.print("Inc");
      Serial.print(b2_time);
      Serial.print("\n");
    }else{
      if(b2_time > 0){
        b2_time--;
        Serial.print("Button");
        Serial.print(" 2 ");
        Serial.print("Dec");
        Serial.print(b2_time);
        Serial.print("\n");
      }
    }
    if(s2){s2 = 0;}
    else{s2 = 1;}    
}

void clear_timer(){
  if (TIFR1 & (1 << OCF1A)) { // wait for time up
    TIFR1 = (1<<OCF1A); // clear overflow flag
  }
}

void led_control(){
  if(buttonState){
    digitalWrite(ledPin, HIGH);
  }else{
    if(b1_time > 0){
      switch(s){
        case 1:
          digitalWrite(ledPin, HIGH);
          break;
        case 0:
          digitalWrite(ledPin, LOW);
          break;
      }
    }else{
      s = 0;
      digitalWrite(ledPin, LOW);
    }
  }
}

void led_control2(){
  if(buttonState2){
    digitalWrite(ledPin2, HIGH);
  }else{
    if(b2_time > 0){
      switch(s2){
        case 1:
          digitalWrite(ledPin2, HIGH);
          break;
        case 0:
          digitalWrite(ledPin2, LOW);
          break;
      }
    }else{
      s2 = 0;
      digitalWrite(ledPin2, LOW);
    }
  }
}

void led_control_rgb(){
  if(buttonState){
    digitalWrite(ledPin, HIGH);
  }else{
    if(b1_time > 0){
      switch(s){
        case 1:
          analogWrite(ledPinG, 255);
          analogWrite(ledPin, 128);
//          digitalWrite(ledPin2, HIGH);
//          digitalWrite(ledPin, HIGH);
          break;
        case 0:
//          digitalWrite(ledPin, LOW);
//          digitalWrite(ledPin2, LOW);
          analogWrite(ledPinG, 0);
          analogWrite(ledPin, 0);
          break;
      }
    }else{
      s = 0;
      digitalWrite(ledPin, LOW);
      digitalWrite(ledPinG, LOW);
    }
  }
}

void stepper_control(){
  if(buttonState2){
    myStepper.step(stepsPerRevolution);
  }else{
    if(b2_time > 0){
      myStepper.step(-stepsPerRevolution);
    }
  }
}

ISR(TIMER1_COMPA_vect) { // Timer1 ISR
  cap_timer();
  cap_timer2();
}

void handle_click() { // button debouncing, toggle LED
  Serial.print("Interrupt\n");
  static unsigned long last_int_time = 0;
  unsigned long int_time = millis(); // Read the clock

  if (int_time - last_int_time > 200 ) {  
    // Ignore when < 200 msec
    buttonState = !reading;  // switch LED
  }

  last_int_time = int_time;
}

void handle_click2() { // button debouncing, toggle LED
  Serial.print("Interrupt2\n");
  static unsigned long last_int_time = 0;
  unsigned long int_time = millis(); // Read the clock

  if (int_time - last_int_time > 200 ) {  
    // Ignore when < 200 msec
    buttonState2 = !reading2;  // switch LED
  }

  last_int_time = int_time;
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  set_timer();
  attachInterrupt(digitalPinToInterrupt(buttonPin), handle_click, CHANGE);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), handle_click2, CHANGE);
  interrupts(); // enable all interrupts

  // set initial LED state
  digitalWrite(ledPin, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPinG, LOW);

  Serial.print("Test\n");
}

void loop() {
  // read the state of the switch into a local variable:
  reading = digitalRead(buttonPin);
  reading2 = digitalRead(buttonPin2);

  led_control_rgb();
  led_control2();
  stepper_control();
}
