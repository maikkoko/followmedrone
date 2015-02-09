var sys = require('sys');
var arDrone = require('ar-drone');
var rotSpeed = 10; // (deg/sec)
var targetDeg = -100;
var droneDeg = 20;
var diffDeg;
var delay=0;
var client = arDrone.createClient();
var ctrdelay=0;

client.takeoff();

function rotate(stop,factor,times)
{
  var rotateCallback = function( t, delay )
  {
    return function()
    {
      if ( --t > 0 )
      {
        stop();
        while(ctrdelay<8000000){
            ctrdelay++;
        }
        ctrdelay=0;
        diffDeg = targetDeg-droneDeg; //in degrees
        delay = diffDeg/rotSpeed;
        if(diffDeg>0){
            console.log("Rotate clockwise");  
            client.clockwise(0.3);
        }
        else{
            delay*=-1;
            console.log("Rotate counterclockwise");
            client.counterClockwise(0.3);
        }
        targetDeg+=20;
        console.log(delay);
        setTimeout(rotateCallback, delay * factor );
      }
    };
  }( times, 0 );

    setTimeout( rotateCallback, factor );
}


rotate(function(){console.log("drone stop"); client.stop();},1000,20);
setTimeout(function(){console.log("drone stop");client.stop();},120000);
setTimeout(function(){console.log("drone land");client.land();},121000);

