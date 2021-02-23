


#include <SoftwareSerial.h> //libaray of I2C
SoftwareSerial BT(3,4); //define serial BT  RX=3, TX=4


int time = millis(); //millisceond

void setup()
{
  Serial.begin(9600);
  BT.begin(9600);
  pinMode(13, OUTPUT);
  
}

void loop()
{
  
  
  
  if(BT.available()) //received BT data
  {
    int LED = BT.read();
    Serial.print("Received BT cmd: ");
    Serial.println(LED);
    if(LED == 1)                     //turn on LED if BT cmd is 1
      digitalWrite(13, HIGH);
    else if(LED == 0)                //turn off LED if BT cmd is 0
      digitalWrite(13, LOW); 
     
    
  }
  int now_time = millis();
  if((now_time - time) >= 100)     //actives every 100 milliseconds
  {
    int LED_status = digitalRead(13);  //send the status of LED to smartphone over BT
    BT.write(LED_status); 
    Serial.print("Send LED_status: ");
    Serial.println(LED_status);
    time = now_time;
  }
  
}



