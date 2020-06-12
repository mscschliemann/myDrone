#include <Arduino.h>
#include <MadgwickAHRS.h>
#include <Wire.h>

Madgwick filter;

//unsigned long microsPerReading, microsPrevious;
//unsigned long microsNow;
#define pySerial_output false

const int MPU = 0x68; // MPU6050 I2C address

float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float AccErrorX=0.f, AccErrorY=0.f, AccErrorZ=0.f, GyroErrorX=0.f, GyroErrorY=0.f, GyroErrorZ=0.f;



void init_AHRS();
void process_AHRS(float Axyz[], float Gxyz[], float Mxyz[], float *pitch,float *roll,float *yaw);
void calculate_IMU_error();

void init_AHRS() {
  //calculate_IMU_error();
  delay(20);
}

void process_AHRS(float Axyz[], float Gxyz[], float Mxyz[], float *pitch,float *roll,float *yaw) {
  // Calculating Roll and Pitch from the accelerometer data
  Axyz[0] -= AccErrorX;
  Axyz[1] += AccErrorY;
  Gxyz[0] += GyroErrorX;
  Gxyz[1] -= GyroErrorY;
  Gxyz[2] += GyroErrorZ;
   
  filter.begin(50);

  // initialize variables to pace updates to correct rate
  //microsPerReading = 1000000 / 25;
  //microsPrevious = micros();

  // check if it's time to read data and update the filter
  //microsNow = micros();
  //if (microsNow - microsPrevious >= microsPerReading) {

    // update the filter, which computes orientation
    //filter.updateIMU(Gxyz[0], Gxyz[1], Gxyz[2], Axyz[0], Axyz[1], Axyz[2]);
    filter.update(Gxyz[0], Gxyz[1], Gxyz[2], Axyz[0], Axyz[1], Axyz[2], Mxyz[0], Mxyz[1], Mxyz[2]);

    // print the heading, pitch and roll
    *roll = filter.getRoll();
    *pitch = filter.getPitch();
    *yaw = filter.getYaw();

/*     Serial.print(*roll);
    Serial.print("/");
    Serial.print(*pitch);
    Serial.print("/");
    Serial.println(*yaw); */

    // increment previous time, so we keep proper pace
    //microsPrevious = microsPrevious + microsPerReading;
  //}
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
    AX = (Wire.read() << 8 | Wire.read()); // / 16384.0 ;
    AY = (Wire.read() << 8 | Wire.read()); // / 16384.0 ;
    AZ = (Wire.read() << 8 | Wire.read()); // / 16384.0 ;
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
    GyroErrorX = GyroErrorX + (GX); // / 131.0);
    GyroErrorY = GyroErrorY + (GY); // / 131.0);
    GyroErrorZ = GyroErrorZ + (GZ); // / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / sample_count;
  GyroErrorY = GyroErrorY / sample_count;
  GyroErrorZ = GyroErrorZ / sample_count;
  // Print the error values on the Serial Monitor
  Serial.print("AX: ");
  Serial.println(AccErrorX);
  Serial.print("AY: ");
  Serial.println(AccErrorY);
  Serial.print("AZ: ");
  Serial.println(AccErrorZ);
  Serial.print("GX: ");
  Serial.println(GyroErrorX);
  Serial.print("GY: ");
  Serial.println(GyroErrorY);
  Serial.print("GZ: ");
  Serial.println(GyroErrorZ); 
}