#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"
#include "io.c"

#define T_OBS 7         // location of top obstacle
#define B_OBS 11        // location of bottom obstacle
#define TRK_LENGTH 16   // max number of columns on display

// Scroller state vars
enum Scr_States { SCR_START, SCR_INIT, SCR_WAIT, SCR_LOSE };
// Top track state vars
enum Track1_States { TRK1_START, TRK1_INIT, TRK1_WAIT, TRK1_LOSE };
// Bottom track state vars
enum Track2_States { TRK2_START, TRK2_INIT, TRK2_WAIT, TRK2_LOSE };
// Player/Game Control
enum Player_States { PLYR_START, PLYR_INIT, PLYR_WAIT, PLYR_PAUSE };

const unsigned char TRACK1[] = "        ☼       ";  // Top track
const unsigned char TRACK2[] = "            #   ";  // Bottom track
//const unsigned char TOP_MSG[] ="   Hurray for   "; // Top game over message
//const unsigned char BTM_MSG[] ="Embedded Systems"; // Bottom game over messsage
unsigned string = 

//score increments as a player goes past an obstacle
unsigned long int score;
//highscore for eeprom
unsigned long int highscore;

// Shared vars
unsigned char st_pos = 0;   // scroll starting position Trk/Scr (R/W)
unsigned char paused, lose; // game state vars Scr+Trk/Plyr (R/W)
// flags for obstacle is in danger zone Plyr/Trk (R/W)
unsigned char top_obs, btm_obs;

// Scroll start position SM
int TickFct_Scr(int state) {

    // Transitions
    switch(state) {
        case SCR_START:
            state = SCR_INIT;           // advance to INIT state
            break;
        case SCR_INIT:
            st_pos = 0;                 // start at first character in track
            state = SCR_WAIT;           // advance to WAIT state
            break;
        case SCR_WAIT:
            // if game has been lost stop the scroll counter
            if (lose) state = SCR_LOSE;
            // reset start position back to 0 if done iterating
            else if (st_pos >= TRK_LENGTH) st_pos = 0;
            score++;
            break;
        case SCR_LOSE:
            // new game, start scrolling over
            if(!lose) state = SCR_INIT;
            break;
        default:
            state = SCR_START; break;   // something went wrong, go back to START
    }

    // Actions
    switch(state) {
        case SCR_WAIT:
            // advanced starting point if game is in play
            if (!paused)
                st_pos++;
            break;
        default: break;
    }

    return state;
}

// SM to draw top track
int TickFct_Track1(int state) {
    static unsigned char i, j;  // active column and character tracker/iterator

    // Transitions
    switch(state) {
        case TRK1_START:
            state = TRK1_INIT;      // advance to INIT state
            break;
        case TRK1_INIT:
            i = 1;                  // starting at first column of top row
            j = st_pos;             // init latest starting position from scroller
            state = TRK1_WAIT;      // advance to WAIT state
            break;
        case TRK1_WAIT:
            // game has been lost, reset a bunch of stuff, clear obs flag
            if(lose) {
                state = TRK1_LOSE;
                j = top_obs = 0;
                i = 1;  // start at first column of top row
            }
         
            // check if we've reached the end of the display
            else if (i > TRK_LENGTH) {
                state = TRK1_INIT;
            }
            break;
        case TRK1_LOSE:
            if(i <= TRK_LENGTH) {
                LCD_Cursor(i);  // set cursor to appropriate column to write at
                // write the next character in the top game over message
                LCD_WriteData(TOP_MSG[j % TRK_LENGTH]);
                i++; j++;
            }
            // new game has been started, reset everything
            else if(!lose) state = TRK1_INIT;
            break;
        default:
            state = TRK1_START; break;   // something went wrong, go back to START
    }

    // Actions
    switch(state) {
        case TRK1_WAIT:
            LCD_Cursor(i);  // reset cursor position
            // write the next character
            LCD_WriteData(TRACK1[j % TRK_LENGTH]);
            i++; j++;
            // we anticipate the obstacle to be in column 2
            if (st_pos == T_OBS) top_obs = 1;   // set obstacle flag
            else top_obs = 0;   // clear flag otherwise
            break;
        default: break;
    }

    return state;
}

// Track2 position and char draw SM
int TickFct_Track2(int state) {
    static unsigned char i, j;

    // Transitions
    switch(state) {
        case TRK2_START:
            state = TRK2_INIT;      // advance to INIT state
            break;
        case TRK2_INIT:
            i = 17;                 // start at first column of bottom row
            j = st_pos;             // init latest starting position from scroller
            state = TRK2_WAIT;      // advance to WAIT state
            break;
        case TRK2_WAIT:
            // game has been lost, reset a bunch of stuff, clear obs flag
            if(lose) {
                state = TRK2_LOSE;
                j = btm_obs = 0;
                i = 17;     // start at first column of bottom row
            }
            // check if we've reached the end of the display
            else if (i > TRK_LENGTH*2) {
                state = TRK2_INIT;
            }
            break;
        case TRK2_LOSE:
            if(i <= TRK_LENGTH*2) {
                LCD_Cursor(i);  // set cursor to appropriate column to write at
                // write the next character in the top game over message
                LCD_WriteData(BTM_MSG[j % TRK_LENGTH]);
                i++; j++;
            }
            // new game has been started, reset everything
            else if(!lose) state = TRK2_INIT;
            break;
        default:
            state = TRK2_START; break;   // something went wrong, go back to START
    }

    // Actions
    switch(state) {
        case TRK2_WAIT:
            LCD_Cursor(i);  // reset cursor position
            // write the next character
            LCD_WriteData(TRACK2[j % TRK_LENGTH]);
            i++; j++;
            // we anticipate the obstacle to be in column 2
            if (st_pos == B_OBS) btm_obs = 1;   // set obstacle
            else btm_obs = 0;   // clear flag otherwise
            break;
        default: break;
    }

    return state;
}

// Player position and buttons draw SM
int TickFct_Player(int state) {
    static unsigned char i, j;
    j = ~PINB;

    // Transitions
    switch(state) {
        case PLYR_START:
            state = PLYR_INIT;  // advance to INIT state
            break;
        case PLYR_INIT:
            i = 2;              // start player on top row
            paused = 1;         // set initial game state to paused
            lose = 0;           // set game state to playable
            state = PLYR_WAIT;  // advance to WAIT state
            break;
        case PLYR_WAIT:
            // top button is pressed
            if((j & 0x01) && !paused) {
                i = 2;          // move player to top row
            }
            // bottom button is pressed
            else if ((j & 0x02) && !paused) {
                i = 18;         // move player to bottom row
            }
            // pause/play button is pressed
            else if (j & 0x04) {
                paused = (paused) ? 0 : 1;  // toggle paused flag
                state = PLYR_PAUSE;         // transition to paused state
                if (lose) lose = 0;         // restart game if game over
            }
            // character position lines up with top or bottom obstacle
            if ((i == 2 && top_obs) || (i == 18 && btm_obs)) {
                paused = lose = 1;          // pause and end game
                state = PLYR_PAUSE;         // go to paused state
            }
            else if ((top_obs==1) || (btm_obs==17)) {
            state=PLYR_WAIT; //goes to PLYR_WAIT case
            score++; //increases score
            }
            if (score > highscore) {
            score= highscore= memval;
            break;
        case PLYR_PAUSE:
            // check that pause/play button is released
            if (j ^ 0x04) {
                state = PLYR_WAIT;
            }
            break;
        default:
            state = PLYR_START; break;   // something went wrong, go back to START
    }

    // Actions
    switch(state) {
        case PLYR_WAIT:
            // update character position if game is in play
            if (!lose) LCD_Cursor(i);
            break;
        default: break;
    }

    return state;
}

int main(void)
{
    DDRA = 0xFF; PORTA = 0x00;  // LCD control lines
    DDRB = 0x00; PORTB = 0xFF;  // initialize as inputs
    DDRD = 0xFF; PORTD = 0x00;  // LCD data lines

    // Period for tasks
    unsigned long int Scroll_Task_calc = 400;
    unsigned long int Track1_Task_calc = 20;
    unsigned long int Track2_Task_calc = 20;
    unsigned long int Player_Task_calc = 20;

    // Calculate GCD
    unsigned long int tmpGCD = 1;
    tmpGCD = findGCD(Scroll_Task_calc, Track1_Task_calc);
    tmpGCD = findGCD(tmpGCD, Track2_Task_calc);
    tmpGCD = findGCD(tmpGCD, Player_Task_calc);

    // Gcd for all tasks or smallest time unit for tasks
    unsigned long int GCD = tmpGCD;

    // Recalc GCD periods for scheduler
    unsigned long int Scroll_period = Scroll_Task_calc/GCD;
    unsigned long int Track1_period = Track1_Task_calc/GCD;
    unsigned long int Track2_period = Track2_Task_calc/GCD;
    unsigned long int Player_period = Player_Task_calc/GCD;

    // create tasks
    static task Scroll_Task, Track1_Task, Track2_Task, Player_Task;
    // insert tasks into task array
    task *tasks[] = { &Scroll_Task, &Track1_Task, &Track2_Task, &Player_Task };
    // calc number of tasks
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    Scroll_Task.state = SCR_START;              // initialize task state
    Scroll_Task.period = Scroll_period;         // set period
    Scroll_Task.elapsedTime = Scroll_period;    // initialize elapsed time
    Scroll_Task.TickFct = &TickFct_Scr;         // set task tick function

    Track1_Task.state = TRK1_START;             // initialize task state
    Track1_Task.period = Track1_period;         // set period
    Track1_Task.elapsedTime = Track1_period;    // initialize elapsed time
    Track1_Task.TickFct = &TickFct_Track1;      // set task tick function

    Track2_Task.state = TRK2_START;             // initialize task state
    Track2_Task.period = Track2_period;         // set period
    Track2_Task.elapsedTime = Track2_period;    // initialize elapsed time
    Track2_Task.TickFct = &TickFct_Track2;      // set task tick function

    Player_Task.state = PLYR_START;             // initialize task state
    Player_Task.period = Player_period;         // set period
    Player_Task.elapsedTime = Player_period;    // initialize elapsed time
    Player_Task.TickFct = &TickFct_Player;      // set task tick function

    LCD_init();                 // initialize LCD screen
    LCD_ClearScreen();          // clear LCD screen

    TimerSet(GCD);              // set timer period
    TimerOn();                  // enable timer

    unsigned short i;           // scheduler loop iterator

	while(1)
	{
        // scheduler loop
        for(i = 0; i < numTasks; i++) {
            // task is ready to tick
            if(tasks[i]->elapsedTime == tasks[i]->period) {
                // setting next state for task
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                // reset the elapsed time for the next tick
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 1;
        }
        while(!TimerFlag);      // wait for a period
        TimerFlag = 0;          // reset TimerFlag
	}
}
