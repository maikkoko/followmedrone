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
}

void loop() {
    rcvHeading = getCompassReading();
    transHeading = receiveCompassReading();
       
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
            return (-1)*((rcv - trans) + 360);
        else
            return (-1)*(trans - rcv);
    }
    else {
        if ((rcv - trans) >= 180)
            return ((trans - rcv) + 360);
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
    }
    else {
        digitalWrite(rotateOnPin, LOW); 
        Serial.println("Rotation: OFF");
    }
}


/*** Previous Implementation ***
void convertToBinary(int num) {
    if (num < 0) {
        negval = 1;
        num = abs(num);
    } 
    else
        negval = 0;
        
    convertNumToBinary(num);
    
    Serial.print(negval);
    for (int i=7; i >= 0; i--) {
        Serial.print(binary[i]);
    }    
    Serial.println();
}

void convertNumToBinary(int number) {
    for(int i = 0; i < 8; i++) {
        if ((number%2) == 1) {
            binary[i] = 1;
        }
        else
            binary[i] = 0;
        
        number = number/2;              
    }
}

void setBinaryPins(){
    digitalWrite(D0, binary[0]);
    digitalWrite(D1, binary[1]);
    digitalWrite(D2, binary[2]);
    digitalWrite(D3, binary[3]);
    digitalWrite(D4, binary[4]);
    digitalWrite(D5, binary[5]);
    digitalWrite(D6, binary[6]);
    digitalWrite(D7, binary[7]);
    digitalWrite(NEG, negval);
}

void setupPins(){
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    pinMode(NEG, OUTPUT); 
}
*/
