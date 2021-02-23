#include <SoftwareSerial.h>
#include <Wire.h>


int LED = 10;
SoftwareSerial I2CBT(2,3);

void setup() {
  Serial.begin(9600);
  I2CBT.begin(9600);//bluetooth baud rate  
  
  pinMode(LED, OUTPUT);    
}

///////////////////main///////////////////////////

void loop() {

  byte cmmd[20];
  int insize;
  
  while(1){
/*
read message from bluetooth
*/
    if ((insize=(I2CBT.available()))>0){
       Serial.print("input size = "); 
       Serial.println(insize);
       for (int i=0; i<insize; i++){
        cmmd[i]=char(I2CBT.read());
         Serial.print((char)cmmd[i]);
         Serial.print("\n"); 
       }
      
    }  
      switch (cmmd[0]) {
        case 97: //"a"     
          
//          digitalWrite(LED,HIGH);
          analogWrite(LED, 64);
          delay(500);
          analogWrite(LED, 128);
          delay(500);
          analogWrite(LED, 196);
          delay(500);
          analogWrite(LED, 255);
          delay(500);
          
          break;  
      case 98://"b"
          
          digitalWrite(LED,LOW);
          
          break;
   
      } //Switch
      
  } //while

}
