#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial I2CBT(10,11);  // RX,  TX

void setup() {
  Serial.begin(9600);
  I2CBT.begin(9600);//bluetooth baud rate
 
  pinMode(13,OUTPUT);
 
}

///////////////////main///////////////////////////

void loop() {

  byte cmmd[20];
  int insize;
  int a=0;
  while(1){
/*
read message from bluetooth
*/
    if ((insize=(I2CBT.available()))>0){
       Serial.print("input size = "); 
       Serial.println(insize);
       for (int i=0; i<insize; i++){
         //cmmd[i]=char(I2CBT.read());
         Serial.print(cmmd[i]=char(I2CBT.read()));
       }
       Serial.println("\n"); 
    }
    if(insize==4){  
      a = (cmmd[0]-48)*10;
      a=a+(cmmd[1]-48);
    }
    if(insize==3){
      a=(cmmd[0]-48);
    }
    Serial.println(a);
    analogWrite(13,map(a,0,80,0,255));
    
  } //while

}
