/**
 * IR Flight Controll
 *
 * @file	main.cpp
 * @date	2020-05-12
 * @version	v0.2
 * 
 * Features:
 * - IR Control
 *       - speed-up, speed-down
 *       - trim pair, trim single
 *       - max speed, halt
 * - Sensor Control
 *       - Temperature, Pressure
 */

#include <Arduino.h>
#include <Wire.h>
#include <IRremote.h>
#include <Servo.h>
#include <PID_v1.h>
#include "myMPU9250.h"
#include "AK8963.h"
#include "sensor.h"
//#include "Filter/CompFilter.h"
#include "Filter/MadgwickFilter.h"
//#include "FIlter/MahonyFilter.h"


myMPU9250 mpu9250;
AK8963 ak8963;

const int RECV_PIN = 4;
int16_t speed_1, speed_2, speed_3, speed_4;
int16_t trim_1 = 0;
int16_t trim_2 = 0;
int16_t trim_3 = 0;
int16_t trim_4 = 0;
int16_t throttle = 1300;

float roll, pitch, yaw;

double setpoint_roll = 0, pid_input_roll, pid_output_roll = 0;
double Kp_roll=0.5, Ki_roll=0.1, Kd_roll=0.1;
double setpoint_pitch = 0, pid_input_pitch, pid_output_pitch = 0;
double Kp_pitch=0.5, Ki_pitch=0.1, Kd_pitch=0.1;
double setpoint_yaw = 0, pid_input_yaw, pid_output_yaw = 0;
double Kp_yaw=0, Ki_yaw=0, Kd_yaw=0;

IRrecv irrecv(RECV_PIN);
decode_results IR_Signal;

PID PID_roll(&pid_input_roll, &pid_output_roll, &setpoint_roll, Kp_roll, Ki_roll, Kd_roll, DIRECT);
PID PID_pitch(&pid_input_pitch, &pid_output_pitch, &setpoint_pitch, Kp_pitch, Ki_pitch, Kd_pitch, DIRECT);
PID PID_yaw(&pid_input_yaw, &pid_output_yaw, &setpoint_yaw, Kp_yaw, Ki_yaw, Kd_yaw, DIRECT);


Servo servo1, servo2, servo3, servo4;

long prev = 0x00000000;

bool update_esc = false;

void serial_output(decode_results IR_Signal, long prev);
void esc_IR_control(decode_results *IR_Signal, boolean &update_esc, long &prev);
void process_AHRS(float *pitch,float *roll,float *yaw);
void process_PID(double *pid_output_pitch, double *pid_output_roll, double *pid_output_yaw);

void setup() {
  /** Wire, Serial Setup
   * """"""""""""""""""""
   */
  Wire.begin();

  Serial.begin(115200);
  //while (!Serial);
  Serial.println("Init!");

  /** Infrarot Setup
   * """"""""""""""""
   */
  irrecv.enableIRIn();
  
  /** Servo Setup
   * """"""""""""""
   */
  servo1.attach(5);
  servo2.attach(6);
  servo3.attach(9);
  servo4.attach(10);

  servo1.writeMicroseconds(throttle);
  servo2.writeMicroseconds(throttle);
  servo3.writeMicroseconds(throttle);
  servo4.writeMicroseconds(throttle);
  
  /** Accel, Gyro, Mag Setup
   * """"""""""""""""""""""""
   */
  mpu9250.reset();
  mpu9250.initialize();
  mpu9250.setXGyroOffset();
  mpu9250.setYGyroOffset();
  mpu9250.setZGyroOffset();  
  mpu9250.setXAccelOffset();
  mpu9250.setYAccelOffset();
  mpu9250.setZAccelOffset();

  ak8963.reset();
  ak8963.initialize(); // continous measurement 1 mode

  /** AHRS Setup
   * """"""""""""
   */
  init_AHRS();

  /** PID Setup
   * """"""""""
   */ 
  PID_roll.SetMode(AUTOMATIC);
  PID_pitch.SetMode(AUTOMATIC);
  PID_yaw.SetMode(AUTOMATIC);
  PID_roll.SetOutputLimits(-50.0,50.0);
  PID_pitch.SetOutputLimits(-50.0,50.0);
  PID_yaw.SetOutputLimits(-50.0,50.0);

}

void loop() {

  if (irrecv.decode(&IR_Signal)) {
    serial_output(IR_Signal, prev);
    if (IR_Signal.value == 0xFFFFFFFF) {
      IR_Signal.value = prev;
    }
    esc_IR_control(&IR_Signal, update_esc, prev);
    //if (SHOW_DEBUG_VALUES) debug_output();
    irrecv.resume(); // Receive the next IR value
  }
  mpu9250.getMotion6();
  //ak8963.getMotion3();
/* 
  Serial.print(mpu9250.Axyz[0]);
  Serial.print("/");
  Serial.print(mpu9250.Axyz[1]);
  Serial.print("/");
  Serial.println(mpu9250.Axyz[2]); */

  process_AHRS(mpu9250.Axyz, mpu9250.Gxyz, ak8963.Mxyz, &pitch, &roll, &yaw);
  process_PID(&pid_output_pitch, &pid_output_roll, &pid_output_yaw);

  speed_1 = throttle + trim_1 + pid_output_pitch - pid_output_roll; // + pid_output_yaw;
  speed_2 = throttle + trim_2 - pid_output_pitch + pid_output_roll; // + pid_output_yaw;
  speed_3 = throttle + trim_3 - pid_output_pitch - pid_output_roll; // - pid_output_yaw;
  speed_4 = throttle + trim_4 + pid_output_pitch + pid_output_roll; // - pid_output_yaw;

  Serial.print(roll);
  Serial.print("/");
  Serial.print(pitch);
  Serial.print("/");
  Serial.print(yaw);
  Serial.print("/");
  Serial.print(speed_1);
  Serial.print("/");
  Serial.print(speed_2);
  Serial.print("/");
  Serial.print(speed_3);
  Serial.print("/");
  Serial.println(speed_4);
  //delay(50); 

  // ToDo: low voltage compensation
  //if (update_esc) {
    //Serial.println("ESC update");
    servo1.writeMicroseconds(constrain(speed_1, 1300, 2000));
    servo2.writeMicroseconds(constrain(speed_2, 1300, 2000));
    servo3.writeMicroseconds(constrain(speed_3, 1300, 2000));
    servo4.writeMicroseconds(constrain(speed_4, 1300, 2000));
    //update_esc = false;
  //}
  //delay(500);
}

void serial_output(decode_results IR_Signal, long prev) {
  switch (IR_Signal.decode_type) {
    case NEC:
      Serial.print("NEC: ");
      break;
    case SONY:
      Serial.print("SONY: ");
      break;
    case RC5:
      Serial.print("RC5: ");
      break;
    case RC6:
      Serial.print("RC6: ");
      break;
    case UNKNOWN:
      Serial.print("UNKNOWN: ");
      break;
    default:
      break;
  }
  Serial.println(IR_Signal.value, HEX);
  Serial.println(prev, HEX);
}

void esc_IR_control(decode_results *IR_Signal, boolean &update_esc, long &prev) {
  switch (IR_Signal->value) {
    case NAD_OFF: case CAR_CH_MINUS: // SPEED OFF
      //prev = IR_Signal->value;
      update_esc = true;
      throttle = 1300;
      break;
    case NAD_ON: case CAR_CH_PLUS: // MAX Speed
      //prev = IR_Signal->value;
      update_esc = true;
      throttle = 2000;
      break;     
      case NAD_PRESET_RIGHT: case CAR_PLUS:  // Speed Up
      //prev = IR_Signal->value; 
      update_esc = true;
      throttle += 25;
      break;
    case NAD_PRESET_LEFT: case CAR_MINUS:  // Speed Down
      // prev = IR_Signal->value;
      update_esc = true;
      throttle -= 25;
      break;
    case NAD_SOURCE_UP: case CAR_2: // Pitch front - B, C zu stark
      // prev = IR_Signal->value;
      update_esc = true;
      trim_2 -= 1;
      trim_3 -= 1;
      break;
    case NAD_VOL_DOWN: case CAR_8: // Pitch back - A, D zu stark
      // prev = IR_Signal->value;
      update_esc = true;
      trim_1 -= 1;
      trim_4 -= 1;
      break;
    case NAD_VOL_UP: case CAR_6: // Roll right - A, C zu stark
      // prev = IR_Signal->value;
      update_esc = true;
      trim_1 -= 1;
      trim_3 -= 1;
      break;
    case NAD_SOURCE_DOWN: case CAR_4: // Roll left - B, D zu stark
      // prev = IR_Signal->value;
      update_esc = true;
      trim_2 -= 1;
      trim_4 -= 1;
      break;
    case CAR_1: case CAR_7: // Yaw left - B, A zu stark
      // prev = IR_Signal->value;
      update_esc = true;
      trim_2 -= 1;
      trim_1 -= 1;
      break;
    case CAR_3: case CAR_9: // Yaw right - D, C zu stark
      // prev = IR_Signal->value;
      update_esc = true;
      trim_3 -= 1;
      trim_4 -= 1;
      break;
    case CAR_CH: // update esc
      prev =IR_Signal->value;
      update_esc = true;
      break;
    default:
      prev = IR_Signal->value;
      update_esc = false;
      break;
  }
}

void process_PID(double *pid_output_pitch, double *pid_output_roll, double *pid_output_yaw) {
  pid_input_roll = roll;
  pid_input_pitch = pitch;
  pid_input_yaw = yaw;
  PID_roll.Compute();
  PID_pitch.Compute();
  PID_yaw.Compute();
}