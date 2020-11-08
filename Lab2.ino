/*
 * EECE 4520 Lab 2
 * Programmed by Soumil Kadam, Andrew Bowler, John Caruso with guidance from the built-in examples in Arduino IDE.
 */

#include <Wire.h>
const int mpu = 0x68; // I2C address
int16_t gx, gy, gz;   // gyro

int VRx;
int VRy;
int dataReceived;  // to store the data received from Python through serial port
int itr;           // iteration counter

// the setup routine runs once when you press reset:
void setup() {
  // put your setup code here, to run once:
  VRx = 512; VRy = 512; dataReceived = 0; itr = 0;
  Serial.begin(9600);
  pinMode(4, OUTPUT);  // BUZZER

  Wire.begin();
  Wire.beginTransmission(mpu);
  Wire.write(0x6B);             // PWR_MGMT_1 register
  Wire.write(0);                // Wakes up MPU6050
  Wire.endTransmission(true);
}

// the loop routine runs over and over again forever:
void loop() {
  // put your main code here, to run repeatedly:
  VRx = analogRead(A0);
  VRy = analogRead(A2);

  /* VRx and VRy can range from 0 to 1023. When the joystick is in the resting position, VRx and VRy should be approximately 512.
     However they are not exactly 512 due to precision / accuracy limitations. Hence I have divided the 0 to 1023 range into 3 parts.
     0 to 340, 341 to 682, and 683 to 1023. A left motion will be detected when the joystick is moved into the left 1/3rd part of the range, and so on.
     This is designed so that a slight movement of the joystick doesn't cause an unintentional turn of the snake. */

  if(VRx >= 0 && VRx <= 340) {         // joystick has been moved to the left
    Serial.print("a");                 // if game is played with keyboard, key A is used for left
  }
  else if(VRx >= 683 && VRx <= 1023) { // joystick has been moved to the right
    Serial.print("d");                 // key D = right
  }
  else if(VRy >= 0 && VRy <= 340) {    // joystick is moved up
    Serial.print("w");                 // key W = up
  }
  else if(VRy >= 683 && VRy <= 1023) { // joystick is moved down
    Serial.print("s");                 // key S = down
  }

  // gyro code

  Wire.beginTransmission(mpu);
  Wire.write(0x3B);             // Register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(mpu, 7, true);
  gx = Wire.read()<<8 | Wire.read();
  gy = Wire.read()<<8 | Wire.read();
  gz = Wire.read()<<8 | Wire.read();

  if (gy < -5000) {
    Serial.print("a");
  }
  else if (gy > 5000) {
    Serial.print("d");
  }
  else if (gx > 5000) {
    Serial.print("s");
  }
  else if (gx < -5000) {
    Serial.print("w");
  }

  // gyro code ends

  // the itr variable ensures that the buzzer turns on during an iteration and turns off 2 iterations later

  if(itr == 1) {
    itr++;
  }
  else if(itr >= 2) {
    digitalWrite(4, LOW);          // turn OFF buzzer
    itr = 0;
  }

  if (Serial.available() > 0) {
    dataReceived = Serial.read();
    if(dataReceived == 'F') {      // F indicates food is eaten by snake on the Python side
      digitalWrite(4, HIGH);       // turn ON buzzer
      itr = itr + 1;
    }
  }

  delay(100);
}
