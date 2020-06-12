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

const long CAR_CH_MINUS = 0xFFA25D; // repeatable
const long CAR_CH = 0xFF629D; // repeatable
const long CAR_CH_PLUS = 0xFFE21D; // repeatable
const long CAR_REW = 0xFF22DD; // repeatable
const long CAR_PLAY = 0xFFC23D; // repeatable
const long CAR_FOR = 0xFF02FD; // repeatable
const long CAR_MINUS = 0xFFE01F; // repeatable
const long CAR_PLUS = 0xFFA857; // repeatable
const long CAR_EQ = 0xFF906F; // repeatable
const long CAR_0 = 0xFF6897; // repeatable
const long CAR_100 = 0xFF9867; // repeatable
const long CAR_200 = 0xFFB04F; // repeatable
const long CAR_1 = 0xFF30CF; // repeatable
const long CAR_2 = 0xFF18E7; // repeatable
const long CAR_3 = 0xFF7A85; // repeatable
const long CAR_4 = 0xFF10EF; // repeatable
const long CAR_5 = 0xFF38C7; // repeatable
const long CAR_6 = 0xFF5AA5; // repeatable
const long CAR_7 = 0xFF42BD; // repeatable
const long CAR_8 = 0xFF4AB5; // repeatable
const long CAR_9 = 0xFF52AD; // repeatable

const long NAD_ON = 0xE13E0DF2; // repeatable
const long NAD_OFF = 0xE13E8D72; // repeatable
const long NAD_MUTE = 0xE13E05FA; // repeatable
const long NAD_SOURCE_UP = 0xE13E5DA2; // repeatable
const long NAD_SOURCE_DOWN = 0xE13EDD22; // repeatable
const long NAD_VOL_UP = 0xE13E44BB; // repeatable
const long NAD_VOL_DOWN = 0xE13EC43B; // repeatable
const long NAD_PRESET_RIGHT = 0xE13EC53A; // repeatable
const long NAD_PRESET_LEFT = 0xE13E25DA; // repeatable
const long NAD_REW = 0xE13EA05F; // repeatable
const long NAD_PLAY = 0xE13E807F; // repeatable
const long NAD_FOR = 0xE13E609F; // repeatable

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


void setup() {
  Wire.begin();

  Serial.begin(115200);
  while(!Serial);
 
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
    case CAR_CH_MINUS: // SPEED OFF
      update_esc = true;
      speed_1 = 1300;
      speed_2 = 1300;
      speed_3 = 1300;
      speed_4 = 1300;
      break;
    case NAD_ON: // MAX Speed
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
