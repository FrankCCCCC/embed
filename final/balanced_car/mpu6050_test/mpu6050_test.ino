/**
* Addison Sears-Collins
* June 13, 2020
* This code is uses the MPU6050 IMU sensor chip that 
* is built-in to the Keyestudio Balance Shield V3 to 
* calculate the angle that the vehicle is tilting.
**/
 
// MPU6050 library
#include <MPU6050.h> 
 
// Arduino's standard library for I2C communication
#include <Wire.h> 
 
// Create an MPU6050 object named mpu6050
MPU6050 mpu6050; 
 
// Define three-axis acceleration and 
// three-axis gyroscope variables
// 16-bit signed integer is the data type
int16_t ax, ay, az, gx, gy, gz;     
 
// Variable that will hold the tilt angle
float Angle;   
 
// Variable that will hold the angle velocity
int16_t Gyro_x;   
 
void setup() {
  // Initialize I2C communication
  Wire.begin(); 
 
  // Set the baud rate
  Serial.begin(9600);   
 
  // Don't do anything for 1500 milliseconds (1.5 seconds)                    
  delay(1500);
 
  // Initialize the MPU6050
  mpu6050.initialize();                       
 
  // Do nothing for 2 milliseconds
  delay(2);
}
 
void loop() {
 
  // I2C to get MPU6050 six-axis data  ax ay az gx gy gz
  mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
 
  // Radial rotation angle calculation formula; 
  // The negative sign indicates the direction.
  // Convert radians to degrees.
  Angle = -atan2(ay , az) * (180/ PI);     
 
  // The x-axis angular velocity calculated by the gyroscope; 
  // The negative sign indicates the direction. The 131 value comes
  // from the MPU6050 datasheet.
  Gyro_x = -gx / 131;  
 
  // Print the tilt angle in degrees
  Serial.print("Angle = ");
  Serial.print(Angle);
 
  // Print the angular velocity in degrees per second
  Serial.print("   Gyro_x = ");
  Serial.println(Gyro_x);
}
