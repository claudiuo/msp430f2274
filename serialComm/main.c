#include "msp430.h"
#include "in430.h"

const unsigned int FAST = 10000;
const unsigned int SLOW = 10000;

unsigned int timerCount = 0; // use it to count timer overflows

void toggleLeds(int,int,unsigned int);
void doIsrWork(int);
void printf(char *, ...);
void sendByte(unsigned char);

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // init watchdog timer to off

    if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
    {
        while(1);                               // do not load, trap CPU!!
    }

    // setup serial communication
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;
    P3SEL = 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt


    P1DIR |= 0x03; // set LED output to P1.0 and P1.1
    // equivalent: P1DIR |= BIT0 + BIT1;

    P1OUT |= BIT0 + BIT1; // turn on LEDs

    P1DIR &= ~0x04; // set P1.2 as input
    P1REN |=  0x04; // enable resistor on P1.2
    P1IE  |=  0x04; // enable interrupts on P1.2

    // set P2 port
    P2DIR &= ~BIT3; // set P2.3 as input
    P2OUT &= ~BIT3; // pin is pulled down
    P2REN |=  BIT3; // enable resistor on P2.3
    P2IES &= ~BIT3; // low to high transition
    P2IE  |=  BIT3; // enable interrupts on P2.3

    // set up ADC
//    ADC10CTL1 = ADC10DF;                        // Conversion code signed format ref.to AVcc
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
    ADC10AE0 |= 0x01;                           // P2.0 ADC option select
    __delay_cycles(30); // allow ref voltage to settle for at least 30us
                        // (30us * 1MHz = 30 cycles): see SLAS504D for info

    // set up Timer A
    BCSCTL3 |= LFXT1S_2;        // ACLK on sourced to the VLO
    TACCTL0 = CCIE;             // enable interrupts for Timer A
    TACCR0 = 48000;             // set the count up value: approx. 2s
                                // (VLO around 12kHz) * 1 (ID_0) = 2s

    __bis_SR_register( GIE + LPM3_bits ); // enable interrupts and sleep
                                          // (LPM3 leaves only ACLK running)
}

#pragma vector=PORT1_VECTOR    // button ISR
interrupt void Port_1 ( void )
{
    printf( "\r\nRestart button" );
    doIsrWork(BIT0);
    P1IFG &= ~0x04;               // clear interrupt flag
}

#pragma vector=PORT2_VECTOR    // P2 ISR
interrupt void Port_2 ( void )
{
    printf( "\r\nRestart port2" );
    doIsrWork(BIT1);
    P2IFG &= ~BIT3;               // clear interrupt flag
}

#pragma vector=TIMERA0_VECTOR    // Timer A ISR
interrupt void Timer_A ( void )
{
    volatile int adcVal;
    volatile long result;

    P1OUT &= ~0x03;                // turn off LEDs
    toggleLeds(2,SLOW,BIT0+BIT1); // blink both LEDs slow to signal timer ISR
    P1OUT &= ~0x03;                // turn off LEDs

    switch(TAIV) {
        case TAIV_NONE:
            if(++timerCount == 5) {  // 10s
                TACTL = MC_0;        // timer halted
                // equivalent: TACTL &= ~(MC_1);
            } else {
                // get the ADC value
                ADC10CTL0 |= ENC + ADC10SC;      // Sampling and conversion start
                __bis_SR_register(CPUOFF + GIE); // LPM0, ADC10_ISR will force exit

                adcVal = ADC10MEM;
                result = ((adcVal - 630) * 761) / 1024;
                printf("\r\nvalue read %i", adcVal);
                printf("\r\nvalue calc %l", result);

                /* Stop and turn off ADC */
                ADC10CTL0 &= ~ENC;
                __bis_SR_register(LPM3_bits + GIE); // LPM3, Timer_A ISR will force exit
            }
            break;
    }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    printf("\r\nADC");
    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

void doIsrWork (int led)
{
    P1OUT &= ~0x03;               // turn off LEDs
    toggleLeds(2,SLOW,led);       // blink red/green LED to signal P1/P2 ISR
    P1OUT |= 0x03;     		      // turn on LEDs

    timerCount = 0;               // reset the timerCount
    // (re)start the timer
    TACTL = MC_1+TASSEL_1+ID_0;   // Timer A count up (MC_1) using
                                  // ACLK (TASSEL_1) no division (ID_0)
}

void toggleLeds (int howMany, int timeout, unsigned int leds )
{
    int i,j = 0;
    // to get some blinking for sure
    if(howMany == 1) {
        howMany = 2;
    }
    for ( j=0; j<howMany; j++) {

        P1OUT ^= leds;  // toggle LEDs
        for ( i=0; i<timeout; i++) {
            __no_operation();
        }
    }
    P1OUT &= ~leds; // turn off LEDs
}

void toggleBothLeds (int howMany, int timeout )
{
    int i,j = 0;
    // to get some blinking for sure
    if(howMany == 1) {
        howMany = 2;
    }
    for ( j=0; j<howMany; j++) {

        P1OUT ^= 0x03;  // toggle LEDs
        for ( i=0; i<timeout; i++) {
            __no_operation();
        }

        P1OUT &= ~0x01; // turn off one LED
        for ( i=0; i<timeout; i++) {
            __no_operation();
        }
    }
}

void puts(char *s) {
    char c;

    // Loops through each character in string 's'
    while (c = *s++) {
        sendByte(c);
    }
}

void putc(unsigned b) {
    sendByte(b);
}

/**
 * Sends a single byte out through UART
 **/
void sendByte(unsigned char byte )
{
    while (!(IFG2&UCA0TXIFG));			// USCI_A0 TX buffer ready?
    UCA0TXBUF = byte;					// TX -> character
}
