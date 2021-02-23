//SG90: 500~2500us
//SG90 degree duration:11.1
//SG90 0 degree =500
//SG90 180 degree =2500

//MG90: 400~2400
//MG90 degree duration:11.1
//MG90 0 degree =400
//MG90 180 degree =2400

//define Servo name
//Servo BottomServo; // 底盤旋轉馬達
//Servo ArmAServo; // 後臂升降馬達
//Servo ArmBServo; // 前臂馬達
//Servo ClampServo; // 夾爪馬達  

//Servo Range
#define Bottom_max  180      //Turn Left
#define Bottom_min  50       //Turn Right
#define Clamp_max  115        //close
#define Clamp_min  60        //open
#define ArmA_max  150
#define ArmA_min  50
#define ArmB_max  180
#define ArmB_min  70

//Servo Initial Degree
int Clamp_pos = Clamp_max;
int Bottom_pos = (Bottom_max + Bottom_min)/2;
int ArmA_pos = ArmA_min;
int ArmB_pos = ArmB_max;

//servo degree duration
int delta = 5;
int delta_bottom = 2;


void Servo_stop()
{
  BottomServo.write(Bottom_pos);
  ClampServo.write(Clamp_pos);
  ArmAServo.write(ArmA_pos);
  ArmBServo.write(ArmB_pos);
}

void BottomLeft()
{
  if(Bottom_pos + delta_bottom < Bottom_max) 
    {Bottom_pos += delta_bottom;}
  MG90(1,Bottom_pos);  
}

void BottomRight()
{
  if(Bottom_pos - delta_bottom > Bottom_min) 
    Bottom_pos -= delta_bottom;
  MG90(1,Bottom_pos);
}

void Arm_A_Up()
{
  if(ArmA_pos + delta < ArmA_max)
    ArmA_pos += delta;
  SG90(2,ArmA_pos);
}

void Arm_A_Down()
{
  if(ArmA_pos - delta > ArmA_min)
    ArmA_pos -= delta;
  SG90(2,ArmA_pos);
}

void Arm_B_Up()
{
  if(ArmB_pos - delta > ArmB_min)
    ArmB_pos -= delta;
  SG90(3,ArmB_pos);
}

void Arm_B_Down()
{
  if(ArmB_pos + delta < ArmB_max)
    ArmB_pos += delta;
  SG90(3,ArmB_pos);
}

void ClampOpen()
{
  Clamp_pos = Clamp_min;
  SG90(4,Clamp_pos);  
}

void ClampClose()
{
  Clamp_pos = Clamp_max;
  SG90(4,Clamp_pos);
}


int SG90(int myServo,int angle)
{
  angle=500+(angle*11.1);

  if(myServo==2)
  {
    ArmAServo.writeMicroseconds(angle);
  }
  if(myServo==3)
  {
    ArmBServo.writeMicroseconds(angle);
  }
  if(myServo==4)
  {
    ClampServo.writeMicroseconds(angle);
  }  
  return(angle);
}



int MG90(int myServo,int angle)
{
  angle=400+(angle*11.1);
  if(myServo==1)
  {
    BottomServo.writeMicroseconds(angle);
  }
  return(angle);
}
