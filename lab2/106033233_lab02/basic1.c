#include <msp430.h> 


/**
 * main.c
 */
#define LED1 BIT0
#define LED2 BIT6
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P1DIR |= LED1 | LED2;
	P1OUT &= ~LED1;
	P1OUT &= ~LED2;

	TA0CCR0 = 5999;
	BCSCTL3 |= LFXT1S_2;
	TA0CTL |= MC_1|TASSEL_1|TACLR;

	volatile unsigned int s = 0;

	for(;;){
	    if(s == 0 || s == 1 || s == 2 || s == 3){
//	        P1OUT |= LED0;

	        while (!(TA0CTL & TAIFG)) {}
//	        TA0CTL &= ~TAIFG;

            TA0CTL &= ~TAIFG;  // Clear overflow flag
	        if(s % 2 == 0){
	            P1OUT |= LED1;
	        }else{
	            P1OUT &= ~LED1;
	        }
//            P1OUT ^= LED1;
//            TA0CCR0 = 5999;
            if(s == 3){
                TA0CCR0 = 10799;
            }

            s = (s + 1) % 6;
	    }else if(s == 4 || s == 5){
	        while (!(TA0CTL & TAIFG)) {}

	        TA0CTL &= ~TAIFG;  // Clear overflow flag
	        P1OUT ^= LED2;
//	        TA0CCR0 = 10799;
	        if(s == 5){
	            TA0CCR0 = 5999;
	        }

	        s = (s + 1) % 6;


	    }
//	    TA0CTL &= ~TAIFG;
	}
	
	return 0;
}
