#include <Servo.h>

Servo BottomServo; // 底盤旋轉馬達
Servo ArmAServo; // 後臂升降馬達
Servo ArmBServo; // 前臂馬達
Servo ClampServo; // 夾爪馬達  


char command;    //read the char
#define ServoDelayTime 50

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);         //The monitor UART
  Serial1.begin(9600);       //The Bluetooth UART
  
  SetUp_Pin();                //Set the Servo and Motor Pin
  Servo_stop();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial1.available()>0)
  {
    command = Serial1.read();
    Serial1.println(command);
    Serial.println(command);
  }  
    switch (command)
    {
      case 'F':
        CarForward();
        break;
      case 'B':
        CarBack();
        break;
      case 'R':
        CarRight();
        break;
      case 'L':
        CarLeft();
        break;
      case 'S':
        CarStop();
        break;        
      case '0':
        Serial.println("Servo all stop");
        //Servo_stop();
        delay(ServoDelayTime);
        break;
      case '1':
        Serial.println("MeArm turn Left");
        //Servo_stop();
        BottomLeft();
        delay(ServoDelayTime);
        break;
      case '2':
        Serial.println("MeArm turn Right");
        //Servo_stop();
        BottomRight();
        delay(ServoDelayTime);
        break;
      case '3':
        Serial.println("Arm A Up");
        //Servo_stop();
        Arm_A_Up();        
        delay(ServoDelayTime);
        break;
      case '4':
        Serial.println("Arm A Down");
        //Servo_stop();
        Arm_A_Down();      
        delay(ServoDelayTime);
        break;
      case '5':
        Serial.println("Arm B Up");
        //Servo_stop();
        Arm_B_Up();      
        delay(ServoDelayTime);
      break;
      case '6':
        Serial.println("Arm B Down");
        //Servo_stop();
        Arm_B_Down();      
        delay(ServoDelayTime);
        break;
      case '7':
        Serial.println("Clamp Open");
        //Servo_stop();
        ClampOpen();      
        delay(ServoDelayTime);
        break;
      case '8':
        Serial.println("Clamp Close");
        //Servo_stop();
        ClampClose();      
        delay(ServoDelayTime);
        break;      
    }

  if(Serial.available()>0)
  {
    command = Serial.read();
    Serial.println(command);    
  }
}
