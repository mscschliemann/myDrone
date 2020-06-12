#include <Arduino.h>
#include <MahonyAHRS.h>
#include <Wire.h>

Mahony filter;

#define pySerial_output false

const int MPU = 0x68; // MPU6050 I2C address

float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float AccErrorX, AccErrorY, AccErrorZ, GyroErrorX, GyroErrorY, GyroErrorZ;

float roll, pitch, yaw;

void init_AHRS();
void process_AHRS(float Axyz[], float Gxyz[], float Mxyz[], float *pitch,float *roll,float *yaw);
void calculate_IMU_error();

void init_AHRS() {
  calculate_IMU_error();
  delay(20);
}

void process_AHRS(float Axyz[], float Gxyz[], float Mxyz[], float *pitch,float *roll,float *yaw) {

  Axyz[0] -= AccErrorX;
  Axyz[1] += AccErrorY;
  Gxyz[0] += GyroErrorX;
  Gxyz[1] -= GyroErrorY;
  Gxyz[2] += GyroErrorZ;
    
  filter.begin(25);
  filter.updateIMU(Gxyz[0], Gxyz[1], Gxyz[2], Axyz[0], Axyz[1], Axyz[2]);
    
  *roll = filter.getRoll();
  *pitch = filter.getPitch();
  *yaw = filter.getYaw();

  Serial.print(*roll);
  Serial.print("/");
  Serial.print(*pitch);
  Serial.print("/");
  Serial.println(*yaw);  
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
