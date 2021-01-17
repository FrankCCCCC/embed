/**
* Addison Sears-Collins
* June 11, 2020
* This code is used to test the MPU6050 IMU sensor chip that is built-in
* to the Keyestudio Balance Shield V3.
* This 3-axis gyroscope and the 3-axis accelerometer enable
* us to measure the angular velocity (in degrees/second)
* and the linear acceleration (in G-forces, g) of the
* self-balancing robot car.
* 
**/
 
// Arduino's standard library for I2C communication
#include <Wire.h>
 
// Used to store accelerometer data
long accelX, accelY, accelZ;     
float gForceX, gForceY, gForceZ;
 
// Used to store the gyroscope data 
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ; // Angular velocity around those axes
  
void setup() {
 
  // Baud rate
  Serial.begin(9600);
 
  // Initialize I2C communication
  Wire.begin();
 
  setupMPU();
}
  
void loop() {
  recordAccelRegisters();
  recordGyroRegisters();
  printData();
  delay(100); // Delay of 100ms
}
 
/** 
 *  Establish communication between Arduino and the MPU6050.
 *  Set up all registers we'll be using to read MPU6050 data
 */
void setupMPU(){
  // REGISTER 0x6B/REGISTER 107:Power Management 1
   
  // This is the I2C address of the MPU 
  //(b1101000/b1101001 for AC0 low/high datasheet Sec. 9.2)
  // You can do a Google search to find the MPU6050 datasheet.
  Wire.beginTransmission(0b1101000); 
 
  //Accessing the register 6B/107 - Power Management (Sec. 4.28) 
  Wire.write(0x6B); 
 
  //Setting SLEEP register to 0, using the internal 8 Mhz oscillator
  Wire.write(0b00000000); 
  Wire.endTransmission();
  
  // REGISTER 0x1b/REGISTER 27:Gyroscope Configuration
 
  //I2C address of the MPU
  Wire.beginTransmission(0b1101000); 
 
  //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4)
  Wire.write(0x1B);  
 
  // Setting the gyro to full scale +/- 250deg./s
  // (250/360 * 60 = 41.67rpm) 
  // The highest can be converted to 2000deg./s (333.3 rpm), but 
  // gyro sensitivity will be reduced.
  Wire.write(0x00000000); 
  Wire.endTransmission();
   
  // REGISTER 0x1C/REGISTER 28:ACCELEROMETER CONFIGURATION
  //I2C address of the MPU
  Wire.beginTransmission(0b1101000); 
 
  // Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5)
  Wire.write(0x1C); 
 
  // Setting the accel to +/- 2g（if choose +/- 16g，
  // the value would be 0b00011000）
  Wire.write(0b00000000); 
  Wire.endTransmission(); 
}
  
void recordAccelRegisters() {
  // REGISTER 0x3B~0x40/REGISTER 59~64
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accelerometer Readings (see datasheet)
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  
  // Use left shift << and bit operations |  Wire.read() read once 1bytes，and automatically read the data of the next address on the next call.
  while(Wire.available() < 6);  // Waiting for all the 6 bytes data to be sent from the slave machine （Must wait for all data to be stored in the buffer before reading） 
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX （Automatically stored as a defined long value）
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
}
 
 // Divide the raw register reading to get a value that is meaningful for us
void processAccelData(){
  gForceX = accelX / 16384.0;     //float = long / float
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}
  
void recordGyroRegisters() {
  // REGISTER 0x43~0x48/REGISTER 67~72
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 ~ 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processGyroData();
}
 
// The 131.0 comes from the MPU6050 datasheet. It helps us convert
// the raw data into a meaningful value.
void processGyroData() {
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;
}
 
// Print out 3-axis gyroscope data and 3-axis accelerometer data
void printData() {
  Serial.print("Gyro (deg)");
  Serial.print(" X=");
  Serial.print(rotX);
  Serial.print(" Y=");
  Serial.print(rotY);
  Serial.print(" Z=");
  Serial.print(rotZ);
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.println(gForceZ);
}
