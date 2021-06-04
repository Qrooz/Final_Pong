/*	Author: Cruz Ramirez
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab 11  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *      Create the arcade game of pong with the new component, the LED matrix
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: Youtube URL> 
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif


//task scheduler data structure
typedef struct _task {
	
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
} task;
//end of task scheduler structure

//shared variables
unsigned char matrix[5][8] = {	
	{0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0}};
unsigned char inv = 0x00;
//end of shared variables


//start of first task to control play1 paddle
enum PAD1_States {PAD1_mid, PAD1_up, PAD1_down};
int PAD1_Tick(int state) {
	switch(state){ //state transitions for paddle 1

		case PAD1_mid:
			if(inv == 0x01){
				state = PAD1_up;
			}
			else if(inv == 0x02){
				state = PAD1_down;
			}
			else{
				state = PAD1_mid;
			}
		break;

		case PAD1_up:
			if(inv == 0x02){
				state = PAD1_mid;
			}
			else{
				state = PAD1_up;
			}
		break;

		case PAD1_down:
			if(inv == 0x01){
                                state = PAD1_mid;
                        }
                        else{
                                state = PAD1_down;
                        }
		break;
		default: state = PAD1_mid; break;

	}
	switch(state){ //state actions
		
		case PAD1_up:
			matrix[0][0] = 0x01;
			matrix[1][0] = 0x01;
                        matrix[2][0] = 0x01;
                        matrix[3][0] = 0x00;
                        matrix[4][0] = 0x00;
		break;

		case PAD1_down:
                        matrix[0][0] = 0x00;
			matrix[1][0] = 0x00;
                        matrix[2][0] = 0x01;
                        matrix[3][0] = 0x01;
                        matrix[4][0] = 0x01;
                break;
	
		case PAD1_mid:
			matrix[0][0] = 0x00;
			matrix[1][0] = 0x01;
			matrix[2][0] = 0x01;
			matrix[3][0] = 0x01;
			matrix[4][0] = 0x00;
		break;

		default:
		break;
	}
return state;
}
//end of first state



//start of last state to update PORTD and PORTC
enum LED_States{LED_update1, LED_update2, LED_update3, LED_update4, LED_update5};

int LED_Tick(int state){
	switch(state){ //transitions 
		case LED_update1:
			state = LED_update2;
		break;
		
		case LED_update2:
                        state = LED_update3;
                break;
		
		case LED_update3:
                        state = LED_update4;
                break;
		
		case LED_update4:
                        state = LED_update5;
                break;
		
		case LED_update5:
                        state = LED_update1;
                break;

		default: state = LED_update1; break;
	}
	switch(state){//state actions
		case LED_update1:
			PORTD = 0x1E;
			PORTC = (matrix[0][0]) | (matrix[0][1] << 1) | (matrix[0][2] << 2) | (matrix[0][3] << 3) | (matrix[0][4] << 4) | (matrix[0][5] << 5) | (matrix[0][6] << 6) | (matrix[0][7] << 7);
		break;

		case LED_update2:
                        PORTD = 0x1D;
			PORTC = (matrix[1][0]) | (matrix[1][1] << 1) | (matrix[1][2] << 2) | (matrix[1][3] << 3) | (matrix[1][4] << 4) | (matrix[1][5] << 5) | (matrix[1][6] << 6) | (matrix[1][7] << 7);
                break;

		case LED_update3:
                        PORTD = 0x1B;
			PORTC = (matrix[2][0]) | (matrix[2][1] << 1) | (matrix[2][2] << 2) | (matrix[2][3] << 3) | (matrix[2][4] << 4) | (matrix[2][5] << 5) | (matrix[2][6] << 6) | (matrix[2][7] << 7);
                break;

		case LED_update4:
                        PORTD = 0x17;
			PORTC = (matrix[3][0]) | (matrix[3][1] << 1) | (matrix[3][2] << 2) | (matrix[3][3] << 3) | (matrix[3][4] << 4) | (matrix[3][5] << 5) | (matrix[3][6] << 6) | (matrix[3][7] << 7);
                break;

		case LED_update5:
                        PORTD = 0x0F;
			PORTC = (matrix[4][0]) | (matrix[4][1] << 1) | (matrix[4][2] << 2) | (matrix[4][3] << 3) | (matrix[4][4] << 4) | (matrix[4][5] << 5) | (matrix[4][6] << 6) | (matrix[4][7] << 7);
                break;

		default: break;
	}
return state;
}



int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
    /* Insert your solution below */

	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;
	//task1
	task1.state = start;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &PAD1_Tick;

	//task2
	task2.state = start;
        task2.period = 1;
        task2.elapsedTime = task2.period;
        task2.TickFct = &LED_Tick;

	TimerSet(1);
	TimerOn();

	unsigned char i = 0x00;
    while (1) {
	
	inv = ~PINA;
	for(i = 0; i < numTasks; i++){
			
			if(tasks[i]->elapsedTime >= tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		} 
	    	while(!TimerFlag);
		TimerFlag = 0;

    }
    return 1;
}
