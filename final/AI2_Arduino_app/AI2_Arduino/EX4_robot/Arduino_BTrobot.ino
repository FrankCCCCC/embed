#include <SoftwareSerial.h>
#include <Servo.h>

Servo servoRight;
Servo servoLeft;

SoftwareSerial BTSerial(10, 11); // RX, TX

byte dis[6];  //distance package
int t,i,pp;   //temp value


void setup(){
  Serial.begin(9600);
  BTSerial.begin(57600);    //BTSerial 9600,19200,38400,57600
  servoRight.attach(12);
  servoLeft.attach(13);
  dis[0] = 65;              //check value
}

void loop(){
  if (BTSerial.available()>0){
    pp=BTSerial.read();
    Serial.println(pp);
    switch(pp){
    case 'f':  //Forward
      Up();
    break;
    case 'x':  //Back
      Down();
    break;
    case 'a':  //Turn Left
      Left();
    break;
    case 'd':  //Turn Right
      Right();
    break;
    case 's':  //Stop
      stop();
    break;
    case 65:  //Return distance value
      for(i=5;i>0;i--){
        dis[i] = t%10;
        t/=10;
      }
      //Pack value
      for(i=0;i<=5;i++)
        BTSerial.write(dis[i]);
        //Return value
    break;
    default:
    break;
    }
  }
}
int Up(){
  servoRight.writeMicroseconds(1300);
  servoLeft.writeMicroseconds(1700);
  //delay(1000);
}
int Down(){
  servoRight.writeMicroseconds(1700);
  servoLeft.writeMicroseconds(1300);
  //delay(1000);  //視情況調整
}
int Right(){
  servoRight.writeMicroseconds(1700);
  servoLeft.writeMicroseconds(1700);
}
int Left(){
  servoRight.writeMicroseconds(1300);
  servoLeft.writeMicroseconds(1300);
}
int stop(){
  servoRight.writeMicroseconds(1500);
  servoLeft.writeMicroseconds(1500);
}