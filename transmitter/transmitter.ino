#include <RH_ASK.h>
#include <SPI.h> 
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

RH_ASK driver;
const float Pi = 3.14159;

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

void setup() {
    Serial.begin(9600);	  
    if(!mag.begin()) {
        Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
        while(1);
    }
    if (!driver.init())
        Serial.println("init failed");
}

void loop() {
    /* Get a new sensor event */
    sensors_event_t event;
    mag.getEvent(&event);

    // Calculate the angle of the vector y,x
    float heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi;
    // Normalize to 0-360
    if (heading < 0) {
        heading = 360 + heading;
    }
    Serial.print("Compass Heading: ");
    Serial.println(heading);
      
    char msg[6];
    dtostrf(heading, 3, 2, msg);

    delay(300);

    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(200);
}
