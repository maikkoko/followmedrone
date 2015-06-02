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
float yunCompassReading, droneCompassReading, readRSSI;

const int rotateOnPin = 9;
const int rotateDirectionPin = 7;
const int rotateSpeedPin1 = 6; /*Added by jm*/
const int rotateSpeedPin2 = 5; /*Added by jm*/

void setup()     
{
  Serial.begin(9600);
  if (!rf22.init()) {
    Serial.println("RFM22B not detected... Check your wiring!");
    while(1);
  }
  if(!mag.begin()) {
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  
}

void loop()
{
  yunCompassReading = getYunCompassReading();
  droneCompassReading = getDroneCompassReading();
  if(droneCompassReading >= 0) {
    float diff = getBearingDiff(yunCompassReading, droneCompassReading);
    //for debugging purposes
    Serial.print("YunCompassReading: ");Serial.println(yunCompassReading);
    Serial.print("DroneCompassReading: ");Serial.println(droneCompassReading);
    Serial.print("Difference: ");Serial.println(diff);
    Serial.print("RSSI: ");Serial.println(readRSSI);
    // -end
    sendRotateCommand((int)diff);
  }
  
  delay(500);
}

float getYunCompassReading(void) {
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

float getDroneCompassReading(void) {
  uint8_t buf[RH_RF22_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf22.waitAvailableTimeout(100)) {    
    if (rf22.recv(buf, &len)) {
      int i;
      char out[len];
      for(i=0; i<len; i++){
        out[i] = buf[i];
      }
      
      out[len] = '\0';
      
      String strNum(out);
      readRSSI = rf22.lastRssi();
      
      return strNum.toFloat();   
    }
    else {
      Serial.println("No data received!");
      return -1.0;
    }
  }
  else {
    return -1.0;
  }
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

  }
/*** end of the code by JM ***/
  
  else {
    digitalWrite(rotateOnPin, LOW);
    Serial.println("Rotation: OFF");
  }
}
