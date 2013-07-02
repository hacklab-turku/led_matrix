
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "connection.h"
#include "font.h"



int main(void)
{
    //Init i2c connection
    initConnection();

    //Make matrix where you draw your scene
    uint8_t matrix[16][24];
    //Pass it to the driver
    setScreenMatrix(*matrix, 16, 24);

    //Set all of your devices, so the driver knows
    //who to draw givwn matrix
    setScreenDevice(2,0,0);
    setScreenDevice(4,0,8);
    setScreenDevice(6,0,16);
    setScreenDevice(8,8,0);
    setScreenDevice(10,8,8);
    setScreenDevice(12,8,16);

    srand(23);

    while(1){

        //Draw one led to max at a time
        uint8_t x,y,i;
        for (x = 0; x < 16; x++){
            for (y = 0; y < 24; y++){
				//Put max value to matrix                
				matrix[x][y] = 15;
				//update it to screen
		   		updateScreen();
				_delay_ms(10);
	 	    }
		}
		
		//Fade all leds out
		for (i = 16; i > 0; i--){ 
			for (x = 0; x < 16; x++){
				for (y = 0; y < 24; y++){
				    matrix[x][y] = i - 1;
		 	    }
			}
			//update it to screen
		    updateScreen();
			_delay_ms(100);
		}
    
    //wait a little bit
    _delay_ms(1000);

    }

    return 0;
}


