#include "msp430.h"
#include "in430.h"

const unsigned int FAST = 10000;
const unsigned int SLOW = 15000;

static unsigned int timerCount = 0; // use it to count timer overflows

void toggleLeds(int,int,unsigned int);
void doIsrWork(int);

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // init watchdog timer to off

    P1DIR |= 0x03; // set LED output to P1.0 and P1.1
    // similar: P1DIR |= BIT0 + BIT1;

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

    // set up Timer A
    BCSCTL3 |= LFXT1S_2;        // ACLK on sourced to the VLO
    TACCTL0 = CCIE;             // enable interrupts for Timer A
    TACCR0 = 24000;             // set the count up value: approx. 1s
                                // (VLO around 12kHz) * 1 (ID_0) = 1s

    __bis_SR_register( GIE + LPM3_bits ); // enable interrupts and sleep
                                          // (LPM3 leaves only ACLK running)
}

#pragma vector=PORT1_VECTOR    // button ISR
interrupt void Port_1 ( void )
{
	doIsrWork(BIT0);
    P1IFG &= ~0x04;               // clear interrupt flag
}

#pragma vector=PORT2_VECTOR    // P2 ISR
interrupt void Port_2 ( void )
{
	doIsrWork(BIT1);
    P2IFG &= ~BIT3;               // clear interrupt flag
}

#pragma vector=TIMERA0_VECTOR    // Timer A ISR
interrupt void Timer_A ( void )
{
    P1OUT &= ~0x03;                // turn off LEDs
    toggleLeds(10,SLOW,BIT0+BIT1); // blink both LEDs slow to signal timer ISR
    P1OUT &= ~0x03;                // turn off LEDs

    switch(TAIV) {
        case TAIV_NONE:
            if(timerCount++ == 9) { // this happens after 10s
                TACTL = MC_0;       // timer halted
            }
            break;
    }
}

void doIsrWork (int led)
{
    P1OUT &= ~0x03;               // turn off LEDs
    toggleLeds(10,SLOW,led);      // blink red/green LED to signal P1/P2 ISR
    P1OUT &= ~led;     		      // turn off LED that was blinking

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
