#include <NewPing.h>
#include <SoftwareSerial.h>

// Bluetooth
#define BlueToothRecvPin 2
#define BlueToothSendPin 3
SoftwareSerial BT(BlueToothRecvPin, BlueToothSendPin); // 接收腳, 傳送腳
char val;  // 儲存接收資料的變數

// Ultrasonic
#define UltraSonicMaxDis 200
#define TrigPinR 13                 //Trig Pin of right ultrasonic detector
#define EchoPinR 12                 //Echo Pin of right ultrasonic detector
#define TrigPinL 7                 //Trig Pin of left ultrasonic detector
#define EchoPinL 6                 //Echo Pin of left ultrasonic detector

enum UtltraSonic{D_Right, D_Left};
NewPing sonarR(TrigPinR, EchoPinR, UltraSonicMaxDis);
NewPing sonarL(TrigPinL, EchoPinL, UltraSonicMaxDis);

// Sound Detection
#define Sound_A0 A0                //0687A sound detector' A0 pin correspond to A0 on UNO board

//Motors
//Right Side
//Motor A on right hand side: pin 8
#define MotorAR_1A 8 // Digital Pin, control directions, HIGH: Back, LOW: Forward
//Motor A on right hand side: pin 8
#define MotorAR_1B 10 // Analog pin, control speed
//Left Side
#define MotorBL_1A 9 // Digital Pin, control directions, HIGH: Back, LOW: Forward
#define MotorBL_1B 11 // Analog pin, control speed

enum Direction{Forward, Left, Right, Back, Stop};

void bluetooth_init(){
    Serial.println("BT is ready!");
    BT.begin(9600);    
}

void timer_init(){
  noInterrupts(); // atomic access to timer reg.
  TCCR1A = 0;    TCCR1B = 0;    TCNT1 = 0;
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1<<CS12) | (1<<CS10); // 1024 prescaler
  OCR1A = 7812;  // give 0.5 sec at 16 MHz/1024
//  OCR1A = 20000;  // give 0.5 sec at 16 MHz/1024
  
  TIMSK1 |= (1<<OCIE1A); // enable timer compare int.
  sei(); // enable all interrupts
}

void usonic_init(){
  pinMode(TrigPinR, OUTPUT);
  pinMode(EchoPinR, INPUT);
  pinMode(TrigPinL, OUTPUT);
  pinMode(EchoPinL, INPUT);
}

void motors_init(){
  pinMode(MotorAR_1A,OUTPUT);
  pinMode(MotorAR_1B,OUTPUT);
  pinMode(MotorBL_1A,OUTPUT);
  pinMode(MotorBL_1B,OUTPUT);
}

void sound_det_init(){
  pinMode(Sound_A0, INPUT);
}

void get_bt_msg(){
  if (Serial.available()) {
    val = Serial.read();
    BT.print(val);
  }

  // 若收到藍牙模組的資料，則送到「序列埠監控視窗」
  if (BT.available()) {
    val = BT.read();
    Serial.print(val);
  }
}

int get_dis(UtltraSonic det){
  long duration, cm, inches;

  switch(det){
    case UtltraSonic::D_Right:
//      digitalWrite(TrigPinR, LOW);
//      delayMicroseconds(5);
//      digitalWrite(TrigPinR, HIGH);     // 給 Trig 高電位，持續 10微秒
//      delayMicroseconds(10);
//      digitalWrite(TrigPinR, LOW);
//
//      duration = pulseIn(EchoPinR, HIGH);   // 收到高電位時的時間
        cm = sonarR.ping_cm();
        break;
    case UtltraSonic::D_Left:
//      digitalWrite(TrigPinL, LOW);
//      delayMicroseconds(5);
//      digitalWrite(TrigPinL, HIGH);     // 給 Trig 高電位，持續 10微秒
//      delayMicroseconds(10);
//      digitalWrite(TrigPinL, LOW);
//
//      duration = pulseIn(EchoPinL, HIGH);   // 收到高電位時的時間

        cm = sonarL.ping_cm();
        break;
  }

//  cm = (duration/2) / 29.1;         // 將時間換算成距離 cm 或 inch  
//  inches = (duration/2) / 74; 

//  Serial.print("Distance : ");  
//  Serial.print(cm);
//  Serial.print("cm");
//  Serial.println();
  
//  delay(250);
  return cm;
}

void move(Direction dir, int speed){
  switch(dir){
    case Direction::Forward:
      digitalWrite(MotorAR_1A, LOW);
      analogWrite(MotorAR_1B, speed);
      digitalWrite(MotorBL_1A, LOW);
      analogWrite(MotorBL_1B, speed);
      break;
      
    case Direction::Back:
      digitalWrite(MotorAR_1A, HIGH);
      analogWrite(MotorAR_1B, speed);
      digitalWrite(MotorBL_1A, HIGH);
      analogWrite(MotorBL_1B, speed);
      break;

    case Direction::Right:
      digitalWrite(MotorAR_1A, HIGH);
      analogWrite(MotorAR_1B, -speed);
      digitalWrite(MotorBL_1A, LOW);
      analogWrite(MotorBL_1B, speed);
      break;

   case Direction::Left:
      digitalWrite(MotorAR_1A, LOW);
      analogWrite(MotorAR_1B, speed);
      digitalWrite(MotorBL_1A, HIGH);
      analogWrite(MotorBL_1B, -speed);
      break;
      
    case Direction::Stop:
      digitalWrite(MotorAR_1A, LOW);
      analogWrite(MotorAR_1B, 0);
      digitalWrite(MotorBL_1A, LOW);
      analogWrite(MotorBL_1B, 0);
      break;
  }
}

int sound_det(){
  int snd = analogRead(Sound_A0);
  return snd;
}

ISR(TIMER1_COMPA_vect) { // Timer1 ISR
//  Serial.print("Right: ");
//  get_dis(UtltraSonic::D_Right);
//  Serial.print("Left: ");
//  get_dis(UtltraSonic::D_Left);

    Serial.print("ISR\n");
    get_bt_msg();
}

void test_motors(){
  Serial.print("Forward\n");
  move(Direction::Forward, 255);
  delay(5000);
  
  Serial.print("Stop\n");
  move(Direction::Stop, 0);
  delay(5000);
  
  Serial.print("Back\n");
  move(Direction::Back, 128);
  delay(5000);
  
  Serial.print("Stop\n");
  move(Direction::Stop, 0);
  delay(5000);
}

void sound_det_test(){
  Serial.print("Sound: ");
  Serial.println(sound_det());
  delay(10);
}

void setup() {
  Serial.begin (9600);             // Serial Port begin
  bluetooth_init();
  timer_init();
  usonic_init();
  motors_init();
  sound_det_init();
}

void loop() {
  // put your main code here, to run repeatedly:
//  get_dis(UtltraSonic::D_Right);
//  test_motors();
//  sound_det_test();

//  Serial.print("Right: ");
//  get_dis(UtltraSonic::D_Right);
//  Serial.print("Left: ");
//  get_dis(UtltraSonic::D_Left);
//  delay(250);

//    get_bt_msg();
}
