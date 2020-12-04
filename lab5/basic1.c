#include <msp430.h>

#define LED1 BIT0
#define LED2 BIT6
#define B1 BIT3

// SMCLK DCO Clock
//#define R_B_T 7812
//#define G_B_T 14062
//#define B_B_T 7812
//#define B_P_T 8593

// ACLK VLO Clock
#define R_B_T 5999
#define G_B_T 10799
#define B_O_T 3599
#define B_C_T 2399
#define M_T_T_NORM 9599 // Measuremeny Frequency in Normal State
#define M_T_P_T_NORM 1 // Measurment Period in Normal State
#define M_T_T_EMG 4799 // Measuremeny Frequency in Emergency State
#define M_T_P_T_EMG 1 // Measurment Period in Emergency State
//#define B_P_T 23999

// State Switch Threshold
#define Threshold 26

volatile unsigned int ADC_val = 0;
volatile float t = 0;
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
  TA0CCR0 = M_T_T_NORM;     // Sampling period
  TA0CCR1 = M_T_T_NORM - M_T_P_T_NORM;       // TA0CCR1 OUT1 on time
  TA0CCTL1 = OUTMOD_3;  // TA0CCR1 set/reset
  TA0CTL = TASSEL_1 + MC_1;    // ACLK, up mode

  // H&S time 16x, interrupt enabled
    ADC10CTL0 =  SREF_1 + REFON + ADC10SHT_2 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_10 + SHS_1 + CONSEQ_0;    // Input from A1
    ADC10AE0 |= 0x10; // Enable pin A1 for analog in

  __enable_interrupt();

  for(;;){
      ADC10CTL0 |= ENC; // Start sampling
      if(s < 6){
          _BIS_SR(LPM3_bits + GIE);
      }else{
          _BIS_SR(LPM0_bits + GIE);
      }

      if(s == 0 || s == 1 || s == 2 || s == 3){
//          Red LED blink twice
          TA1CCR0 = R_B_T;
          TA0CCR0 = M_T_T_NORM;     // Sampling period
          TA0CCR1 = M_T_T_NORM - M_T_P_T_NORM;       // TA0CCR1 OUT1 on time

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
          TA0CCR0 = M_T_T_NORM;     // Sampling period
          TA0CCR1 = M_T_T_NORM - M_T_P_T_NORM;       // TA0CCR1 OUT1 on time

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

          TA0CCR0 = M_T_T_EMG;     // Sampling period
          TA0CCR1 = M_T_T_EMG - M_T_P_T_EMG;       // TA0CCR1 OUT1 on time

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
    ADC_val = ADC10MEM;
    t = (((float)ADC_val * 1.5 / 1023) - 0.986) / 0.00355;

  if (t < Threshold){
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

  __bic_SR_register_on_exit(CPUOFF);
//  if(s < 6){
//      __bic_SR_register_on_exit(LPM3_bits);
//  }else if(s >= 6){
//      __bic_SR_register_on_exit(LPM0_bits);
//  }


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
    __bic_SR_register_on_exit(CPUOFF);
//    if(s < 6){
//        __bic_SR_register_on_exit(LPM3_bits);
//    }else if(s >= 6){
//        __bic_SR_register_on_exit(LPM0_bits);
//    }

}

