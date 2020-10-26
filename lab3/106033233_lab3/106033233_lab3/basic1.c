#include <msp430.h>


/**
 * main.c
 */
#define LED1 BIT0
#define LED2 BIT6
#define B1 BIT3

// SMCLK DCO Clock
//#define R_B_T 7812
//#define G_B_T 14062
//#define B_B_T 7812
#define B_P_T 8593

// ACLK VLO Clock
#define R_B_T 5999
#define G_B_T 10799
#define B_B_T 5999
//#define B_P_T 23999

volatile unsigned int s = 0;;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR |= LED1 | LED2;
    P1OUT &= ~LED1;
    P1OUT &= ~LED2;
    P1REN |= B1;
    P1OUT |= B1;

//    Set up Interrupt
    P1IE |= B1;              // P1.4 interrupt enabled
    P1IES |= B1;             // P1.4 Hi/lo edge
    P1IFG &= ~B1;            // P1.4 IFG cleared
    _BIS_SR(GIE);              // Enter interrupt


//  LED Clock
    TA0CCR0 = R_B_T;
    BCSCTL3 |= LFXT1S_2;
    TA0CTL |= MC_1|TASSEL_1|TACLR;
    TA0CCTL0 = CCIE; // Enable interrupts

// DCO, SMCLK
//    BCSCTL1 = CALBC1_1MHZ;
//    DCOCTL = CALDCO_1MHZ;
//    BCSCTL2 = DIVS_3;
//    TA1CCR0 = B_P_T;
//    TA1CTL |= MC_1|ID_3|TASSEL_2|TACLR;
//    TA1CCTL0 = CCIE; // Enable interrupts

    volatile unsigned int p = 0;

    for(;;){
        if(s == 0 || s == 1 || s == 2 || s == 3){
//          Red LED blink twice
            TA0CCR0 = R_B_T;

            if(!(s % 2)){
                P1OUT &= ~LED2;
                P1OUT |= LED1;
            }else{
                P1OUT &= ~LED2;
                P1OUT &= ~LED1;
            }

        }else if(s == 4 || s == 5){
//          Green LED blink once
            TA0CCR0 = G_B_T;

            if(!(s % 2)){
                P1OUT &= ~LED1;
                P1OUT |= LED2;
            }else{
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
            }

        }else if(s == 6 || s == 7){
//          Pressed the button, blink both
            TA0CCR0 = B_B_T;

            if(!(P1IN & B1) == 0){
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
                s = 0;
                TA0R = 0;
                continue;
            }

            if(!(s % 2)){
                P1OUT |= LED1;
                P1OUT |= LED2;
            }else{
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
            }
        }
    }

    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    P1IFG &= ~B1;             // P1.4 IFG cleared
    s = 6;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void) {
    TA0CTL &= ~TAIFG;  // Clear overflow flag
    TA0R = 0;
    if(s == 6){
        s = 7;
    }else if(s == 7){
        s = 6;
    }else{
        s = (s + 1) % 6;
    }
}
