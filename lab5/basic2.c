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

#define M_T_T 2999 // Measurement Frequency
#define M_T_P_T 1 // Measurement Period

#define B_CLK_T 5999 // The duration of double click

#define NUM_SAMPLES 4
//#define B_P_T 23999

// State Switch Threshold
#define Threshold 28

int adc[4]; // Buffer to store the ADC values
volatile unsigned int ADC_val = 0;
volatile unsigned int avg_ADC_val = 0;
volatile float t = 0;
volatile float avg_t = 0;
volatile unsigned int is_above_thres = 0;

volatile unsigned int s = 0; // LED State

volatile unsigned int b_s = 0; // Button State
volatile unsigned int is_doub_clk = 0; // Is double click successful
volatile unsigned int b_count = 0;

volatile unsigned int is_LPM3_on = 0;
volatile unsigned int time_in_LPM0 = 0;

void set_LEDs(){
    //  Set up LED
    P1DIR |= LED1 | LED2;
    P1OUT &= ~LED1;
    P1OUT &= ~LED2;
}

void set_LEDs_clk(){
    //  LED Clock
    TA0CCR0 = R_B_T;
    BCSCTL3 |= LFXT1S_2;
    TA1CTL |= MC_1|TASSEL_1|TACLR;
    TA1CCTL0 = CCIE; // Enable interrupts
}

void set_btn(){
//  Set up Button
    P1REN |= B1;
    P1OUT |= B1;

    P1IE |= B1;              // P1.4 interrupt enabled
    P1IES |= B1;             // P1.4 Hi/lo edge
    P1IFG &= ~B1;            // P1.4 IFG cleared
    _BIS_SR(GIE);              // Enter interrupt
}

void set_TA0_measu(){
    TA0CCR0 = M_T_T;     // Sampling period
    TA0CCR1 = M_T_T - M_T_P_T;       // TA0CCR1 OUT1 on time
    TA0CCTL0 = 0; // Disable interrupts
}

void set_TA0_btn(){
    TA0CCR0 = B_CLK_T;
//    TA0CCR1 = B_CLK_T;
    TA0CCTL0 = CCIE; // Enable interruptsTA0CCTL0
}

void TA0_clear(){
    TA0CTL &= ~TAIFG;  // Clear overflow flag
    TA0R = 0;
}

void TA1_clear(){
    TA1CTL &= ~TAIFG;  // Clear overflow flag
    TA1R = 0;
}

void btn_clear(){
    P1IFG &= ~B1;             // P1.4 IFG cleared
}

void main(void) {
  WDTCTL = WDTPW + WDTHOLD;    // Stop WDT

  set_LEDs();
  set_LEDs_clk();

  set_btn();

  // Sensor Timer
  set_TA0_btn();
  TA0CCTL1 = OUTMOD_3;  // TA0CCR1 set/reset
  TA0CTL = TASSEL_1 + MC_1;    // ACLK, up mode

  // H&S time 16x, interrupt enabled
    ADC10CTL0 =  SREF_1 + REFON + ADC10SHT_2 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_10 + SHS_1 + CONSEQ_2;    // Input from A1
    ADC10AE0 |= 0x10; // Enable pin A1 for analog in
//    ADC10CTL0 |= ENC; // Start sampling

    ADC10DTC1 = 4;     // # of transfers
    ADC10SA = (int)adc; // Buffer starting address

  __enable_interrupt();

  for(;;){
      if(s < 6){
          _BIS_SR(LPM3_bits + GIE);
          is_LPM3_on = 1;
      }else{
          _BIS_SR(LPM0_bits + GIE);
          is_LPM3_on = 0;
          time_in_LPM0++;
      }

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

          if(s == 6){
              TA1CCR0 = B_O_T;
          }else{
              TA1CCR0 = B_C_T;
          }

          if(!(s % 2)){
              if(is_above_thres){
                  P1OUT |= LED1;
                  P1OUT &= ~LED2;
              }else{
                  P1OUT |= LED1;
                  P1OUT |= LED2;
              }
          }else{
              P1OUT &= ~LED1;
              P1OUT &= ~LED2;
          }
      }

// Button State
      int is_press = 0;
      if((P1IN & B1) == 0){is_press = 1;}

      if(b_s == 0){
// Not Pressed yet
      }else if(b_s == 1){
// Press Once, Detect a Gap
          set_TA0_btn();
          if(!is_press){
              b_s = 2;
//              if(b_s == 1){b_s = 2;}
//              if(b_s == 4){b_s = 5;}
              is_doub_clk = 0;
          }
      }else if(b_s == 2){
// Detect Second Click
          set_TA0_btn();
      }else if(b_s == 3){
// Measuring Temperature
        set_TA0_measu();
      }else if(b_s == 4){
// Press Once, Detect a Gap
        set_TA0_btn();
        if(!is_press){
            b_s = 5;
            is_doub_clk = 0;
        }
      }else if(b_s == 5){
// Detect Second Click
        set_TA0_btn();
      }
  }
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    ADC_val = ADC10MEM;
    avg_ADC_val = (adc[0] + adc[1] + adc[2] + adc[3]) / 4;
    ADC10SA = (int)adc;

    t = (((float)ADC_val * 1.5 / 1023) - 0.986) / 0.00355;
    avg_t = (((float)avg_ADC_val * 1.5 / 1023) - 0.986) / 0.00355;

  if (avg_t < Threshold){
//     Below Threshold
      is_above_thres = 0;
  }else{
//     Above Threshold
      is_above_thres = 1;
  }

  if(is_LPM3_on){
      __bic_SR_register_on_exit(LPM3_bits);
  }else{
      __bic_SR_register_on_exit(LPM0_bits);
  }

//  __bic_SR_register_on_exit(CPUOFF);
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TA1_ISR(void) {
    TA1_clear();
    if(s == 6){
        s = 7;
    }else if(s == 7){
        s = 6;
    }else{
        s = (s + 1) % 6;
    }

    if(is_LPM3_on){
        __bic_SR_register_on_exit(LPM3_bits);
    }else{
        __bic_SR_register_on_exit(LPM0_bits);
    }
//    __bic_SR_register_on_exit(CPUOFF);
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void) {
    TA0_clear();

    if(b_s == 1 || b_s == 4){
// Press Once, Detect a Gap
        if(is_doub_clk > 0){
            b_s = 0;
            if(b_s == 1){b_s = 0;}
            else if(b_s == 4){b_s = 3;}
            is_doub_clk = 0;
        }
    }else if(b_s == 2 || b_s == 5){
// Detect Second Click
        if(is_doub_clk > 0){
//            s = 6;
//            b_s = 3;
            if(b_s == 2){
                // Try to enter measurement state
                s = 6;
                b_s = 3;
//                is_LPM3_on = 0;
            } else if(b_s == 5){
                // Try to leave measurement state
                s = 0;
                b_s = 0;
//                is_LPM3_on = 1;
            }
            is_doub_clk = 0;
        }else{
//            b_s = 0;
            if(b_s == 2){b_s = 0;}
            else if(b_s == 5){b_s = 3;}
        }
    }
//    The original one, longer but has the same utility
//    else if(b_s == 4){
//// Press Once, Detect a Gap
//        if(is_doub_clk > 0){
////          Go Back
//            b_s = 3;
//            is_doub_clk = 0;
//        }
//    }else if(b_s == 5){
//// Detect Second Click
//        if(is_doub_clk > 0){
//            s = 0;
//            b_s = 0;
//            is_doub_clk = 0;
//        }else{
////          Go Back
//            b_s = 3;
//        }
//    }


    if(is_LPM3_on){
        __bic_SR_register_on_exit(LPM3_bits);
    }else{
        __bic_SR_register_on_exit(LPM0_bits);
    }
//    __bic_SR_register_on_exit(CPUOFF);
}

// Button Interrupt
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    btn_clear();
    b_count++;
    is_doub_clk = 1;

    if(b_s == 0){
//        Not Clicked Yet
        b_s = 1;
        is_doub_clk = 0;
    }else if(b_s == 3){
//        In measuring temperature
        b_s = 4;
        is_doub_clk = 0;
    }


    if(is_LPM3_on){
        __bic_SR_register_on_exit(LPM3_bits);
    }else{
        __bic_SR_register_on_exit(LPM0_bits);
    }
//    __bic_SR_register_on_exit(CPUOFF);
}

