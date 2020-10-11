#include <msp430.h> 


/**
 * main.c
 */
#define LED1 BIT0
#define LED2 BIT6
#define B1 BIT3
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P1DIR |= LED1 | LED2;
	P1OUT &= ~LED1;
	P1OUT &= ~LED2;
	P1REN |= B1;
	P1OUT |= B1;

//	LED Clock
	TA0CCR0 = 5999;
	BCSCTL3 |= LFXT1S_2;
	TA0CTL |= MC_1|TASSEL_1|TACLR;

//  Button Clock
//	BCSCTL1 =
//	TA0CCR1 = 19999;
//	TA0CCTL0 |= MC_1|TASSEL_2|TACLR;

	volatile unsigned int s = 0;
	volatile unsigned int p = 0;

	for(;;){
	    if(s == 0 || s == 1 || s == 2 || s == 3){
	        if((P1IN & B1) == 0){
	            s = 6;
	            TA0CCR0 = 5999;
	            continue;
	        }

	        while (!(TA0CTL & TAIFG)) {}

            TA0CTL &= ~TAIFG;  // Clear overflow flag
            P1OUT ^= LED1;
            if(s == 3){
                TA0CCR0 = 10799;
            }

            s = (s + 1) % 6;
	    }else if(s == 4 || s == 5){
	        if((P1IN & B1) == 0){
                s = 6;
                TA0CCR0 = 5999;
                continue;
            }

	        while (!(TA0CTL & TAIFG)) {}

	        TA0CTL &= ~TAIFG;  // Clear overflow flag
	        P1OUT ^= LED2;

	        if(s == 5){
	            TA0CCR0 = 5999;
	        }

	        s = (s + 1) % 6;


	    }else if(s == 6 || s == 7){
	        if(!(P1IN & B1) == 0){
	            P1OUT &= ~LED1;
	            P1OUT &= ~LED2;
                s = 0;
                TA0CCR0 = 5999;
                continue;
            }

	        while (!(TA0CTL & TAIFG)) {}

            TA0CTL &= ~TAIFG;  // Clear overflow flag
            if(s == 6){
                P1OUT |= LED1;
                P1OUT |= LED2;
            }else{
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
            }


            if(s == 6){
                s = 7;
            }else{
                s = 6;
            }
//            s = (s + 1) % 6;
	    }
	}
	
	return 0;
}
