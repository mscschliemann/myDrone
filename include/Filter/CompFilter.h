#include <Arduino.h>
#include <Wire.h>

#define pySerial_output true

const int MPU = 0x68; // MPU6050 I2C address

float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float AccErrorX, AccErrorY, AccErrorZ, GyroErrorX, GyroErrorY, GyroErrorZ;

float yaw, roll, pitch;
float elapsedTime, currentTime, previousTime;

void init_AHRS();
void process_AHRS(float Axyz[], float Gxyz[], float Mxyz[], float *pitch,float *roll,float *yaw);
void calculate_IMU_error();

void init_AHRS() {
  calculate_IMU_error();
  delay(20);
}

void process_AHRS(float Axyz[], float Gxyz[], float Mxyz[], float *pitch,float *roll,float *yaw) {
  // Calculating Roll and Pitch from the accelerometer data
  Axyz[0] -= AccErrorX;
  Axyz[1] += AccErrorY;
  Gxyz[0] += GyroErrorX;
  Gxyz[1] -= GyroErrorY;
  Gxyz[2] += GyroErrorZ;
  
  accAngleX = (atan(Axyz[1] / sqrt(pow(Axyz[0], 2) + pow(Axyz[2], 2))) * 180 / PI); // - AccErrorX; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * Axyz[0] / sqrt(pow(Axyz[1], 2) + pow(Axyz[2], 2))) * 180 / PI); // + AccErrorY; // AccErrorY ~(-1.58)

  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  //Gxyz[0] = Gxyz[0] + GyroErrorX; // GyroErrorX ~(-0.56)
  //Gxyz[1] = Gxyz[1] - GyroErrorY; // GyroErrorY ~(2)
  //Gxyz[2] = Gxyz[2] + GyroErrorZ; // GyroErrorZ ~ (-0.8)

  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + Gxyz[0] * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + Gxyz[1] * elapsedTime;
  *yaw =  *yaw + Gxyz[2] * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  *roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  *pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
    
  // Print the values on the serial monitor
  if (pySerial_output) {
    Serial.print(*roll);
    Serial.print("/");
    Serial.print(*pitch);
    Serial.print("/");
    Serial.println(*yaw);
  }
}
void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  float GX, GY, GZ, AX, AY, AZ;
  int c = 0;
  int sample_count = 10;
  while (c < sample_count) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX += AX;
    AccErrorY += AY;
    AccErrorZ += AZ; 
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / sample_count;
  AccErrorY = AccErrorY / sample_count;
  AccErrorZ = AccErrorZ / sample_count;

  c = 0;
  // Read gyro values 200 times
  while (c < sample_count) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GX = Wire.read() << 8 | Wire.read();
    GY = Wire.read() << 8 | Wire.read();
    GZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GX / 131.0);
    GyroErrorY = GyroErrorY + (GY / 131.0);
    GyroErrorZ = GyroErrorZ + (GZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / sample_count;
  GyroErrorY = GyroErrorY / sample_count;
  GyroErrorZ = GyroErrorZ / sample_count;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("AccErrorZ: ");
  Serial.println(AccErrorZ);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ); 
}