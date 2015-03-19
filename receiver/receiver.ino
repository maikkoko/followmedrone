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

const int rotateOnPin = 9;
const int rotateDirectionPin = 7;
const int rotateSpeedPin1 = 6;    /*Added by jm*/
const int rotateSpeedPin2 = 5;    /*Added by jm*/

int binary[8];
int negval;
float rcvHeading, transHeading;

void setup() {
    Serial.begin(9600);	  
    if(!mag.begin()) {
        Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
        while(1);
    }
    if (!driver.init())
        Serial.println("init failed");
        
    pinMode(rotateOnPin,OUTPUT);
    pinMode(rotateDirectionPin,OUTPUT);
    pinMode(rotateSpeedPin1, OUTPUT); /*Added by jm*/
    pinMode(rotateSpeedPin2, OUTPUT); /*Added by jm*/
}

void loop() {
    rcvHeading = getCompassReading();
    transHeading = receiveCompassReading();
    //Serial.print("rcv: ");Serial.println(rcvHeading);
    if(transHeading >= 0) {
        float diff = getBearingDiff(rcvHeading, transHeading);  
      
        //for debugging purposes
        Serial.print("rcv: ");Serial.println(rcvHeading);
        Serial.print("trans: ");Serial.println(transHeading);
        Serial.print("diff: ");Serial.println(diff);
        // -end
        
        sendRotateCommand((int)diff);
    }

    delay(500);    
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
    if (trans > rcv) {
        if ((trans - rcv) >= 180)
            return (1)*((rcv - trans) + 360);
        else
            return (-1)*(trans - rcv);
    }
    else {
        if ((rcv - trans) >= 180)
            return (-1)*((trans - rcv) + 360);
        else
            return (rcv - trans);
    }    
}

void sendRotateCommand(int compassDifference) {
    int polarity = 0;
  
    if(compassDifference < 0) {
        compassDifference *= -1;
        polarity = 0;
    }
    else {
      polarity = 1;
    }
    
    if(compassDifference > 10) {
        digitalWrite(rotateOnPin, HIGH);
        Serial.print("Rotation: ");
        if(polarity == 0){
            Serial.println("Counter Clockwise");
            digitalWrite(rotateDirectionPin, LOW);
        }
        else {
            Serial.println("Clockwise");
            digitalWrite(rotateDirectionPin, HIGH);
        }
        /****The whole code from this line is added by JM, any errors after this version is my fault***/
        if(compassDifference > 100){
          Serial.println("Speed is 1");
          digitalWrite(rotateSpeedPin1, LOW);
          digitalWrite(rotateSpeedPin2, LOW);
        }
        else if(compassDifference > 50){
         Serial.println("Speed is 0.5"); 
          digitalWrite(rotateSpeedPin1, LOW);
          digitalWrite(rotateSpeedPin2, HIGH);
        }
        else{
          Serial.println("Speed is 0.1");
          digitalWrite(rotateSpeedPin1, HIGH);
          digitalWrite(rotateSpeedPin2, HIGH);
        }    
        /*** end of the code by JM ***/
    }
    else {
        digitalWrite(rotateOnPin, LOW); 
        Serial.println("Rotation: OFF");
    }
}
