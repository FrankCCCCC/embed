#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR |= BIT0 | BIT6;

	for(;;){
	    volatile unsigned int i;

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

	return 0;
}
