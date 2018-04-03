#include <SoftwareSerial.h>

int rxPin = 10;
int txPin = 11;
int ledPin = 13;

SoftwareSerial Roomba(rxPin,txPin);

//#define bumpright (sensorbytes[0] & 0x01)
//#define bumpleft  (sensorbytes[0] & 0x02)

void setup() {
  pinMode(ledPin, OUTPUT);   // sets the pins as output
  
  pinMode(txPin, OUTPUT);
  //pinMode(rxPin, INPUT);
  
  Serial.begin(57600);
  Roomba.begin(57600);   
  
  digitalWrite(ledPin, HIGH); // say we're alive
  
  Serial.println ("Sending start command...");
  delay(500);
  Roomba.write(128);              // START
  delay(500);
  
  Serial.println ("Sending Safe Mode command...");
  delay(500);
  Roomba.write(131);              // CONTROL
  delay(500);
  
  digitalWrite(ledPin, LOW);      // say we've finished setup
  Serial.println ("Ready to go!");
  delay (1000);
}

void loop() {
 
  /*ledBlink(); // say we're starting */
  
  Serial.println ("Go Forward");
  delay (500);
  goForward();
  delay (1000);
  
  Serial.println ("Halt!");
  delay (500);
  halt();
  delay (1000);
  
  Serial.println ("Go Backwards");
  delay (500);
  goBackward();
  delay (1000);
  
  Serial.println ("Halt!");
  delay (500);
  halt();
  delay (1000);
  
  while(1) { } // Stop program
}

void goForward() {
 
  Roomba.write(137);   // DRIVE
  Roomba.write((byte)0x00);   // 0x00c8 == 200
  Roomba.write(0xc8);
  Roomba.write(0x80);
  Roomba.write((byte)0x00);
}
void goBackward() {
  ledBlink(); // say we're starting 
  Roomba.write(137);   // DRIVE
  Roomba.write(0xff);   // 0xff38 == -200
  Roomba.write(0x38);
  Roomba.write(0x80);
  Roomba.write((byte)0x00);
}

void halt(){
 byte j = 0x00;
 ledBlink(); // say we're starting 
 Roomba.write(137);   
 Roomba.write(j);   
 Roomba.write(j);
 Roomba.write(j);
 Roomba.write(j);
}

//Blinks the LED twice
void ledBlink() {
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);   
}
