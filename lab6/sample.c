#include "msp430.h"
#define UART_TXD 0x02 // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD 0x04 // RXD on P1.2 (Timer0_A.CCI1A)
#define UART_TBIT_DIV_2     (1000000 / (9600 * 2))
#define UART_TBIT           (1000000 / 9600)
unsigned int txData;  // UART internal TX variable 
unsigned char rxBuffer; // Received UART character

void TimerA_UART_init(void);
void TimerA_UART_tx(unsigned char byte);
void TimerA_UART_print(char *string);
void main(void) {
  WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer
  DCOCTL = 0x00;             // Set DCOCLK to 1MHz
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  P1OUT = 0x00;       // Initialize all GPIO
  P1SEL = UART_TXD + UART_RXD; // Use TXD/RXD pins
  P1DIR = 0xFF & ~UART_RXD; // Set pins to output
  __enable_interrupt();
  TimerA_UART_init();     // Start Timer_A UART
  TimerA_UART_print("G2xx3 TimerA UART\r\n");
  TimerA_UART_print("READY.\r\n");
  for (;;) {
    // Wait for incoming character
    __bis_SR_register(LPM0_bits);
    // Echo received character
    TimerA_UART_tx(rxBuffer);
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
      TA0CCTL0 &= ~OUTMOD2; // TX '1’ by OUTMODE0/OUT
    } else {
      TA0CCTL0 |= OUTMOD2;} // TX '0‘
    txData >>= 1;        txBitCnt--;
  }
} 
