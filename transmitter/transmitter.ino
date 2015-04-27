// Maikko Aleman

#include <SPI.h>
#include <RH_RF22.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Singleton instance of the radio driver */
RH_RF22 rf22;

/* Unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

const float Pi = 3.14159;
float compassReading;

void setup() {  
  Serial.begin(9600);
  if(!mag.begin()) {
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  if (!rf22.init()) {
    Serial.println("No RFM22B detected... Check your wiring!");
    while(1);
  }
}


void loop() {
  compassReading = getCompassReading();
  sendCompassReading(compassReading);
  
  
}

float getCompassReading(void) {
  /* Get a new sensor event */
  sensors_event_t event;
  mag.getEvent(&event);
  // Calculate the angle of the vector y,x
  float heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi;
  // Normalize to 0-360
  if (heading < 0) {
    heading = 360 + heading;
  }
  Serial.print(compassReading); //for debugging
  return heading;
}

void sendCompassReading(float heading) {
  char msg[6];
  dtostrf(heading, 3, 2, msg);
  delay(50);
  
  rf22.send((uint8_t *)msg, strlen(msg));
  rf22.waitPacketSent();
  delay(100);
  
  Serial.println(" Sent!");
}
