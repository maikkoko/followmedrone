var linino = require('ideino-linino-lib'),

    board = new linino.Board();




var arDrone = require('ar-drone');

//var sleep = require('sleep.js');

var client = arDrone.createClient();




var pin2 = board.pin.digital.D2;







var pinTakeOffInd = board.pin.digital.D3;

var pinFR = board.pin.digital.D5;

var pinM = board.pin.digital.D4;

var pinROT = board.pin.digital.D6;

var pinS1 = board.pin.digital.D7;

var pinS2 = board.pin.digital.D8;

var pinAfterTakeOff = board.pin.digital.D9;

var pinTL = board.pin.digital.D11;




var bitS2;

var bitFR;

var bitM;

var bitROT;

var bitS1;

var bitAfterTakeOff;

var bitTL;







var sys = require('sys');

//var exec1 = require('sync-exec');










//var exec = require('child_process').exec,

//    child;

    

        

var ctr = 0;

var ctr2 = 0;

var randomVar = 0;

var temp = 0;

var A=0;

var rssi = 0;

var prevDistance = 0;

var distance = 0;

var sentinel = 0;

var buttonState = 0;

var prevBit11 = 0;

var sentinelFR = 0;

var forward_ctr=0;

var prevBitAfterTakeOff=0;

var commandstate = 0;

var rotate_ctr=0;




board.connect( function(){

    

    board.pinMode(pin2, board.MODES.OUTPUT);

    board.pinMode(pinS2, board.MODES.INPUT);

    board.pinMode(pinM, board.MODES.INPUT);

    board.pinMode(pinFR, board.MODES.INPUT);

    board.pinMode(pinROT, board.MODES.INPUT);

    board.pinMode(pinS1, board.MODES.INPUT);

    board.pinMode(pinTL, board.MODES.INPUT);

    board.pinMode(pinTakeOffInd, board.MODES.OUTPUT);

    board.pinMode(pinAfterTakeOff, board.MODES.INPUT);

    




    

    console.log("Calibration ongoing. Stand approximately 1m away from the drone");

    

    

    setTimeout(function(){

        client.ftrim();

        console.log("Flat trim");

    },100);

   

    setTimeout(function(){

        

        board.digitalWrite(pin2, board.HIGH); /*ready or not ready indicator*/

        setInterval(function(){

            

            

            

            bitS2 = board.digitalRead(pinS2); /*speed 11*/ 

            bitM = board.digitalRead(pinM); /*move or not to move 7*/

            bitFR = board.digitalRead(pinFR); /*forward or rotate 5*/

            bitROT = board.digitalRead(pinROT); /*cw or ccw 6*/

            bitS1 = board.digitalRead(pinS1); /*speed 5*/

            bitTL = board.digitalRead(pinTL); /*takeoff/land button 11*/

            bitAfterTakeOff = board.digitalRead(pinAfterTakeOff); /*successful take off signal 9);*/

            

    

    

    

        if(bitTL==1 && prevBit11!=1){    

            if(buttonState==1){

                buttonState=0;

            }

            else{

                buttonState=1;

            }

            console.log("buttonstate");

            console.log(buttonState);

        }

        

    

        prevBit11=bitTL;

            

        if(buttonState==1){

            if(buttonState!=sentinel){

                client.takeoff();

                console.log("drone takeoff"); 

                board.digitalWrite(pinTakeOffInd, board.HIGH);

            }

            if(bitAfterTakeOff==1){

                if(prevBitAfterTakeOff!=1){

                    client.stop();

                    console.log("stop ascending");

                }

                prevBitAfterTakeOff=1;

                if(bitFR==1){

                    

                    

                    if(sentinelFR!=1){ /*Added just now*/

                        client.stop();

                        console.log("sentinel stop");

                        sentinelFR=bitFR; /*up to here*/

                    }

                    else{

                    

                    //if(rotate_ctr<4){

                    

                        if(bitROT==1){

                            if(bitS2==1){

                                if(commandstate!=1){ //*******************COMMAND is 1

                                    client.clockwise(0.3);

                                    console.log('clockwise 0.3');  

                                }

                                commandstate=1;

                            }

                            else{

                                if(bitS1==1){

                                    if(commandstate!=2){ //*******************COMMAND is 2

                                        client.clockwise(0.6);

                                        console.log('clockwise 0.6');

                                    }

                                    commandstate=2;

                                }

                                else{

                                    if(commandstate!=3){ //******************COMMAND is 3

                                        client.clockwise(1);

                                        console.log('clockwise 1');

                                    }

                                    commandstate=3;

                                }

                            }

                        }

                        else{

                            if(bitS2==1){

                                if(commandstate!=4){ //**************COMMAND is 4

                                    client.counterClockwise(0.3);

                                    console.log('counterClockwise 0.3');

                                }

                                commandstate=4;

                            }

                            else{

                                if(bitS1==1){

                                    if(commandstate!=5){ //****************COMMAND is 5

                                        client.counterClockwise(0.6);

                                        console.log('counterClockwise 0.6');

                                    }

                                    commandstate=5;

                                }

                                else{

                                    if(commandstate!=6){ //***************COMMAND is 6

                                        client.counterClockwise(1);

                                        console.log('counterClockwise 1');

                                    }

                                    commandstate=6;

                                }

                            }

                        }

                    /*    rotate_ctr++;

                    }

                    else{

                        client.stop();

                        console.log("rotate stopper");

                        rotate_ctr=0;

                    }

                    

                    */

                    

                    }

                    

                }

                else{

                    

                    if(sentinelFR==1){ /*Added just now*/

                        client.stop();

                        console.log("sentinel stop");

                        sentinelFR=bitFR; /*up to here*/

                    }

                    else{

                    

                    

                        if(bitM==1){

                        

                            //if(forward_ctr<4){

                                if(bitS2==1){

                                    if(commandstate!=7){ //************COMMAND is 7

                                        client.front(0.1);

                                        console.log('forward 0.3');

                                    }

                                    commandstate=7;

                                }

                                else{

                                    if(bitS1==1){

                                        if(commandstate!=8){//***********COMMAND is 8

                                            client.front(0.3);

                                            console.log('forward 0.6');

                                        }

                                        commandstate=8;

                                    }

                                    else{

                                        if(commandstate!=9){ //*********COMMAND is 9;

                                            client.front(0.5);

                                            console.log('forward 1');

                                        }

                                        commandstate=9;

                                    }

                        

                                }

                            /*    forward_ctr=forward_ctr+1;

                            }

                            else{

                                forward_ctr=0;

                                if(commandstate!=10){ //************COMMAND is 10;

                                    client.stop();

                                    console.log('forward stopper');

                                }

                                commandstate=10;

                            }*/

                            

                        }

                        else{

                            if(commandstate!=11){ //**************COMMAND is 11;

                                client.stop();

                                console.log('stop');

                            }

                            commandstate=11;

                        }

                    

                    

                    

                    }

                    

                }

            }

            else{

                //if(prevBitAfterTakeOff==1){

                    //client.up(0.2);

                    //console.log("ASCENDING");

                console.log('waiting for successful takeoff');

                //}

                prevBitAfterTakeOff=0;

                

            }

        } 

        else{

            if(buttonState!=sentinel){

                client.stop();

                console.log("drone stop");

                client.land();

                console.log("drone land");

                board.digitalWrite(pinTakeOffInd, board.LOW);

            }

        }

        

        sentinel=buttonState;

        

        },100);

    },1000);    




});

