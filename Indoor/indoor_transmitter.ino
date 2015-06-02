#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#define MAX_RANGE 5.0
RH_ASK driver;

const int triggerFront = 8;
const int echoFront = 9;
const int triggerLeft = 6;
const int echoLeft = 7;
const int triggerRight = 10;
const int echoRight = 11;

long distance = 0, distanceLeft = 0, distanceRight = 0;

void setup(){
  Serial.begin(9600);
  
  pinMode(triggerFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(triggerLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(triggerRight, OUTPUT);
  pinMode(echoRight, INPUT);

  if (!driver.init())
    Serial.println("init failed");  
}

void loop(){
  Serial.print("\nDistance in meters: ");
  Serial.print("Front: "); Serial.print(distance*0.00034);
  Serial.print(" | Left: "); Serial.print(distanceLeft*0.0001657);
  Serial.print(" | Right: "); Serial.println(distanceRight*0.0001657);
  
  String d = String(distance*0.00034);
  d += ",";
  d += String(distanceLeft*0.0001657);
  d += ",";
  d += String(distanceRight*0.0001657);
  
  sendUltrasoundReading(d);
  delay(100);
}

int getDistance(int trigger, int echo) {
   digitalWrite(trigger, LOW);
   delayMicroseconds(2);
   
   // Start Measurement
   digitalWrite(trigger, HIGH);
   delayMicroseconds(5);
   digitalWrite(trigger, LOW);
   
   // Acquire and convert to mtrs
   return pulseIn(echo, HIGH, 38000);
}

void sendUltrasoundReading(String reading) {
  char msg[15];
  reading.toCharArray(msg, sizeof(msg));  
  delay(5);
  
  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
  
  Serial.print("Data sent: '"); Serial.print(msg); Serial.println("'");
  distance = getDistance(triggerFront, echoFront);
  distanceLeft = getDistance(triggerLeft, echoLeft);
  distanceRight = getDistance(triggerRight, echoRight);
}
