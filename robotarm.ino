/* OWI Robotic Arm Control

*/


#include <Adafruit_MotorShield.h>
#include <Wire.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(3);
Adafruit_DCMotor *myMotor4 = AFMS.getMotor(4);

void setup(){
  Serial.begin(9600);
  AFMS.begin(); 
  myMotor1->run(RELEASE);
}
void loop(){
  myMotor1->run(FORWARD);
  myMotor1->setSpeed(15);
  myMotor2->run(FORWARD);
  myMotor2->setSpeed(15);
  myMotor3->run(FORWARD);
  myMotor3->setSpeed(15);
  myMotor4->run(FORWARD);
  myMotor4->setSpeed(15);

  myMotor1->run(RELEASE);
  
  }


