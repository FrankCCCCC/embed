#include <SoftwareSerial.h> 
// use pins 10 and 11 instead of Tx and Rx pins
SoftwareSerial mySerial(10,11);
int incomingByte = 0; // for incoming serial data

void setup() { 
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

    if (Serial.available() > 0) {
        // read the incoming byte:
        incomingByte = Serial.read();
        
        // say what you got:
        Serial.print("I received: ");
        Serial.println(incomingByte, DEC);
    }
}