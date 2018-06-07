/*
 * automated voice controlled lamp
 * IR sensor to count number of people in the room
 */

#include <SoftwareSerial.h>

const int rxPin = 2;
const int txPin = 3;               
SoftwareSerial mySerial(rxPin, txPin);

String voice;
int relay_bulb = 9;  //trigger pin for the relay. connect to a digital pin. else change cmd to analogWrite

int IR_A = A0;  //analog pin to read distance
//int IR_B = 11;

void setup() 
{
  
  Serial.begin(9600);
  mySerial.begin(9600);
  
  pinMode(relay_bulb, OUTPUT);
  pinMode(IR_A, INPUT);
  //pinMode(IR_B, INPUT);
  digitalWrite(relay_bulb, LOW);
}

void loop()
{
  char ch = 0;
  voice = "";
  while(1)
  {
    while(mySerial.available() <= 0); //do nothing till voice string is captured
    delay(10);
    ch = mySerial.read();
    if(ch == '#')  break;           //<* ... #> is the string format
    voice += ch;   
  }

  /*Serial.print("distance (cm): "); Serial.println(analogRead(IR_A));
  delay(1000);*/
  Serial.println(voice);
  if(voice == "*lamp on") //digitalWrite(relay_bulb, HIGH);
  {
     digitalWrite(relay_bulb, HIGH);
     Serial.println("===========light on");
  }
  else if(voice == "*lamp off") //digitalWrite(relay_bulb, HIGH);
  {
    digitalWrite(relay_bulb, LOW);
    Serial.println("===========light off");
  }
  voice = "";
}
