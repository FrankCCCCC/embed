#include <msp430.h>

// LED Blinks & Timer
#define LED0 BIT0
#define LED1 BIT6
#define LED0_ID '0'
#define LED1_ID '1'
#define B1 BIT3
// ACLK VLO Clock
#define R_B_T 5999
#define G_B_T 10799

// Temperature Measuring & ADC
#define Threshold 740
#define TempLen 64
//#define TempDivAdc 32 // =TempLen/AdcLen

// UART
#define UART_TXD 0x02 // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD 0x04 // RXD on P1.2 (Timer0_A.CCI1A)
#define UART_TBIT_DIV_2     (1000000 / (9600 * 2))
#define UART_TBIT           (1000000 / 9600)

#define MAX_RC_ACCUM 100

// General Purpose
#define MAX_INT 65535

// LED Blinks & Timer
volatile unsigned int target_id = 0;
volatile unsigned int id_state = 0;
volatile unsigned int id_on_time = 0;
volatile unsigned int id_off_time = 0;
volatile unsigned int LED_intp_counter = 0;
void set_LEDs();
void set_LEDs_clk();
void TA1_clear();
void flash(char, int, int);
void flashE(char, int, int);

// // Temperature Measuring & ADC
int adc[TempLen]; // Buffer to store the ADC values
volatile unsigned int Temp[64] = {0};
volatile unsigned int AdcLen = 2;
volatile unsigned int TempDivAdc = 32;
volatile unsigned int is_above_thres = 0;
volatile unsigned int ADC_val = 0;
volatile unsigned int avg_ADC_val = 0;
volatile unsigned int Temp_idx = 0;
void set_ADC();
void temp(int, int);

// UART
unsigned int txData;  // UART internal TX variable
unsigned char rxBuffer; // Received UART character

volatile unsigned int rc_count = 0;
volatile unsigned char rc_accum[MAX_RC_ACCUM] = {0};
volatile unsigned int is_pressed = 0;
volatile unsigned int rc_counter = 0;
volatile unsigned int tx_counter = 0;

void TimerA_UART_print(char *string);
void TimerA_UART_init(void);
void TimerA_UART_tx(unsigned char byte);

void set_UART();
unsigned int check_ack(char);
unsigned int itoa(unsigned char *buf, unsigned int integer);
void add_to_rc_accm(char byte);
void clr_rc_accum();

// General Purpose
volatile unsigned int s = 0;
volatile unsigned int is_print_temp = 0;
volatile unsigned int is_in_LPM3 = 0;
volatile unsigned int is_should_back = 0;

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;    // Stop WDT

    set_LEDs();
    set_LEDs_clk();
    flashE(LED1_ID, 9000, 5000);
    set_ADC();
    temp(200, 4);

//    set_UART();

    __enable_interrupt();
//    TimerA_UART_init();     // Start Timer_A UART
//    TimerA_UART_print("G2xx3 TimerA UART\r\n");
//    TimerA_UART_print("READY.\r\n");

    for(;;){
        __bis_SR_register(LPM0_bits);
//        flash(LED1_ID, 9000, 5000);
//      TimerA_UART_tx(rxBuffer);
//      add_to_rc_accm(rxBuffer);

        if(s == 0){
            flash(LED1_ID, 9000, 5000);
            set_ADC();
            temp(200, 4);
            if(is_above_thres && LED_intp_counter >= 4){
                s = 1;
                LED_intp_counter = 0;
                set_UART();
                TimerA_UART_init();
                TimerA_UART_print("Emergency!\r\n");
            }
        }else if(s == 1){
            flashE(LED0_ID, 2000, 3000);
            is_should_back = is_should_back || check_ack(rxBuffer);
            if(!is_print_temp){
                unsigned char temp_chars[10] = {0};
                unsigned int temp = itoa(temp_chars, avg_ADC_val);
                TimerA_UART_print(temp_chars);
                TimerA_UART_print("\r\n");

                is_print_temp = 1;
                LED_intp_counter = 0;
            }
            if(is_should_back){
//                TimerA_UART_print("Back to Normal State\r\n");
                s = 0;
                is_should_back = 0;
                is_print_temp = 0;
                LED_intp_counter = 0;
            }else if(LED_intp_counter >= 8){
                s = 2;
                set_ADC();
                temp(200, 4);
                LED_intp_counter = 0;
                is_print_temp = 0;
            }
        }else if(s == 2){
            flashE(LED0_ID, 2000, 3000);
            if(LED_intp_counter >= 1){
               s = 1;
               set_UART();
               TimerA_UART_init();
               LED_intp_counter = 0;
               is_print_temp = 0;
            }

        }
    }
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TA1_ISR(void) {
    TA1_clear();
    LED_intp_counter = (LED_intp_counter + 1) % MAX_INT;
    id_state = (id_state + 1) % 2;
    if(is_in_LPM3){
        __bic_SR_register_on_exit(LPM3_bits);
    }else{
        __bic_SR_register_on_exit(LPM0_bits);
    }

}



// Flash with LEDs
void set_LEDs(){
    //  Set up LED
    P1DIR |= LED0 | LED1;
    P1OUT &= ~LED0;
    P1OUT &= ~LED1;
}

void set_LEDs_clk(){
    //  LED Clock
    TA1CCR0 = id_on_time;
    BCSCTL3 |= LFXT1S_2;
    TA1CTL |= MC_1|TASSEL_1|TACLR;
    TA1CCTL0 = CCIE; // Enable interrupts
}

void TA1_clear(){
    TA1CTL &= ~TAIFG;  // Clear overflow flag
    TA1R = 0;
}

void flash(char id, int on, int off){
//    id_state = 0;
    target_id = id;
    id_on_time = on + on / 5;
    id_off_time = off + off / 5;

    int target_LED = 0, non_target_LED = 1;
    if(target_id == LED0_ID){
        target_LED = LED0;
        non_target_LED = LED1;
    }else if(target_id == LED1_ID){
        target_LED = LED1;
        non_target_LED = LED0;
    }
    switch(id_state){
        case 0:
//              Turn On
            P1OUT |= target_LED;
            P1OUT &= ~non_target_LED;
            TA1CCR0 = id_off_time;
            break;
        case 1:
//              Turn Off
            P1OUT &= ~target_LED;
            P1OUT &= ~non_target_LED;
            TA1CCR0 = id_on_time;
            break;
        default:
            id_state = 1;
            P1OUT &= ~target_LED;
            P1OUT &= ~non_target_LED;
            TA1CCR0 = id_on_time;
            break;
    }
    is_in_LPM3 = 1;
    __bis_SR_register(LPM3_bits);
}

void flashE(char id, int on, int off){
//    id_state = 0;
    target_id = id;
    id_on_time = on + on / 5;
    id_off_time = off + off / 5;

    int target_LED = 0, non_target_LED = 1;
    if(target_id == LED0_ID){
        target_LED = LED0;
        non_target_LED = LED1;
    }else if(target_id == LED1_ID){
        target_LED = LED1;
        non_target_LED = LED0;
    }
    switch(id_state){
        case 0:
//              Turn On
            P1OUT |= target_LED;
            P1OUT &= ~non_target_LED;
            TA1CCR0 = id_off_time;
            break;
        case 1:
//              Turn Off
            P1OUT &= ~target_LED;
            P1OUT &= ~non_target_LED;
            TA1CCR0 = id_on_time;
            break;
        default:
            id_state = 1;
            P1OUT &= ~target_LED;
            P1OUT &= ~non_target_LED;
            TA1CCR0 = id_on_time;
            break;
    }
}

// Temperature Measuring with ADC

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    unsigned int i = 0;
    ADC_val = ADC10MEM;

    avg_ADC_val = adc[0];
    for(i = 1; i < AdcLen; i++){
        avg_ADC_val += adc[i];
    }
    avg_ADC_val = avg_ADC_val / AdcLen;

    ADC10SA = (int)adc;

    for(i = 0; i < AdcLen; i++){
        Temp[Temp_idx * AdcLen + i] = adc[i];
    }
    Temp_idx = (Temp_idx + 1) % TempDivAdc;

  if (avg_ADC_val < Threshold){
//     Below Threshold
      is_above_thres = 0;
  }else{
//     Above Threshold
      is_above_thres = 1;
  }
  __bic_SR_register_on_exit(LPM0_bits);

  if(is_in_LPM3){
      __bic_SR_register_on_exit(LPM3_bits);
  }else{
      __bic_SR_register_on_exit(LPM0_bits);
  }
}

void set_ADC(){
    TA0CCTL1 = OUTMOD_3;  // TA0CCR1 set/reset
    TA0CTL = TASSEL_1 + MC_1;    // ACLK, up mode

//  H&S time 16x, interrupt enabled
    ADC10CTL0 =  SREF_1 + REFON + ADC10SHT_2 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_10 + SHS_1 + CONSEQ_2;    // Input from A1
    ADC10AE0 |= 0x10; // Enable pin A1 for analog in
    ADC10CTL0 |= ENC; // Start sampling

    ADC10DTC1 = AdcLen;     // # of transfers
    ADC10SA = (int)adc; // Buffer starting address
}

void temp(int interval, int times){
    AdcLen = times;
    TempDivAdc = TempLen / AdcLen;
    TA0CCR0 = interval + 10;     // Sampling period
    TA0CCR1 = interval;       // TA0CCR1 OUT1 on time
    ADC10DTC1 = AdcLen;     // # of transfers
    TA0CCTL0 = 0; // Disable interrupts
}

// UART with TimerA
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
  txData |= 0x100;    // Add stop bit to TXData
  txData <<= 1;       // Add start bit
}
#pragma vector = TIMER0_A0_VECTOR  // TXD interrupt
__interrupt void Timer_A0_ISR(void) {
  static unsigned char txBitCnt = 10;
  TA0CCR0 += UART_TBIT; // Set TA0CCR0 for next intrp
  tx_counter++;
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
//  __bic_SR_register_on_exit(LPM0_bits);
}
#pragma vector = TIMER0_A1_VECTOR // RXD interrupt
__interrupt void Timer_A1_ISR(void) {
  static unsigned char rxBitCnt = 8;
  static unsigned char rxData = 0;
  rc_counter++;
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
          if(is_in_LPM3){
              __bic_SR_register_on_exit(LPM3_bits);
          }else{
              __bic_SR_register_on_exit(LPM0_bits);
          }
          // Clear LPM0 bits SR
        }
      }
      break;
    }
}
void set_UART(){
    DCOCTL = 0x00;             // Set DCOCLK to 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    P1OUT |= 0x00;       // Initialize all GPIO
    P1SEL |= UART_TXD + UART_RXD; // Use TXD/RXD pins
    P1DIR |= 0xFF & ~UART_RXD; // Set pins to output

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

unsigned int check_ack(char byte){
    unsigned int flag = 0;
    if((int)(byte) != 0){
        rc_accum[rc_count] = byte;
        rc_count++;
    }
    if((int)(byte) == 13){
//        unsigned int i;
//        unsigned int rc_count_len = itoa(&(rc_accum[rc_count - 1]), rc_count);

//        for(i = 0; i < rc_count + rc_count_len; i++){
//            TimerA_UART_tx(rc_accum[i]);
//            rc_accum[i] = '\0';
//        }
        unsigned int i;
        if(rc_accum[0] == 'A' &&
           rc_accum[1] == 'C' &&
           rc_accum[2] == 'K' &&
           rc_accum[3] == '!'){
            flag = 1;
        }
        for(i = 0; i < rc_count; i++){
            rc_accum[i] = '\0';
        }

        rc_count = 0;
    }

    return flag;
}
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

