// This is the code we use to test the motors of the keyesstudio Self-balancing car kit.
// Source: https://wiki.keyestudio.com/Ks0193_keyestudio_Self-balancing_Car
// Code Modified by Addison Sears-Collins at https://automaticaddison.com
 
// Digital Pins 8 and 12 control the right motor's direction.
// Speed is controlled by Digital Pin 10.
const int right_R1=8;    
const int right_R2=12;
const int PWM_R=10;
 
// Digital Pins 7 and 6 control the left motor's direction.
// Speed is controlled by Digital Pin 9.
const int left_L1=7;
const int left_L2=6;
const int PWM_L=9;
 
 
void setup() 
{
  //set the baud rate to 9600
  Serial.begin(9600);          
 
  // Set all the pins to OUTPUT
  // The motors' speed and direction is controlled by 
  // the output of these pins
  pinMode(right_R1,OUTPUT);     
  pinMode(right_R2,OUTPUT);
  pinMode(PWM_R,OUTPUT);
  pinMode(left_L1,OUTPUT);
  pinMode(left_L2,OUTPUT);
  pinMode(PWM_L,OUTPUT);
}
 
void loop() 
{
  // Go forward for 3 seconds
  digitalWrite(right_R1,HIGH); 
  digitalWrite(right_R2,LOW);
  digitalWrite(left_L1,LOW);
  digitalWrite(left_L2,HIGH);
  analogWrite(PWM_R,100);   // write into PWM value 0~255（speed）
  analogWrite(PWM_L,100);
 
  delay(3000);
 
   // Stop for 3 seconds
  digitalWrite(right_R1,HIGH); 
  digitalWrite(right_R2,HIGH);
  digitalWrite(left_L1,HIGH);
  digitalWrite(left_L2,HIGH);
  analogWrite(PWM_R,100);   // write into PWM value 0~255（speed）
  analogWrite(PWM_L,100);
 
  delay(3000);
 
   // Reverse for 3 seconds
  digitalWrite(right_R1,LOW); 
  digitalWrite(right_R2,HIGH);
  digitalWrite(left_L1,HIGH);
  digitalWrite(left_L2,LOW);
  analogWrite(PWM_R,100);   // write into PWM value 0~255（speed）
  analogWrite(PWM_L,100);   // The higher the PWM value, the faster the speed
 
  delay(3000);
 
  // Stop for 3 seconds
  // Notice that we can do all LOW or all HIGH to stop the robot.
  digitalWrite(right_R1,LOW); 
  digitalWrite(right_R2,LOW);
  digitalWrite(left_L1,LOW);
  digitalWrite(left_L2,LOW);
  analogWrite(PWM_R,100);   // write into PWM value 0~255（speed）
  analogWrite(PWM_L,100);
 
  delay(3000);
   
}