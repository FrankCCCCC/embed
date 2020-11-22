#include "msp430.h"
#include "stdio.h"
#include <stdlib.h>
//#include "cstdlib"
#define UART_TXD 0x02 // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD 0x04 // RXD on P1.2 (Timer0_A.CCI1A)
#define UART_TBIT_DIV_2     (1000000 / (9600 * 2))
#define UART_TBIT           (1000000 / 9600)
#define MAX_RC_ACCUM 100
#define MAX_CONCAT_ACCUM 110
#define HELLO_TIMER 35999

//#define LED1 BIT0
//#define LED2 BIT6
#define B1 BIT3

unsigned int txData;  // UART internal TX variable
unsigned char rxBuffer; // Received UART character

volatile unsigned int rc_count = 0;
volatile unsigned char rc_accum[MAX_RC_ACCUM] = {0};
volatile unsigned int is_pressed = 0;
volatile unsigned int is_print_hello = 0;
volatile unsigned int clk_count = 0;

void TimerA_UART_init(void);
void TimerA_UART_tx(unsigned char byte);
void TimerA_UART_print(char *string);
void set_btn();
void TA1_clear();
unsigned int itoa(unsigned char *buf, unsigned int i);
void add_to_rc_accm(char byte){

    if((int)(byte) != 0){
        rc_accum[rc_count] = byte;
        rc_count++;
    }
    if((int)(byte) == 13){
        unsigned int i;
        unsigned int rc_count_len = itoa(&(rc_accum[rc_count - 1]), rc_count);

        for(i = 0; i < rc_count + rc_count_len; i++){
            TimerA_UART_tx(rc_accum[i]);
            rc_accum[i] = '\0';
        }

        rc_count = 0;
    }
}
void clr_rc_accum(){
    unsigned int i;
    for(i = 0; i < MAX_RC_ACCUM; i++){
        rc_accum[i] = 0;
    }
    rc_count = 0;
}
void main(void) {
  WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer
  DCOCTL = 0x00;             // Set DCOCLK to 1MHz
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  P1OUT |= 0x00;       // Initialize all GPIO
  P1SEL |= UART_TXD + UART_RXD; // Use TXD/RXD pins
  P1DIR |= 0xFF & ~UART_RXD; // Set pins to output

  set_btn();
  set_hello_timer();

  __enable_interrupt();
  TimerA_UART_init();     // Start Timer_A UART
  TimerA_UART_print("G2xx3 TimerA UART\r\n");
  TimerA_UART_print("READY.\r\n");

  for (;;) {
    // Wait for incoming character
    __bis_SR_register(LPM0_bits);
    // Echo received character
//    TimerA_UART_tx(rxBuffer);


    if(is_pressed){
        if(is_print_hello){
            TimerA_UART_print("HELLO\r\n");
            is_print_hello=0;
        }

    }else{
        add_to_rc_accm(rxBuffer);
    }
  }
}
void TimerA_UART_print(char *string) {
  while (*string) TimerA_UART_tx(*string++);
}
void TimerA_UART_init(void) {
  TA0CCTL0 = OUT;   // Set TXD idle as '1'
  TA0CCTL1 = SCS + CM1 + CAP + CCIE; // CCIS1 = 0
  // Set RXD: sync, neg edge, capture, interrupt
  TA0CTL = TASSEL_2 + MC_2; // SMCLK, continuous mode
}
void TimerA_UART_tx(unsigned char byte) {
  while (TA0CCTL0 & CCIE); // Ensure last char TX'd
  TA0CCR0 = TA0R;      // Current count of TA counter
  TA0CCR0 += UART_TBIT; // One bit time till 1st bit
  TA0CCTL0 = OUTMOD0 + CCIE; // Set TXD on EQU0, Int
  txData = byte;       // Load char to be TXD
  txData |= 0x100;    // Add stop bit to TXData
  txData <<= 1;       // Add start bit
}
#pragma vector = TIMER0_A0_VECTOR  // TXD interrupt
__interrupt void Timer_A0_ISR(void) {
  static unsigned char txBitCnt = 10;
  TA0CCR0 += UART_TBIT; // Set TA0CCR0 for next intrp
  if (txBitCnt == 0) {  // All bits TXed?
    TA0CCTL0 &= ~CCIE;  // Yes, disable intrpt
    txBitCnt = 10;      // Re-load bit counter
  } else {
    if (txData & 0x01) {// Check next bit to TX
      TA0CCTL0 &= ~OUTMOD2; // TX '1¡¦ by OUTMODE0/OUT
    } else {
      TA0CCTL0 |= OUTMOD2;} // TX '0¡¥
    txData >>= 1;        txBitCnt--;
  }
}
#pragma vector = TIMER0_A1_VECTOR // RXD interrupt
__interrupt void Timer_A1_ISR(void) {
  static unsigned char rxBitCnt = 8;
  static unsigned char rxData = 0;
  switch (__even_in_range(TA0IV, TA0IV_TAIFG)) {
    case TA0IV_TACCR1:     // TACCR1 - UART RXD
      TA0CCR1 += UART_TBIT;// Set TACCR1 for next int
      if (TA0CCTL1 & CAP) { // On start bit edge
        TA0CCTL1 &= ~CAP;   // Switch to compare mode
        TA0CCR1 += UART_TBIT_DIV_2;// To middle of D0
      } else {             // Get next data bit
        rxData >>= 1;
        if (TA0CCTL1 & SCCI) { // Get bit from latch
          rxData |= 0x80; }
        rxBitCnt--;
        if (rxBitCnt == 0) {  // All bits RXed?
          rxBuffer = rxData;  // Store in global
          rxBitCnt = 8;       // Re-load bit counter
          TA0CCTL1 |= CAP;     // Switch to capture
          __bic_SR_register_on_exit(LPM0_bits);
          // Clear LPM0 bits SR
        }
      }
      break;
    }
}

//
void set_hello_timer(){
    // Hello Timer
    TA1CCR0 = HELLO_TIMER;
    BCSCTL3 |= LFXT1S_2;
    TA1CTL |= MC_1|TASSEL_1|TACLR;
    TA1CCTL0 = CCIE; // Enable interrupts
}

void TA1_clear(){
    TA1CTL &= ~TAIFG;  // Clear overflow flag
    TA1R = 0;
}

void set_btn(){
//  Set up Button
    P1DIR &= ~B1;
    P1REN |= B1;
    P1OUT |= B1;

    P1IE |= B1;              // P1.4 interrupt enabled
    P1IES |= B1;             // P1.4 Hi/lo edge
    P1IFG &= ~B1;            // P1.4 IFG cleared
    _BIS_SR(GIE);              // Enter interrupt
}


void btn_clear(){
    P1IFG &= ~B1;             // P1.4 IFG cleared
}

unsigned int itoa(unsigned char *buf, unsigned int integer){
    unsigned int div = 1, i = 0, last_quo = 0;
    for(; div < integer; div *= 10){}
    div /= 10;
    for(i = 0; div >= 1; div /= 10, i++){
        buf[i] = (integer / div - last_quo * 10) + '0';
        last_quo = integer / div;
    }
    buf[i+1] = '\0';

    return i + 2;
}

// Button Interrupt
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    btn_clear();
    if(is_pressed){
        is_pressed = 0;
    }else{
        is_pressed = 1;
    }
    clk_count++;
    __bic_SR_register_on_exit(LPM0_bits);
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TA1_ISR(void) {
    TA1_clear();
    if(is_pressed){
        is_print_hello = 1;
    }

    __bic_SR_register_on_exit(LPM0_bits);
}
