#include <msp430.h> 


/**
 * main.c
 */
#define LED1 BIT0
#define LED2 BIT6
#define B1 BIT3
// SMCLK DCO Clock
#define R_B_T 7812
#define G_B_T 14062
#define B_B_T 7812
#define B_P_T 31249

// ACLK VLO Clock
//#define R_B_T 5999
//#define G_B_T 10799
//#define B_B_T 5999
//#define B_P_T 23999

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR |= LED1 | LED2;
    P1OUT &= ~LED1;
    P1OUT &= ~LED2;
    P1REN |= B1;
    P1OUT |= B1;

//  DCO SMCLK for TA0CTL
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL2 = DIVS_3;
    TA0CCR0 = R_B_T;
    TA0CTL |= MC_1|ID_3|TASSEL_2|TACLR;

//  LED Clock, VLO ACLK for TA0CTL
//    TA0CCR0 = R_B_T;
//    BCSCTL3 |= LFXT1S_2;
//    TA0CTL |= MC_1|TASSEL_1|TACLR;

//  Button Clock, DCO SMCLK for TA1CTL
//    BCSCTL1 = CALBC1_1MHZ;
//    DCOCTL = CALDCO_1MHZ;
//    BCSCTL2 = DIVS_3;
//    TA1CCR1 = 3;
//    TA1CTL |= MC_1|ID_3|TASSEL_2|TACLR;

//    TA1CCR1 = 5999;
//    TA1CTL |= MC_1|TASSEL_1|TACLR;
//  TA0CCTL0 |= MC_1|TASSEL_2|TACLR;

    volatile unsigned int s = 0;
    volatile unsigned int p = 0;
    volatile unsigned int prev_s = 0;

    for(;;){
        if(s == 0 || s == 1 || s == 2 || s == 3){
//          Red LED blink twice
            TA0CCR0 = R_B_T;

            if((P1IN & B1) == 0){
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
                prev_s = s;
                s = 8;
//                TA0CCR0 = B_P_T;
                continue;
            }

            while (!(TA0CTL & TAIFG)) {}

            TA0CTL &= ~TAIFG;  // Clear overflow flag
            if(!(s % 2)){
                P1OUT |= LED1;
            }else{
                P1OUT &= ~LED1;
            }

            s = (s + 1) % 6;
        }else if(s == 4 || s == 5){
//          Green LED blink once
            TA0CCR0 = G_B_T;

            if((P1IN & B1) == 0){
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
                prev_s = s;
                s = 8;
//                TA0CCR0 = B_P_T;
                continue;
            }

            while (!(TA0CTL & TAIFG)) {}

            TA0CTL &= ~TAIFG;  // Clear overflow flag
            if(!(s % 2)){
                P1OUT |= LED2;
            }else{
                P1OUT &= ~LED2;
            }

            s = (s + 1) % 6;
        }else if(s == 6 || s == 7){
//          Pressed the button, blink both
            TA0CCR0 = B_B_T;

            if(!(P1IN & B1) == 0){
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
                prev_s = 0;
                s = 0;
//                TA0CCR0 = R_B_T;
                continue;
            }

            while (!(TA0CTL & TAIFG)) {}

            TA0CTL &= ~TAIFG;  // Clear overflow flag
            if(!(s % 2)){
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
        }else if(s == 8){
//          Checking button pressed time
            volatile unsigned int is_pressed = 1;
            TA0CCR0 = B_P_T;

            while (!(TA0CTL & TAIFG)) {
                if(!(P1IN & B1) == 0){
                    is_pressed = 0;
                    break;
                }
            }

            TA0CTL &= ~TAIFG;  // Clear overflow flag
            if(is_pressed){
//              Pressed button for 2sec, go to state 6
                s = 6;
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
//                TA0CCR0 = B_B_T;
            }else{
//              Released button, back to original state
                P1OUT &= ~LED1;
                P1OUT &= ~LED2;
                s = prev_s;
                prev_s = 0;
            }
        }
    }

    return 0;
}
