#include <msp430.h>

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
#define B_O_T 4199
#define B_C_T 7799
//#define B_P_T 23999

volatile unsigned int t = 0;
volatile unsigned int s = 0;
//volatile unsigned int t1 = 0;

void main(void) {
  WDTCTL = WDTPW + WDTHOLD;    // Stop WDT

  P1DIR |= LED1 | LED2;
  P1OUT &= ~LED1;
  P1OUT &= ~LED2;
  P1REN |= B1;
  P1OUT |= B1;

//  LED Clock
  TA0CCR0 = R_B_T;
  BCSCTL3 |= LFXT1S_2;
  TA1CTL |= MC_1|TASSEL_1|TACLR;
  TA1CCTL0 = CCIE; // Enable interrupts

// Sensor Timer
    TA0CCR0 = 2048-1;     // Sampling period
    TA0CCTL1 = OUTMOD_3;  // TA0CCR1 set/reset
    TA0CCR1 = 2046;       // TA0CCR1 OUT1 on time
    TA0CTL = TASSEL_2 + MC_1 + TACLR;    // ACLK, up mode

  BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
//    BCSCTL2 = DIVS_3;
//    TA0CCR0 = B_P_T;
//    TA0CTL |= MC_1|ID_3|TASSEL_2|TACLR;
//    TA0CCTL0 = CCIE; // Enable interrupts


//  Temperature Sensor
//  ADC10CTL1 = SHS_1 + CONSEQ_2 + INCH_1;
//  ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE;
//  ADC10CTL0 |= ENC;    // ADC10 Enable
//  ADC10AE0 |= 0x02;    // P1.1 ADC10 option select

// H&S time 16x, interrupt enabled
  ADC10CTL0 =  SREF_1 + REFON + ADC10SHT_2 + ADC10ON + ADC10IE;
  ADC10CTL1 = INCH_10 + SHS_1 + CONSEQ_2;    // Input from A1
  ADC10AE0 |= 0x10; // Enable pin A1 for analog in
  ADC10CTL0 |= ENC; // Start sampling

//


//

//

  __enable_interrupt();


  for(;;){
      if(s == 0 || s == 1 || s == 2 || s == 3){
//          Red LED blink twice
          TA1CCR0 = R_B_T;

          if(!(s % 2)){
              P1OUT &= ~LED2;
              P1OUT |= LED1;
          }else{
              P1OUT &= ~LED2;
              P1OUT &= ~LED1;
          }

      }else if(s == 4 || s == 5){
//          Green LED blink once
          TA1CCR0 = G_B_T;

          if(!(s % 2)){
              P1OUT &= ~LED1;
              P1OUT |= LED2;
          }else{
              P1OUT &= ~LED1;
              P1OUT &= ~LED2;
          }

      }else if(s == 6 || s == 7){
//          Pressed the button, blink both
//          TA0CCR0 = B_B_T;
          if(s == 6){
              TA1CCR0 = B_O_T;
          }else{
              TA1CCR0 = B_C_T;
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
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
  t = ADC10MEM;
  if (ADC10MEM < 740){
//      To Pattern 1
//      P1OUT &= ~0x01;
      if(s >= 6){
        s = 0;
    }
  }else{
//      To Pattern 2
//      P1OUT |= 0x01;
      if(s < 6){
          s = 6;
      }
  }
//  ADC10CTL0 |= ENC + ADC10SC; // enable sampling
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TA1_ISR(void) {
    TA1CTL &= ~TAIFG;  // Clear overflow flag
    TA1R = 0;
    if(s == 6){
        s = 7;
    }else if(s == 7){
        s = 6;
    }else{
        s = (s + 1) % 6;
    }
}

