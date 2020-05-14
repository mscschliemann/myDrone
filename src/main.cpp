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
#include "bmp280.h"
#include "sensor.h"

const int RECV_PIN = 4;
int speed_1 = 1300;
int speed_2 = 1300;
int speed_3 = 1300;
int speed_4 = 1300;

IRrecv irrecv(RECV_PIN);
decode_results IR_Signal;

Servo servo1, servo2, servo3, servo4;

long prev = 0x00000000;

boolean update_esc = false;
boolean esc_single_trim = false;

void serial_output(decode_results IR_Signal, long prev);
void esc_IR_control(decode_results *IR_Signal, boolean &update_esc, long &prev);
void get_sensor_data();
void calc_PID_calibration();
void esc_PID_trim();

void setup() {
  Wire.begin();

  Serial.begin(115200);
  if(!Serial) {}
 
  Serial.print("Initialized!");

  pinMode(LED_BUILTIN, OUTPUT);

  irrecv.enableIRIn();

  servo1.attach(5);
  servo2.attach(6);
  servo3.attach(9);
  servo4.attach(10);

  servo1.writeMicroseconds(speed_1);
  servo2.writeMicroseconds(speed_2);
  servo3.writeMicroseconds(speed_3);
  servo4.writeMicroseconds(speed_4);


  bmp.delay_ms = delay_ms;
  bmp.dev_id = BMP280_I2C_ADDR_SEC; // device addres = 0x77
  bmp.intf = BMP280_I2C_INTF; // I2C Interface
  bmp.read = i2c_reg_read;
  bmp.write = i2c_reg_write;
  rslt = bmp280_init(&bmp);
  print_rslt(" bmp280_init status", rslt);
  /* Always read the current settings before writing, especially when
   * all the configuration is not modified
   */
  rslt = bmp280_get_config(&conf, &bmp);
  print_rslt(" bmp280_get_config status", rslt);
  conf.filter = BMP280_FILTER_COEFF_2; // Filter Settings
  conf.os_temp = BMP280_OS_4X; // Temperature Resoluition
  conf.os_pres = BMP280_OS_NONE; // Pressure Resolution
  conf.odr = BMP280_ODR_1000_MS; // Sample Rate
  rslt = bmp280_set_config(&conf, &bmp);
  print_rslt(" bmp280_set_config status", rslt);
  /* Always set the power mode after setting the configuration */
  rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);
  print_rslt(" bmp280_set_power_mode status", rslt);
}

void loop() {
  if (irrecv.decode(&IR_Signal)) {
    serial_output(IR_Signal, prev);
    if (IR_Signal.value == 0xFFFFFFFF) {
      IR_Signal.value = prev;
    }
    esc_IR_control(&IR_Signal, update_esc, prev);
    irrecv.resume(); // Receive the next IR value
  }
  get_sensor_data();
  calc_PID_calibration();
  esc_PID_trim();
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

  Serial.print("UT: ");
  Serial.println(ucomp_data.uncomp_temp);
  Serial.print("T32: ");
  Serial.println(temp32);
  Serial.print("T: ");
  Serial.println(temp);
  Serial.println();
  Serial.print("UP: ");
  Serial.println(ucomp_data.uncomp_press);
  Serial.print("P32: ");
  Serial.println(pres32);
  Serial.print("P: ");
  Serial.println(pres);
}

void esc_IR_control(decode_results *IR_Signal, boolean &update_esc, long &prev) {
  switch (IR_Signal->value) {
    case NAD_OFF: // SPEED OFF
      prev = NAD_OFF;
      update_esc = true;
      speed_1 = 1300;
      speed_2 = 1300;
      speed_3 = 1300;
      speed_4 = 1300;
      break;
    case NAD_ON: // MAX Speed
      prev = NAD_ON;
      update_esc = true;
      speed_1 = 2000;
      speed_2 = 2000;
      speed_3 = 2000;
      speed_4 = 2000;
      break;
    case NAD_PRESET_RIGHT: // Speed Up
      //prev = NAD_PRESET_RIGHT; 
      update_esc = true;
      speed_1 += 25;
      speed_2 += 25;
      speed_3 += 25;
      speed_4 += 25;
      break;
    case NAD_PRESET_LEFT: // Speed Down
      // prev = NAD_PRESET_LEFT;
      update_esc = true;
      speed_1 -= 25;
      speed_2 -= 25;
      speed_3 -= 25;
      speed_4 -= 25;
      break;
    case NAD_SOURCE_UP: 
      prev = NAD_SOURCE_UP;
      update_esc = true;
      if (esc_single_trim) {  // B zu stark
        speed_2 -= 1;
      } else {  // B, C zu stark
        speed_2 -= 1;
        speed_3 -= 1;
      }
      break;
    case NAD_VOL_DOWN: 
      prev = NAD_VOL_DOWN;
      update_esc = true;
      if (esc_single_trim) { // A stark
        speed_1 -= 1;
      } else { // A, D zu stark
        speed_1 -= 1;
        speed_4 -= 1;
      }
        break;
    case NAD_VOL_UP: 
      prev = NAD_VOL_UP;
      update_esc = true;
      if (esc_single_trim) { // C zu stark
        speed_3 -= 1;
      } else {  // A, C zu stark
        speed_1 -= 1;
        speed_3 -= 1;
      }
      break;
    case NAD_SOURCE_DOWN: 
      prev = NAD_SOURCE_DOWN;
      update_esc = true;
      if (esc_single_trim) { // D zu stark
        speed_4 -= 1;
      } else {  // B, D zu stark
        speed_2 -= 1;
        speed_4 -= 1;
      }
      break;
    case NAD_MUTE:
      if (esc_single_trim) {
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
      }
      esc_single_trim = !esc_single_trim;
      break;
    default:
      prev = IR_Signal->value;
      update_esc = false;
      break;
  }
  if (update_esc) {
    Serial.println("update ESC....");
    servo1.writeMicroseconds(constrain(speed_1, 1300, 2000));
    servo2.writeMicroseconds(constrain(speed_2, 1300, 2000));
    servo3.writeMicroseconds(constrain(speed_3, 1300, 2000));
    servo4.writeMicroseconds(constrain(speed_4, 1300, 2000));
  }
}

void get_sensor_data() {

  /* Reading the raw data from sensor */
  rslt = bmp280_get_uncomp_data(&ucomp_data, &bmp);

  /* Getting the 32 bit compensated temperature */
  rslt = bmp280_get_comp_temp_32bit(&temp32, ucomp_data.uncomp_temp, &bmp);

  /* Getting the compensated temperature as floating point value */
  rslt = bmp280_get_comp_temp_double(&temp, ucomp_data.uncomp_temp, &bmp);

  /* Getting the compensated pressure using 32 bit precision */
  rslt = bmp280_get_comp_pres_32bit(&pres32, ucomp_data.uncomp_press, &bmp);

  /* Getting the compensated pressure as floating point value */
  rslt = bmp280_get_comp_pres_double(&pres, ucomp_data.uncomp_press, &bmp);
}
void calc_PID_calibration() {

}
void esc_PID_trim() {

}