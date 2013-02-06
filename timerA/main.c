#include "msp430.h"
#include "in430.h"

const unsigned int FAST = 5000;
const unsigned int SLOW = 15000;

void toggleLeds(int,int);

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // init watchdog timer to off

	P1DIR |= 0x03; // set LED output to P1.0 and P1.1
	// similar: P1DIR |= BIT0 + BIT1;

	P1OUT |= BIT0 + BIT1; // turn on LEDs

	P1DIR &= ~0x04; // set P1.2 as input
	P1REN |=  0x04; // enable resistor on P1.2
    P1IE  |=  0x04; // enable interrupts on P1.2

	// set up Timer A
	BCSCTL3 |= LFXT1S_2;        // ACLK on sourced to the VLO
	TACCTL0 = CCIE;             // enable interrupts for Timer A
//	TACCR0 = 48000;             // start the timer: approx. 4s (VLO around 12kHz) * 2 (ID_1) = 8s
	TACCR0 = 0;                 // make sure timer is stopped
	TACTL = MC_1+TASSEL_1+ID_1; // Timer A count up (MC_1) using ACLK (TASSEL_1) divided by 2 (ID_1)

    __bis_SR_register( GIE + LPM3_bits ); // enable interrupts and sleep
    									  // (LPM3 leaves only ACLK running)
}

#pragma vector=PORT1_VECTOR    // button ISR
interrupt void Port_1 ( void )
{
	P1IFG &= ~0x04;     // clear interrupt flag

	TACCR0 = 0;         // stop the timer

	toggleLeds(5,FAST); // blink the LEDs fast to signal button ISR

	P1OUT |= 0x03;      // turn on LEDs

	TACCR0 = 48000;     // restart the timer
}

#pragma vector=TIMERA0_VECTOR    // Timer A ISR
interrupt void Timer_A ( void )
{
	TACCR0 = 0;         // stop the timer

	toggleLeds(5,SLOW); // blink the LEDs slow to signal timer ISR

 	P1OUT &= ~0x03;     // turn off LEDs
}

void toggleLeds (int howMany, int timeout)
{
	int i,j = 0;
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
