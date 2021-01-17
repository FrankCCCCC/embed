/**
* Addison Sears-Collins
* June 14, 2020
* This code uses a Kalman Filter on data from the MPU6050 
* IMU sensor chip (built-in to the Keyestudio Balance Shield V3) 
* to calculate the tilt angle and the angular velocity.
* Reference: https://wiki.keyestudio.com/Ks0193_keyestudio_Self-balancing_Car
**/
 
// MPU6050 library
#include <MPU6050.h>   
 
// I2C communication library
#include <Wire.h>        
 
// Create an MPU6050 object named mpu60500
MPU6050 mpu6050;     
 
//Define three-axis acceleration, three-axis gyroscope variables
int16_t ax, ay, az, gx, gy, gz; 
 
// The tilt angle
float Angle;
 
// Angular velocity along each axis as measured by the gyroscope
// The units are degrees per second.
float Gyro_x,Gyro_y,Gyro_z;  
 
///////////////////////Kalman_Filter////////////////////////////
// Covariance of gyroscope noise
float Q_angle = 0.001;  
 
// Covariance of gyroscope drift noise
float Q_gyro = 0.003;  
 
// Covariance of accelerometer
float R_angle = 0.5;    
char C_0 = 1;
 
// The filter sampling time.
float dt = 0.005;
 
// a function containing the Kalman gain is used to 
// calculate the deviation of the optimal estimate.
float K1 = 0.05; 
float K_0,K_1,t_0,t_1;
float angle_err;
 
// Gyroscope drift 
float q_bias;    
 
float accelz = 0;
float angle;
float angle_speed;
 
float Pdot[4] = { 0, 0, 0, 0};
float P[2][2] = {{ 1, 0 }, { 0, 1 }};
float  PCt_0, PCt_1, E;
//////////////////////Kalman_Filter/////////////////////////
 
void setup() 
{
  // Join the I2C bus 
  Wire.begin();    
 
  // Set the baud rate
  Serial.begin(9600); 
 
  // 1.5 second delay
  delay(1500);
 
  // Initialize the MPU6050 sensor
  mpu6050.initialize();   
 
  // Delay 2 milliseconds
  delay(2);  
}
 
void loop() 
{
  Serial.print("Angle = ");
  Serial.print(Angle);
  Serial.print("  K_angle = ");
  Serial.println(angle);
  Serial.print("Gyro_x = ");
  Serial.print(Gyro_x);
  Serial.print("  K_Gyro_x = ");
  Serial.println(angle_speed);
 
  // I2C to get MPU6050 six-axis ax ay az gx gy gz
  mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);     
 
  // Obtain angle and Kalman Filter 
  angle_calculate(ax, ay, az, gx, gy, gz, dt, Q_angle, Q_gyro, R_angle, C_0, K1);      
}
 
/////////////////////////////angle calculate///////////////////////
void angle_calculate(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,float dt,float Q_angle,float Q_gyro,float R_angle,float C_0,float K1)
{
  // Radial rotation angle calculation formula; negative sign is direction processing
  Angle = -atan2(ay , az) * (180/ PI); 
 
  // The X-axis angular velocity calculated by the gyroscope; the negative sign is the direction processing
  Gyro_x = -gx / 131;      
 
  // KalmanFilter 
  Kalman_Filter(Angle, Gyro_x);            
}
////////////////////////////////////////////////////////////////
 
///////////////////////////////KalmanFilter/////////////////////
void Kalman_Filter(double angle_m, double gyro_m)
{
  // Prior estimate
  angle += (gyro_m - q_bias) * dt;          
  angle_err = angle_m - angle;
 
  // Differential of azimuth error covariance
  Pdot[0] = Q_angle - P[0][1] - P[1][0];    
  Pdot[1] = - P[1][1];
  Pdot[2] = - P[1][1];
  Pdot[3] = Q_gyro;
 
  // The integral of the covariance differential of the prior estimate error
  P[0][0] += Pdot[0] * dt;    
  P[0][1] += Pdot[1] * dt;
  P[1][0] += Pdot[2] * dt;
  P[1][1] += Pdot[3] * dt;
   
  // Intermediate variable of matrix multiplication
  PCt_0 = C_0 * P[0][0];
  PCt_1 = C_0 * P[1][0];
   
  // Denominator
  E = R_angle + C_0 * PCt_0;
   
  // Gain value
  K_0 = PCt_0 / E;
  K_1 = PCt_1 / E;
 
  // Intermediate variable of matrix multiplication
  t_0 = PCt_0;  
  t_1 = C_0 * P[0][1];
 
  // Posterior estimation error covariance
  P[0][0] -= K_0 * t_0;     
  P[0][1] -= K_0 * t_1;
  P[1][0] -= K_1 * t_0;
  P[1][1] -= K_1 * t_1;
 
  // Posterior estimation
  q_bias += K_1 * angle_err;    
 
  // The differential value of the output value; work out the optimal angular velocity
  angle_speed = gyro_m - q_bias;   
 
  ////Posterior estimation; work out the optimal angle
  angle += K_0 * angle_err; 
}
