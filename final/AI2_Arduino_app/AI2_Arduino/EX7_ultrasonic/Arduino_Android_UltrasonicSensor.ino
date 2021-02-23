#include <SoftwareSerial.h> //Arduino I2C library

const int trig =  8; //define Arduino pin
const int echo =  9;
const int TX   = 10;
const int RX   = 11;

const int delay_time = 1000; //delay 1000 ms for every measurment

SoftwareSerial BT(RX, TX); 
//set Bluetooth RX (-> pin TX of Bluetooth Module), TX (-> pin RX of Bluetooth Module)

void setup() {
	Serial.begin(9600); //set baud rate of Serial Monitor
	BT.begin(9600); //set baud rate of Bluetooth
	pinMode(trig, OUTPUT); //set trigger pin to OUTPUT (-> pin trig of Ultrasonic Sensor)
	pinMode(echo, INPUT); //set echo pin to INPUT (-> pin echo of Ultrasonic Sensor)
	Serial.println("Start!!!");
}

void loop() {
	float duration; //duration to record the time of every back and forth 
	float distance; 
	digitalWrite(trig, HIGH); //trigger sona for 1 ms
	delayMicroseconds(1000); 
	digitalWrite(trig, LOW);
	duration = pulseIn(echo, HIGH); 
	//use pulseIn() function to get the time of pin echo from HIGH to LOW, unit=microseconds
	distance = (duration / 2) * 0.034;
	//distance = ( half of time of back and forth )x( wave velocity(use 0.034 cm per microsecond) )
	
	Serial.print("Distance ="); //show result on Serial Monitor
	Serial.print(distance);
	Serial.println("cm");
	delay(delay_time);

	int sendData = (int) (distance * 100); //times 100 and convert disance to integer 
	byte packet[3]; 
	packet[0] = 97; //key send to phone
	packet[1] = sendData / 256; //divides sendData to two 1 byte packets
	packet[2] = sendData % 256;
	

	
	if(BT.available() > 0) //check BT is succeed
		if(BT.read() == 97) //check recieve key from phone
		{
			Serial.println("succeed!");
			for(int i = 0; i < 3; i++) 
				BT.write(packet[i]); //send packet to phone
		}


}


