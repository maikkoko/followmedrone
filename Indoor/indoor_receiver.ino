#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#define MAX_SIDE_DISTANCE 0.5
#define MIN_SIDE_DISTANCE 0.1

RH_ASK driver;

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
}

void loop()
{
  takeOffState = digitalRead(takeOffSig);
  forward = digitalRead(rangePin);
  if (getSideDistance()) {
    Serial.print("Front: "); Serial.print(front);
    Serial.print(" | Left: "); Serial.print(left);
    Serial.print(" | Right: "); Serial.println(right); 
    
    displaytoggle=0;
  }
  
  else {
    //Serial.println("NO DATA"); 
  }
  
  sendCommands();   
  
  //Serial.print("Stop: "); Serial.println(digitalRead(rangePin));
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
    //left = 0;
    str = strtok_r(p, ",", &p);
    String strRight(str);
    right = strRight.toFloat();
    //right = 0;

    return 1;
  }
  
  else return 0;
}

void sendUltrasoundTrigger() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

}

void sendCommands() {
  //int displaytoggle = 0;
  if(takeOffState==1){
    
    if(prevTakeOffState==0){
     Serial.println("Starting take off");
      delay(6000);
     Serial.println("Finished take off");
      digitalWrite(afterTakeOff,HIGH); 
    }
    prevTakeOffState=1;
  
    if ((left>MIN_SIDE_DISTANCE)&&(left < MAX_SIDE_DISTANCE) && (right > left)) {
      if(displaytoggle!=1){
        Serial.println("Move right!");
      }
      displaytoggle=1;
      digitalWrite(FS, HIGH);
      digitalWrite(ROT, LOW);
    }
    else if ((right>MIN_SIDE_DISTANCE)&&(right < MAX_SIDE_DISTANCE) && (right < left)) {
      if(displaytoggle!=1){
        Serial.println("Move left!");
      }
      displaytoggle=1;
      digitalWrite(FS, HIGH);
      digitalWrite(ROT, HIGH);
    }
    else{
      //displaytoggle=3;
      if(front>1 || front==0){
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
