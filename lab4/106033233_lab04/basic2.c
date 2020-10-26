#include <msp430.h>

#define LED1 BIT0
#define LED2 BIT6
#define B1 BIT3

// SMCLK DCO Clock
//#define R_B_T 7812
//#define G_B_T 14062
//#define B_B_T 7812
#define B_P_T 42968 // 42968 // total: 85936, but use up/down mode, so half
#define M_T_T 1952 // total: 3905 // Period for Measuring Temperature

// ACLK VLO Clock
#define R_B_T 5999 // Red & Green Blink
#define G_B_T 10799 // Red & Green Blink
#define B_B_T 5999 // Red & Green Blink
#define B_O_T 4199 // Both Open
#define B_C_T 7799 // Both Close
#define RB_GK_T 2399 // Red Blink, Green Keep Open
//#define B_P_T 23999

int adc[4]; // Buffer to store the ADC values
volatile unsigned int rt = 0;
volatile unsigned int at = 0;
volatile unsigned int s = 0; // LED State
volatile unsigned int b_s = 0; // Button State
volatile unsigned int c = 0;
//volatile unsigned int c1 = 0;

void main(void) {
  WDTCTL = WDTPW + WDTHOLD;    // Stop WDT

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
  TA1CCR0 = R_B_T;
  BCSCTL3 |= LFXT1S_2;
  TA1CTL |= MC_1|TASSEL_1|TACLR;
  TA1CCTL0 = CCIE; // Enable interrupts


  TA0CCTL1 = OUTMOD_0;  // TA0CCR1 output only
  TA0CCR0 = B_P_T;     // Button Pressed Time Threshold
//  TA0CCR1 = M_T_T;       // TA0CCR1 OUT1 on time
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 = DIVS_3;
  TA0CTL |= MC_3|ID_3|TASSEL_2|TACLR;
  TA0CCTL0 = CCIE; // Enable interrupts
  TA0R = 0;

// Sensor Timer
//  TA0CCR0 = M_T_T + 1;     // Sampling period
//  TA0CCTL1 = OUTMOD_3;  // TA0CCR1 set/reset
//  TA0CCR1 = M_T_T;       // TA0CCR1 OUT1 on time
//  TA0CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;    // SMCLK, up mode
//  BCSCTL1 = CALBC1_1MHZ; // DCO
//  BCSCTL2 = DIVS_3;
//  DCOCTL = CALDCO_1MHZ; // DCO

// Temperature Sensor H&S time 16x, interrupt enabled
  ADC10CTL0 =  SREF_1 + REFON + ADC10SHT_2 + ADC10ON + ADC10IE;
  ADC10CTL1 = INCH_10 + SHS_1 + CONSEQ_2;    // Input from A1
  ADC10AE0 |= 0x10; // Enable pin A1 for analog in
  ADC10CTL0 |= ENC; // Start sampling

  ADC10DTC1 = 4;     // # of transfers
  ADC10SA = (int)adc; // Buffer starting address


  __enable_interrupt();


  for(;;){
      if(s == 0 || s == 1 || s == 2 || s == 3){
//        Pattern 1  Red LED blink twice
          TA1CCR0 = R_B_T;

          if(!(s % 2)){
              P1OUT &= ~LED2;
              P1OUT |= LED1;
          }else{
              P1OUT &= ~LED2;
              P1OUT &= ~LED1;
          }

      }else if(s == 4 || s == 5){
//        Pattern 1  Green LED blink once
          TA1CCR0 = G_B_T;

          if(!(s % 2)){
              P1OUT &= ~LED1;
              P1OUT |= LED2;
          }else{
              P1OUT &= ~LED1;
              P1OUT &= ~LED2;
          }

      }else if(s == 6 || s == 7){
//          Pattern 2, blink both
//          TA1CCR0 = B_B_T;
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
      }else if(s == 8 || s == 9){
//          Pattern 2, blink both
          TA1CCR0 = RB_GK_T;

        if(!(s % 2)){
            P1OUT |= LED1;
            P1OUT |= LED2;
        }else{
            P1OUT &= ~LED1;
            P1OUT |= LED2;
        }
    }

// Control Button State
    int is_press = 0;
    if((P1IN & B1) == 0){is_press = 1;}
    if(b_s == 0){
//        Released
//        TA0CCTL1 = OUTMOD_0;  // TA0CCR1 output only
//        TA0CCR0 = B_P_T;     // Button Pressed Time Threshold
//        TA0CCTL0 = CCIE; // Enable interrupts
    }else if(b_s == 1){
//        Pressed & Counting Pressed Time
//        TA0CCTL1 = OUTMOD_0;  // TA0CCR1 output only
//        TA0CCR0 = B_P_T;     // Button Pressed Time Threshold
//        TA0CCTL0 = CCIE; // Enable interrupts
        if(!is_press){
            TA0CTL &= ~TAIFG;  // Clear overflow flag
            TA0R = 0;
            b_s = 0;
        }
    }else if(b_s == 2){
//        TA0CCTL1 = OUTMOD_3;  // TA0CCR1 set/reset
//        TA0CCR0 = M_T_T + 1;     // Sampling period
//        TA0CCTL0 = 0; // Disable interrupts
        c = 1;
//        s = 8;
        if(!is_press){
            TA0CTL &= ~TAIFG;  // Clear overflow flag
            TA0R = 0;

            TA0CCTL1 = OUTMOD_0;  // TA0CCR1 output only
            TA0CCR0 = B_P_T;     // Button Pressed Time Threshold
            TA0CCTL0 = CCIE; // Enable interrupts
            b_s = 0;
            s = 0;
        }
    }
  }
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
  rt = ADC10MEM;

  if(b_s == 2){
      at = at / 3 * 2 + rt / 3;
      int st = (adc[0] + adc[1] + adc[2] + adc[3]) / 4;
      ADC10SA = (int)adc;
      if(s < 6){s = 6;}
      if (st < 740){
      //      To Pattern 1
            if(s == 8 || s == 9){
              s = 6;
            }
       }else{
      //      To Pattern 2
            if(s == 6 || s == 7){
                s = 8;
            }
       }
  }

//  ADC10CTL0 |= ENC + ADC10SC; // enable sampling
}

// Timer0 Interrupt For Button
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void) {
    TA0CTL &= ~TAIFG;  // Clear overflow flag
    TA0R = 0;

    if(b_s == 1){
//    Pressed & Counting Pressed Time
        TA0CCTL1 = OUTMOD_3;  // TA0CCR1 set/reset
        TA0CCR0 = M_T_T + 1;     // Sampling period
        TA0CCTL0 = 0; // Disable interrupts
        b_s = 2;
    }
}

// Timer1 Interrupt For LEDs
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TA1_ISR(void) {
    TA1CTL &= ~TAIFG;  // Clear overflow flag
    TA1R = 0;
    if(s == 6){
        s = 7;
    }else if(s == 7){
        s = 6;
    }else if(s == 8){
        s = 9;
    }else if(s == 9){
        s = 8;
    }else{
        s = (s + 1) % 6;
    }
}
// Button Interrupt
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    P1IFG &= ~B1;             // P1.4 IFG cleared

    if(b_s == 0){
        TA0CTL &= ~TAIFG;  // Clear overflow flag
        TA0R = 0;
        b_s = 1;
    }
}
