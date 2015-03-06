//Maikko A. Aleman 2010-49834

#include <RH_ASK.h>
#include <SPI.h> 
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

RH_ASK driver;
const float Pi = 3.14159;

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

float rcvHeading, transHeading;

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
    rcvHeading = getCompassReading();
    transHeading = receiveCompassReading();
       
    if(transHeading >= 0) {
        Serial.print("rcv: ");Serial.println(rcvHeading);
        Serial.print("trans: ");Serial.println(transHeading);
        float diff = getBearingDiff(rcvHeading, transHeading);
        Serial.print("diff: ");Serial.println(diff);
    }
    
    delay(1000);
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
    
    return heading;
}

float receiveCompassReading(void) {
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) {
	int i;
        char out[buflen];
        
        for(i=0; i<buflen; i++){
            out[i] = buf[i];
        }
        
        out[buflen] = '\0';
        
        String strNum(out);
        return strNum.toFloat();
    }
    
    else 
        return -1.0;
}

float getBearingDiff(float rcv, float trans){
 
     if(abs(rcv - trans) >= 180)
       return ((rcv - trans) + 360);
     else 
       return (-1) * (trans - rcv);    
}
