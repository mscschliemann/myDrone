/*
    Arduino and MPU6050 IMU - 3D Visualization Example 
     by Dejan, https://howtomechatronics.com
*/
import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;
Serial myPort;
String data="";
float roll, pitch, yaw;
float speed_1, speed_2, speed_3, speed_4;
void setup() {
  size (1200, 700, P3D);
  myPort = new Serial(this, "COM8", 115200); // starts the serial communication
  myPort.bufferUntil('\n');
}
void draw() {
  translate(width/2, height/2, 0);
  background(0);
  textSize(22);
  //text("Roll: " + (roll) + "      Pitch: " + (pitch) + "        Yaw: " + (yaw), -100, 265);
  text("Pitch: " + (pitch), -500, 250);
  text("Roll:  " + (roll), -500, 275);
  text("Yaw:   " + (yaw), -500, 300);
  text("S2: " + (speed_2), -100, 250);
  text("S3: " + (speed_3), 150, 250);
  text("S4: " + (speed_4), -100, 300);
  text("S1: " + (speed_1), 150, 300);
  //text("S1: " + (speed_1) + "    S2: " + (speed_2) + "        S3: " + (speed_3) + "         S4: " + (speed_4), -100, 300);
  // Rotate the object
  rotateX(radians(pitch));
  rotateZ(radians(roll));
  rotateY(radians(yaw));
  
  // 3D 0bject
  textSize(30);  
  fill(0, 76, 153);
  box (386, 40, 200); // Draw box
  textSize(25);
  fill(255, 255, 255);
  text("vorne", -183, 10, 101);
  //delay(10);
  //println("ypr:\t" + angleX + "\t" + angleY); // Print the values to check whether we are getting proper values
  // Print values to console
  //print(roll);
  //print("\t");
  //print(pitch);
  //print("\t");
  //print(yaw);
  //println();
}
// Read data from the Serial Port
void serialEvent (Serial myPort) { 
  // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
  data = myPort.readStringUntil('\n');
  // if you got any bytes other than the linefeed:
  if (data != null) {
    data = trim(data);
    // split the string at "/"
    String items[] = split(data, '/');
    if (items.length > 1) {
      //--- Roll,Pitch in degrees
      roll = float(items[0]);
      pitch = float(items[1]);
      yaw = float(items[2]);
      speed_1 = float(items[3]);
      speed_2 = float(items[4]);
      speed_3 = float(items[5]);
      speed_4 = float(items[6]);
    }
  }
}
