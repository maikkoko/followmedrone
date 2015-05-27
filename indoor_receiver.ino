#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

#define MAX_SIDE_DISTANCE 0.4
#define MIN_SIDE_DISTANCE 0.1
#define DS1307_ADDRESS 0x68

RH_ASK driver;
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

byte zero = 0x00; //workaround for issue #527
const float Pi = 3.14159;
float yunCompassReading, droneCompassReading, diff;

float left = 0, right = 0, front = 0;
const int trigger = 12;

const int rangePin = 10;
const int takeOffSig = 3;
const int afterTakeOff = 9;
const int FS = 5;
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
    Serial.begin(9600);	// Debugging only
    if (!driver.init())
         Serial.println("init failed");
         
    pinMode(rangePin, INPUT);
    pinMode(takeOffSig, INPUT);
    pinMode(afterTakeOff, OUTPUT);
    pinMode(FS, OUTPUT);
    pinMode(ROT, OUTPUT);
    pinMode(M, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    digitalWrite(afterTakeOff, LOW);
    //Serial.println("aftertakeoff LOW");

    //setDateTime();
}

void loop()
{
  takeOffState = digitalRead(takeOffSig);
  //Serial.print("takeoff");Serial.println(takeOffState);
  forward = digitalRead(rangePin);
  
  
  
  if (getSideDistance()) {
    yunCompassReading = getYunCompassReading();
    Serial.print("------ "); printDate(); Serial.println(" ------");
    
    diff = getBearingDiff(yunCompassReading, droneCompassReading);
    
    Serial.print("Front: "); Serial.println(front);
    //Serial.print(" | Left: "); Serial.print(left);
    //Serial.print(" | Right: "); Serial.println(right); 
    Serial.print("Drone Compass Reading: "); Serial.println(droneCompassReading);
    Serial.print("Yun Compass Reading: "); Serial.println(yunCompassReading);
    Serial.print("Bearing Diff: "); Serial.println(diff);
    
    displaytoggle=0;
    //diff = 0;
    sendCommands(); 
  }
   
    
}

int getSideDistance() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) {
    sendUltrasoundTrigger();
    
    int i;
    char out[buflen];
        
    for(i=0; i<buflen; i++){
      out[i] = buf[i];
    }
        
    out[buflen] = '\0';
        
    char *p = out;
    char *str;
    
    str = strtok_r(p, ",", &p);
    String strFront(str);
    front = strFront.toFloat();   
    str = strtok_r(p, ",", &p);
    String strLeft(str);
    left = strLeft.toFloat();
    str = strtok_r(p, ",", &p);
    String strRight(str);
    right = strRight.toFloat();
    str = strtok_r(p, ",", &p);
    String strCompass(str);
    droneCompassReading = strCompass.toFloat();

    return 1;
  }
  
  else return 0;
}

void sendUltrasoundTrigger() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigger, LOW);
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
  Serial.print("YUN READING COMPASS: ");Serial.println(heading);
  return heading;
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

void sendCommands() {
  
  
  /**********COMPASSSSSSSSSSS***************/
  
  int polarity = 0;
  
  if(diff < 0) {
    diff *= -1;
    polarity = 0;
  }
  else {
    polarity = 1;
  }
  
  /**********COMPASSSSSSSSSSSS***************/
  
  
  if(takeOffState==1){
    
    if(prevTakeOffState==0){
     Serial.println("Starting take off");
      delay(6000);
     Serial.println("Finished take off");
      digitalWrite(afterTakeOff,HIGH); 
      setDateTime();
    }
    prevTakeOffState=1;
  
    /*if ((left>MIN_SIDE_DISTANCE)&&(left < MAX_SIDE_DISTANCE) && (right > left)) {
      if(displaytoggle!=1){
        Serial.println("Move right!");
      }
      displaytoggle=1;
      digitalWrite(FS, HIGH);
      digitalWrite(M, LOW);
      digitalWrite(ROT, LOW);
    }
    else if ((right>MIN_SIDE_DISTANCE)&&(right < MAX_SIDE_DISTANCE) && (right < left)) {
      if(displaytoggle!=1){
        Serial.println("Move left!");
      }
      displaytoggle=1;
      digitalWrite(FS, HIGH);
      digitalWrite(M, LOW);
      digitalWrite(ROT, HIGH);
    }*/
    
    /*PLACE COMPASS CODE*/
    
    
    
    /*else*/ if(diff > 10) {
      digitalWrite(FS, HIGH);
      //digitalWrite(M, HIGH);
      //Serial.print("Rotation: ");
    
      if(polarity == 0){
        if(displaytoggle!=1){
          Serial.println("Counter Clockwise");
        }
        displaytoggle=1;
        digitalWrite(ROT, LOW);
      }
      else {
        //Serial.println("PUMAPAS
        if(displaytoggle!=1){
          Serial.println("Clockwise");
        }
        displaytoggle=1;
        digitalWrite(ROT, HIGH);
      }
    
      if(diff > 70){
        //Serial.println("Speed is 1");
        digitalWrite(S1, LOW);
        digitalWrite(S2, LOW);
      }
      else if(diff > 25){
        //Serial.println("Speed is 0.5");
        digitalWrite(S1, LOW);
        digitalWrite(S2, HIGH);
      }
      else{
        //Serial.println("Speed is 0.1");
        digitalWrite(S1, HIGH);
        digitalWrite(S2, HIGH);
      }

    }
    
    
    
    
    /*END OF CODE */
    else{
      //displaytoggle=3;
      if(front>5 || front ==0){
         if(displaytoggle!=1){
            Serial.println("Move forward! 0.2");
         } 
         displaytoggle=1;
         digitalWrite(FS, LOW);
         digitalWrite(M, HIGH);
         digitalWrite(S2, LOW);
         digitalWrite(S1, LOW);
      }
      else if(front>2 || front==0){
         if(displaytoggle!=1){
            Serial.println("Move forward! 1.0");
         }
        displaytoggle=1;
       digitalWrite(FS, LOW);
        digitalWrite(M, HIGH);
       digitalWrite(S2, LOW);
        digitalWrite(S1, HIGH); 
      }
      else if(front>1 || front==0){
        if(displaytoggle!=1){
          Serial.println("Move forward!");
        }
        displaytoggle=1;
        digitalWrite(FS, LOW);
        digitalWrite(M, HIGH);
        digitalWrite(S2, HIGH);
      }
      else{
        if(displaytoggle!=1){
          Serial.println("Stop!");
        }
        displaytoggle=1;
        digitalWrite(FS, LOW); 
        digitalWrite(M, LOW);
      }
    }
  
  }
  else{
   prevTakeOffState=0;
   digitalWrite(afterTakeOff, LOW); 
  }

}

/*void sendRotateCommand(int compassDifference) {
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

/****The whole code from this line is added by JM, any errors after this version is my fault
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
/*** end of the code by JM 
 
  else {
    digitalWrite(rotateOnPin, LOW);
    Serial.println("Rotation: OFF");
  }
  
}
***/
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

