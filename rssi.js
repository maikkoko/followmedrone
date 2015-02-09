var sys = require('sys');
var exec = require('sync-exec');
//var intervalctr = 0;    						//***********added************/
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
//var controlprocess = require('child_process').exec,   //***********added************/
//    child;									//***********added************/
//var timerID;								//***********added************/

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
//while(1){
	
	
    while(ctr<5){
        randomVar = exec('/usr/bin/getRSSI.lua');
        temp = parseInt(randomVar.stdout);
        rssi = rssi + temp;
        console.log(rssi);
        ctr++;
    }
    ctr=0;
    rssi/=5;
    console.log('Average RSSI:');
    console.log(rssi);
    prevDistance = distance;
    distance = Math.pow(10,((-rssi+A)/(10*2.4)));
    console.log("Previous");
    console.log(prevDistance);
    console.log("Present");
    console.log(distance);	
	
    if(distance>2){
//		child = exec('', function(error,stdout,stderr){		//***********added************/	
//			timerID = setInterval(function(){			//***********added************/
//                intervalctr++;					//***********added************/
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
                console.log(speed);
                client.front(speed);
                console.log('drone move forward');
//			},1000);							//***********added************/
//			if(intervalctr<5){						//***********added************/
//				intervalctr=0;					//***********added************/
//				clearInterval(timerID);				//***********added************/
//			}								//***********added************/
//		});									//***********added************/
        }
    else{
              
//		child = exec('', function(error,stdout,stderr){		//***********added************/
//			timerID = setInterval(function(){				//***********added************/
//				intervalctr++;						//***********added************/
				client.stop();
                console.log('drone stop');
//			},1000);								//***********added************/
//			if(intervalctr<5){						//***********added************/
//				intervalctr=0;					//***********added************/
//				clearInterval(timerID);				//***********added************/
//			}								//***********added************/
//		});										//***********added************/
        }


rssi = 0;
},1500);
//}
},delay);

setTimeout(function(){client.stop();},delay+25000);
setTimeout(function(){client.land();},delay+27000);


