#include <Arduino_FreeRTOS.h>

//LEDs
const int ledPinG = 8;
const int ledPinY = 9;
const int ledPinR = 10;

// Photon detect
const int photoPin0 = A0;
const int photoPin1 = A1;

// Ultrasonic 
const int trigPin = 11;  
const int echoPin = A2;

const int disPins[8] = {2, 3, 4, 5, 6, 7, 13, 12};//pins to 7-seg.
const boolean data[10][8] = { 
// define the pins to light the 3 number: 0, 1, 2
  {true, true, true, true, true, true, false, true}, // 0
  {false, true, true, false, false, false, false, true}, // 1
  {true, true, false, true, true, false, true, true}, // 2
  {true, true, true, true, false, false, true, true}, // 3
  {false, true, true, false, false, true, true, true}, // 4
  {true, false, true, true, false, true, true, true}, // 5
  {true, false, true, true, true, true, true, true}, // 6
  {true, true, true, false, false, false, false, true}, // 7
  {true, true, true, true, true, true, true, true}, // 8
  {true, true, true, true, false, true, true, true}, // 9
};

int speed = 0;
int sonic_dis = 0;
int distance = 0;
int duration = 0;
int is_ignore_photondet = 0;

void disNum(int num){
  for(int i = 0; i < 8; i++){
    digitalWrite(disPins[i], data[num][i] == true? HIGH : LOW);  
  }
}

void disAllNum(){
  for(int i = 0; i < 10; i++){
    disNum(i);
    delay(500);
  }
}

void t_potoDet0(void *pvParameters) {
//  (void) pvParameters;
  
  for( ;; ) // A Task shall never return or exit.
   {
    if(!is_ignore_photondet){
      if(analogRead(photoPin0) - 50 > analogRead(photoPin1)){
        speed++;
        if(speed > 9){speed = 9;}
      }
  //    disNum(speed);
//      delay(500); 
    }
    delay(500); 
   }  
}

void t_potoDet1(void *pvParameters) {
//  (void) pvParameters;
  for( ;; ) // A Task shall never return or exit.
   {
    if(!is_ignore_photondet){
      if(analogRead(photoPin0) < analogRead(photoPin1) - 50){
        speed--;
        if(speed < 0){speed = 0;}
      }
  //    disNum(speed);
//      delay(500); 
    }
    delay(500);
   }
}

void t_disNum(void *pvParameters) {
//  (void) pvParameters;
  for( ;; ) // A Task shall never return or exit.
   {
    // do whatever the task is to do 
    disNum(speed);
    if(speed < 4){
      digitalWrite(ledPinG, HIGH);
      digitalWrite(ledPinY, LOW);
      digitalWrite(ledPinR, LOW);
    }else if(speed >= 4 && speed < 7){
      digitalWrite(ledPinG, LOW);
      digitalWrite(ledPinY, HIGH);
      digitalWrite(ledPinR, LOW);
    }else if(speed >= 7){
      digitalWrite(ledPinG, LOW);
      digitalWrite(ledPinY, LOW);
      digitalWrite(ledPinR, HIGH);
    }
//    delay(300);
   }
}

void t_sonic(void *pvParameters){
  for( ;; ) // A Task shall never return or exit.
  {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  
    pinMode(echoPin, INPUT);             // 讀取 echo 的電位
    duration = pulseIn(echoPin, HIGH);   // 收到高電位時的時間
   
    distance = (duration/2) / 29.1;         // 將時間換算成距離 cm 或 inch  
    if(distance < 15){is_ignore_photondet = 1; speed = 0;}
    else{is_ignore_photondet = 0;}
    Serial.print(distance);
    Serial.print(" | ");
    Serial.print(is_ignore_photondet);
    Serial.print("\n");

    delay(500);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinY, OUTPUT);
  pinMode(ledPinR, OUTPUT);
  
  pinMode(photoPin0, INPUT);
  pinMode(photoPin1, INPUT);

  pinMode(trigPin, OUTPUT);        // 定義輸入及輸出 
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600);
  Serial.print("test");
    
  xTaskCreate(
    t_potoDet0,   // Pointer to function for the task
    "t_potoDet0", // Name for the task
    128,      // Stack size
    NULL,     // NULL task parameter
    1,        // This task will run at priority 1
    NULL );   // Do not use the task handle
  xTaskCreate(t_potoDet1, "t_potoDet1", 128, NULL, 1, NULL );
  xTaskCreate(t_disNum, "t_disNum", 128, NULL, 1, NULL );
  xTaskCreate(t_sonic, "t_sonic", 128, NULL, 1, NULL );

  digitalWrite(ledPinG, LOW);
  digitalWrite(ledPinY, LOW);
  digitalWrite(ledPinR, LOW);
}

void loop() {
 
}
