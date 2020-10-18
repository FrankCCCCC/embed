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
//	P1OUT |= RED;
	
	P1REN |= B1;
	P1OUT |= B1;

	for(;;){
	    volatile unsigned int i;
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
