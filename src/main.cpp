/**
 * IR Flight Controll
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include "Arduino.h"
#include <IRremote.h>
#include <Servo.h>

const int RECV_PIN = 4;
int speed_1 = 1300;
int speed_2 = 1300;
int speed_3 = 1300;
int speed_4 = 1300;

IRrecv irrecv(RECV_PIN);

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

decode_results results;

const long TRIM_VOR = 0x407f708f;
const long TRIM_RECHTS = 0x407fe01f;
const long TRIM_RUECK = 0x407f10ef;
const long TRIM_LINKS = 0x407fa857;
const long SPEED_UP = 0x407FF00F;
const long SPEED_DOWN = 0x407f58a7;
const long HALT = 0x407F40BF;
const long MAX = 0x407F807F;

long prev = 0x00000000;

boolean update_esc = false;

void serial_output();
void esc_output_pair_trim();
void esc_output_single_trim();

void setup()
{
  // Serial.begin(9600);115200
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver
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
  if (irrecv.decode(&results)) {
    serial_output();
    if (results.value == 0xFFFFFFFF) {
      results.value = prev;
    }
    esc_output_single_trim();
    irrecv.resume(); // Receive the next value
  }
}

void serial_output() {
  switch (results.decode_type) {
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
  }
  Serial.println(results.value, HEX);
}

void esc_output_pair_trim() {
  switch (results.value) {
    case HALT:
      prev = HALT;
      update_esc = true;
      speed_1 = 1300;
      speed_2 = 1300;
      speed_3 = 1300;
      speed_4 = 1300;
      break;
    case MAX:
      prev = MAX;
      update_esc = true;
      speed_1 = 2000;
      speed_2 = 2000;
      speed_3 = 2000;
      speed_4 = 2000;
      break;
    case SPEED_UP:
      prev = SPEED_UP;
      update_esc = true;
      speed_1 += 25;
      speed_2 += 25;
      speed_3 += 25;
      speed_4 += 25;
      break;
    case SPEED_DOWN:
      prev = SPEED_DOWN;
      update_esc = true;
      speed_1 -= 25;
      speed_2 -= 25;
      speed_3 -= 25;
      speed_4 -= 25;
      break;
    case TRIM_VOR: // B, C zu stark
      prev = TRIM_VOR;
      update_esc = true;
      speed_2 -= 1;
      speed_3 -= 1;
      break;
    case TRIM_RUECK: // A, D zu stark
      prev = TRIM_RUECK;
      update_esc = true;
      speed_1 -= 1;
      speed_4 -= 1;
      break;
    case TRIM_RECHTS: // A, C zu stark
      prev = TRIM_RECHTS;
      update_esc = true;
      speed_1 -= 1;
      speed_3 -= 1;
      break;
    case TRIM_LINKS: // B, D zu stark
      prev = TRIM_LINKS;
      update_esc = true;
      speed_2 -= 1;
      speed_4 -= 1;
      break;
    default:
      prev = results.value;
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
  
void esc_output_single_trim() {
  switch (results.value) {
    case HALT:
      prev = HALT;
      update_esc = true;
      speed_1 = 1300;
      speed_2 = 1300;
      speed_3 = 1300;
      speed_4 = 1300;
      break;
    case MAX:
      prev = MAX;
      update_esc = true;
      speed_1 = 2000;
      speed_2 = 2000;
      speed_3 = 2000;
      speed_4 = 2000;
      break;
    case SPEED_UP:
      prev = SPEED_UP;
      update_esc = true;
      speed_1 += 25;
      speed_2 += 25;
      speed_3 += 25;
      speed_4 += 25;
      break;
    case SPEED_DOWN:
      prev = SPEED_DOWN;
      update_esc = true;
      speed_1 -= 25;
      speed_2 -= 25;
      speed_3 -= 25;
      speed_4 -= 25;
      break;
    case TRIM_VOR: // B zu stark
      prev = TRIM_VOR;
      update_esc = true;
      speed_2 -= 1;
      break;
    case TRIM_RUECK: // A stark
      prev = TRIM_RUECK;
      update_esc = true;
      speed_1 -= 1;
      break;
    case TRIM_RECHTS: // C zu stark
      prev = TRIM_RECHTS;
      update_esc = true;
      speed_3 -= 1;
      break;
    case TRIM_LINKS: // D zu stark
      prev = TRIM_LINKS;
      update_esc = true;
      speed_4 -= 1;
      break;
    default:
      prev = results.value;
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