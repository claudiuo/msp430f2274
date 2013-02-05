#include "msp430.h"
#include "in430.h"
int main(void)
{
	// init watchdog timer to off
	WDTCTL = WDTPW + WDTHOLD;

	// set LED output to pin 1.0 and 1.1
	P1DIR |= 0x03;
	// similar: P1DIR |= BIT0 + BIT1;
	// turn on LEDs
	P1OUT |= BIT0 + BIT1;
	
	int i = 0;
	while(1)
	{
		// toggle LEDs
		P1OUT ^= 0x03;
		for ( i=0; i<10000; i++) {
			__no_operation();
	 	}
	 	// turn off one LED
	 	P1OUT &= ~0x01;
		for ( i=0; i<10000; i++) {
			__no_operation();
	 	}
	}	
}
