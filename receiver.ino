#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

void setup()
{
    Serial.begin(9600);	// Debugging only
    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) // Non-blocking
    {
	int i;

	// Message with a good checksum received, dump it.
	//driver.printBuffer("Got:", buf, buflen);
        char out[buflen];
        
        for(i=0; i<buflen; i++){
            out[i] = buf[i];
        }
        
        out[buflen] = '\0';

        Serial.println(out);
    }
}
