#define Bottom_pin  3      // 底盤旋轉馬達
#define ArmA_pin  6        // 後臂升降馬達
#define ArmB_pin  9        // 前臂馬達
#define Clamp_pin  10      // 夾爪馬達

#define RightMotor1 4
#define RightMotor2 5
#define LeftMotor1 7
#define LeftMotor2 8

void SetUp_Pin()
{  
  BottomServo.attach(Bottom_pin);
  ArmAServo.attach(ArmA_pin);
  ArmBServo.attach(ArmB_pin);
  ClampServo.attach(Clamp_pin);
  
  //克定馬達正反轉腳位
  pinMode(RightMotor1,OUTPUT);
  pinMode(RightMotor2,OUTPUT);
  pinMode(LeftMotor1,OUTPUT);
  pinMode(LeftMotor2,OUTPUT);
  
  digitalWrite(RightMotor1,LOW);
  digitalWrite(RightMotor2,LOW);
  digitalWrite(LeftMotor1,LOW);
  digitalWrite(LeftMotor2,LOW);  
}
