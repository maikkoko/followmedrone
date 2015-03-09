var linino = require('ideino-linino-lib'),
    board = new linino.Board();

var compassReading;

var bit2,
    bit3,
    bit4,
    bit5,
    bit6,
    bit7,
    bit8,
    bit9,
    bit10;
    
    
var pin2 = board.pin.digital.D2,
    pin3 = board.pin.digital.D3,
    pin4 = board.pin.digital.D4,
    pin5 = board.pin.digital.D5,
    pin6 = board.pin.digital.D6,
    pin7 = board.pin.digital.D7,
    pin8 = board.pin.digital.D8,
    pin9 = board.pin.digital.D9,
    pin10 = board.pin.digital.D10;

var sys = require('sys');
var exec = require('sync-exec');
var ctr = 0;
var ctr2 = 0;
var rssi = 0;
var temp = 0;
var distance = 0;
var randomVar = 0;
var A=0;
var arDrone = require('ar-drone');
var client = arDrone.createClient();
var delay = 17000;
var prevDistance = 0;
var speed = 1;
var wait = require('wait').wait;
var sleep = require('sleep.js');


var rotSpeed = 30; // (deg/sec)          /*ROTATION*/
var targetDeg = -10;
var droneDeg = 20;
var diffDeg=0;
var delay1=0;
var ctrdelay=0;
//var bit10=1;



board.connect( function(){
    
    
    board.pinMode(pin2, board.MODES.INPUT);
    board.pinMode(pin3, board.MODES.INPUT);
    board.pinMode(pin4, board.MODES.INPUT);
    board.pinMode(pin5, board.MODES.INPUT);
    board.pinMode(pin6, board.MODES.INPUT);
    board.pinMode(pin7, board.MODES.INPUT);
    board.pinMode(pin8, board.MODES.INPUT);
    board.pinMode(pin9, board.MODES.INPUT);
    board.pinMode(pin10, board.MODES.INPUT);
    
     


client.takeoff();
console.log("takeoff");

console.log("Calibration ongoing. Stand approximately 1m away from the drone");


setTimeout(function(){
while(ctr2<30){
    randomVar = exec('/usr/bin/getRSSI.lua');
    temp = parseInt(randomVar.stdout);
    A = A + temp;
    console.log('value of A:');
    console.log(A);
    ctr2++;
}
A/=30;
console.log('Average A:');
console.log(A);

}, 2000);



setTimeout(function(){


    

    console.log('Starting auto-follow...');
    setInterval(function(){
        
            
            bit2=board.digitalRead(pin2);
            bit3=board.digitalRead(pin3)*2;
            bit4=board.digitalRead(pin4)*4;
            bit5=board.digitalRead(pin5)*8;
            bit6=board.digitalRead(pin6)*16;
            bit7=board.digitalRead(pin7)*32;
            bit8=board.digitalRead(pin8)*64;
            bit9=board.digitalRead(pin9)*128;
            bit10=board.digitalRead(pin10);
            
            console.log(bit2);
            console.log(bit3);
            console.log(bit4);
            console.log(bit5);
            console.log(bit6);
            console.log(bit7);
            console.log(bit8);
            console.log(bit9);
            console.log(bit10);
            
            compassReading = bit2+bit3+bit4+bit5+bit6+bit7+bit8+bit9;
            console.log("compassReading");
            console.log(compassReading);
            
            console.log("bit10 value");
            console.log(bit10);
            delay1 = compassReading/rotSpeed;
            
            if(bit10===0){
                console.log("Rotate clockwise");  
                client.clockwise(0.5);
            }
            else{
                console.log("Rotate counterclockwise");
                client.counterClockwise(0.5);
            }
            diffDeg+=40;
            console.log(delay1);
            
        
        
        sleep(delay1*1000);
        client.stop();
        console.log("rotation stop");
        //wait(delay1*1000,function(){client.stop();console.log("rotation stop");});
        
        
            
            
            while(ctr<5){
                randomVar = exec('/usr/bin/getRSSI.lua');
                temp = parseInt(randomVar.stdout);
                rssi = rssi + temp;
                //console.log(rssi);
                ctr++;
            }
            ctr=0;
            rssi/=5;
            console.log('Average RSSI:');
            console.log(rssi);
            prevDistance = distance;
            distance = Math.pow(10,((-rssi+A)/(10*2.4)));
            //console.log("Previous");
            //console.log(prevDistance);
            //console.log("Present");
            //console.log(distance);	
        	
            if(distance>2){
                if((distance-prevDistance)<1){
                    speed=0.2;
                }
                else if((distance-prevDistance)<2){
                    speed=0.5;
                }
                else if((distance-prevDistance)<3){ 
                    speed=0.7;
                }
                else{
                    speed=1;
                }
                //console.log(speed);
                client.front(speed);
                console.log('drone move forward');
            }
            else{
	    		client.stop();
                console.log('drone stop');
            }   
            rssi = 0;
            
            
            
            
            
            
           
            
    },1500);
    
  
    
},delay);

setTimeout(function(){client.stop();},delay+25000);
setTimeout(function(){client.land();},delay+27000);


});

