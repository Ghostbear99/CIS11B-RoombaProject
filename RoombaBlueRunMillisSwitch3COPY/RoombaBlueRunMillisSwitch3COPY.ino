// Stating which libraries shoudl be imported in this code
#include <SoftwareSerial.h>
#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <SPI.h>
#include <MFRC522.h>

// Defining where the motors are plugged into on the motor shield
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(3);
Adafruit_DCMotor *myMotor4 = AFMS.getMotor(4);

// Defining all of the pins that were used on arduino
// Defining pins used on the arduino for bluetooth
#define rxPin 1
#define txPin 0
// Defning pins used on arduino for sensor
#define trigPin 2
#define echoPin 4
// Defining pins used on arduino for roomba
int rorxPin = 10;
int rotxPin = 11;
// Defining pin used on arduino for LED
int ledPin = 13;


// will store last time LED was updated
unsigned long previousMillis = 0;
const long interval = 390;
// Float type variable declaration.
float distance = 0.00;
// variable to receive data from the serial port
char val;
uint8_t i;

//RX,TX HC-06 Bluetooth shield
SoftwareSerial BT(rxPin, txPin);
SoftwareSerial Roomba(rorxPin, rotxPin);

// Configurable, see typical pin layout above
constexpr uint8_t RST_PIN = 5;
// Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 53;
// Create MFRC522 instance
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
// Stores the UID of the card into an array. Makes it easier to store multiple card UIDs
String cardID [] = {"2D:8D:96:D3"};

// Search method that is used to tell the sensor to
// turn on  and detect what distance an object is at.
int Search (void) {                   // Integer type variable declaration.
  float Duration = 0.0;               // Float type variable declaration.
  float CM = 0.0;                     // Float type variable declaration.
  digitalWrite (trigPin, LOW);        // TrigPIN output as 0V (Logic low level).
  delayMicroseconds (2);              // Delay for 2us, Send 10 us high pulse to Ultrasonic Sonar Sensor "TrigPIN".
  digitalWrite (trigPin, HIGH);       // TrigPIN output as 5V (Logic high level).
  delayMicroseconds (10);             // Delay for 10us.
  digitalWrite (trigPin, LOW);        // TrigPIN output as 0V (Logic low level).
  Duration = pulseIn (echoPin, HIGH); // Start counting time, Upto again EchoPIN back to logic "High Level" and puting the "Time" into variable called "Duration".
  CM = (Duration / 58.8);             // Convert Distance into CM.
  return CM;                          // Return to CM.
}

void setup() {

  Serial.begin(9600);      // start serial communication at 9600bps
  pinMode(ledPin, OUTPUT); // Led
  pinMode(rotxPin, OUTPUT); // data Roomba

  Roomba.begin(57600);     // start
  Serial.println ("Sending start command...");
  delay(500);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);

  startSafeMode(); // Start and safe mode Roomba method
  AFMS.begin();
  SPI.begin();
  rfid.PCD_Init();
}

void loop() {

  digitalWrite(ledPin, LOW);
  BT.listen();
  int btVal;

  while ( BT.available() ) // if mySerial data is available to read
  {
    val = Serial.read();         // read it and store it in 'val'
    Serial.println(val);
  }

  while ( Serial.available() )      // if Serial data is available to read
  {
    val = Serial.read();            // echo
    Serial.println(val);
    BT.println(val);
    int btVal = (int) val;

    //tested for equality values
    switch (btVal) {

      case '1':

        startSafeMode(); // Start and safe mode Roomba method
        digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
        Serial.println(btVal);
        ActionForward();

        while (btVal == '1') {
          distance = Search ();
          if ((distance >= 0) && (distance < 5))
          {
            Serial.println("distance >= 0) && (distance < 4");
            if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
              return;
            Serial.println(F("PICC type: "));
            MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
            // Serial.println(rfid.PICC_GetTypeName(piccType));
            // Check is the PICC of Classic MIFARE type
            if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
                piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
                piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
              Serial.println(F("Your tag is not of type MIFARE Classic."));
              return;
            }
            String strID = "";
            for (byte i = 0; i < 4; i++) {
              strID +=
                (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
                String(rfid.uid.uidByte[i], HEX) +
                (i != 3 ? ":" : "");
            }
            strID.toUpperCase();
            Serial.print("Tap card key: ");
            Serial.println(strID);
            delay(1000);
            if (strID.indexOf("2D:8D:96:D3") >= 0) {
              Serial.println("It scanned");

             
              m1forward();
              delay(1000);
              
              m2backward();
              m3forward();
              ActionBackward();
              m2forward();

              m1backward();
              m3backward();



            }
          }
          if (distance <= 9) {
            Serial.println(" In of range <=9");
            while ((distance >= 6) && (distance < 9)) {
              Serial.println("Scanning Card");
              distance = Search ();
              Serial.print(distance);
              Serial.println(" cm");

             
              unsigned long currentMillis = millis();
              if (currentMillis - previousMillis >= interval)
              {

                previousMillis = previousMillis + interval;
                if (previousMillis = 400) {
                  for (int i = 0; i < previousMillis - 390; i++) {
                    goForward();

                  }
                  halt();
                  Serial.println(previousMillis);
                }

              }


             
          }
          else if ((distance >= 10) && (distance < 30)) {
            Serial.println(" In of range <=30***60");
          }

          else if ((distance >= 30) && (distance < 40)) {
            Serial.println(" In of range <=60***90");
            // m3backward();
            //m4forward();
          }

          else {
            Serial.print(distance);
            Serial.println(" cm");
          }
          delay(500);
        }
        break;

      case'2':
        startSafeMode(); // Start and safe mode Roomba method
        digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
        Serial.println("LED On");

        break;

      case'3':
        startSafeMode(); // Start and safe mode Roomba method
        digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
        Serial.println("LED On");
        ActionForward();
        delay(1000);
        ActionBackward();
        break;
      case '4' :
        break;

      default:
        digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
        Serial.println("LED Off");
    }
  }



}

void startSafeMode() {
  Roomba.write(128); // START
  //delay(500);
  Serial.println ("Sending Safe Mode command...");
  Roomba.write(131); // Safe Mode
  //delay(500);
}
// Method that tells the Roomba to go forward in hexadecimal
void goForward() {
  // Code necessary to drive the Roomba Create
  // Telling Roomba to drive to the bytes
  // Roomba understands how to drive in byte

  Serial.println("Go Forward");
  Roomba.write(137);
  Roomba.write((byte)0x00);
  Roomba.write(0xc8);
  Roomba.write(0x80);
  Roomba.write((byte)0x00);

}
// Method that tells the Roomba to go backward in hexadecimal
void goBackward() {
  // LED blinks to let tester know that it is going backwards
  // Byte code telling Roomba create to drive backwards
  Serial.println("Go BACK");
  Roomba.write(137);
  Roomba.write(0xff);
  Roomba.write(0x38);
  Roomba.write(0x80);
  Roomba.write((byte)0x00);
}
// Method that tells the Roomab to go left in hexadecimal
void goLeft() {
  Serial.println("LEFT");
  Roomba.write(137);
  Roomba.write((byte)0x00);
  Roomba.write(0xc8);
  Roomba.write((byte)0x00);
  Roomba.write(0x01);
}
// Method that tells the Roomba to go right in hexadecimal
void goRight() {
  Serial.println("RIGHT");
  Roomba.write(137);
  Roomba.write((byte)0x00);
  Roomba.write(0xc8);
  Roomba.write(0xff);
  Roomba.write(0xff);
}
// Tells the Roomba to stop in hexadecimal
void halt() {
  // Byte code telling Roomba Create to stop moving
  byte j = 0x00;
  Roomba.write(137);
  Roomba.write(j);
  Roomba.write(j);
  Roomba.write(j);
  Roomba.write(j);
  Roomba.end();
}
// Method in which the Roomba moves to the box
void ActionForward() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;
    if (previousMillis = 400) {
      for (int i = 0; i < previousMillis; i++) {
        goForward();
      }
      Serial.println(previousMillis);
    }

    if (previousMillis = 400) {
      for (int i = 0; i < previousMillis - 265; i++) {
        goRight();
      }
      Serial.println(previousMillis);
    }

    if (previousMillis = 400) {
      for (int i = 0; i < previousMillis; i++) {
        goForward();
      }
      Serial.println(previousMillis);
    }

  }
  BT.end();
  halt();
  digitalWrite(ledPin, LOW);
}
// Method that tells the Roomba to go back to its original position
void ActionBackward() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = previousMillis + interval;
    if (previousMillis = 400) {
      for (int i = 0; i < previousMillis + 105; i++) {
        goBackward();
      }
      Serial.println(previousMillis);
    }

    if (previousMillis = 400) {
      for (int i = 0; i < previousMillis - 240; i++) {
        goLeft();
      }
      Serial.println(previousMillis);
    }

    if (previousMillis = 400) {
      for (int i = 0; i < previousMillis + 113; i++) {
        goBackward();
      }
      Serial.println(previousMillis);
    }
  }

  BT.end();
  halt();
  digitalWrite(ledPin, LOW);
}


// Arm motor 1 moves forward
void m1forward() {
  myMotor1->setSpeed(30);


  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;

    if (previousMillis = 900) {
      Serial.println("break");
      for (int i = 0; i < previousMillis; i++) {
        myMotor1->run(BACKWARD);
        Serial.println(previousMillis);
      }
    }
    myMotor1->run(RELEASE);
  }
}

// Arm motor 1 goes backward
void m1backward() {
  myMotor1->setSpeed(30);


  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;

    if (previousMillis = 900) {
      Serial.println("break");
      for (int i = 0; i < previousMillis; i++) {
        myMotor1->run(FORWARD);
        Serial.println(previousMillis);
      }
    }
    myMotor1->run(RELEASE);
  }
}

///Arm motor 2 goes forward
void m2forward() {
  myMotor2->setSpeed(30);


  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;

    if (previousMillis = 400) {
      Serial.println("break");
      for (int i = 0; i < previousMillis; i++) {
        myMotor2->run(FORWARD);
        Serial.println(previousMillis);
      }
    }
    myMotor2->run(RELEASE);
  }
}

// Arm motor 2 goes backward
void m2backward() {
  myMotor2->setSpeed(30);


  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;

    if (previousMillis = 400) {
      Serial.println("break");
      for (int i = 0; i < previousMillis; i++) {
        myMotor2->run(BACKWARD);
        Serial.println(previousMillis);
      }
    }
    myMotor2->run(RELEASE);
  }
}
//Arm motor 3 moves forward
void m3forward() {
  myMotor3->setSpeed(30);


  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;

    if (previousMillis = 150) {
      Serial.println("break");
      for (int i = 0; i < previousMillis; i++) {
        myMotor3->run(BACKWARD);
        Serial.println(previousMillis);
      }
    }
    myMotor3->run(RELEASE);
  }
}
// Arm motor 3 moves backward
void m3backward() {
  myMotor3->setSpeed(30);


  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;

    if (previousMillis = 150) {
      Serial.println("break");
      for (int i = 0; i < previousMillis + 20; i++) {
        myMotor3->run(FORWARD);
        Serial.println(previousMillis);
      }
    }
    myMotor3->run(RELEASE);
  }
}

//Arm motor 4 moves forward
void m4forward() {
  myMotor4->setSpeed(30);


  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;

    if (previousMillis = 200) {
      Serial.println("break");
      for (int i = 0; i < previousMillis; i++) {
        myMotor4->run(BACKWARD);
        Serial.println(previousMillis);
      }
    }
    myMotor4->run(RELEASE);
  }
}
// Arm motor 4 moves backward
void m4backward() {
  myMotor4->setSpeed(30);


  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval)
  {

    previousMillis = previousMillis + interval;

    if (previousMillis = 200) {
      Serial.println("break");
      for (int i = 0; i < previousMillis + 20; i++) {
        myMotor4->run(FORWARD);
        Serial.println(previousMillis);
      }
    }
    myMotor4->run(RELEASE);
  }
}





