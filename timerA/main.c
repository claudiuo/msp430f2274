#include "msp430.h"
#include "in430.h"

const unsigned int FAST = 5000;
const unsigned int SLOW = 15000;

void toggleLeds(int,int);

int main(void)
{
	// init watchdog timer to off
	WDTCTL = WDTPW + WDTHOLD;

	// set LED output to P1.0 and P1.1
	P1DIR |= 0x03;
	// similar: P1DIR |= BIT0 + BIT1;

	// turn on LEDs
	P1OUT |= BIT0 + BIT1;

	// set P1.2 as input
	P1DIR &= ~0x04;
	// enable resistor on P1.2
    P1REN |=  0x04;
    // enable interrupts on P1.2
    P1IE  |=  0x04;

	// set up Timer A
	// ACLK on sourced to the VLO
	BCSCTL3 |= LFXT1S_2;
	// enable interrupts for Timer A
	TACCTL0 = CCIE;
	// start the timer: count up to this value: approx. 4s (VLO around 12kHz)
//	TACCR0 = 48000;
	// Timer A count up (MC_1) using ACLK (TASSEL_1)
	TACTL = MC_1+TASSEL_1;
	// make sure timer doesn't run
	TACCR0 = 0;

	// enable interrupts and sleep
    //__bis_SR_register( GIE + LPM4_bits );

   	// enable interrupts and sleep (LPM3 leaves only ACLK running)
    __bis_SR_register( GIE + LPM3_bits );
}

// button ISR
#pragma vector=PORT1_VECTOR
interrupt void Port_1 ( void )
{
	// clear interrupt flag
	P1IFG &= ~0x04;
	toggleLeds(5,FAST);

	// turn on LEDs
	P1OUT |= 0x03;

	// stop and restart the timer
	TACCR0 = 0;
	TACCR0 = 48000;
}

// Timer A ISR
#pragma vector=TIMERA0_VECTOR
interrupt void Timer_A ( void )
{
	// blink the LEDs to signal timer ISR
	toggleLeds(5,SLOW);
 	// turn off LEDs
 	P1OUT &= ~0x03;

	// stop the timer
	TACCR0 = 0;
}

void toggleLeds (int howMany, int timeout)
{
	int i,j = 0;
    for ( j=0; j<howMany; j++) {

		// toggle LEDs
		P1OUT ^= 0x03;
		for ( i=0; i<timeout; i++) {
			__no_operation();
	 	}
	 	// turn off one LED
	 	P1OUT &= ~0x01;
		for ( i=0; i<timeout; i++) {
			__no_operation();
	 	}
	}
}
