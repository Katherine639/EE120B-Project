#include <avr/io.h>
#include "timer.h"
#include "io.c"
#include "bit.h"

unsigned char *top = "                #            #         #      #      #               ";
unsigned char *bot = "                      #          #          #   #                    ";
unsigned char textSize = 68;
unsigned char tmpA, player_position, game_pause, game_over, pauseHS;
//top
int k = 0;
int j = 1;
//bot
int l = 0;
int m = 17;

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}

typedef struct _task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TaskFct) (int);
} task;

enum states1 {SM_start1, move} state1;
enum states2 {SM_start2, display} state2;

int SM_Player(int state) {
	switch (state){
		case SM_start1:
		state = move;
		break;
		
		case move:
		if(GetBit(tmpA, 2) && (player_position > 17)){
			player_position -= 16;
		}
		if(GetBit(tmpA, 3) && (player_position < 17)){
			player_position += 16;
		}
		if(GetBit(tmpA,4) && !pauseHS){
			pauseHS = 1;
			if(game_over == 1){
				game_over = 0;
				break;
			}
			else{game_pause = ~game_pause;}
		}
		state = move;
		break;
		
		default:
		state = SM_start1;
		break;
	}
	LCD_Cursor(player_position);
	
	return state;
}

int SM_LCD(int state){
	switch (state) {
		case SM_start2:
		if(game_over == 1){
			state = SM_start2;
			break;
		}
		state = display;
		break;
		
		case display:
		state = display;
		break;
		
		default:
		state = SM_start2;
		break;
	}
	
	switch (state) {
		case SM_start2:
		break;
		
		case display:
		//top
		pauseHS = 0;
		if(game_pause == 0){
			for (j = 1; j <= 16; j++) {
				LCD_Cursor(j);
				//add check here
				if(((*(top + k + j - 1)) == '#') && j == player_position){
					LCD_ClearScreen();
					k = l = 0;
					LCD_DisplayString(1,"Game Over!");
					game_over = 1;
					state = SM_start2;
					break;
				}
				LCD_WriteData(*(top + k + j - 1));
			}
			if ( k > (textSize - 16)) {
				k = 0;
			}
			else {k++;}
			
			//bot
			for (m = 17; m <= 32; m++) {
				LCD_Cursor(m);
				//add check here
				if(((*(bot + l + (m - 16) - 1)) == '#') && m == player_position){
					//lose game
					LCD_ClearScreen();
					k = l = 0;
					LCD_DisplayString(1,"Game Over!");
					game_over = 1;
					state = SM_start2;
					break;
				}
				LCD_WriteData(*(bot + l + (m - 16) - 1));
			}
			if ( l > (textSize - 16)) {
				l = 0;
			}
			else {l++;}
		}
		break;
		
		default: break;
	}
	return state;
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0x03; PORTD = 0xFC;
	//var initialization
	game_pause = game_over = 0;
	player_position = 2;
	unsigned long int player_calc = 10;
	unsigned long int writeLCD_calc = 300;
	
	unsigned long int tempGCD = 1;
	tempGCD = findGCD(player_calc, writeLCD_calc);
	
	unsigned long int GCD;
	GCD = tempGCD;
	
	unsigned long int player_period = player_calc / GCD;
	unsigned long int writeLCD_period = writeLCD_calc / GCD;
	
	static task player, writeLCD;
	task *tasks[] = { &player, &writeLCD};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	player.state = SM_start1;
	player.period = player_period;
	player.elapsedTime = player_period;
	player.TaskFct = &SM_Player;
	
	writeLCD.state = SM_start2;
	writeLCD.period = writeLCD_period;
	writeLCD.elapsedTime = writeLCD_period;
	writeLCD.TaskFct = &SM_LCD;
	
	TimerSet(GCD);
	TimerOn();
	LCD_init();
	
	unsigned short i = 0;
	while(1)
	{
		tmpA = ~PIND;
		for (i = 0; i < numTasks; i++) {
			if (tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TaskFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}
