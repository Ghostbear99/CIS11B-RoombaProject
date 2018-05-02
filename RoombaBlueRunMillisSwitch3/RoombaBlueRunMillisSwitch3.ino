// Includes all of the libraries needed for this code
#include <SoftwareSerial.h>
#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <SPI.h>
#include <MFRC522.h>

// Declares where all of the motors are plugged into the motor shield V2
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(3);
Adafruit_DCMotor *myMotor4 = AFMS.getMotor(4);

// Defines where the bluetooth pins are plugged into the arduino
#define rxPin 1
#define txPin 0
// Defines where the ultrasonic sensor pins are plugged into the arduino
#define trigPin 2
#define echoPin 4
// Defines where the Roomba is plugged into the arduino
int rorxPin = 10;
int rotxPin = 11;
int ledPin = 13;


int y = 400;
// will store last time LED was updated
unsigned long previousMillis = 0;        
const long interval = 390;  
// Float type variable declaration.
float distance = 0.00;            
// variable to receive data from the serial port
char val; 
uint8_t i;

//RX,TX HC-06 Bluetooth shield
SoftwareSerial BT(rxPin,txPin); 
SoftwareSerial Roomba(rorxPin,rotxPin);

// Configurable, see typical pin layout above
constexpr uint8_t RST_PIN = 5;     
// Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 53;    
// Creat MFRC522 instance
MFRC522 rfid(SS_PIN, RST_PIN);  
MFRC522::MIFARE_Key key;
String cardID [] = {"2D:8D:96:D3","2D:8D:96:D4"};

// Integer type variable declaration.
int Search (void) {                   
  // Float type variable declaration.
  float Duration = 0.0;              
  // Float type variable declaration.
  float CM = 0.0;                     
  // TrigPIN output as 0V (Logic low level).
  digitalWrite (trigPin, LOW);        
  // Delay for 2us, Send 10 us high pulse to Ultrasonic Sonar Sensor "TrigPIN".
  delayMicroseconds (2);              
  // TrigPIN output as 5V (Logic high level).
  digitalWrite (trigPin, HIGH);       
  // Delay for 10us.
  delayMicroseconds (10);             
  // TrigPIN output as 0V (Logic low level).
  digitalWrite (trigPin, LOW);        
  // Start counting time, Upto again EchoPIN back to logic "High Level" and puting the "Time" into variable called "Duration".
  Duration = pulseIn (echoPin, HIGH); 
  // Convert Distance into CM.
  CM = (Duration/58.8);               
  // Return to CM.
  return CM;                          
}

void setup() { 
  // start serial communication at 9600bps
  Serial.begin(9600);     
  // Led 
  pinMode(ledPin, OUTPUT); 
  // data Roomba
  pinMode(rotxPin, OUTPUT); 
  
  // start
  Roomba.begin(57600);     
  Serial.println ("Sending start command...");
  delay(500);  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  
  // Start and safe mode Roomba method
  startSafeMode(); 
  AFMS.begin();  
  SPI.begin();
  rfid.PCD_Init();
} 

void loop() { 
 
  digitalWrite(ledPin, LOW);
  BT.listen();
  int btVal;
  // if mySerial data is available to read
  while( BT.available() ) 
  {
    // read it and store it in 'val'
    val = Serial.read();         
    Serial.println(val); 
  }
  // if Serial data is available to read
  while( Serial.available() )      
  {
    // echo
    val = Serial.read();            
    Serial.println(val);
    BT.println(val);    
    int btVal = (int) val;  
     
    //tested for equality values
    switch(btVal){
     
    case '1':
      // Start and safe mode Roomba method       
      startSafeMode(); 
      // turn the LED on (HIGH is the voltage level)
      digitalWrite(ledPin, HIGH);   
      Serial.println(btVal);
      ActionForward();
             
      while(btVal=='1'){
       distance = Search ();
       if ((distance >= 0) && (distance < 5))
             {
              // If the sensor detects an object between a distance of 0-4cm
              // The RFID will attempt to get the tag UID and compare it to another UID
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
          // If the scanned tag UID matches the UID "2D;8D:96:D3" the arm and Roomba will do some actions
          if(strID.indexOf("2D:8D:96:D3")>=0){
          Serial.println("It scanned");
                
//         // The grabber (motor 1) will close around the object    
           m1forward();
            delay(1000);
                //
            // The head (motor 2) will move up
             m2backward();        
            // The 1st motor of the arm (motor 3) will move up
            m3forward();
            // The Roomba will go back to the starting area
            ActionBackward();
            // The head (motor 2) will move down
            m2forward(); 
            // The grabber (motor 1) will release the object  
            m1backward();
            // The 1st motor of the arm (motor 3) will move down
            m3backward();
             
           
           
          }
             }
        // If the sensor detects an object below or equal to 9cm the following actions will occur
       if (distance <= 9) { 
         Serial.println(" In of range <=9"); 
         // If the sensor detects and object between 6-9 the Roomba will drive forward for 10 milliseconds
         while((distance >= 6) && (distance < 9)){
          Serial.println("Scanning Card"); 
           distance = Search ();  
           Serial.print(distance);
           Serial.println(" cm");

           ///
             unsigned long currentMillis = millis();
             if (currentMillis - previousMillis >= interval)
             {
  
              previousMillis = previousMillis + interval;
              if(previousMillis=400){
              for(int i = 0; i<previousMillis-390; i++){  
              goForward();
              
              }
              halt();
               Serial.println(previousMillis);
             }
             
             }
         
         }
   
      }
      // If the sensor detects an object between 10-29cm a message will be printed out to the serial port
      else if ((distance >= 10) && (distance < 30)) { 
        Serial.println(" In of range <=30***60");             
      }
      // If the sensor detects an object between 30-39cm a message will be printed out to the serial port
      else if ((distance >= 30) && (distance < 40)){
        Serial.println(" In of range <=60***90"); 
      
      }
      // If the sensor does not detect and object at the above distanes the distance will be printed to the serial port
      else {
        Serial.print(distance);
        Serial.println(" cm");
      }
      delay(500);
    }
    break;  
     
    case'2':
             // Start and safe mode Roomba method
             startSafeMode(); 
             // turn the LED on (HIGH is the voltage level)
             digitalWrite(ledPin, HIGH);  
             Serial.println("LED On"); 
             ActionBackward();  
             break; 
      
       case'3':
             // Start and safe mode Roomba method
             startSafeMode(); 
             // turn the LED on (HIGH is the voltage level)
             digitalWrite(ledPin, HIGH);  
             Serial.println("LED On");
             ActionForward(); 
             delay(1000);
             ActionBackward();  
             break;
       
             
      default:
      // turn the LED off by making the voltage LOW   
      digitalWrite(ledPin, LOW);    
      Serial.println("LED Off");    
   }
  }

  
  
  } 
  
  void startSafeMode(){  
   // START
   Roomba.write(128); 
   //delay(500);  
   Serial.println ("Sending Safe Mode command...");
   // Safe Mode 
   Roomba.write(131);            
  //delay(500);
  } 
  // Tells Roomba to move forward in hexadecimal
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
// Tells Roomba to go backward in hexadecimal
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
// Tell Roomba to go left in hexadecimal
void goLeft(){
  Serial.println("LEFT");
  Roomba.write(137);
  Roomba.write((byte)0x00);
  Roomba.write(0xc8);
  Roomba.write((byte)0x00);
  Roomba.write(0x01);
}
// Tell Roomba to go right in hexadeciaml
void goRight(){
  Serial.println("RIGHT");
  Roomba.write(137);
  Roomba.write((byte)0x00);
  Roomba.write(0xc8);
  Roomba.write(0xff);
  Roomba.write(0xff);
}
// Tells Roomba to halt in hexadecimal
void halt(){
 // Byte code telling Roomba Create to stop moving
 byte j = 0x00;
 Roomba.write(137);   
 Roomba.write(j);   
 Roomba.write(j);
 Roomba.write(j);
 Roomba.write(j);
 Roomba.end();
}
// Method that gets Roomba to drive to box
void ActionForward(){
    unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  if(previousMillis=400){
  for(int i = 0; i<previousMillis; i++){  
  goForward();}
  Serial.println(previousMillis);
  }
  
  if(previousMillis=400){
  for(int i = 0; i<previousMillis-265; i++){  
  goRight();}  
  Serial.println(previousMillis);
  }

  if(previousMillis=400){
  for(int i = 0; i<previousMillis; i++){  
  goForward();}
  Serial.println(previousMillis);
  }
  
  }
    BT.end();
    halt();
    digitalWrite(ledPin, LOW);
}
// Method that gets Roomba to drive back to its original position
void ActionBackward(){
  unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= interval)
{
  previousMillis = previousMillis + interval;
  if(previousMillis=400){
  for(int i = 0; i<previousMillis+105; i++){  
  goBackward();}
  Serial.println(previousMillis);
  }

  if(previousMillis=400){
  for(int i = 0; i<previousMillis-240; i++){  
  goLeft();}  
  Serial.println(previousMillis);
  }

  if(previousMillis=400){
  for(int i = 0; i<previousMillis+113; i++){  
  goBackward();}
  Serial.println(previousMillis);
  }
  }

    BT.end();
    halt();
    digitalWrite(ledPin, LOW);
}


//Gets arm motor 1 to move forward
void m1forward(){
   myMotor1->setSpeed(30);
  
  
  unsigned long currentMillis = millis();


if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  
  if(previousMillis=900){  
  Serial.println("break");
  for(int i =0; i<previousMillis;i++){
  myMotor1->run(BACKWARD);
  Serial.println(previousMillis);
  }
  }
   myMotor1->run(RELEASE);
}
}
  
// Tells arm motor 1 to move backwards
void m1backward(){
  myMotor1->setSpeed(30);
  
  
  unsigned long currentMillis = millis();


if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  
  if(previousMillis=900){  
  Serial.println("break");
  for(int i =0; i<previousMillis;i++){
  myMotor1->run(FORWARD);
  Serial.println(previousMillis);
  }
  }
   myMotor1->run(RELEASE);
}
}

/// Gets arm motor 2 to move forward
void m2forward(){
  myMotor2->setSpeed(30);
  
  
  unsigned long currentMillis = millis();


if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  
  if(previousMillis=400){  
  Serial.println("break");
  for(int i =0; i<previousMillis;i++){
  myMotor2->run(FORWARD);
  Serial.println(previousMillis);
  }
  }
   myMotor2->run(RELEASE);
}
}  
 
 // Gets arm motor 2 to go backwards
 void m2backward(){
   myMotor2->setSpeed(30);
  
  
  unsigned long currentMillis = millis();


if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  
  if(previousMillis=400){  
  Serial.println("break");
  for(int i =0; i<previousMillis;i++){
  myMotor2->run(BACKWARD);
  Serial.println(previousMillis);
  }
  }
   myMotor2->run(RELEASE);
}
}
//Gets arm motor 3 to move forward 
void m3forward(){
  myMotor3->setSpeed(30);
  
  
  unsigned long currentMillis = millis();


if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  
  if(previousMillis=150){  
  Serial.println("break");
  for(int i =0; i<previousMillis;i++){
  myMotor3->run(BACKWARD);
  Serial.println(previousMillis);
  }
  }
   myMotor3->run(RELEASE);
}
}
// Tells arm motor 3 to move backward
void m3backward(){
  myMotor3->setSpeed(30);
  
  
  unsigned long currentMillis = millis();


if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  
  if(previousMillis=150){  
  Serial.println("break");
  for(int i =0; i<previousMillis+20;i++){
  myMotor3->run(FORWARD);
  Serial.println(previousMillis);
  }
  }
   myMotor3->run(RELEASE);
}
}

// Gets arm motor 4 to go forward
void m4forward(){
  myMotor4->setSpeed(30);
  
  
  unsigned long currentMillis = millis();


if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  
  if(previousMillis=200){  
  Serial.println("break");
  for(int i =0; i<previousMillis;i++){
  myMotor4->run(BACKWARD);
  Serial.println(previousMillis);
  }
  }
   myMotor4->run(RELEASE);
}
}
// Gets arm motor 4 to go backward
void m4backward(){   
  myMotor4->setSpeed(30);
  
  
  unsigned long currentMillis = millis();


if (currentMillis - previousMillis >= interval)
{
  
  previousMillis = previousMillis + interval;
  
  if(previousMillis=200){  
  Serial.println("break");
  for(int i =0; i<previousMillis+20;i++){
  myMotor4->run(FORWARD);
  Serial.println(previousMillis);
  }
  }
   myMotor4->run(RELEASE);
}
}





