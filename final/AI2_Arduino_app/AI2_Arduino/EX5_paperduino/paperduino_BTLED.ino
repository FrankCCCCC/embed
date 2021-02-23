#include <SoftwareSerial.h> //引用SoftwareSerial函式庫

int led = 1;  //LED在DI1
SoftwareSerial BT(3, 4); //設定藍牙在Paperduino上TX、RX的腳位

void setup() {
  BT.begin(9600);//設定與藍芽通訊的鮑率
  pinMode(led, OUTPUT);
}

void loop() {
  int insize;
  byte cmmd[20];
  if ((insize = BT.available() > 0)) //判斷是否有接收到藍牙訊息
  {
    for (int i = 0; i < insize; i++)
      cmmd[i] = BT.read(); //讀取藍牙資訊
    switch (cmmd[0])
    {
      case 'a'://若接受到字元'a'時，則讓LED亮
        digitalWrite(led, HIGH);
        break;
      case 'b'://若接受到字元'b'時，則讓LED亮
        digitalWrite(led, LOW);
        break;
    }
  }
}