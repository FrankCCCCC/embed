#include <msp430.h>

#define RED BIT0
#define GREEN BIT6
#define B1 BIT3
/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR |= RED | GREEN;
	P1OUT &= ~GREEN;
	P1OUT &= ~RED;
	
	P1REN |= B1;
	P1OUT |= B1;

	volatile unsigned int state = 0;
//	State 0 blink red 1
//  State 1 blink red 2
//  State 2 blink green
//  State 3 blink red & green
	for(unsigned int count = 0; ; count++){
	    volatile unsigned int i;

	    if((P1IN & B1) == 0){
	        state = 3;
	    }

	    if(state == 0){

	    }

	    if((P1IN & B1) == 0){
	        P1OUT |= RED;
	        P1OUT |= GREEN;
            i = 10000;
            do i--;
            while(i != 0);

            P1OUT &= ~RED;
            P1OUT &= ~GREEN;
            i = 10000;
            do i--;
            while(i != 0);

	    }else{
	        P1OUT |= BIT0;
            i = 10000;
            do i--;
            while(i != 0);

            P1OUT &= ~BIT0;
            i = 10000;
            do i--;
            while(i != 0);

            P1OUT |= BIT0;
            i = 10000;
            do i--;
            while(i != 0);

            P1OUT &= ~BIT0;
            i = 10000;
            do i--;
            while(i != 0);



            P1OUT |= BIT6;
            i = 20000;
            do i--;
            while(i != 0);

            P1OUT &= ~BIT6;
            i = 20000;
            do i--;
            while(i != 0);
	    }
	}

	return 0;
}
