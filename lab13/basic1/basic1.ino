#include <SoftwareSerial.h> 
#define RXPIN 10
#define TXPIN 11

// use pins 10 and 11 instead of Tx and Rx pins
SoftwareSerial mySerial(RXPIN,TXPIN);
int incomingByte = 0; // for incoming serial data

void setup() { 
  pinMode(RXPIN, INPUT);
  pinMode(TXPIN, OUTPUT);
  
	Serial.begin(9600);		
	mySerial.begin(9600);		
}

void loop(){
    // // useful functions
    // Serial.available();   // UART buffer not empty?
    // mySerial.available();
    // Data = Serial.read(); //get the data in the buffer
    // Data = mySerial.read();
    // Serial.println(data);
    // mySerial.println(data);
    
//    mySerial.println("Enter\r\n");
    if (Serial.available()) {
        // read the incoming byte:
//        incomingByte = mySerial.read();
        String str = Serial.readString();
        
        // say what you send:
        Serial.print("I sended: ");
        Serial.print(str);
//        mySerial.print("I sended: ");
        mySerial.println(str);
    }
    
    if (mySerial.available()) {
        // read the incoming byte:
//        incomingByte = mySerial.read();
        String str = mySerial.readString();
        
        // say what you got:
        Serial.print("I received: ");
        Serial.println(str);
    }
}
