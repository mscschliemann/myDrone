/**
 * IR Flight Controll - Spielplatz
 *
 * @file	main.cpp
 * @date	2020-05-12
 * @version	v0.2
 * 
 * Features:
 * 
 * Hier ist der Ort, um diversn code isoliert auszuprobieren
 * 
 * 
 * 
 * 
 */

#include <Arduino.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU9250.h"

void setup() {
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    Serial.begin(115200);
    if (!Serial) {}
    Serial.println("Initializing I2C devices...");
}

void loop() {
  // put your main code here, to run repeatedly:

}