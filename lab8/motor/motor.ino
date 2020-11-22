///*
// * MotorKnob
// *
// * A stepper motor follows the turns of a potentiometer
// * (or other sensor) on analog input 0.
// *
// * http://www.arduino.cc/en/Reference/Stepper
// * This example code is in the public domain.
// */
//
//#include <Stepper.h>
//
//// change this to the number of steps on your motor
//#define STEPS 100
//
//// create an instance of the stepper class, specifying
//// the number of steps of the motor and the pins it's
//// attached to
//Stepper stepper(STEPS, 8, 9, 10, 11);
//
//// the previous reading from the analog input
//int previous = 0;
//
//void setup() {
//  // set the speed of the motor to 30 RPMs
//  stepper.setSpeed(30);
//}
//
//void loop() {
//  // get the sensor value
//  int val = analogRead(0);
//
//  // move a number of steps equal to the change in the
//  // sensor reading
//  stepper.step(val - previous);
//
//  // remember the previous value of the sensor
//  previous = val;
//}

#include <Stepper.h>

const int stepsPerRevolution = 2048;  
// change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 4, 6, 7, 5);
/*
pin腳接法
IN1=4
IN2=6
IN3=7
IN4=5
*/

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(5);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  // step one revolution  in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);

  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
}
