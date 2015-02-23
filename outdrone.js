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


var rotSpeed = 10; // (deg/sec)          /*ROTATION*/
var targetDeg = -10;
var droneDeg = 20;
var diffDeg;
var delay1=0;
var ctrdelay=0;




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
	
	
   
            diffDeg = targetDeg-droneDeg; //in degrees
            delay1 = diffDeg/rotSpeed;
            if(diffDeg>0){
                console.log("Rotate clockwise");  
                client.clockwise(0.5);
            }
            else{
                delay1*=-1;
                console.log("Rotate counterclockwise");
                client.counterClockwise(0.5);
            }
            targetDeg+=20;
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



