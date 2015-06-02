// Maikko Aleman




#include <SPI.h>

#include <RH_RF22.h>




#include <Wire.h>

#include <Adafruit_Sensor.h>

#include <Adafruit_LSM303_U.h>




#define DS1307_ADDRESS 0x68




/* Singleton instance of the radio driver */

RH_RF22 rf22;

byte zero = 0x00; //workaround for issue #527




/* Unique ID to this sensor at the same time */

Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);




const float Pi = 3.14159;

float yunCompassReading, droneCompassReading, diff, readRSSI, averageRSSI = 0, totalRSSI = 0.0, distance, A;

int ctr = 0, count = 0;




/*const int rotateOnPin = 9;

const int rotateDirectionPin = 7;

const int rotateSpeedPin1 = 6; 

const int rotateSpeedPin2 = 5; */







const int takeOffSig = 3;

const int afterTakeOff = 9;

const int FR = 5;

const int ROT = 6;

const int M = 4;

const int S1 = 7;

const int S2 = 8;

int takeOffState = 0;

int prevTakeOffState=0;

int forward=0;

int displaytoggle=0;




void setup()     

{

  Serial.begin(9600);

  //setDateTime();

  if (!rf22.init()) {

    Serial.println("RFM22B not detected... Check your wiring!");

    while(1);

  }

    pinMode(takeOffSig, INPUT);

    pinMode(afterTakeOff, OUTPUT);

    pinMode(FR, OUTPUT);

    pinMode(ROT, OUTPUT);

    pinMode(M, OUTPUT);

    pinMode(S1, OUTPUT);

    pinMode(S2, OUTPUT);

    digitalWrite(afterTakeOff, LOW); 

    A = computeA();

}




void loop()

{

  //float diff;

  takeOffState = digitalRead(takeOffSig);

  yunCompassReading = getYunCompassReading();

  droneCompassReading = getDroneCompassReading();

  if(droneCompassReading >= 0) {

    Serial.print("------ "); printDate(); Serial.println(" ------");

    diff = getBearingDiff(yunCompassReading, droneCompassReading);

    //for debugging purposes

    Serial.print("YunCompassReading: ");Serial.println(yunCompassReading);

    Serial.print("DroneCompassReading: ");Serial.println(droneCompassReading);

    Serial.print("Difference: ");Serial.println(diff);

    distance = computeDistance();

    Serial.print("RSSI: ");Serial.println(readRSSI); 

    Serial.print("distance: "); Serial.println(distance);

    Serial.print("A:");Serial.println(A);

    Serial.println("---------------------------");

    

  }

  //diff=0;

  sendRotateCommand(diff);

  /*else{

   diff = 0; 

  }*/

  

}













float computeA(void){

  int ctr=0;

  float ave;

  while(ctr<30){

    droneCompassReading = getDroneCompassReading();

    ave = ave + readRSSI;

    ctr++;

  }

  return ave/30;

  

}

float computeDistance(void){

 return pow(10, (-readRSSI+A)/(10*2.4)); 

}




float getYunCompassReading(void) {

  if(!mag.begin()) {

    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");

    while(1);

  }

  

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




void sendRotateCommand(float compassDifference) {

  int polarity = 0;

  

  

  if(takeOffState==1){

    

    if(prevTakeOffState==0){

     

     Serial.println("Starting take off");

      delay(6000);

     Serial.println("Finished take off");

      digitalWrite(afterTakeOff,HIGH); 

       setDateTime();

    }

    prevTakeOffState=1;

  

  

  

  if(compassDifference < 0) {

    compassDifference *= -1;

    polarity = 0;

  }




  else {

    polarity = 1;

  }

  

  if(compassDifference > 15) {

    digitalWrite(FR, HIGH);

    Serial.print("Rotation: ");

    

    if(polarity == 0){

      Serial.println("Counter Clockwise");

      digitalWrite(ROT, LOW);

    }

    else {

      Serial.println("Clockwise");

      digitalWrite(ROT, HIGH);

    }







    if(compassDifference > 65){

      Serial.println("Speed is 1");

      digitalWrite(S1, LOW);

      digitalWrite(S2, LOW);

    }




    else if(compassDifference > 25){

      Serial.println("Speed is 0.6");

      digitalWrite(S1, LOW);

      digitalWrite(S2, HIGH);

    }




    else{

      Serial.println("Speed is 0.3");

      digitalWrite(S1, HIGH);

      digitalWrite(S2, HIGH);

    }




  }




  

  else {

    digitalWrite(FR, LOW);

    Serial.println("Rotation: OFF");

    if(distance>7){ 

       digitalWrite(M, HIGH);

       digitalWrite(S1, LOW);

       digitalWrite(S2, LOW);

       Serial.println("Drone forward. Speed: 0.5"); 

    }

    else if(distance>4){

       digitalWrite(M, HIGH);

       digitalWrite(S1, HIGH);

       digitalWrite(S2, LOW);                                                                                                                                                              

       Serial.println("Drone forward. Speed: 0.3"); 

    }

    else if(distance>1){

       digitalWrite(M, HIGH);

       digitalWrite(S2, HIGH);

       Serial.println("Drone forward. Speed: 0.1");

    }

    else{

       digitalWrite(M, LOW);

       Serial.println("Drone stop"); 

    }

  }

  

  

  }

  else{

   prevTakeOffState=0;

   digitalWrite(afterTakeOff, LOW); 

  }

}




byte decToBcd(byte val){

// Convert normal decimal numbers to binary coded decimal

  return ( (val/10*16) + (val%10) );

}




byte bcdToDec(byte val)  {

// Convert binary coded decimal to normal decimal numbers

  return ( (val/16*10) + (val%16) );

}




void printDate(){

  Wire.begin();

  // Reset the register pointer

  Wire.beginTransmission(DS1307_ADDRESS);

  Wire.write(zero);

  Wire.endTransmission();




  Wire.requestFrom(DS1307_ADDRESS, 7);




  long ms = millis();

  int second = bcdToDec(Wire.read());

  int minute = bcdToDec(Wire.read());

  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time

  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday

  int monthDay = bcdToDec(Wire.read());

  int month = bcdToDec(Wire.read());

  int year = bcdToDec(Wire.read());




  //print the date EG   3/1/11 23:59:59

  /*Serial.print(month);

  Serial.print("/");

  Serial.print(monthDay);

  Serial.print("/");

  Serial.print(year);

  Serial.print(" ");*/

  Serial.print(hour);

  Serial.print(":");

  Serial.print(minute);

  Serial.print(":");

  Serial.print(second);

  Serial.print("::");

  Serial.print(ms%1000);

}




void setDateTime(){

  Wire.begin();

  byte second =      0; //0-59

  byte minute =      0; //0-59

  byte hour =        0; //0-23

  byte weekDay =     0; //1-7

  byte monthDay =    0; //1-31

  byte month =       0; //1-12

  byte year  =       0; //0-99




  Wire.beginTransmission(DS1307_ADDRESS);

  Wire.write(zero); //stop Oscillator




  Wire.write(decToBcd(second));

  Wire.write(decToBcd(minute));

  Wire.write(decToBcd(hour));

  Wire.write(decToBcd(weekDay));

  Wire.write(decToBcd(monthDay));

  Wire.write(decToBcd(month));

  Wire.write(decToBcd(year));




  Wire.write(zero); //start 




  Wire.endTransmission();




}
