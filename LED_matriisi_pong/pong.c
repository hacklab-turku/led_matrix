
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "connection.h"
#include "nes.h"


#define MAX_X 16
#define MAX_Y 24

#define START_SPEED 0.5



struct ball{
	float pos_x;
	float pos_y;
	uint8_t prev_x;
	uint8_t prev_y;
	float speed;
	float speed_x;
	float speed_y;
};

struct pedal{
	uint8_t pos_x;
	uint8_t got_ball;
	uint8_t score;
};

uint8_t float_to_int(float f){
	uint8_t i = (uint8_t)f;
	if (f - (float)i < 0.5) return i;
	else return i + 1;
}



int main(void)
{
    //Init i2c connection
    initConnection();

    //Make matrix where you draw your scene
    uint8_t matrix[16][24];
    //Pass it to the driver
    setScreenMatrix(*matrix, 16, 24);

    //Set all of your devices, so the driver knows
    //who to draw given matrix
    setScreenDevice(2,0,0);
    setScreenDevice(4,0,8);
    setScreenDevice(6,0,16);
    setScreenDevice(8,8,0);
    setScreenDevice(10,8,8);
    setScreenDevice(12,8,16);

	nessetup();

    srand(23);

	struct ball ball;
	struct pedal player1;
	struct pedal player2;

	uint8_t pass = 0;

	ball.pos_x = 8;
	ball.pos_y = 1.1;
	ball.speed_x = 0;
	ball.speed_y = 0;

	player1.pos_x = 8;
	player1.got_ball = 1;
	player2.pos_x = 8;


    while(1){
		uint8_t x,y; 

		//clear matrix
		for(x = 0; x < MAX_X; x++)
			for(y = 0; y < MAX_Y; y++)
				matrix[x][y] = 0;
		
//Read input
		

		if (!(nes[0] & (1 << NES_BUTTON_UP)))
			if (player1.pos_x < MAX_X - 3) player1.pos_x++;
		if (!(nes[1] & (1 << NES_BUTTON_UP)))
			if (player2.pos_x < MAX_X - 3) player2.pos_x++;
		if (!(nes[0] & (1 << NES_BUTTON_DOWN)))
			if (player1.pos_x > 2) player1.pos_x--;
		if (!(nes[1] & (1 << NES_BUTTON_DOWN)))
			if (player2.pos_x > 2) player2.pos_x--;

		if (!(nes[0] & (1 << NES_BUTTON_A)) && player1.got_ball){
			player1.got_ball = 0;
			ball.speed = START_SPEED;
			ball.speed_y = 0.1*(player1.pos_x - 8);
			ball.speed_x = ball.speed*ball.speed - ball.speed_y*ball.speed_y;
		}
		if (!(nes[1] & (1 << NES_BUTTON_A)) && player2.got_ball){
			player2.got_ball = 0;
			ball.speed = START_SPEED;
			ball.speed_y = 0.1*(player1.pos_x - 8);
			ball.speed_x = -(ball.speed*ball.speed - ball.speed_y*ball.speed_y);
		}

		if (player1.got_ball){
			ball.pos_x = player1.pos_x;
			ball.pos_y = 1;
		}else if(player2.got_ball){
			ball.pos_x = player2.pos_x;
			ball.pos_y = MAX_Y - 2;
		}else{
			ball.pos_x += ball.speed_x;
			ball.pos_y += ball.speed_y;		
		}

		//Hit on boarder

		if (ball.pos_x > MAX_X-1){
			//ball.speed_x = -ball.speed_x;
			//ball.pos_x = MAX_X-1;
			player2.got_ball = 1;
			pass = 0;
			player1.score++;
		}
		if (ball.pos_x < 0){
			//ball.speed_x = -ball.speed_x;
			//ball.pos_x = 0;
			player1.got_ball = 1;
			pass = 0;
			player2.score++;
		}

		if (ball.pos_y > MAX_Y-1){
			ball.speed_y = -ball.speed_y;
			ball.pos_y = MAX_Y-1;
		}
 		if (ball.pos_y < 0){
	 		ball.speed_y = -ball.speed_y;			
			ball.pos_y = 0;
		}

		//Hit on pedal		

		if (ball.pos_y < 1 && player1.pos_x + 2 > ball.pos_x && player1.pos_x - 2 > ball.pos_x){
			int8_t diff = player1.pos_x - ball.pos_x;
			ball.speed_y = diff * 0.2;
			ball.speed_x = ball.speed*ball.speed - ball.speed_y*ball.speed_y;
			ball.pos_y = 1;
			pass++;
		} 
		if (ball.pos_y > MAX_Y - 2 && player2.pos_x + 2 > ball.pos_x && player2.pos_x - 2 > ball.pos_x){
			int8_t diff = player2.pos_x - ball.pos_x;
			ball.speed_y = diff * 0.2;
			ball.speed_x = -ball.speed*ball.speed - ball.speed_y*ball.speed_y;
			ball.pos_y = MAX_Y - 2;
			pass++;
		} 
		
		//inc speed
		if (pass > 3){
			ball.speed += 0.1;
			if (ball.speed > 1)
				ball.speed = 1;
		}


		//draw ball
		x = float_to_int(ball.pos_x);
		y = float_to_int(ball.pos_y);
		matrix[x][y] = 15;
		matrix[ball.prev_x][ball.prev_y] = 1;
		ball.prev_x = x;
		ball.prev_y = y;

		//draw pedal 1
		for (x = 0; x < 5; x++)
			matrix[player1.pos_x + x - 2][0] = 15;
		//draw pedal 2
		for (x = 0; x < 5; x++)
			matrix[player2.pos_x + x - 2][23] = 15;

		int i;
		for(i=0;i<8;i++)
		{
			matrix[0][i]=(nes[0]>>i)&1;
		}

		//Render matrix
		updateScreen();    

		//wait a little bit
		//_delay_ms(10);

    }

    return 0;
}


