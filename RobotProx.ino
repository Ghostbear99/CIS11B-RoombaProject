#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#define echoPin 9
#define trigPin8

int maximumRange = 200;
int minimumRange = 5;
int duration,distance;
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(3);
void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  AFMS.begin();

}

void loop() {
  myMotor1->setSpeed(150);
  myMotor2->setSpeed(150);
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  delay(1000);
  myMotor1->run(RELEASE);
  myMotor2->run(RELEASE);
  digitalWrite(trigPin,LOW);
  delay(2);
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin,LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration/58.2;
  if(distance <= mininumRange){
    Serial.println("0");
    digitalWrite(trigPin, LOW);
    myMotor3->setSpeed(150);
    myMotor3->run(FORWARD);
    delay(1000);
    myMotor->run(RELEASE);
    while(1) ()
  }else{
    Serial.println(distance);
  }

}
