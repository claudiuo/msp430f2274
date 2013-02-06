#include "msp430.h"
#include "in430.h"

void toggleLeds(int);

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

    __bis_SR_register( GIE + LPM4_bits );   // Enable interrupts and sleep

}

// ISR for button
#pragma vector=PORT1_VECTOR
interrupt void Port_1 ( void )
{
	// clear interrupt flag
	P1IFG &= ~0x04;
	toggleLeds(5);
	// turn off LEDs
	P1OUT &= ~0x03;
}

void toggleLeds (int howMany)
{
	int i,j = 0;
	const unsigned int LIMIT = 5000;
    for ( j=0; j<howMany; j++) {

		// toggle LEDs
		P1OUT ^= 0x03;
		for ( i=0; i<LIMIT; i++) {
			__no_operation();
	 	}
	 	// turn off one LED
	 	P1OUT &= ~0x01;
		for ( i=0; i<LIMIT; i++) {
			__no_operation();
	 	}
	}
}
